"use strict"

const crypto = require("crypto")
const fs = require("fs")
const http = require("http")
const https = require("https")
const zlib = require("zlib")
const { URL } = require("url")
const { createSyncReceiver } = require("./sync-receiver")

const LOCATION_CACHE_MS = 24 * 60 * 60 * 1000
const WEATHER_CACHE_MS = 3 * 60 * 1000
const REQUEST_TIMEOUT_MS = 10000
const MAX_RESPONSE_BYTES = 1024 * 1024
const LOCATION_CACHE_LIMIT = 128
const WEATHER_CACHE_LIMIT = 64
const MAX_PENDING_WEATHER = 16
const RATE_LIMIT_WINDOW_MS = 60 * 1000
const RATE_LIMIT_REQUESTS = 30
const RATE_LIMIT_CLIENTS = 128

const locationCache = new Map()
const weatherCache = new Map()
const pendingWeather = new Map()
const requestRates = new Map()
const syncReceiver = createSyncReceiver()

const trustedLocations = {
  "101010100": {
    id: "101010100", name: "北京", adm2: "北京", adm1: "北京", country: "中国"
  },
  "101020100": {
    id: "101020100", name: "上海", adm2: "上海", adm1: "上海", country: "中国"
  },
  "101280101": {
    id: "101280101", name: "广州", adm2: "广州", adm1: "广东", country: "中国"
  },
  "101280601": {
    id: "101280601", name: "深圳", adm2: "深圳", adm1: "广东", country: "中国"
  },
  "101200101": {
    id: "101200101", name: "武汉", adm2: "武汉", adm1: "湖北", country: "中国"
  }
}

let privateKey = null
let tokenCache = null
let administrativeRows = null

function publicError(statusCode, message) {
  const error = new Error(message)
  error.statusCode = statusCode
  return error
}

function setBoundedCache(cache, key, value, limit) {
  const now = Date.now()
  for (const entry of cache) {
    if (!entry[1] || entry[1].expiresAt <= now) cache.delete(entry[0])
  }
  while (cache.size >= limit) {
    const oldest = cache.keys().next()
    if (oldest.done) break
    cache.delete(oldest.value)
  }
  cache.set(key, value)
}

function clientAllowed(request) {
  const address = String(
    request.socket && request.socket.remoteAddress || "unknown"
  )
  const now = Date.now()
  let rate = requestRates.get(address)

  if (!rate || rate.resetAt <= now) {
    rate = { count: 0, resetAt: now + RATE_LIMIT_WINDOW_MS }
  }
  rate.count += 1
  setBoundedCache(
    requestRates,
    address,
    { count: rate.count, resetAt: rate.resetAt, expiresAt: rate.resetAt },
    RATE_LIMIT_CLIENTS
  )
  return rate.count <= RATE_LIMIT_REQUESTS
}

function requiredEnvironment(name) {
  const value = String(process.env[name] || "").trim()
  if (!value) {
    throw publicError(500, "Missing required environment variable: " + name)
  }
  return value
}

function parsePort(value) {
  const port = Number(value)
  if (!Number.isInteger(port) || port < 1 || port > 65535) {
    throw publicError(500, "Invalid WEATHER_SERVER_PORT")
  }
  return port
}

function apiBaseUrl() {
  const configured = requiredEnvironment("QWEATHER_API_HOST")
  let base

  try {
    base = new URL(
      configured.indexOf("://") === -1 ? "https://" + configured : configured
    )
  } catch (error) {
    throw publicError(500, "Invalid QWEATHER_API_HOST")
  }

  if (
    base.protocol !== "https:" ||
    base.username ||
    base.password ||
    base.search ||
    base.hash
  ) {
    throw publicError(500, "QWEATHER_API_HOST must be an HTTPS host")
  }

  return base
}

function readPrivateKey() {
  if (privateKey) return privateKey

  const keyPath = requiredEnvironment("QWEATHER_PRIVATE_KEY_PATH")
  let keyBytes

  try {
    const stat = fs.statSync(keyPath)
    if (!stat.isFile()) {
      throw new Error("not a regular file")
    }
    keyBytes = fs.readFileSync(keyPath)

    try {
      privateKey = crypto.createPrivateKey(keyBytes)
      keyBytes.fill(0)
    } catch (error) {
      /* Some Node.js 12/OpenSSL builds report a missing optional DSO while
       * initializing Ed25519, but can sign the PEM after that one-time
       * initialization attempt.  Fall back only for that runtime defect;
       * malformed or unsupported keys must still fail closed. */
      if (error.code !== "ERR_OSSL_DSO_COULD_NOT_LOAD_THE_SHARED_LIBRARY") {
        keyBytes.fill(0)
        throw error
      }
      privateKey = keyBytes
    }
  } catch (error) {
    throw publicError(500, "QWeather private key is unavailable")
  }

  return privateKey
}

function base64url(value) {
  const buffer = Buffer.isBuffer(value) ? value : Buffer.from(value)
  return buffer
    .toString("base64")
    .replace(/\+/g, "-")
    .replace(/\//g, "_")
    .replace(/=+$/g, "")
}

function base64urlJson(value) {
  return base64url(JSON.stringify(value))
}

function createJwt() {
  const now = Math.floor(Date.now() / 1000)
  if (tokenCache && tokenCache.refreshAt > now) return tokenCache.value

  const header = base64urlJson({
    alg: "EdDSA",
    kid: requiredEnvironment("QWEATHER_CREDENTIAL_ID")
  })
  const payload = base64urlJson({
    sub: requiredEnvironment("QWEATHER_PROJECT_ID"),
    iat: now - 30,
    exp: now + 900
  })
  const unsignedToken = header + "." + payload
  let signature

  try {
    signature = crypto.sign(
      null,
      Buffer.from(unsignedToken, "utf8"),
      readPrivateKey()
    )
  } catch (error) {
    throw publicError(500, "Unable to sign QWeather request")
  }

  const value = unsignedToken + "." + base64url(signature)
  tokenCache = { value: value, refreshAt: now + 600 }
  return value
}

function requestJson(url, headers) {
  return new Promise(function (resolve, reject) {
    let settled = false
    const request = https.request(
      {
        protocol: "https:",
        hostname: url.hostname,
        port: url.port || 443,
        method: "GET",
        path: url.pathname + url.search,
        headers: headers
      },
      function (response) {
        const chunks = []
        let received = 0

        response.on("data", function (chunk) {
          if (settled) return
          received += chunk.length
          if (received > MAX_RESPONSE_BYTES) {
            settled = true
            response.destroy()
            reject(publicError(502, "QWeather response is too large"))
            return
          }
          chunks.push(chunk)
        })

        response.on("end", function () {
          if (settled) return
          settled = true

          const rawBody = Buffer.concat(chunks)
          const contentEncoding = String(
            response.headers["content-encoding"] || "identity"
          ).toLowerCase()
          let decodedBody = rawBody

          try {
            if (contentEncoding === "gzip") {
              decodedBody = zlib.gunzipSync(rawBody)
            } else if (contentEncoding === "deflate") {
              decodedBody = zlib.inflateSync(rawBody)
            } else if (
              contentEncoding === "br" &&
              typeof zlib.brotliDecompressSync === "function"
            ) {
              decodedBody = zlib.brotliDecompressSync(rawBody)
            } else if (
              contentEncoding !== "identity" && contentEncoding !== ""
            ) {
              throw new Error("unsupported content encoding")
            }
          } catch (error) {
            reject(publicError(502, "Unable to decode QWeather response"))
            return
          }
          if (decodedBody.length > MAX_RESPONSE_BYTES) {
            reject(publicError(502, "QWeather response is too large"))
            return
          }
          let data
          try {
            data = JSON.parse(decodedBody.toString("utf8"))
          } catch (error) {
            const contentType = String(
              response.headers["content-type"] || "unknown"
            ).split(";")[0]
            reject(publicError(
              502,
              "QWeather returned invalid JSON" +
                " (status=" + String(response.statusCode || 0) +
                ", type=" + contentType +
                ", encoding=" + contentEncoding +
                ", prefix=" + rawBody.slice(0, 8).toString("hex") + ")"
            ))
            return
          }

          if (
            response.statusCode < 200 ||
            response.statusCode >= 300 ||
            data.code !== "200"
          ) {
            const code = data && data.code ? String(data.code) :
              String(response.statusCode || 502)
            let localStatus = 502

            if (response.statusCode >= 400 && response.statusCode <= 599) {
              localStatus = response.statusCode
            } else if (code === "204") {
              localStatus = 404
            }
            reject(publicError(
              localStatus,
              "QWeather request failed: " + code
            ))
            return
          }

          resolve(data)
        })
      }
    )

    request.setTimeout(REQUEST_TIMEOUT_MS, function () {
      request.destroy(publicError(504, "QWeather request timed out"))
    })
    request.on("error", function (error) {
      if (settled) return
      settled = true
      if (error && error.statusCode) {
        reject(error)
      } else {
        reject(publicError(502, "Unable to reach QWeather"))
      }
    })
    request.end()
  })
}

function requestQWeather(pathname, query) {
  const url = new URL(pathname, apiBaseUrl())
  const entries = Object.entries(query || {})

  for (const entry of entries) {
    const key = entry[0]
    const value = entry[1]
    if (value !== undefined && value !== null && value !== "") {
      url.searchParams.set(key, String(value))
    }
  }

  return requestJson(url, {
    Authorization: "Bearer " + createJwt(),
    Accept: "application/json",
    "Accept-Encoding": "identity",
    "User-Agent": "openvela-weather-proxy/1.0"
  })
}

function normalizeLocationName(value) {
  return String(value || "")
    .replace(/\s+/g, "")
    .replace(/特别行政区$/u, "")
    .replace(/自治州$/u, "")
    .replace(/自治县$/u, "")
    .replace(/自治区$/u, "")
    .replace(/地区$/u, "")
    .replace(/林区$/u, "")
    .replace(/[省市区县盟旗]$/u, "")
}

function loadAdministrativeRows() {
  if (administrativeRows !== null) return administrativeRows
  administrativeRows = []

  const csvPath = String(
    process.env.WEATHER_ADMINISTRATIVE_DATA_PATH || ""
  ).trim()
  if (!csvPath) return administrativeRows

  let text
  try {
    text = fs.readFileSync(csvPath, "utf8").replace(/^\uFEFF/, "")
  } catch (error) {
    throw publicError(500, "Administrative location data is unavailable")
  }

  const lines = text.trim().split(/\r?\n/).slice(1)
  for (const line of lines) {
    const match = line.match(
      /^(\d+),(\d+),(\d+),"([^"]*)","([^"]*)","([^"]*)","([^"]*)","([^"]*)"$/
    )
    if (!match) continue
    administrativeRows.push({
      id: match[1],
      pid: match[2],
      deep: Number(match[3]),
      name: match[4],
      fullName: match[8]
    })
  }

  return administrativeRows
}

function enrichAdministrativeLocation(location) {
  const rows = loadAdministrativeRows()
  let province = null
  let city = null
  let district = null

  for (const row of rows) {
    if (
      row.deep === 0 &&
      normalizeLocationName(row.name) === normalizeLocationName(location.adm1)
    ) {
      province = row
      break
    }
  }
  for (const row of rows) {
    if (
      row.deep === 1 &&
      (!province || row.pid === province.id) &&
      normalizeLocationName(row.name) === normalizeLocationName(location.adm2)
    ) {
      city = row
      break
    }
  }
  for (const row of rows) {
    if (
      row.deep === 2 &&
      (!city || row.pid === city.id) &&
      normalizeLocationName(row.name) === normalizeLocationName(location.name)
    ) {
      district = row
      break
    }
  }

  return Object.assign({}, location, {
    displayName: district ? district.fullName : location.name,
    displayAdm2: city ? city.fullName : location.adm2,
    displayAdm1: province ? province.fullName : location.adm1
  })
}

function locationScore(candidate, locationName, admName) {
  const target = normalizeLocationName(locationName)
  const candidateName = normalizeLocationName(candidate.name)
  const admTarget = normalizeLocationName(admName)
  const admOne = normalizeLocationName(candidate.adm1)
  const admTwo = normalizeLocationName(candidate.adm2)
  let score = 0

  if (candidateName === target) {
    score += 120
  } else if (
    target.indexOf(candidateName) === 0 ||
    candidateName.indexOf(target) === 0
  ) {
    score += 70
  }

  if (admTarget) {
    if (admOne === admTarget || admTwo === admTarget) {
      score += 50
    } else if (
      admOne.indexOf(admTarget) === 0 ||
      admTwo.indexOf(admTarget) === 0 ||
      admTarget.indexOf(admOne) === 0 ||
      admTarget.indexOf(admTwo) === 0
    ) {
      score += 25
    }
  }

  return score
}

async function resolveLocation(locationName, admName) {
  const cacheKey = normalizeLocationName(locationName) + "|" +
    normalizeLocationName(admName)
  const cached = locationCache.get(cacheKey)
  if (cached && cached.expiresAt > Date.now()) return cached.value

  const lookup = await requestQWeather("/geo/v2/city/lookup", {
    location: locationName,
    adm: admName,
    number: 20,
    lang: "zh"
  })
  const candidates = Array.isArray(lookup.location) ? lookup.location : []
  const coordinateLookup = /^-?\d+(?:\.\d+)?,-?\d+(?:\.\d+)?$/.test(
    locationName
  )
  let best = coordinateLookup && candidates.length ? candidates[0] : null
  let bestScore = best ? 120 : -1

  if (!coordinateLookup) {
    for (const candidate of candidates) {
      const score = locationScore(candidate, locationName, admName)
      if (score > bestScore) {
        best = candidate
        bestScore = score
      }
    }
  }

  if (!best || bestScore < 70) {
    throw publicError(404, "Unable to resolve an exact QWeather location")
  }

  setBoundedCache(locationCache, cacheKey, {
    value: best,
    expiresAt: Date.now() + LOCATION_CACHE_MS
  }, LOCATION_CACHE_LIMIT)
  return best
}

function weekdayText(dateValue) {
  const parts = String(dateValue || "").split("-")
  const date = new Date(
    Number(parts[0] || 2000),
    Number(parts[1] || 1) - 1,
    Number(parts[2] || 1)
  )
  return ["周日", "周一", "周二", "周三", "周四", "周五", "周六"][
    date.getDay()
  ]
}

function formatUpdateTime(value) {
  const match = String(value || "").match(
    /^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2})/
  )
  if (!match) return ""
  return match[2] + "-" + match[3] + " " + match[4] + ":" + match[5]
}

function buildWeatherPayload(location, nowData, dailyData) {
  const allDaily = Array.isArray(dailyData.daily) ? dailyData.daily : []
  const forecastSource = allDaily.length >= 4 ?
    allDaily.slice(1, 4) : allDaily.slice(0, 3)
  const forecast = forecastSource.map(function (item) {
    return {
      date: item.fxDate,
      weekday: weekdayText(item.fxDate),
      text: item.textDay,
      icon: item.iconDay,
      min: item.tempMin,
      max: item.tempMax
    }
  })

  return {
    code: "200",
    live: true,
    source: "QWeather",
    updatedAt: formatUpdateTime(nowData.updateTime || nowData.now.obsTime),
    observedAt: nowData.now.obsTime,
    location: {
      id: location.id,
      name: location.name,
      adm2: location.adm2,
      adm1: location.adm1,
      country: location.country,
      displayName: location.displayName || location.name,
      displayAdm2: location.displayAdm2 || location.adm2,
      displayAdm1: location.displayAdm1 || location.adm1
    },
    now: {
      temp: nowData.now.temp,
      feelsLike: nowData.now.feelsLike,
      humidity: nowData.now.humidity,
      visibility: nowData.now.vis,
      text: nowData.now.text,
      icon: nowData.now.icon
    },
    forecast: forecast
  }
}

async function loadWeather(locationId) {
  const cached = weatherCache.get(locationId)
  if (cached && cached.expiresAt > Date.now()) return cached.value
  if (pendingWeather.has(locationId)) return pendingWeather.get(locationId)
  if (pendingWeather.size >= MAX_PENDING_WEATHER) {
    throw publicError(503, "Weather proxy is busy")
  }

  const pending = Promise.all([
    requestQWeather("/v7/weather/now", {
      location: locationId,
      lang: "zh"
    }),
    requestQWeather("/v7/weather/7d", {
      location: locationId,
      lang: "zh"
    })
  ]).then(function (responses) {
    const weatherData = {
      now: responses[0],
      daily: responses[1]
    }
    setBoundedCache(weatherCache, locationId, {
      value: weatherData,
      expiresAt: Date.now() + WEATHER_CACHE_MS
    }, WEATHER_CACHE_LIMIT)
    return weatherData
  }).finally(function () {
    pendingWeather.delete(locationId)
  })

  pendingWeather.set(locationId, pending)
  return pending
}

async function weatherHandler(url) {
  const locationId = String(url.searchParams.get("locationId") || "").trim()
  const locationName = String(url.searchParams.get("location") || "").trim()
  const admName = String(url.searchParams.get("adm") || "").trim()

  if (locationId.length > 32 || locationName.length > 80 || admName.length > 80) {
    throw publicError(400, "Location query is too long")
  }

  let resolvedLocation
  if (/^\d{9}$/.test(locationId)) {
    resolvedLocation = trustedLocations[locationId]
    if (!resolvedLocation) {
      throw publicError(404, "Unsupported fixed location ID")
    }
  } else {
    if (!locationName) throw publicError(400, "Missing location")
    resolvedLocation = await resolveLocation(locationName, admName)
  }

  if (!/^\d{9}$/.test(locationId)) {
    resolvedLocation = enrichAdministrativeLocation(resolvedLocation)
  }
  const weatherData = await loadWeather(resolvedLocation.id)
  return buildWeatherPayload(
    resolvedLocation,
    weatherData.now,
    weatherData.daily
  )
}

function sendJson(response, statusCode, body) {
  const text = JSON.stringify(body)
  response.writeHead(statusCode, {
    "Content-Type": "application/json; charset=utf-8",
    "Content-Length": Buffer.byteLength(text),
    "Cache-Control": "no-store",
    "X-Content-Type-Options": "nosniff"
  })
  response.end(text)
}

function errorMessage(error) {
  if (error && error.statusCode && error.message) return error.message
  return "Weather proxy request failed"
}

const port = parsePort(process.env.WEATHER_SERVER_PORT || "8790")
const server = http.createServer(async function (request, response) {
  try {
    const url = new URL(request.url, "http://localhost")
    if (await syncReceiver.handle(request, response, url)) return

    if (request.method === "GET" && url.pathname === "/health") {
      createJwt()
      sendJson(response, 200, {
        ok: true,
        service: "smart-band-weather",
        qweatherHost: apiBaseUrl().host
      })
      return
    }

    if (request.method === "GET" && url.pathname === "/api/weather") {
      if (!clientAllowed(request)) {
        throw publicError(429, "Weather request limit exceeded")
      }
      const payload = await weatherHandler(url)
      sendJson(response, 200, payload)
      console.log(
        new Date().toISOString(),
        "weather updated",
        payload.location.id,
        payload.observedAt
      )
      return
    }

    sendJson(response, 404, { code: "404", message: "Not found" })
  } catch (error) {
    const statusCode = error && error.statusCode ? error.statusCode : 502
    const message = errorMessage(error)
    console.error(new Date().toISOString(), message)
    sendJson(response, statusCode, {
      code: String(statusCode),
      message: message,
      live: false
    })
  }
})

server.on("error", function (error) {
  console.error(
    "Weather proxy failed to listen: " +
    (error && error.code ? error.code : "unknown error")
  )
  process.exitCode = 1
})

server.listen(port, "0.0.0.0", function () {
  console.log("Weather proxy listening on port " + port)
  console.log("Daily sync records: " + syncReceiver.outputDirectory)
})
