"use strict"

const http = require("http")

const port = Number(process.env.WEATHER_SERVER_PORT || 8790)
const location = String(process.env.WEATHER_SMOKE_LOCATION || "北京")
const adm = String(process.env.WEATHER_SMOKE_ADM || "北京")

function getJson(pathname) {
  return new Promise(function (resolve, reject) {
    const request = http.get(
      {
        hostname: "127.0.0.1",
        port: port,
        path: pathname,
        timeout: 15000
      },
      function (response) {
        const chunks = []
        response.on("data", function (chunk) {
          chunks.push(chunk)
        })
        response.on("end", function () {
          let body
          try {
            body = JSON.parse(Buffer.concat(chunks).toString("utf8"))
          } catch (error) {
            reject(new Error("Proxy returned invalid JSON"))
            return
          }
          resolve({ statusCode: response.statusCode, body: body })
        })
      }
    )
    request.on("timeout", function () {
      request.destroy(new Error("Proxy request timed out"))
    })
    request.on("error", reject)
  })
}

async function main() {
  const health = await getJson("/health")
  if (health.statusCode !== 200 || health.body.ok !== true) {
    throw new Error("Health check failed with status " + health.statusCode)
  }

  const query = "/api/weather?location=" + encodeURIComponent(location) +
    "&adm=" + encodeURIComponent(adm)
  const weather = await getJson(query)
  if (
    weather.statusCode !== 200 ||
    weather.body.code !== "200" ||
    weather.body.live !== true ||
    !weather.body.now ||
    !Array.isArray(weather.body.forecast)
  ) {
    throw new Error("Weather check failed with status " + weather.statusCode)
  }

  console.log(JSON.stringify({
    health: "ok",
    weather: "ok",
    forecastItems: weather.body.forecast.length
  }))
}

main().catch(function (error) {
  console.error(error.message)
  process.exitCode = 1
})

