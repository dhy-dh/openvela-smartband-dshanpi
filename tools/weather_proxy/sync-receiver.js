"use strict"

const fs = require("fs")
const http = require("http")
const path = require("path")
const { URL } = require("url")

const PROTOCOL = "smart-band-daily-sync"
const VERSION = 1
const MAX_FRAME_BYTES = 16 * 1024
const MAX_PENDING_TRANSFERS = 8
const MAX_PENDING_PER_DEVICE = 4
const MAX_CHUNKS = 512
const MAX_CHUNK_CHARACTERS = 768
const MAX_TRANSFER_CHARACTERS = MAX_CHUNKS * MAX_CHUNK_CHARACTERS
const TRANSFER_TTL_MS = 10 * 60 * 1000
const MAX_RECORD_RESULTS = 1024
const MAX_CONTROL_TARGETS = 128
const MAX_CONTROL_STATE_BYTES = 64 * 1024

function numericPort(value, fallback) {
  const parsed = Number(value || fallback)
  if (!Number.isInteger(parsed) || parsed < 1 || parsed > 65535) {
    throw new Error("Invalid SYNC_RECEIVER_PORT")
  }
  return parsed
}

function sendJson(response, statusCode, payload) {
  const body = statusCode === 204 ? "" : JSON.stringify(payload)
  response.writeHead(statusCode, {
    "Access-Control-Allow-Origin": "*",
    "Access-Control-Allow-Headers": "Content-Type",
    "Access-Control-Allow-Methods": "GET,POST,OPTIONS",
    "Cache-Control": "no-store",
    "Content-Type": "application/json; charset=utf-8",
    "Content-Length": Buffer.byteLength(body),
    "X-Content-Type-Options": "nosniff"
  })
  response.end(body)
}

function bodyError(message) {
  const error = new Error(message)
  error.isBodyError = true
  return error
}

function readJsonBody(request) {
  return new Promise(function (resolve, reject) {
    let size = 0
    let settled = false
    const chunks = []

    request.on("data", function (chunk) {
      if (settled) return
      size += chunk.length
      if (size > MAX_FRAME_BYTES) {
        settled = true
        chunks.length = 0
        reject(bodyError("frame_too_large"))
        request.resume()
        return
      }
      chunks.push(chunk)
    })

    request.on("end", function () {
      if (settled) return
      settled = true
      try {
        const text = Buffer.concat(chunks).toString("utf8")
        resolve(JSON.parse(text || "{}"))
      } catch (error) {
        reject(bodyError("invalid_json_frame"))
      }
    })

    request.on("error", function () {
      if (settled) return
      settled = true
      reject(bodyError("request_failed"))
    })
  })
}

/* This intentionally matches src/common/daily-sync.js. It hashes JavaScript
 * string characters rather than UTF-8 bytes, so the material protocol and
 * this receiver produce the same checksum. */
function fnv1a(text) {
  let hash = 2166136261
  for (let index = 0; index < text.length; index += 1) {
    hash ^= text.charCodeAt(index)
    hash +=
      (hash << 1) +
      (hash << 4) +
      (hash << 7) +
      (hash << 8) +
      (hash << 24)
  }
  return ("00000000" + (hash >>> 0).toString(16)).slice(-8)
}

function validDate(value) {
  const match = /^(\d{4})-(\d{2})-(\d{2})$/.exec(String(value || ""))
  if (!match) return false
  const year = Number(match[1])
  const month = Number(match[2])
  const day = Number(match[3])
  const date = new Date(Date.UTC(year, month - 1, day))
  return (
    date.getUTCFullYear() === year &&
    date.getUTCMonth() === month - 1 &&
    date.getUTCDate() === day
  )
}

function compactDate(value) {
  return validDate(value) ? String(value).replace(/-/g, "") : ""
}

function simulationId(value) {
  const match = /^simulation-([1-9]\d{0,15})$/.exec(String(value || ""))
  return match ? match[1] : ""
}

function validTransferDate(value) {
  return validDate(value) || !!simulationId(value)
}

function validDeviceId(value) {
  return (
    typeof value === "string" &&
    value.length >= 1 &&
    value.length <= 80 &&
    /^[A-Za-z0-9][A-Za-z0-9_.-]*$/.test(value)
  )
}

function identityFromSyncId(syncId, date) {
  if (
    typeof syncId !== "string" ||
    syncId.length > 112 ||
    !validTransferDate(date)
  ) {
    return null
  }
  const suffix = ":" + date + ":v" + VERSION
  if (syncId.length <= suffix.length || syncId.slice(-suffix.length) !== suffix) {
    return null
  }
  const deviceId = syncId.slice(0, syncId.length - suffix.length)
  return validDeviceId(deviceId) ? { deviceId: deviceId, date: date } : null
}

function ack(syncId, status) {
  return {
    protocol: PROTOCOL,
    version: VERSION,
    type: "sync_ack",
    syncId: typeof syncId === "string" ? syncId : "",
    status: status
  }
}

function accepted(type, extra) {
  const payload = { ok: true, accepted: type }
  if (extra) {
    Object.keys(extra).forEach(function (key) {
      payload[key] = extra[key]
    })
  }
  return { statusCode: 200, payload: payload }
}

function rejected(statusCode, status, syncId, useAck) {
  const payload = { ok: false }
  if (useAck) payload.ack = ack(syncId, status)
  else payload.error = status
  return { statusCode: statusCode, payload: payload }
}

function recordSummary(document) {
  const sport = document && document.sport
  const health = document && document.health
  return {
    steps: sport && Number.isFinite(Number(sport.steps))
      ? Number(sport.steps) : 0,
    caloriesKcal: sport && Number.isFinite(Number(sport.caloriesKcal))
      ? Number(sport.caloriesKcal) : 0,
    activeMinutes: sport && Number.isFinite(Number(sport.activeMinutes))
      ? Number(sport.activeMinutes) : 0,
    heartRateCount: health && Array.isArray(health.heartRate)
      ? health.heartRate.length : 0,
    bloodPressureCount: health && Array.isArray(health.bloodPressure)
      ? health.bloodPressure.length : 0
  }
}

function createSyncReceiver(options) {
  const settings = options || {}
  const outputDirectory = path.resolve(
    settings.outputDirectory ||
      process.env.SYNC_RECEIVER_OUTPUT ||
      path.join(__dirname, "..", "..", "tmp", "daily-sync-receiver")
  )
  const controlStatePath = path.resolve(
    settings.controlStatePath ||
      process.env.SYNC_RECEIVER_CONTROL_STATE ||
      path.join(outputDirectory, "control-state.json")
  )
  const transfers = new Map()
  const controlRequests = new Map()
  let latestResult = null
  let lastControlPollAt = 0
  let nextRequestId = Date.now()

  function transferKey(identity, syncId) {
    return identity.deviceId + "\u0000" + syncId
  }

  function removeExpiredTransfers() {
    const cutoff = Date.now() - TRANSFER_TTL_MS
    transfers.forEach(function (transfer, key) {
      if (transfer.startedAt < cutoff) transfers.delete(key)
    })
  }

  function deviceTransferCount(deviceId) {
    let count = 0
    transfers.forEach(function (transfer) {
      if (transfer.deviceId === deviceId) count += 1
    })
    return count
  }

  function validateProtocolFrame(frame) {
    return !!(
      frame &&
      typeof frame === "object" &&
      !Array.isArray(frame) &&
      frame.protocol === PROTOCOL &&
      Number(frame.version) === VERSION &&
      typeof frame.type === "string"
    )
  }

  function handleBegin(frame) {
    const totalChunks = Number(frame.totalChunks)
    const totalCharacters = Number(frame.totalCharacters)
    const checksum = String(frame.checksum || "").toLowerCase()
    const identity = identityFromSyncId(frame.syncId, frame.date)

    if (
      !identity ||
      !Number.isInteger(totalChunks) ||
      totalChunks < 1 ||
      totalChunks > MAX_CHUNKS ||
      !Number.isInteger(totalCharacters) ||
      totalCharacters < 0 ||
      totalCharacters > MAX_TRANSFER_CHARACTERS ||
      totalCharacters > totalChunks * MAX_CHUNK_CHARACTERS ||
      !/^[0-9a-f]{8}$/.test(checksum)
    ) {
      return rejected(400, "invalid_begin", frame.syncId, false)
    }

    removeExpiredTransfers()
    const key = transferKey(identity, frame.syncId)
    const existing = transfers.get(key)
    if (existing) {
      if (
        existing.date === frame.date &&
        existing.totalChunks === totalChunks &&
        existing.totalCharacters === totalCharacters &&
        existing.checksum === checksum
      ) {
        return accepted("sync_begin", {
          syncId: frame.syncId,
          duplicate: true
        })
      }
      transfers.delete(key)
      return rejected(409, "conflicting_begin", frame.syncId, false)
    }

    if (
      transfers.size >= MAX_PENDING_TRANSFERS ||
      deviceTransferCount(identity.deviceId) >= MAX_PENDING_PER_DEVICE
    ) {
      return rejected(429, "resource_limit", frame.syncId, false)
    }

    transfers.set(key, {
      syncId: frame.syncId,
      deviceId: identity.deviceId,
      date: frame.date,
      totalChunks: totalChunks,
      totalCharacters: totalCharacters,
      checksum: checksum,
      chunks: new Array(totalChunks),
      receivedCharacters: 0,
      startedAt: Date.now()
    })
    return accepted("sync_begin", { syncId: frame.syncId })
  }

  function locateTransfer(syncId) {
    if (typeof syncId !== "string") return null
    let found = null
    transfers.forEach(function (transfer, key) {
      if (!found && transfer.syncId === syncId) {
        found = { key: key, transfer: transfer }
      }
    })
    return found
  }

  function handleChunk(frame) {
    removeExpiredTransfers()
    const located = locateTransfer(frame.syncId)
    const index = Number(frame.index)
    if (
      !located ||
      !Number.isInteger(index) ||
      index < 0 ||
      index >= located.transfer.totalChunks ||
      typeof frame.data !== "string" ||
      frame.data.length > MAX_CHUNK_CHARACTERS
    ) {
      return rejected(409, "invalid_chunk", frame.syncId, false)
    }

    const transfer = located.transfer
    const previous = transfer.chunks[index]
    if (typeof previous === "string") {
      if (previous === frame.data) {
        return accepted("sync_chunk", { index: index, duplicate: true })
      }
      transfers.delete(located.key)
      return rejected(409, "conflicting_chunk", frame.syncId, false)
    }

    if (transfer.receivedCharacters + frame.data.length > transfer.totalCharacters) {
      transfers.delete(located.key)
      return rejected(409, "invalid_chunk", frame.syncId, false)
    }
    transfer.chunks[index] = frame.data
    transfer.receivedCharacters += frame.data.length
    return accepted("sync_chunk", { index: index })
  }

  function recordFilename(deviceId, date) {
    const requestId = simulationId(date)
    if (requestId) {
      return path.join(
        outputDirectory,
        deviceId,
        "simulation",
        "simulation-" + requestId + ".json"
      )
    }
    return path.join(outputDirectory, deviceId, compactDate(date) + ".json")
  }

  function rememberResult(transfer, status, filename, document) {
    latestResult = {
      deviceId: transfer.deviceId,
      syncId: transfer.syncId,
      date: transfer.date,
      status: status,
      filename: filename ? path.relative(outputDirectory, filename) : "",
      receivedAt: Date.now(),
      summary: document ? recordSummary(document) : null
    }
  }

  function writeNewRecord(filename, text) {
    fs.mkdirSync(path.dirname(filename), { recursive: true })
    const temporary = filename + "." + process.pid + "." + Date.now() + ".tmp"
    try {
      fs.writeFileSync(temporary, text, { encoding: "utf8", flag: "wx" })
      fs.linkSync(temporary, filename)
    } finally {
      try {
        fs.unlinkSync(temporary)
      } catch (error) {
        if (error && error.code !== "ENOENT") throw error
      }
    }
  }

  function persistRecord(transfer, text, document) {
    const filename = recordFilename(transfer.deviceId, transfer.date)
    if (fs.existsSync(filename)) {
      const existing = fs.readFileSync(filename, "utf8")
      if (existing === text) {
        return { status: "duplicate", filename: filename }
      }

      let existingDocument = null
      try {
        existingDocument = JSON.parse(existing)
      } catch (error) {
        /* A damaged existing record must never be overwritten. */
      }
      return {
        status: existingDocument && existingDocument.syncId === document.syncId
          ? "sync_id_conflict" : "date_conflict",
        filename: filename
      }
    }

    try {
      writeNewRecord(filename, text)
      return { status: "ok", filename: filename }
    } catch (error) {
      if (error && error.code === "EEXIST") {
        const existing = fs.readFileSync(filename, "utf8")
        if (existing === text) {
          return { status: "duplicate", filename: filename }
        }
        return { status: "date_conflict", filename: filename }
      }
      throw error
    }
  }

  function handleCommit(frame) {
    removeExpiredTransfers()
    const located = locateTransfer(frame.syncId)
    if (!located) {
      return rejected(409, "incomplete", frame.syncId, true)
    }

    const transfer = located.transfer
    transfers.delete(located.key)
    for (let index = 0; index < transfer.chunks.length; index += 1) {
      if (typeof transfer.chunks[index] !== "string") {
        rememberResult(transfer, "incomplete", "", null)
        return rejected(409, "incomplete", frame.syncId, true)
      }
    }

    const text = transfer.chunks.join("")
    if (
      text.length !== transfer.totalCharacters ||
      fnv1a(text) !== transfer.checksum
    ) {
      rememberResult(transfer, "checksum_failed", "", null)
      return rejected(422, "checksum_failed", frame.syncId, true)
    }

    let document
    try {
      document = JSON.parse(text)
    } catch (error) {
      rememberResult(transfer, "payload_mismatch", "", null)
      return rejected(422, "payload_mismatch", frame.syncId, true)
    }

    const expectedSyncId =
      document.deviceId + ":" + document.date + ":v" + VERSION
    const requestId = simulationId(document.date)
    const simulation = document.simulation
    const simulationValid = requestId
      ? !!(
          simulation &&
          simulation.enabled === true &&
          Number.isSafeInteger(Number(simulation.requestId)) &&
          String(Number(simulation.requestId)) === requestId &&
          validDate(simulation.sourceDate)
        )
      : !simulation
    if (
      document.schemaVersion !== VERSION ||
      document.messageType !== "daily_activity_health" ||
      document.syncId !== transfer.syncId ||
      document.date !== transfer.date ||
      document.deviceId !== transfer.deviceId ||
      document.syncId !== expectedSyncId ||
      !simulationValid ||
      !document.sport ||
      typeof document.sport !== "object" ||
      !document.health ||
      typeof document.health !== "object"
    ) {
      rememberResult(transfer, "payload_mismatch", "", null)
      return rejected(422, "payload_mismatch", frame.syncId, true)
    }

    let result
    try {
      result = persistRecord(transfer, text, document)
    } catch (error) {
      console.error(
        new Date().toISOString(),
        "sync record save failed",
        transfer.deviceId,
        transfer.date,
        error && error.code ? error.code : "unknown"
      )
      rememberResult(transfer, "save_failed", "", document)
      return rejected(500, "save_failed", frame.syncId, true)
    }

    rememberResult(transfer, result.status, result.filename, document)
    if (result.status === "ok" || result.status === "duplicate") {
      console.log(
        new Date().toISOString(),
        "sync " + result.status,
        transfer.deviceId,
        transfer.date
      )
      return {
        statusCode: 200,
        payload: { ok: true, ack: ack(frame.syncId, result.status) }
      }
    }
    return rejected(409, result.status, frame.syncId, true)
  }

  function handleFrame(frame) {
    if (!validateProtocolFrame(frame)) {
      return rejected(400, "unsupported_protocol", frame && frame.syncId, false)
    }
    if (frame.type === "sync_begin") return handleBegin(frame)
    if (frame.type === "sync_chunk") return handleChunk(frame)
    if (frame.type === "sync_commit") return handleCommit(frame)
    return rejected(400, "unknown_frame", frame.syncId, false)
  }

  function listRecords(deviceFilter) {
    const records = []
    let truncated = false
    if (!fs.existsSync(outputDirectory)) return { records: records, truncated: false }

    const devices = fs.readdirSync(outputDirectory, { withFileTypes: true })
    for (let deviceIndex = 0; deviceIndex < devices.length; deviceIndex += 1) {
      const deviceEntry = devices[deviceIndex]
      if (!deviceEntry.isDirectory() || !validDeviceId(deviceEntry.name)) continue
      if (deviceFilter && deviceEntry.name !== deviceFilter) continue
      const deviceDirectory = path.join(outputDirectory, deviceEntry.name)
      const files = fs.readdirSync(deviceDirectory, { withFileTypes: true })
      for (let fileIndex = 0; fileIndex < files.length; fileIndex += 1) {
        const fileEntry = files[fileIndex]
        if (!fileEntry.isFile() || !/^\d{8}\.json$/.test(fileEntry.name)) continue
        if (records.length >= MAX_RECORD_RESULTS) {
          truncated = true
          break
        }
        const filename = path.join(deviceDirectory, fileEntry.name)
        const stat = fs.statSync(filename)
        records.push({
          deviceId: deviceEntry.name,
          date: fileEntry.name.slice(0, 8),
          name: deviceEntry.name + "/" + fileEntry.name,
          bytes: stat.size,
          receivedAt: stat.mtimeMs
        })
      }
      if (truncated) break
    }
    records.sort(function (left, right) {
      return right.receivedAt - left.receivedAt
    })
    return { records: records, truncated: truncated }
  }

  function boundedControlSet(requests, key, value) {
    if (!requests.has(key) && requests.size >= MAX_CONTROL_TARGETS) {
      const oldest = requests.keys().next()
      if (!oldest.done) requests.delete(oldest.value)
    }
    requests.delete(key)
    requests.set(key, value)
  }

  function serializeControlState(requests, identifier) {
    const entries = []
    requests.forEach(function (request) {
      entries.push({
        deviceId: request.deviceId,
        id: request.id,
        requestedAt: request.requestedAt
      })
    })
    return JSON.stringify({
      schemaVersion: 1,
      nextRequestId: identifier,
      requests: entries
    })
  }

  function saveControlState(requests, identifier) {
    const directory = path.dirname(controlStatePath)
    const temporary =
      controlStatePath + "." + process.pid + "." + Date.now() + ".tmp"
    const text = serializeControlState(requests, identifier)

    fs.mkdirSync(directory, { recursive: true })
    try {
      fs.writeFileSync(temporary, text, {
        encoding: "utf8",
        flag: "wx",
        mode: 0o600
      })
      fs.renameSync(temporary, controlStatePath)
    } finally {
      try {
        fs.unlinkSync(temporary)
      } catch (error) {
        if (error && error.code !== "ENOENT") throw error
      }
    }
  }

  function loadControlState() {
    if (!fs.existsSync(controlStatePath)) return

    try {
      const stat = fs.lstatSync(controlStatePath)
      if (!stat.isFile() || stat.size > MAX_CONTROL_STATE_BYTES) {
        throw new Error("invalid control-state file")
      }
      const parsed = JSON.parse(fs.readFileSync(controlStatePath, "utf8"))
      if (
        !parsed ||
        parsed.schemaVersion !== 1 ||
        !Number.isSafeInteger(parsed.nextRequestId) ||
        parsed.nextRequestId < 1 ||
        !Array.isArray(parsed.requests) ||
        parsed.requests.length > MAX_CONTROL_TARGETS
      ) {
        throw new Error("invalid control-state contents")
      }

      const restored = new Map()
      let greatestIdentifier = parsed.nextRequestId
      parsed.requests.forEach(function (request) {
        const target = request && request.deviceId
        if (
          (target !== "*" && !validDeviceId(target)) ||
          !Number.isSafeInteger(request.id) ||
          request.id < 1 ||
          !Number.isSafeInteger(request.requestedAt) ||
          request.requestedAt < 1
        ) {
          throw new Error("invalid persisted control request")
        }
        const previous = restored.get(target)
        if (!previous || request.id > previous.id) {
          boundedControlSet(restored, target, {
            deviceId: target,
            id: request.id,
            requestedAt: request.requestedAt
          })
        }
        if (request.id > greatestIdentifier) greatestIdentifier = request.id
      })

      controlRequests.clear()
      restored.forEach(function (request, target) {
        controlRequests.set(target, request)
      })
      nextRequestId = Math.max(Date.now(), greatestIdentifier)
    } catch (error) {
      console.error(
        new Date().toISOString(),
        "sync control state ignored:",
        error && error.message ? error.message : "unknown"
      )
    }
  }

  function controlFor(deviceId) {
    const directed = controlRequests.get(deviceId)
    const globalRequest = controlRequests.get("*")
    if (directed && globalRequest) {
      if (directed.id !== globalRequest.id) {
        return directed.id > globalRequest.id ? directed : globalRequest
      }
      return directed.requestedAt >= globalRequest.requestedAt
        ? directed : globalRequest
    }
    return directed || globalRequest || {
      id: 0,
      requestedAt: 0,
      deviceId: deviceId || "*"
    }
  }

  async function handle(request, response, url) {
    if (!url || url.pathname.indexOf("/api/sync/") !== 0) return false

    if (request.method === "OPTIONS") {
      sendJson(response, 204, {})
      return true
    }

    try {
      removeExpiredTransfers()
      if (request.method === "GET" && url.pathname === "/api/sync/status") {
        const listed = listRecords("")
        sendJson(response, 200, {
          ok: true,
          mode: "wifi",
          protocol: PROTOCOL,
          version: VERSION,
          pendingTransfers: transfers.size,
          recordCount: listed.records.length,
          recordsTruncated: listed.truncated,
          lastControlPollAt: lastControlPollAt,
          latest: latestResult,
          limits: {
            transferTtlMs: TRANSFER_TTL_MS,
            maxPendingTransfers: MAX_PENDING_TRANSFERS,
            maxPendingPerDevice: MAX_PENDING_PER_DEVICE,
            maxChunks: MAX_CHUNKS,
            maxChunkCharacters: MAX_CHUNK_CHARACTERS
          }
        })
        return true
      }

      if (request.method === "GET" && url.pathname === "/api/sync/records") {
        const deviceId = String(url.searchParams.get("deviceId") || "")
        if (deviceId && !validDeviceId(deviceId)) {
          sendJson(response, 400, { ok: false, error: "invalid_device_id" })
          return true
        }
        const listed = listRecords(deviceId)
        sendJson(response, 200, {
          ok: true,
          outputDirectory: outputDirectory,
          records: listed.records,
          truncated: listed.truncated
        })
        return true
      }

      if (request.method === "GET" && url.pathname === "/api/sync/control") {
        const deviceId = String(url.searchParams.get("deviceId") || "")
        if (deviceId && !validDeviceId(deviceId)) {
          sendJson(response, 400, { ok: false, error: "invalid_device_id" })
          return true
        }
        lastControlPollAt = Date.now()
        const control = controlFor(deviceId)
        sendJson(response, 200, {
          ok: true,
          requestId: control.id,
          requestedAt: control.requestedAt,
          deviceId: control.deviceId
        })
        return true
      }

      if (request.method === "POST" && url.pathname === "/api/sync/request") {
        const body = await readJsonBody(request)
        const requestedDeviceId = String(body.deviceId || "")
        if (requestedDeviceId && !validDeviceId(requestedDeviceId)) {
          sendJson(response, 400, { ok: false, error: "invalid_device_id" })
          return true
        }
        const candidateIdentifier = Math.max(nextRequestId, Date.now()) + 1
        const target = requestedDeviceId || "*"
        const control = {
          id: candidateIdentifier,
          requestedAt: Date.now(),
          deviceId: target
        }
        const candidateRequests = new Map(controlRequests)
        boundedControlSet(candidateRequests, target, control)
        saveControlState(candidateRequests, candidateIdentifier)
        controlRequests.clear()
        candidateRequests.forEach(function (request, deviceId) {
          controlRequests.set(deviceId, request)
        })
        nextRequestId = candidateIdentifier
        sendJson(response, 200, {
          ok: true,
          message: "已请求设备发送数据",
          requestId: control.id,
          deviceId: target
        })
        return true
      }

      if (request.method === "POST" && url.pathname === "/api/sync/frame") {
        const frame = await readJsonBody(request)
        const result = handleFrame(frame)
        sendJson(response, result.statusCode, result.payload)
        return true
      }

      sendJson(response, 404, { ok: false, error: "not_found" })
      return true
    } catch (error) {
      console.error(
        new Date().toISOString(),
        "sync receiver request failed",
        error && error.message ? error.message : "unknown"
      )
      sendJson(response, error && error.isBodyError ? 400 : 500, {
        ok: false,
        error: error && error.isBodyError ? error.message : "receiver_failed"
      })
      return true
    }
  }

  loadControlState()

  const cleanupTimer = setInterval(removeExpiredTransfers, 15 * 1000)
  cleanupTimer.unref()

  return {
    handle: handle,
    handleFrame: handleFrame,
    outputDirectory: outputDirectory,
    controlStatePath: controlStatePath,
    close: function () {
      clearInterval(cleanupTimer)
    }
  }
}

module.exports = {
  createSyncReceiver: createSyncReceiver,
  fnv1a: fnv1a
}

if (require.main === module) {
  const host = process.env.SYNC_RECEIVER_HOST || "0.0.0.0"
  const port = numericPort(process.env.SYNC_RECEIVER_PORT, 8792)
  const receiver = createSyncReceiver()
  const server = http.createServer(async function (request, response) {
    const url = new URL(request.url, "http://localhost")
    if (await receiver.handle(request, response, url)) return
    sendJson(response, 404, { ok: false, error: "not_found" })
  })
  server.on("error", function (error) {
    console.error(
      "Sync receiver failed to listen: " +
      (error && error.code ? error.code : "unknown error")
    )
    process.exitCode = 1
  })
  server.listen(port, host, function () {
    console.log("Daily sync receiver listening on port " + port)
    console.log("Sync records: " + receiver.outputDirectory)
  })
}
