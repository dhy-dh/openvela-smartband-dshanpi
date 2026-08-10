# openvela weather proxy

This is a Node.js 12 compatible HTTP proxy for the smart-band weather
protocol. It uses only Node's built-in `http`, `https`, `crypto`, and `fs`
modules. The QWeather private key is never stored in this directory: the
server reads it only from the path supplied through
`QWEATHER_PRIVATE_KEY_PATH`.

Required environment variables:

- `QWEATHER_API_HOST`: the HTTPS API host assigned by QWeather.
- `QWEATHER_PROJECT_ID`: the QWeather project identifier.
- `QWEATHER_CREDENTIAL_ID`: the QWeather credential identifier.
- `QWEATHER_PRIVATE_KEY_PATH`: an existing private-key file outside this
  directory.

Optional environment variables:

- `WEATHER_SERVER_PORT`: listening port, default `8790`.
- `WEATHER_ADMINISTRATIVE_DATA_PATH`: optional level-three administrative
  division CSV used to enrich display names.
- `SYNC_RECEIVER_OUTPUT`: health/activity record directory. The default is
  `../../tmp/daily-sync-receiver` relative to this directory.
- `SYNC_RECEIVER_CONTROL_STATE`: optional control-request state path. The
  default is `control-state.json` inside `SYNC_RECEIVER_OUTPUT`.

Run:

```bash
cd /path/to/openvela-contest/tools/weather_proxy
export QWEATHER_API_HOST='<assigned-host>'
export QWEATHER_PROJECT_ID='<project-id>'
export QWEATHER_CREDENTIAL_ID='<credential-id>'
export QWEATHER_PRIVATE_KEY_PATH='/absolute/path/to/private-key.pem'
node server.js
```

Endpoints:

- `GET /health`
- `GET /api/weather?location=<name>&adm=<administrative-name>`
- `GET /api/weather?locationId=<nine-digit-id>&location=<name>&adm=<name>`
- `POST /api/sync/frame`: accept `sync_begin`, up to 512 `sync_chunk`
  frames (768 characters each), and `sync_commit`.
- `GET /api/sync/status`: receiver status and the latest commit result.
- `GET /api/sync/records?deviceId=<id>`: stored-record metadata only.
- `GET /api/sync/control?deviceId=<id>`: device polling endpoint.
- `POST /api/sync/request`: request a global snapshot, or send JSON such as
  `{"deviceId":"vela-main"}` to target one device.

The API response matches the desktop reference protocol and includes
`location`, `now`, and a three-item `forecast` array. Requests to QWeather are
always HTTPS. The JWT and private key are never returned or logged.

Daily sync uses the same listening port and therefore the same Cloudflare
Tunnel as weather. Records are atomically stored as
`<SYNC_RECEIVER_OUTPUT>/<deviceId>/YYYYMMDD.json`. A record is acknowledged as
`duplicate` only when the existing file and the retransmitted JSON are exactly
the same; conflicting IDs or dates are never overwritten. Incomplete
transfers expire after ten minutes and the in-memory queue is bounded.
Explicit control-request snapshots whose date is `simulation-<requestId>` are
validated separately and stored under `<deviceId>/simulation/`; they never
replace formal daily records.
Global and per-device upload requests are atomically persisted with mode
`0600`. A proxy restart therefore does not lose a pending request, and a newer
global request takes precedence over an older per-device request.

The integrated service is the normal deployment. For receiver-only local
testing, run a separate process on port `8792` (or set
`SYNC_RECEIVER_PORT`/`SYNC_RECEIVER_HOST`):

```bash
npm run start:sync
```

The receiver itself is plain HTTP. When devices send health data over the
Internet, expose it only through the existing HTTPS Tunnel and do not publish
the local HTTP port directly.

Check syntax and run a local smoke test against a running proxy:

```bash
npm run check
npm run test:sync
npm run smoke
```
