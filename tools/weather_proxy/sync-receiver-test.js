"use strict"

const assert = require("assert")
const fs = require("fs")
const os = require("os")
const path = require("path")
const stream = require("stream")
const { URL } = require("url")
const { createSyncReceiver } = require("./sync-receiver")

function invoke(receiver, method, pathname, body) {
  const request = new stream.PassThrough()
  request.method = method
  const result = {
    statusCode: 0,
    headers: null,
    body: ""
  }
  const response = {
    writeHead: function (statusCode, headers) {
      result.statusCode = statusCode
      result.headers = headers
    },
    end: function (text) {
      result.body = text || ""
    }
  }
  const handled = receiver.handle(
    request,
    response,
    new URL(pathname, "http://localhost")
  )
  request.end(body === undefined ? undefined : JSON.stringify(body))
  return handled.then(function (wasHandled) {
    assert.strictEqual(wasHandled, true)
    result.json = result.body ? JSON.parse(result.body) : null
    return result
  })
}

async function main() {
  const temporary = fs.mkdtempSync(
    path.join(os.tmpdir(), "openvela-sync-control-")
  )
  const outputDirectory = path.join(temporary, "records")
  const controlStatePath = path.join(temporary, "control-state.json")
  let receiver = createSyncReceiver({
    outputDirectory: outputDirectory,
    controlStatePath: controlStatePath
  })

  try {
    let response = await invoke(
      receiver,
      "POST",
      "/api/sync/request",
      { deviceId: "vela-main" }
    )
    assert.strictEqual(response.statusCode, 200)
    const directedId = response.json.requestId
    assert.ok(Number.isSafeInteger(directedId))
    assert.ok(fs.existsSync(controlStatePath))
    assert.strictEqual(fs.statSync(controlStatePath).mode & 0o777, 0o600)

    receiver.close()
    receiver = createSyncReceiver({
      outputDirectory: outputDirectory,
      controlStatePath: controlStatePath
    })
    response = await invoke(
      receiver,
      "GET",
      "/api/sync/control?deviceId=vela-main"
    )
    assert.strictEqual(response.json.requestId, directedId)
    assert.strictEqual(response.json.deviceId, "vela-main")

    response = await invoke(receiver, "POST", "/api/sync/request", {})
    assert.strictEqual(response.statusCode, 200)
    const globalId = response.json.requestId
    assert.ok(globalId > directedId)

    response = await invoke(
      receiver,
      "GET",
      "/api/sync/control?deviceId=vela-main"
    )
    assert.strictEqual(response.json.requestId, globalId)
    assert.strictEqual(response.json.deviceId, "*")

    receiver.close()
    receiver = createSyncReceiver({
      outputDirectory: outputDirectory,
      controlStatePath: controlStatePath
    })
    response = await invoke(
      receiver,
      "GET",
      "/api/sync/control?deviceId=vela-main"
    )
    assert.strictEqual(response.json.requestId, globalId)
    assert.strictEqual(response.json.deviceId, "*")
  } finally {
    receiver.close()
    fs.rmdirSync(temporary, { recursive: true })
  }

  console.log("sync control persistence and request ordering: PASS")
}

main().catch(function (error) {
  console.error(error && error.stack ? error.stack : error)
  process.exitCode = 1
})
