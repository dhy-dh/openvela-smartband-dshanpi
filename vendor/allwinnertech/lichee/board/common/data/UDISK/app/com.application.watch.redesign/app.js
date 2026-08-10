export default function(global, globalThis, window, $app_exports$, $app_evaluate$) {
    var org_app_require = $app_require$;
    (function(global, globalThis, window, $app_exports$, $app_evaluate$) {
        var setTimeout = global.setTimeout;
        var setInterval = global.setInterval;
        var clearTimeout = global.clearTimeout;
        var clearInterval = global.clearInterval;
        var $app_require$1 = global.$app_require$ || org_app_require;
        var createAppHandler = function() {
            return (()=>{
                var __webpack_modules__ = {
                    "./src/common/assistant-service.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports["default"] = void 0;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.app"));
                        var _system2 = _interopRequireDefault($app_require$1("@app-module/system.fetch"));
                        var _system3 = _interopRequireDefault($app_require$1("@app-module/system.record"));
                        var _system4 = _interopRequireDefault($app_require$1("@app-module/system.uploadtask"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const PROTOCOL_NAME = "smart-band-assistant";
                        const PROTOCOL_VERSION = 1;
                        const ASSISTANT_SERVER_URL = "http://10.0.2.2:8793";
                        const STATE_POLL_MS = 800;
                        const STATE_TIMEOUT_MS = 90000;
                        const RECORD_DURATION_MS = 15000;
                        const RECORD_FINALIZE_TIMEOUT_MS = 4000;
                        function createSessionId() {
                            return "vela-" + Date.now().toString(36) + "-" + Math.floor(1679616 * Math.random()).toString(36);
                        }
                        function copyState(source) {
                            const value = source || {};
                            return {
                                phase: value.phase || "idle",
                                transcript: value.transcript || "",
                                answer: value.answer || "",
                                detail: value.detail || "",
                                progress: Math.max(0, Math.min(100, Number(value.progress) || 0)),
                                updatedAt: Number(value.updatedAt) || 0,
                                simulated: !!value.simulated
                            };
                        }
                        function normalizeResponse(response) {
                            let data = response && response.data;
                            if ("string" == typeof data) {
                                try {
                                    data = JSON.parse(data);
                                } catch (error) {
                                    data = null;
                                }
                            }
                            return data && "object" == typeof data ? data : null;
                        }
                        const assistantService = {
                            state: copyState(),
                            listeners: [],
                            recordingUri: "",
                            recordingStopRequested: false,
                            recordingCancelRequested: false,
                            recordFinalizeTimerId: null,
                            sessionId: "",
                            uploadTask: null,
                            pollTimerId: null,
                            pollStartedAt: 0,
                            subscribe (callback) {
                                if ("function" != typeof callback) return;
                                if (this.listeners.indexOf(callback) < 0) this.listeners.push(callback);
                                callback(copyState(this.state));
                            },
                            unsubscribe (callback) {
                                const next = [];
                                for(let index = 0; index < this.listeners.length; index += 1)if (this.listeners[index] !== callback) next.push(this.listeners[index]);
                                this.listeners = next;
                            },
                            notify () {
                                const snapshot = copyState(this.state);
                                const callbacks = this.listeners.slice();
                                for(let index = 0; index < callbacks.length; index += 1){
                                    try {
                                        callbacks[index](snapshot);
                                    } catch (error) {
                                        console.log("assistant listener failed", error);
                                    }
                                }
                            },
                            update (next) {
                                this.state = copyState(next);
                                this.notify();
                            },
                            isBusy () {
                                return "recording" === this.state.phase || "finalizing" === this.state.phase || "uploading" === this.state.phase || "transcribing" === this.state.phase || "thinking" === this.state.phase;
                            },
                            toggleRecording () {
                                if ("recording" === this.state.phase) return void this.stopRecording();
                                if (this.isBusy()) return;
                                this.startRecording();
                            },
                            canRecord () {
                                try {
                                    if ("function" == typeof _system.default.canIUse) return !!_system.default.canIUse("@system.record.start");
                                } catch (error) {
                                    console.log("assistant record capability check failed", error);
                                }
                                return true;
                            },
                            submitText (text) {
                                const prompt = String(text || "").trim();
                                if (!prompt || this.isBusy()) return false;
                                this.cancelPolling();
                                this.resetRecordingState();
                                this.sessionId = createSessionId();
                                this.update({
                                    phase: "thinking",
                                    transcript: prompt,
                                    answer: "",
                                    detail: "正在处理文字问题",
                                    progress: 100,
                                    updatedAt: Date.now(),
                                    simulated: false
                                });
                                try {
                                    _system2.default.fetch({
                                        url: ASSISTANT_SERVER_URL + "/api/assistant/query",
                                        method: "POST",
                                        header: {
                                            "Content-Type": "application/json"
                                        },
                                        data: JSON.stringify({
                                            sessionId: this.sessionId,
                                            transcript: prompt
                                        }),
                                        responseType: "json",
                                        success: (response)=>{
                                            const body = normalizeResponse(response);
                                            if (!response || response.code < 200 || response.code >= 300 || !body) return void this.fail("文字请求被服务拒绝，请检查 AI 助手服务");
                                            this.startPolling();
                                        },
                                        fail: (data, code)=>{
                                            this.fail("无法连接 AI 助手服务（" + (void 0 === code ? "未知" : code) + "）");
                                        }
                                    });
                                    return true;
                                } catch (error) {
                                    this.fail("无法发起文字请求");
                                    return false;
                                }
                            },
                            startRecording () {
                                if (!this.canRecord()) return void this.fail("当前设备不支持录音，请使用文字输入");
                                this.cancelPolling();
                                this.sessionId = createSessionId();
                                this.resetRecordingState();
                                this.update({
                                    phase: "recording",
                                    transcript: "",
                                    answer: "",
                                    detail: "正在使用开发板麦克风录音，再点一次结束",
                                    progress: 0,
                                    updatedAt: Date.now(),
                                    simulated: false
                                });
                                try {
                                    _system3.default.start({
                                        duration: RECORD_DURATION_MS,
                                        sampleRate: 16000,
                                        numberOfChannels: 1,
                                        encodeBitRate: 256000,
                                        format: "wav",
                                        success: (data)=>{
                                            this.recordingUri = data && data.uri ? data.uri : "";
                                            if (this.recordingCancelRequested) return void this.resetRecordingState();
                                            if (!this.recordingUri) return void this.fail("录音接口没有返回文件，请使用文字输入");
                                            this.clearRecordFinalizeTimer();
                                            this.uploadRecording();
                                        },
                                        fail: (data, code)=>{
                                            this.fail("录音接口不可用（" + (void 0 === code ? "未知" : code) + "），请使用文字输入");
                                        }
                                    });
                                } catch (error) {
                                    this.fail("当前 Vela 镜像不支持录音，请使用文字输入");
                                }
                            },
                            stopRecording () {
                                if ("recording" !== this.state.phase || this.recordingStopRequested) return;
                                this.recordingStopRequested = true;
                                this.update({
                                    phase: "finalizing",
                                    transcript: "",
                                    answer: "",
                                    detail: "正在生成录音文件",
                                    progress: 0,
                                    updatedAt: Date.now(),
                                    simulated: false
                                });
                                try {
                                    _system3.default.stop();
                                } catch (error) {
                                    this.fail("停止录音失败，请使用文字输入");
                                    return;
                                }
                                this.clearRecordFinalizeTimer();
                                this.recordFinalizeTimerId = setTimeout(()=>{
                                    this.recordFinalizeTimerId = null;
                                    if ("finalizing" === this.state.phase) this.fail("录音文件生成超时，请使用文字输入");
                                }, RECORD_FINALIZE_TIMEOUT_MS);
                            },
                            cancelRecording () {
                                if ("recording" !== this.state.phase && "finalizing" !== this.state.phase) return;
                                this.recordingCancelRequested = true;
                                this.clearRecordFinalizeTimer();
                                try {
                                    _system3.default.stop();
                                } catch (error) {
                                    console.log("assistant recording cancel failed", error);
                                }
                                this.recordingUri = "";
                                this.recordingStopRequested = false;
                                this.uploadTask = null;
                                this.update({
                                    phase: "idle",
                                    transcript: this.state.transcript,
                                    answer: this.state.answer,
                                    detail: "",
                                    progress: 0,
                                    updatedAt: Date.now(),
                                    simulated: false
                                });
                            },
                            uploadRecording () {
                                if (!this.recordingUri || this.recordingCancelRequested) return;
                                this.update({
                                    phase: "uploading",
                                    transcript: "",
                                    answer: "",
                                    detail: "正在上传录音",
                                    progress: 0,
                                    updatedAt: Date.now(),
                                    simulated: false
                                });
                                try {
                                    this.uploadTask = _system4.default.uploadFile({
                                        url: ASSISTANT_SERVER_URL + "/api/assistant/audio",
                                        filePath: this.recordingUri,
                                        name: "audio",
                                        formData: {
                                            sessionId: this.sessionId
                                        },
                                        timeout: 60000,
                                        success: (response)=>{
                                            const statusCode = Number(response && response.statusCode) || 0;
                                            if (statusCode < 200 || statusCode >= 300) return void this.fail("录音上传被服务拒绝（HTTP " + statusCode + "）");
                                            this.update({
                                                phase: "transcribing",
                                                transcript: "",
                                                answer: "",
                                                detail: "正在把语音转成文字",
                                                progress: 100,
                                                updatedAt: Date.now(),
                                                simulated: false
                                            });
                                            this.startPolling();
                                        },
                                        fail: (data, code)=>{
                                            this.fail("录音上传失败（" + (void 0 === code ? "未知" : code) + "）");
                                        }
                                    });
                                    if (this.uploadTask && this.uploadTask.onProgressUpdate) this.uploadTask.onProgressUpdate((progress)=>{
                                        if ("uploading" !== this.state.phase) return;
                                        this.state.progress = Math.max(0, Math.min(100, Number(progress && progress.progress) || 0));
                                        this.notify();
                                    });
                                } catch (error) {
                                    this.fail("当前 Vela 镜像不支持文件上传，可使用模拟语音");
                                }
                            },
                            simulateVoice () {
                                if (this.isBusy()) return;
                                this.cancelPolling();
                                this.sessionId = createSessionId();
                                this.update({
                                    phase: "transcribing",
                                    transcript: "",
                                    answer: "",
                                    detail: "正在执行本地模拟语音转写",
                                    progress: 0,
                                    updatedAt: Date.now(),
                                    simulated: true
                                });
                                try {
                                    _system2.default.fetch({
                                        url: ASSISTANT_SERVER_URL + "/api/assistant/simulate",
                                        method: "POST",
                                        header: {
                                            "Content-Type": "application/json"
                                        },
                                        data: JSON.stringify({
                                            sessionId: this.sessionId
                                        }),
                                        responseType: "json",
                                        success: (response)=>{
                                            const body = normalizeResponse(response);
                                            if (!response || response.code < 200 || response.code >= 300 || !body) return void this.fail("模拟语音请求被服务拒绝，请启动 demo 代理");
                                            this.startPolling();
                                        },
                                        fail: (data, code)=>{
                                            this.fail("无法连接 AI 助手服务（" + (void 0 === code ? "未知" : code) + "）");
                                        }
                                    });
                                } catch (error) {
                                    this.fail("无法发起模拟语音请求");
                                }
                            },
                            startPolling () {
                                this.cancelPolling();
                                this.pollStartedAt = Date.now();
                                this.pollState();
                            },
                            pollState () {
                                if (!this.sessionId) return;
                                if (Date.now() - this.pollStartedAt > STATE_TIMEOUT_MS) return void this.fail("语音转写或模型请求超时");
                                _system2.default.fetch({
                                    url: ASSISTANT_SERVER_URL + "/api/assistant/state?sessionId=" + encodeURIComponent(this.sessionId),
                                    method: "GET",
                                    responseType: "json",
                                    success: (response)=>{
                                        const body = normalizeResponse(response);
                                        if (!response || response.code < 200 || response.code >= 300 || !body) return void this.queuePoll();
                                        this.applyServerState(body);
                                        if ("answer" !== body.phase && "error" !== body.phase) this.queuePoll();
                                    },
                                    fail: ()=>this.queuePoll()
                                });
                            },
                            queuePoll () {
                                this.cancelPollTimer();
                                this.pollTimerId = setTimeout(()=>{
                                    this.pollTimerId = null;
                                    this.pollState();
                                }, STATE_POLL_MS);
                            },
                            applyServerState (body) {
                                this.update({
                                    phase: body.phase || "thinking",
                                    transcript: body.transcript || this.state.transcript,
                                    answer: body.answer || "",
                                    detail: body.detail || "",
                                    progress: 100,
                                    updatedAt: Number(body.updatedAt) || Date.now(),
                                    simulated: !!body.simulated || this.state.simulated
                                });
                                if ("answer" === body.phase || "error" === body.phase) this.cancelPolling();
                            },
                            handleIncoming (message) {
                                if (!message || message.protocol !== PROTOCOL_NAME || Number(message.version) !== PROTOCOL_VERSION || "assistant_state" !== message.type) return false;
                                this.cancelPolling();
                                this.update({
                                    phase: message.state || "idle",
                                    transcript: message.transcript || "",
                                    answer: message.answer || "",
                                    detail: "来自 Android companion",
                                    progress: 100,
                                    updatedAt: Number(message.updatedAt) || Date.now(),
                                    simulated: false
                                });
                                return true;
                            },
                            fail (detail) {
                                this.cancelPolling();
                                this.clearRecordFinalizeTimer();
                                this.update({
                                    phase: "error",
                                    transcript: this.state.transcript,
                                    answer: "",
                                    detail: detail || "AI 助手发生错误",
                                    progress: this.state.progress,
                                    updatedAt: Date.now(),
                                    simulated: this.state.simulated
                                });
                            },
                            cancelPollTimer () {
                                if (!this.pollTimerId) return;
                                clearTimeout(this.pollTimerId);
                                this.pollTimerId = null;
                            },
                            cancelPolling () {
                                this.cancelPollTimer();
                                this.pollStartedAt = 0;
                            },
                            clearRecordFinalizeTimer () {
                                if (!this.recordFinalizeTimerId) return;
                                clearTimeout(this.recordFinalizeTimerId);
                                this.recordFinalizeTimerId = null;
                            },
                            resetRecordingState () {
                                this.clearRecordFinalizeTimer();
                                this.recordingUri = "";
                                this.recordingStopRequested = false;
                                this.recordingCancelRequested = false;
                                this.uploadTask = null;
                            }
                        };
                        var _default = exports["default"] = assistantService;
                    },
                    "./src/common/charging-monitor.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports["default"] = void 0;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.battery"));
                        var _system2 = _interopRequireDefault($app_require$1("@app-module/system.event"));
                        var _system3 = _interopRequireDefault($app_require$1("@app-module/system.file"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const BATTERY_BRIDGE_URI = "/common/emulator-battery.json";
                        const BRIDGE_POLL_MS = 1000;
                        const SYSTEM_POLL_MS = 300000;
                        function chargingFromStatus(data) {
                            if (!data) return null;
                            if (void 0 !== data.charging) return !!data.charging;
                            if (void 0 !== data.isCharging) return !!data.isCharging;
                            if (void 0 === data.status) return null;
                            const normalized = String(data.status).toLowerCase();
                            if ("charging" === normalized || "full" === normalized) return true;
                            if ("discharging" === normalized || "not charging" === normalized || "not_charging" === normalized) return false;
                            return null;
                        }
                        const chargingMonitor = {
                            started: false,
                            initialized: false,
                            charging: false,
                            bridgeAuthoritative: false,
                            listener: null,
                            systemReading: false,
                            bridgeReading: false,
                            systemTimerId: null,
                            bridgeTimerId: null,
                            batteryEventId: null,
                            start (listener) {
                                if (listener) this.listener = listener;
                                if (this.started) return;
                                this.started = true;
                                this.refreshSystemBattery();
                                this.refreshBridgeBattery();
                                this.systemTimerId = setInterval(()=>this.refreshSystemBattery(), SYSTEM_POLL_MS);
                                this.bridgeTimerId = setInterval(()=>this.refreshBridgeBattery(), BRIDGE_POLL_MS);
                                this.subscribeBatteryEvent();
                            },
                            stop () {
                                this.started = false;
                                this.listener = null;
                                if (this.systemTimerId) clearInterval(this.systemTimerId);
                                if (this.bridgeTimerId) clearInterval(this.bridgeTimerId);
                                this.systemTimerId = null;
                                this.bridgeTimerId = null;
                                if (null !== this.batteryEventId) {
                                    try {
                                        _system2.default.unsubscribe({
                                            id: this.batteryEventId
                                        });
                                    } catch (error) {
                                        console.log("charging event unsubscribe failed", error);
                                    }
                                    this.batteryEventId = null;
                                }
                            },
                            resume () {
                                if (!this.started) return void this.start(this.listener);
                                this.systemReading = false;
                                this.bridgeReading = false;
                                if (this.systemTimerId) clearInterval(this.systemTimerId);
                                if (this.bridgeTimerId) clearInterval(this.bridgeTimerId);
                                this.systemTimerId = setInterval(()=>this.refreshSystemBattery(), SYSTEM_POLL_MS);
                                this.bridgeTimerId = setInterval(()=>this.refreshBridgeBattery(), BRIDGE_POLL_MS);
                                if (null !== this.batteryEventId) {
                                    try {
                                        _system2.default.unsubscribe({
                                            id: this.batteryEventId
                                        });
                                    } catch (error) {
                                        console.log("charging resume unsubscribe failed", error);
                                    }
                                    this.batteryEventId = null;
                                }
                                this.subscribeBatteryEvent();
                                this.refreshSystemBattery();
                                this.refreshBridgeBattery();
                            },
                            applyStatus (charging, source) {
                                if (null == charging) return;
                                const nextCharging = !!charging;
                                if ("system" === source && this.bridgeAuthoritative) return;
                                if ("bridge" === source && !this.bridgeAuthoritative) {
                                    this.bridgeAuthoritative = true;
                                    this.initialized = true;
                                    this.charging = nextCharging;
                                    return;
                                }
                                if (!this.initialized) {
                                    this.initialized = true;
                                    this.charging = nextCharging;
                                    return;
                                }
                                const startedCharging = !this.charging && nextCharging;
                                this.charging = nextCharging;
                                if (!startedCharging || !this.listener) return;
                                try {
                                    this.listener();
                                } catch (error) {
                                    console.log("charging listener failed", error);
                                }
                            },
                            refreshSystemBattery () {
                                if (!this.started || this.systemReading || this.bridgeAuthoritative) return;
                                this.systemReading = true;
                                try {
                                    _system.default.getStatus({
                                        success: (data)=>{
                                            this.applyStatus(chargingFromStatus(data), "system");
                                        },
                                        fail: (data, code)=>{
                                            console.log("charging status read failed", code, data);
                                        },
                                        complete: ()=>{
                                            this.systemReading = false;
                                        }
                                    });
                                } catch (error) {
                                    this.systemReading = false;
                                    console.log("charging status unavailable", error);
                                }
                            },
                            refreshBridgeBattery () {
                                if (!this.started || this.bridgeReading) return;
                                this.bridgeReading = true;
                                try {
                                    _system3.default.readText({
                                        uri: BATTERY_BRIDGE_URI,
                                        success: (data)=>{
                                            try {
                                                const payload = JSON.parse(data && data.text ? data.text : "{}");
                                                if (payload && "vela-emulator" === payload.source) this.applyStatus(payload.charging, "bridge");
                                            } catch (error) {
                                                console.log("charging bridge parse failed", error);
                                            }
                                        },
                                        fail: (data, code)=>{
                                            console.log("charging bridge read failed", code, data);
                                        },
                                        complete: ()=>{
                                            this.bridgeReading = false;
                                        }
                                    });
                                } catch (error) {
                                    this.bridgeReading = false;
                                    console.log("charging bridge unavailable", error);
                                }
                            },
                            subscribeBatteryEvent () {
                                if (null !== this.batteryEventId) return;
                                try {
                                    const id = _system2.default.subscribe({
                                        eventName: "usual.event.BATTERY_CHANGED",
                                        callback: (result)=>{
                                            if (this.bridgeAuthoritative) return;
                                            const params = result && result.params ? result.params : result;
                                            this.applyStatus(chargingFromStatus(params), "system");
                                        }
                                    });
                                    if (void 0 !== id) this.batteryEventId = id;
                                } catch (error) {
                                    console.log("charging event unavailable", error);
                                }
                            }
                        };
                        var _default = exports["default"] = chargingMonitor;
                    },
                    "./src/common/customization.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports.DEFAULT_BACKGROUND_ID = exports.DEFAULT_ACTION_ID = void 0;
                        exports.getAction = getAction;
                        exports.getActions = getActions;
                        exports.getBackground = getBackground;
                        exports.getBackgrounds = getBackgrounds;
                        const DEFAULT_BACKGROUND_ID = exports.DEFAULT_BACKGROUND_ID = "night-stage";
                        const DEFAULT_ACTION_ID = exports.DEFAULT_ACTION_ID = "cover-dance";
                        const backgroundOrder = [
                            "sky-blue",
                            "mint-green",
                            "warm-beige",
                            "sunset-coral",
                            "night-stage"
                        ];
                        const backgrounds = {
                            "sky-blue": {
                                id: "sky-blue",
                                name: "天空蓝",
                                src: "/common/backgrounds/sky-blue.png",
                                foreground: "sky",
                                edgeColor: "#a0d1f9"
                            },
                            "mint-green": {
                                id: "mint-green",
                                name: "薄荷绿",
                                src: "/common/backgrounds/mint-green.png",
                                foreground: "brown",
                                edgeColor: "#fceecf"
                            },
                            "warm-beige": {
                                id: "warm-beige",
                                name: "暖米色",
                                src: "/common/backgrounds/warm-beige.png",
                                foreground: "brown",
                                edgeColor: "#fee6bc"
                            },
                            "sunset-coral": {
                                id: "sunset-coral",
                                name: "橙粉日落",
                                src: "/common/backgrounds/sunset-coral.png",
                                foreground: "plum",
                                edgeColor: "#fdc897"
                            },
                            "night-stage": {
                                id: "night-stage",
                                name: "暗黑紫",
                                src: "/common/backgrounds/dark-purple.png",
                                foreground: "light",
                                edgeColor: "#0f0d2c"
                            }
                        };
                        const actionOrder = [
                            "cover-dance",
                            "shy-wave",
                            "phone-rest",
                            "balloon-rise",
                            "laugh",
                            "toilet-break"
                        ];
                        function framePath(folder, index) {
                            const number = index < 10 ? "0" + index : "" + index;
                            return "/common/actions/" + folder + "/frame-" + number + ".png";
                        }
                        function createFrames(folder, count) {
                            const frames = [];
                            for(let index = 1; index <= count; index += 1)frames.push(framePath(folder, index));
                            return frames;
                        }
                        function createLegacyFrames(count) {
                            const frames = [];
                            for(let index = 1; index <= count; index += 1){
                                const number = index < 10 ? "0" + index : "" + index;
                                frames.push("/common/cat/frame-" + number + ".png");
                            }
                            return frames;
                        }
                        function createTimedFrames(folder, delays, tick) {
                            const frames = [];
                            for(let index = 0; index < delays.length; index += 1){
                                const repeats = Math.max(1, Math.round(delays[index] / tick));
                                for(let repeat = 0; repeat < repeats; repeat += 1)frames.push(framePath(folder, index + 1));
                            }
                            return {
                                frames: frames,
                                duration: frames.length * tick
                            };
                        }
                        function action(id, name, folder, count, duration, previewIndex) {
                            return {
                                id: id,
                                name: name,
                                preview: framePath(folder, previewIndex),
                                duration: duration,
                                frames: createFrames(folder, count)
                            };
                        }
                        const balloonTiming = createTimedFrames("balloon-rise", [
                            120,
                            10,
                            110,
                            240,
                            10,
                            230,
                            10,
                            110,
                            120,
                            120,
                            120,
                            120,
                            120,
                            120,
                            120,
                            240,
                            120,
                            10,
                            110,
                            240,
                            120,
                            10,
                            110,
                            120,
                            120,
                            240,
                            10,
                            110,
                            120,
                            120,
                            120,
                            120,
                            120
                        ], 60);
                        const laughTiming = createTimedFrames("laugh", [
                            40,
                            80,
                            40,
                            80,
                            40,
                            80,
                            40,
                            80,
                            40,
                            80,
                            710,
                            40,
                            40,
                            80,
                            40,
                            80,
                            40,
                            80,
                            40,
                            80,
                            40,
                            80,
                            40,
                            40,
                            40,
                            40,
                            40,
                            40,
                            40,
                            30,
                            40,
                            40
                        ], 50);
                        const actions = {
                            "cover-dance": {
                                id: "cover-dance",
                                name: "捂鼻摆手",
                                preview: "/common/cat/frame-18.png",
                                duration: 3400,
                                frames: createLegacyFrames(34)
                            },
                            "shy-wave": action("shy-wave", "害羞挥手", "shy-wave", 19, 2300, 10),
                            "phone-rest": action("phone-rest", "躺平刷手机", "phone-rest", 35, 4200, 18),
                            "balloon-rise": {
                                id: "balloon-rise",
                                name: "气球升空",
                                preview: framePath("balloon-rise", 17),
                                duration: balloonTiming.duration,
                                frames: balloonTiming.frames
                            },
                            laugh: {
                                id: "laugh",
                                name: "仰头大笑",
                                preview: framePath("laugh", 17),
                                duration: laughTiming.duration,
                                frames: laughTiming.frames
                            },
                            "toilet-break": action("toilet-break", "马桶摸鱼", "toilet-break", 46, 2300, 23)
                        };
                        function getBackground(id) {
                            return backgrounds[id] || backgrounds[DEFAULT_BACKGROUND_ID];
                        }
                        function getBackgrounds() {
                            return backgroundOrder.map((id)=>backgrounds[id]);
                        }
                        function getAction(id) {
                            return actions[id] || actions[DEFAULT_ACTION_ID];
                        }
                        function getActions() {
                            return actionOrder.map((id)=>actions[id]);
                        }
                    },
                    "./src/common/daily-sync.js" (__unused_rspack_module, exports, __webpack_require__) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports["default"] = void 0;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.interconnect"));
                        var _system2 = _interopRequireDefault($app_require$1("@app-module/system.fetch"));
                        var _system3 = _interopRequireDefault($app_require$1("@app-module/system.storage"));
                        var _healthRecords = _interopRequireDefault(__webpack_require__("./src/common/health-records.js"));
                        var _assistantService = _interopRequireDefault(__webpack_require__("./src/common/assistant-service.js"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const PROTOCOL_NAME = "smart-band-daily-sync";
                        const PROTOCOL_VERSION = 1;
                        const OUTBOX_STORAGE_KEY = "bluetooth_sync_outbox_v1";
                        const STATE_STORAGE_KEY = "bluetooth_sync_state_v1";
                        const STEP_STORAGE_PREFIX = "daily_steps_v1_";
                        const CHUNK_SIZE = 768;
                        const ACK_TIMEOUT_MS = 15000;
                        const RETRY_DELAY_MS = 30000;
                        const SIMULATOR_RETRY_DELAY_MS = 5000;
                        const SIMULATOR_FALLBACK_DELAY_MS = 2500;
                        const SIMULATOR_CONTROL_POLL_MS = 2000;
                        const DATE_AUDIT_MS = 60000;
                        const MAX_OUTBOX_DAYS = 30;
                        const SIMULATOR_SYNC_ENABLED = true;
                        const SIMULATOR_SYNC_URL = "http://10.0.2.2:8792/api/sync/frame";
                        const SIMULATOR_CONTROL_URL = "http://10.0.2.2:8792/api/sync/control";
                        function pad(value) {
                            return value < 10 ? "0" + value : "" + value;
                        }
                        function dateKey(date) {
                            return date.getFullYear() + "-" + pad(date.getMonth() + 1) + "-" + pad(date.getDate());
                        }
                        function todayKey() {
                            return dateKey(new Date());
                        }
                        function dateFromKey(key) {
                            const parts = String(key || "").split("-");
                            if (3 !== parts.length) return null;
                            const year = Number(parts[0]);
                            const month = Number(parts[1]);
                            const day = Number(parts[2]);
                            if (!year || !month || !day) return null;
                            return new Date(year, month - 1, day);
                        }
                        function nextDateKey(key) {
                            const date = dateFromKey(key);
                            if (!date) return "";
                            date.setDate(date.getDate() + 1);
                            return dateKey(date);
                        }
                        function previousDateKey() {
                            const date = new Date();
                            date.setDate(date.getDate() - 1);
                            return dateKey(date);
                        }
                        function createDeviceId() {
                            return "vela-" + Date.now().toString(36) + "-" + Math.floor(1679616 * Math.random()).toString(36);
                        }
                        function defaultState() {
                            return {
                                deviceId: createDeviceId(),
                                activatedDate: todayKey(),
                                syncedDates: [],
                                lastSuccessDate: "",
                                lastSuccessAt: 0,
                                lastError: "",
                                lastErrorAt: 0
                            };
                        }
                        function safeParse(value, fallback) {
                            if (!value) return fallback;
                            try {
                                return JSON.parse(value);
                            } catch (error) {
                                return fallback;
                            }
                        }
                        function cloneHours(source) {
                            const result = [];
                            const list = Array.isArray(source) ? source : [];
                            for(let index = 0; index < 24; index += 1){
                                const value = Number(list[index]);
                                result.push(isFinite(value) && value >= 0 ? Math.round(value) : 0);
                            }
                            return result;
                        }
                        function normalizeSportRecord(source, key) {
                            const record = source || {};
                            const steps = Math.max(0, Math.round(Number(record.steps) || 0));
                            return {
                                dateKey: key,
                                steps: steps,
                                hours: cloneHours(record.hours),
                                goal: Math.max(1, Math.round(Number(record.goal) || 8000)),
                                calorieGoal: Math.max(1, Math.round(Number(record.calorieGoal) || 400)),
                                durationGoal: Math.max(1, Math.round(Number(record.durationGoal) || 30))
                            };
                        }
                        function createEmptySportRecord(key) {
                            return normalizeSportRecord({
                                dateKey: key,
                                steps: 0,
                                hours: [],
                                goal: 8000,
                                calorieGoal: 400,
                                durationGoal: 30
                            }, key);
                        }
                        function checksum(text) {
                            let hash = 2166136261;
                            for(let index = 0; index < text.length; index += 1){
                                hash ^= text.charCodeAt(index);
                                hash += (hash << 1) + (hash << 4) + (hash << 7) + (hash << 8) + (hash << 24);
                            }
                            return ("00000000" + (hash >>> 0).toString(16)).slice(-8);
                        }
                        function chunkText(text) {
                            const result = [];
                            for(let offset = 0; offset < text.length; offset += CHUNK_SIZE)result.push(text.slice(offset, offset + CHUNK_SIZE));
                            if (0 === result.length) result.push("");
                            return result;
                        }
                        function buildPayload(deviceId, sportRecord, healthRecord) {
                            const sport = normalizeSportRecord(sportRecord, sportRecord.dateKey);
                            const health = healthRecord || {
                                heartRate: [],
                                bloodPressure: []
                            };
                            return {
                                schemaVersion: PROTOCOL_VERSION,
                                messageType: "daily_activity_health",
                                syncId: deviceId + ":" + sport.dateKey + ":v" + PROTOCOL_VERSION,
                                deviceId: deviceId,
                                date: sport.dateKey,
                                timezoneOffsetMinutes: -new Date().getTimezoneOffset(),
                                generatedAt: Date.now(),
                                sport: {
                                    steps: sport.steps,
                                    caloriesKcal: Math.max(0, Math.round(0.04 * sport.steps)),
                                    activeMinutes: Math.max(0, Math.floor(sport.steps / 100)),
                                    hourlyCumulativeSteps: sport.hours,
                                    goals: {
                                        steps: sport.goal,
                                        caloriesKcal: sport.calorieGoal,
                                        activeMinutes: sport.durationGoal
                                    },
                                    source: "sensor_or_derived"
                                },
                                health: {
                                    heartRate: Array.isArray(health.heartRate) ? health.heartRate.slice() : [],
                                    bloodPressure: Array.isArray(health.bloodPressure) ? health.bloodPressure.slice() : []
                                }
                            };
                        }
                        function normalizeIncoming(event) {
                            let value = event;
                            if (value && void 0 !== value.data) value = value.data;
                            if ("string" == typeof value) {
                                try {
                                    return JSON.parse(value);
                                } catch (error) {
                                    return null;
                                }
                            }
                            return value && "object" == typeof value ? value : null;
                        }
                        const dailySync = {
                            started: false,
                            initialized: false,
                            connected: false,
                            connection: null,
                            state: defaultState(),
                            outbox: [],
                            auditing: false,
                            sending: false,
                            activeSyncId: "",
                            activeTransport: "",
                            simulatorFallbackReady: false,
                            ackTimerId: null,
                            retryTimerId: null,
                            auditTimerId: null,
                            simulatorFallbackTimerId: null,
                            simulatorControlTimerId: null,
                            simulatorControlLoading: false,
                            lastSimulatorRequestId: 0,
                            listeners: [],
                            start () {
                                if (this.started) return;
                                this.started = true;
                                this.openConnection();
                                this.scheduleSimulatorFallback();
                                this.loadState(()=>{
                                    this.loadOutbox(()=>{
                                        this.initialized = true;
                                        this.auditClosedDays();
                                        this.checkConnection();
                                        this.notify();
                                    });
                                });
                                this.restartAuditTimer();
                            },
                            stop () {
                                this.started = false;
                                this.connected = false;
                                this.sending = false;
                                this.activeSyncId = "";
                                this.activeTransport = "";
                                this.simulatorFallbackReady = false;
                                this.clearAckTimer();
                                this.clearRetryTimer();
                                this.clearSimulatorFallbackTimer();
                                this.stopSimulatorControlPolling();
                                if (this.auditTimerId) {
                                    clearInterval(this.auditTimerId);
                                    this.auditTimerId = null;
                                }
                                if (this.connection) {
                                    this.connection.onmessage = null;
                                    this.connection.onopen = null;
                                    this.connection.onclose = null;
                                    this.connection.onerror = null;
                                }
                                this.connection = null;
                            },
                            onAppShow () {
                                this.start();
                                this.restartAuditTimer();
                                this.recoverSuspendedSend();
                                this.clearRetryTimer();
                                this.simulatorControlLoading = false;
                                if (!this.connection) this.openConnection();
                                if (this.simulatorFallbackReady && !this.connected) {
                                    this.stopSimulatorControlPolling();
                                    this.startSimulatorControlPolling();
                                } else this.scheduleSimulatorFallback();
                                this.auditClosedDays();
                                this.checkConnection();
                                this.flush();
                            },
                            onAppHide () {
                                if (!this.initialized) return;
                                this.persistState();
                                this.persistOutbox();
                            },
                            restartAuditTimer () {
                                if (this.auditTimerId) clearInterval(this.auditTimerId);
                                this.auditTimerId = setInterval(()=>{
                                    this.auditClosedDays();
                                }, DATE_AUDIT_MS);
                            },
                            recoverSuspendedSend () {
                                if (!this.sending) return;
                                let activeEntry = null;
                                for(let index = 0; index < this.outbox.length; index += 1)if (this.outbox[index].syncId === this.activeSyncId) {
                                    activeEntry = this.outbox[index];
                                    break;
                                }
                                const lastAttemptAt = Math.max(0, Number(activeEntry && activeEntry.lastAttemptAt) || 0);
                                if (activeEntry && lastAttemptAt && Date.now() - lastAttemptAt < ACK_TIMEOUT_MS) return;
                                this.clearAckTimer();
                                this.sending = false;
                                this.activeSyncId = "";
                                this.activeTransport = "";
                            },
                            subscribe (callback) {
                                if (!callback) return;
                                let exists = false;
                                for(let index = 0; index < this.listeners.length; index += 1)if (this.listeners[index] === callback) exists = true;
                                if (!exists) this.listeners.push(callback);
                                callback(this.snapshot());
                            },
                            unsubscribe (callback) {
                                const next = [];
                                for(let index = 0; index < this.listeners.length; index += 1)if (this.listeners[index] !== callback) next.push(this.listeners[index]);
                                this.listeners = next;
                            },
                            snapshot () {
                                return {
                                    initialized: this.initialized,
                                    connected: this.connected,
                                    transport: this.connected ? "interconnect" : this.simulatorFallbackReady ? "simulator" : "disconnected",
                                    pendingDays: this.outbox.length,
                                    lastSuccessDate: this.state.lastSuccessDate || "",
                                    lastSuccessAt: this.state.lastSuccessAt || 0,
                                    lastError: this.state.lastError || ""
                                };
                            },
                            notify () {
                                const snapshot = this.snapshot();
                                const listeners = this.listeners.slice();
                                for(let index = 0; index < listeners.length; index += 1){
                                    try {
                                        listeners[index](snapshot);
                                    } catch (error) {
                                        console.log("daily sync listener failed", error);
                                    }
                                }
                            },
                            loadState (callback) {
                                try {
                                    _system3.default.get({
                                        key: STATE_STORAGE_KEY,
                                        default: "",
                                        success: (value)=>{
                                            const parsed = safeParse(value, null);
                                            if (parsed && parsed.deviceId && parsed.activatedDate) {
                                                this.state = parsed;
                                                if (!Array.isArray(this.state.syncedDates)) this.state.syncedDates = this.state.lastSuccessDate ? [
                                                    this.state.lastSuccessDate
                                                ] : [];
                                            } else {
                                                this.state = defaultState();
                                                this.persistState();
                                            }
                                            callback();
                                        },
                                        fail: (data, code)=>{
                                            console.log("load daily sync state failed", code, data);
                                            this.state = defaultState();
                                            callback();
                                        }
                                    });
                                } catch (error) {
                                    console.log("daily sync state unavailable", error);
                                    this.state = defaultState();
                                    callback();
                                }
                            },
                            loadOutbox (callback) {
                                try {
                                    _system3.default.get({
                                        key: OUTBOX_STORAGE_KEY,
                                        default: "",
                                        success: (value)=>{
                                            const parsed = safeParse(value, []);
                                            this.outbox = Array.isArray(parsed) ? parsed : [];
                                            this.trimOutbox();
                                            callback();
                                        },
                                        fail: (data, code)=>{
                                            console.log("load daily sync outbox failed", code, data);
                                            this.outbox = [];
                                            callback();
                                        }
                                    });
                                } catch (error) {
                                    console.log("daily sync outbox unavailable", error);
                                    this.outbox = [];
                                    callback();
                                }
                            },
                            persistState () {
                                try {
                                    _system3.default.set({
                                        key: STATE_STORAGE_KEY,
                                        value: JSON.stringify(this.state),
                                        fail: (data, code)=>{
                                            console.log("save daily sync state failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save daily sync state unavailable", error);
                                }
                            },
                            persistOutbox (callback) {
                                try {
                                    _system3.default.set({
                                        key: OUTBOX_STORAGE_KEY,
                                        value: JSON.stringify(this.outbox),
                                        success: ()=>{
                                            if (callback) callback();
                                        },
                                        fail: (data, code)=>{
                                            console.log("save daily sync outbox failed", code, data);
                                            if (callback) callback();
                                        }
                                    });
                                } catch (error) {
                                    console.log("save daily sync outbox unavailable", error);
                                    if (callback) callback();
                                }
                            },
                            trimOutbox () {
                                this.outbox.sort((left, right)=>String(left.date || "").localeCompare(String(right.date || "")));
                                if (this.outbox.length > MAX_OUTBOX_DAYS) this.outbox = this.outbox.slice(this.outbox.length - MAX_OUTBOX_DAYS);
                            },
                            hasDate (key) {
                                const syncedDates = Array.isArray(this.state.syncedDates) ? this.state.syncedDates : [];
                                for(let index = 0; index < syncedDates.length; index += 1)if (syncedDates[index] === key) return true;
                                for(let index = 0; index < this.outbox.length; index += 1)if (this.outbox[index].date === key) return true;
                                return false;
                            },
                            auditClosedDays () {
                                if (!this.started || !this.initialized || this.auditing || !this.state.activatedDate) return;
                                const lastClosed = previousDateKey();
                                if (this.state.activatedDate > lastClosed) return;
                                const keys = [];
                                let key = this.state.activatedDate;
                                let guard = 0;
                                while(key && key <= lastClosed && guard < MAX_OUTBOX_DAYS){
                                    if (!this.hasDate(key)) keys.push(key);
                                    key = nextDateKey(key);
                                    guard += 1;
                                }
                                if (0 === keys.length) return void this.flush();
                                this.auditing = true;
                                const readNext = (index)=>{
                                    if (index >= keys.length) {
                                        this.auditing = false;
                                        this.persistOutbox(()=>{
                                            this.notify();
                                            this.flush();
                                        });
                                        return;
                                    }
                                    this.readSportDay(keys[index], (sportRecord)=>{
                                        if (sportRecord) return void this.enqueueDay(sportRecord, false, ()=>{
                                            readNext(index + 1);
                                        });
                                        const emptyRecord = createEmptySportRecord(keys[index]);
                                        this.persistSportDay(emptyRecord, ()=>{
                                            this.enqueueDay(emptyRecord, false, ()=>{
                                                readNext(index + 1);
                                            });
                                        });
                                    });
                                };
                                readNext(0);
                            },
                            readSportDay (key, callback) {
                                try {
                                    _system3.default.get({
                                        key: STEP_STORAGE_PREFIX + key,
                                        default: "",
                                        success: (value)=>{
                                            const parsed = safeParse(value, null);
                                            callback(parsed ? normalizeSportRecord(parsed, key) : null);
                                        },
                                        fail: (data, code)=>{
                                            console.log("read daily sport for sync failed", key, code, data);
                                            callback(null);
                                        }
                                    });
                                } catch (error) {
                                    console.log("daily sport storage unavailable", key, error);
                                    callback(null);
                                }
                            },
                            persistSportDay (sportRecord, callback) {
                                const record = normalizeSportRecord(sportRecord, sportRecord && sportRecord.dateKey);
                                if (!record.dateKey) {
                                    if (callback) callback(false);
                                    return;
                                }
                                try {
                                    _system3.default.set({
                                        key: STEP_STORAGE_PREFIX + record.dateKey,
                                        value: JSON.stringify(record),
                                        success: ()=>{
                                            if (callback) callback(true);
                                        },
                                        fail: (data, code)=>{
                                            console.log("save missing daily sport for sync failed", record.dateKey, code, data);
                                            if (callback) callback(false);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save missing daily sport for sync unavailable", record.dateKey, error);
                                    if (callback) callback(false);
                                }
                            },
                            enqueueClosedDay (sportRecord) {
                                if (!sportRecord || !sportRecord.dateKey) return;
                                if (!this.initialized) return void setTimeout(()=>this.enqueueClosedDay(sportRecord), 250);
                                if (sportRecord.dateKey >= todayKey() || this.hasDate(sportRecord.dateKey)) return;
                                this.enqueueDay(normalizeSportRecord(sportRecord, sportRecord.dateKey), true);
                            },
                            enqueueDay (sportRecord, persist, callback) {
                                if (!sportRecord || !sportRecord.dateKey || this.hasDate(sportRecord.dateKey)) {
                                    if (callback) callback();
                                    return;
                                }
                                _healthRecords.default.sealDay(sportRecord.dateKey, (healthRecord)=>{
                                    const payload = buildPayload(this.state.deviceId, sportRecord, healthRecord);
                                    this.outbox.push({
                                        syncId: payload.syncId,
                                        date: payload.date,
                                        createdAt: Date.now(),
                                        attempts: 0,
                                        lastAttemptAt: 0,
                                        payload: payload
                                    });
                                    this.trimOutbox();
                                    const finish = ()=>{
                                        this.notify();
                                        if (persist) this.flush();
                                        if (callback) callback();
                                    };
                                    if (persist) this.persistOutbox(finish);
                                    else finish();
                                });
                            },
                            openConnection () {
                                try {
                                    this.connection = _system.default.instance();
                                    if (!this.connection) return;
                                    this.connection.onmessage = (event)=>this.handleIncoming(event);
                                    this.connection.onopen = (data)=>{
                                        this.connected = true;
                                        this.simulatorFallbackReady = false;
                                        this.clearSimulatorFallbackTimer();
                                        this.stopSimulatorControlPolling();
                                        console.log("daily sync connection opened", data && data.isReconnected);
                                        this.notify();
                                        this.flush();
                                    };
                                    this.connection.onclose = (data)=>{
                                        this.connected = false;
                                        this.enableSimulatorFallback();
                                        this.failActiveSend("connection closed " + (data && data.code ? data.code : ""));
                                    };
                                    this.connection.onerror = (data)=>{
                                        this.connected = false;
                                        this.enableSimulatorFallback();
                                        this.failActiveSend("connection error " + (data && data.code ? data.code : ""));
                                    };
                                } catch (error) {
                                    this.connection = null;
                                    this.connected = false;
                                    this.enableSimulatorFallback();
                                    this.recordError("interconnect unavailable");
                                    console.log("daily sync interconnect unavailable", error);
                                }
                            },
                            checkConnection () {
                                if (!this.connection || !this.initialized) return;
                                try {
                                    this.connection.getReadyState({
                                        success: (data)=>{
                                            this.connected = !!data && 1 === data.status;
                                            if (this.connected) {
                                                this.simulatorFallbackReady = false;
                                                this.clearSimulatorFallbackTimer();
                                            } else this.enableSimulatorFallback();
                                            this.notify();
                                            this.flush();
                                        },
                                        fail: (data, code)=>{
                                            this.connected = false;
                                            this.enableSimulatorFallback();
                                            console.log("daily sync ready state failed", code, data);
                                            this.notify();
                                            this.flush();
                                        }
                                    });
                                } catch (error) {
                                    this.connected = false;
                                    this.enableSimulatorFallback();
                                    console.log("daily sync ready state unavailable", error);
                                    this.flush();
                                }
                            },
                            flush () {
                                const canUseInterconnect = this.connected && !!this.connection;
                                const canUseSimulator = SIMULATOR_SYNC_ENABLED && this.simulatorFallbackReady;
                                if (!this.initialized || !canUseInterconnect && !canUseSimulator || this.sending || 0 === this.outbox.length) return;
                                const entry = this.outbox[0];
                                this.sending = true;
                                this.activeSyncId = entry.syncId;
                                this.activeTransport = canUseInterconnect ? "interconnect" : "simulator";
                                entry.attempts = Math.max(0, Number(entry.attempts) || 0) + 1;
                                entry.lastAttemptAt = Date.now();
                                this.persistOutbox();
                                this.sendEntry(entry);
                            },
                            sendEntry (entry) {
                                const json = JSON.stringify(entry.payload);
                                const chunks = chunkText(json);
                                const frames = [
                                    {
                                        protocol: PROTOCOL_NAME,
                                        version: PROTOCOL_VERSION,
                                        type: "sync_begin",
                                        syncId: entry.syncId,
                                        date: entry.date,
                                        totalChunks: chunks.length,
                                        totalCharacters: json.length,
                                        checksum: checksum(json)
                                    }
                                ];
                                for(let index = 0; index < chunks.length; index += 1)frames.push({
                                    protocol: PROTOCOL_NAME,
                                    version: PROTOCOL_VERSION,
                                    type: "sync_chunk",
                                    syncId: entry.syncId,
                                    index: index,
                                    data: chunks[index]
                                });
                                frames.push({
                                    protocol: PROTOCOL_NAME,
                                    version: PROTOCOL_VERSION,
                                    type: "sync_commit",
                                    syncId: entry.syncId
                                });
                                const sendNext = (index)=>{
                                    if (!this.sending || this.activeSyncId !== entry.syncId) return;
                                    if (index >= frames.length) return void this.waitForAck(entry.syncId);
                                    this.sendFrame(frames[index], (response)=>{
                                        const message = response && response.ack ? response.ack : response;
                                        if (message && message.protocol === PROTOCOL_NAME && "sync_ack" === message.type) return void this.handleIncoming(message);
                                        sendNext(index + 1);
                                    }, (data, code)=>{
                                        this.failActiveSend("send failed " + (void 0 !== code ? code : ""));
                                    });
                                };
                                sendNext(0);
                            },
                            sendFrame (frame, success, fail) {
                                if ("simulator" === this.activeTransport) return void this.sendSimulatorFrame(frame, success, fail);
                                try {
                                    this.connection.send({
                                        data: frame,
                                        success: success,
                                        fail: fail
                                    });
                                } catch (error) {
                                    fail(error, -1);
                                }
                            },
                            sendSimulatorFrame (frame, success, fail) {
                                try {
                                    _system2.default.fetch({
                                        url: SIMULATOR_SYNC_URL,
                                        method: "POST",
                                        header: {
                                            "Content-Type": "application/json"
                                        },
                                        data: JSON.stringify(frame),
                                        responseType: "json",
                                        success: (response)=>{
                                            let body = response && response.data;
                                            if ("string" == typeof body) body = safeParse(body, null);
                                            if (!response || response.code < 200 || response.code >= 300 || !body) return void fail(body || response, response && response.code);
                                            success(body);
                                        },
                                        fail: (data, code)=>{
                                            fail(data, code);
                                        }
                                    });
                                } catch (error) {
                                    fail(error, -1);
                                }
                            },
                            waitForAck (syncId) {
                                this.clearAckTimer();
                                this.ackTimerId = setTimeout(()=>{
                                    this.ackTimerId = null;
                                    if (this.activeSyncId !== syncId) return;
                                    this.failActiveSend("ack timeout");
                                }, ACK_TIMEOUT_MS);
                            },
                            handleIncoming (event) {
                                const message = normalizeIncoming(event);
                                if (_assistantService.default.handleIncoming(message)) return;
                                if (message && message.protocol === PROTOCOL_NAME && "sync_request" === message.type) return void this.requestSyncNow();
                                if (!message || message.protocol !== PROTOCOL_NAME || "sync_ack" !== message.type || !message.syncId) return;
                                if (message.status && "ok" !== message.status && "duplicate" !== message.status) return void this.failActiveSend("receiver rejected " + message.status);
                                this.acknowledge(message.syncId);
                            },
                            acknowledge (syncId) {
                                let date = "";
                                const next = [];
                                for(let index = 0; index < this.outbox.length; index += 1){
                                    const entry = this.outbox[index];
                                    if (entry.syncId === syncId) date = entry.date;
                                    else next.push(entry);
                                }
                                if (!date) return;
                                this.outbox = next;
                                this.clearAckTimer();
                                this.clearRetryTimer();
                                this.sending = false;
                                this.activeSyncId = "";
                                this.activeTransport = "";
                                this.state.lastSuccessDate = date;
                                this.state.lastSuccessAt = Date.now();
                                this.state.lastError = "";
                                if (!Array.isArray(this.state.syncedDates)) this.state.syncedDates = [];
                                let dateExists = false;
                                for(let index = 0; index < this.state.syncedDates.length; index += 1)if (this.state.syncedDates[index] === date) dateExists = true;
                                if (!dateExists) this.state.syncedDates.push(date);
                                this.state.syncedDates.sort();
                                if (this.state.syncedDates.length > MAX_OUTBOX_DAYS) this.state.syncedDates = this.state.syncedDates.slice(this.state.syncedDates.length - MAX_OUTBOX_DAYS);
                                this.persistState();
                                this.persistOutbox(()=>{
                                    this.notify();
                                    this.flush();
                                });
                            },
                            failActiveSend (reason) {
                                this.clearAckTimer();
                                this.sending = false;
                                this.activeSyncId = "";
                                this.activeTransport = "";
                                this.recordError(reason);
                                this.scheduleRetry();
                            },
                            recordError (reason) {
                                this.state.lastError = reason || "unknown error";
                                this.state.lastErrorAt = Date.now();
                                this.persistState();
                                this.notify();
                            },
                            scheduleRetry () {
                                if (this.retryTimerId || !this.started) return;
                                const delay = this.simulatorFallbackReady ? SIMULATOR_RETRY_DELAY_MS : RETRY_DELAY_MS;
                                this.retryTimerId = setTimeout(()=>{
                                    this.retryTimerId = null;
                                    this.checkConnection();
                                    this.flush();
                                }, delay);
                            },
                            scheduleSimulatorFallback () {
                                if (!SIMULATOR_SYNC_ENABLED || this.simulatorFallbackReady || this.simulatorFallbackTimerId) return;
                                this.simulatorFallbackTimerId = setTimeout(()=>{
                                    this.simulatorFallbackTimerId = null;
                                    if (!this.connected) this.enableSimulatorFallback();
                                }, SIMULATOR_FALLBACK_DELAY_MS);
                            },
                            enableSimulatorFallback () {
                                if (!SIMULATOR_SYNC_ENABLED || this.connected) return;
                                this.clearSimulatorFallbackTimer();
                                this.simulatorFallbackReady = true;
                                this.startSimulatorControlPolling();
                                this.notify();
                                this.flush();
                            },
                            clearSimulatorFallbackTimer () {
                                if (!this.simulatorFallbackTimerId) return;
                                clearTimeout(this.simulatorFallbackTimerId);
                                this.simulatorFallbackTimerId = null;
                            },
                            startSimulatorControlPolling () {
                                if (this.simulatorControlTimerId || !this.started) return;
                                this.pollSimulatorControl();
                                this.simulatorControlTimerId = setInterval(()=>{
                                    this.pollSimulatorControl();
                                }, SIMULATOR_CONTROL_POLL_MS);
                            },
                            stopSimulatorControlPolling () {
                                if (this.simulatorControlTimerId) {
                                    clearInterval(this.simulatorControlTimerId);
                                    this.simulatorControlTimerId = null;
                                }
                                this.simulatorControlLoading = false;
                            },
                            pollSimulatorControl () {
                                if (!this.started || !this.simulatorFallbackReady || this.simulatorControlLoading) return;
                                this.simulatorControlLoading = true;
                                try {
                                    _system2.default.fetch({
                                        url: SIMULATOR_CONTROL_URL,
                                        method: "GET",
                                        responseType: "json",
                                        success: (response)=>{
                                            this.simulatorControlLoading = false;
                                            let body = response && response.data;
                                            if ("string" == typeof body) body = safeParse(body, null);
                                            const requestId = Math.max(0, Number(body && body.requestId) || 0);
                                            if (!requestId || requestId === this.lastSimulatorRequestId) return;
                                            this.lastSimulatorRequestId = requestId;
                                            this.enqueueSimulatorSnapshot(requestId);
                                        },
                                        fail: ()=>{
                                            this.simulatorControlLoading = false;
                                        }
                                    });
                                } catch (error) {
                                    this.simulatorControlLoading = false;
                                }
                            },
                            enqueueSimulatorSnapshot (requestId) {
                                const sourceDate = todayKey();
                                const simulationDate = "simulation-" + requestId;
                                if (this.hasDate(simulationDate)) return;
                                this.readSportDay(sourceDate, (storedSport)=>{
                                    const sport = storedSport || normalizeSportRecord({
                                        dateKey: sourceDate,
                                        steps: 0,
                                        hours: [],
                                        goal: 8000,
                                        calorieGoal: 400,
                                        durationGoal: 30
                                    }, sourceDate);
                                    const simulatorSport = normalizeSportRecord(sport, simulationDate);
                                    _healthRecords.default.loadDay(sourceDate, (healthRecord)=>{
                                        const payload = buildPayload(this.state.deviceId, simulatorSport, healthRecord);
                                        payload.simulation = {
                                            enabled: true,
                                            requestId: requestId,
                                            sourceDate: sourceDate
                                        };
                                        this.outbox.push({
                                            syncId: payload.syncId,
                                            date: payload.date,
                                            createdAt: Date.now(),
                                            attempts: 0,
                                            lastAttemptAt: 0,
                                            payload: payload
                                        });
                                        this.trimOutbox();
                                        this.persistOutbox(()=>{
                                            this.notify();
                                            this.flush();
                                        });
                                    });
                                });
                            },
                            clearAckTimer () {
                                if (!this.ackTimerId) return;
                                clearTimeout(this.ackTimerId);
                                this.ackTimerId = null;
                            },
                            clearRetryTimer () {
                                if (!this.retryTimerId) return;
                                clearTimeout(this.retryTimerId);
                                this.retryTimerId = null;
                            },
                            requestSyncNow () {
                                this.auditClosedDays();
                                this.checkConnection();
                                this.flush();
                            }
                        };
                        var _default = exports["default"] = dailySync;
                    },
                    "./src/common/health-records.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports["default"] = void 0;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.storage"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const DAILY_HEALTH_PREFIX = "daily_health_v1_";
                        const HEART_RATE_HISTORY_KEY = "heart_rate_history_v1";
                        const BLOOD_PRESSURE_HISTORY_KEY = "blood_pressure_history_v1";
                        const RETENTION_DAYS = 30;
                        function pad(value) {
                            return value < 10 ? "0" + value : "" + value;
                        }
                        function dateKeyFromTimestamp(timestamp) {
                            const date = new Date(timestamp);
                            return date.getFullYear() + "-" + pad(date.getMonth() + 1) + "-" + pad(date.getDate());
                        }
                        function dateKeyBefore(daysBefore) {
                            const now = new Date();
                            const date = new Date(now.getFullYear(), now.getMonth(), now.getDate() - daysBefore);
                            return dateKeyFromTimestamp(date.getTime());
                        }
                        function emptyRecord(key) {
                            return {
                                dateKey: key,
                                heartRate: [],
                                bloodPressure: []
                            };
                        }
                        function normalizeHeartRate(source, key) {
                            const result = [];
                            const list = Array.isArray(source) ? source : [];
                            for(let index = 0; index < list.length; index += 1){
                                const bpm = Math.round(Number(list[index].bpm) || 0);
                                const measuredAt = Math.max(0, Number(list[index].measuredAt) || 0);
                                if (!(bpm < 30) && !(bpm > 240) && measuredAt && dateKeyFromTimestamp(measuredAt) === key) result.push({
                                    bpm: bpm,
                                    measuredAt: measuredAt,
                                    source: list[index].source || "sensor"
                                });
                            }
                            return result;
                        }
                        function normalizeBloodPressure(source, key) {
                            const result = [];
                            const list = Array.isArray(source) ? source : [];
                            for(let index = 0; index < list.length; index += 1){
                                const systolic = Math.round(Number(list[index].systolic) || 0);
                                const diastolic = Math.round(Number(list[index].diastolic) || 0);
                                const pulse = Math.round(Number(list[index].pulse) || 0);
                                const measuredAt = Math.max(0, Number(list[index].measuredAt) || 0);
                                if (!(systolic < 70) && !(systolic > 240) && !(diastolic < 40) && !(diastolic > 160) && !(pulse < 30) && !(pulse > 240) && measuredAt && dateKeyFromTimestamp(measuredAt) === key) result.push({
                                    systolic: systolic,
                                    diastolic: diastolic,
                                    pulse: pulse,
                                    measuredAt: measuredAt,
                                    source: list[index].source || "simulated"
                                });
                            }
                            return result;
                        }
                        function normalizeRecord(source, key) {
                            const record = source || {};
                            return {
                                dateKey: key,
                                heartRate: normalizeHeartRate(record.heartRate, key),
                                bloodPressure: normalizeBloodPressure(record.bloodPressure, key)
                            };
                        }
                        function parseStoredRecord(value, key) {
                            if (!value) return null;
                            try {
                                return normalizeRecord(JSON.parse(value), key);
                            } catch (error) {
                                console.log("parse daily health failed", key, error);
                                return null;
                            }
                        }
                        function readStorage(key, callback) {
                            try {
                                _system.default.get({
                                    key: key,
                                    default: "",
                                    success: (value)=>callback(value || ""),
                                    fail: (data, code)=>{
                                        console.log("read health storage failed", key, code, data);
                                        callback("");
                                    }
                                });
                            } catch (error) {
                                console.log("health storage unavailable", key, error);
                                callback("");
                            }
                        }
                        function saveRecord(record, callback) {
                            try {
                                _system.default.set({
                                    key: DAILY_HEALTH_PREFIX + record.dateKey,
                                    value: JSON.stringify(record),
                                    success: ()=>{
                                        if (callback) callback(record);
                                    },
                                    fail: (data, code)=>{
                                        console.log("save daily health failed", code, data);
                                        if (callback) callback(record);
                                    }
                                });
                            } catch (error) {
                                console.log("save daily health unavailable", error);
                                if (callback) callback(record);
                            }
                        }
                        function readLegacyDay(key, callback) {
                            let heartRateValue = "";
                            let bloodPressureValue = "";
                            let pending = 2;
                            const finishOne = ()=>{
                                pending -= 1;
                                if (pending > 0) return;
                                let heartRate = [];
                                let bloodPressure = [];
                                try {
                                    heartRate = JSON.parse(heartRateValue || "[]");
                                } catch (error) {
                                    heartRate = [];
                                }
                                try {
                                    bloodPressure = JSON.parse(bloodPressureValue || "[]");
                                } catch (error) {
                                    bloodPressure = [];
                                }
                                callback(normalizeRecord({
                                    heartRate: heartRate,
                                    bloodPressure: bloodPressure
                                }, key));
                            };
                            readStorage(HEART_RATE_HISTORY_KEY, (value)=>{
                                heartRateValue = value;
                                finishOne();
                            });
                            readStorage(BLOOD_PRESSURE_HISTORY_KEY, (value)=>{
                                bloodPressureValue = value;
                                finishOne();
                            });
                        }
                        function appendUnique(list, item) {
                            const result = [];
                            for(let index = 0; index < list.length; index += 1)if (list[index].measuredAt !== item.measuredAt) result.push(list[index]);
                            result.push(item);
                            result.sort((left, right)=>left.measuredAt - right.measuredAt);
                            return result;
                        }
                        function pruneExpiredDay() {
                            const expiredKey = dateKeyBefore(RETENTION_DAYS + 1);
                            try {
                                _system.default.delete({
                                    key: DAILY_HEALTH_PREFIX + expiredKey,
                                    fail: (data, code)=>{
                                        console.log("delete expired health failed", expiredKey, code, data);
                                    }
                                });
                            } catch (error) {
                                console.log("delete expired health unavailable", expiredKey, error);
                            }
                        }
                        const healthRecords = {
                            loadDay (key, callback) {
                                readStorage(DAILY_HEALTH_PREFIX + key, (value)=>{
                                    const stored = parseStoredRecord(value, key);
                                    if (stored) return void callback(stored);
                                    readLegacyDay(key, callback);
                                });
                            },
                            sealDay (key, callback) {
                                readStorage(DAILY_HEALTH_PREFIX + key, (value)=>{
                                    const stored = parseStoredRecord(value, key);
                                    if (stored) return void callback(stored);
                                    readLegacyDay(key, (record)=>{
                                        saveRecord(record, callback);
                                    });
                                });
                            },
                            recordHeartRate (bpm, measuredAt) {
                                const timestamp = Math.max(0, Number(measuredAt) || Date.now());
                                const key = dateKeyFromTimestamp(timestamp);
                                this.loadDay(key, (record)=>{
                                    record.heartRate = appendUnique(record.heartRate, {
                                        bpm: Math.max(30, Math.min(240, Math.round(Number(bpm) || 0))),
                                        measuredAt: timestamp,
                                        source: "sensor"
                                    });
                                    saveRecord(record);
                                    pruneExpiredDay();
                                });
                            },
                            recordBloodPressure (systolic, diastolic, pulse, measuredAt) {
                                const timestamp = Math.max(0, Number(measuredAt) || Date.now());
                                const key = dateKeyFromTimestamp(timestamp);
                                this.loadDay(key, (record)=>{
                                    record.bloodPressure = appendUnique(record.bloodPressure, {
                                        systolic: Math.round(Number(systolic) || 0),
                                        diastolic: Math.round(Number(diastolic) || 0),
                                        pulse: Math.round(Number(pulse) || 0),
                                        measuredAt: timestamp,
                                        source: "simulated"
                                    });
                                    saveRecord(record);
                                    pruneExpiredDay();
                                });
                            }
                        };
                        var _default = exports["default"] = healthRecords;
                    },
                    "./src/common/music-player.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports.MUSIC_LIBRARY = void 0;
                        const MUSIC_LIBRARY = exports.MUSIC_LIBRARY = [
                            {
                                id: 1,
                                name: "一路向北",
                                artists: "周杰伦",
                                playUrl: "/common/music/tracks/track-01.mp3"
                            },
                            {
                                id: 2,
                                name: "稻香",
                                artists: "周杰伦",
                                playUrl: "/common/music/tracks/track-02.mp3"
                            },
                            {
                                id: 3,
                                name: "等你下课",
                                artists: "周杰伦、杨瑞代",
                                playUrl: "/common/music/tracks/track-03.mp3"
                            }
                        ];
                    },
                    "./src/common/notification-bridge.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports["default"] = void 0;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.fetch"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const NOTIFICATION_API_URL = "http://10.0.2.2:8791/api/notifications/next";
                        const POLL_INTERVAL_MS = 1200;
                        let running = false;
                        let polling = false;
                        let timerId = null;
                        let receiver = null;
                        let clientId = "";
                        function createClientId() {
                            return "watch-" + Date.now() + "-" + Math.round(100000 * Math.random());
                        }
                        function schedule(delay) {
                            if (!running) return;
                            if (timerId) clearTimeout(timerId);
                            timerId = setTimeout(()=>{
                                timerId = null;
                                poll();
                            }, delay);
                        }
                        function deliver(data) {
                            if (!data || !data.notification || "function" != typeof receiver) return;
                            try {
                                receiver(data.notification);
                            } catch (error) {
                                console.log("computer notification receiver failed", error);
                            }
                        }
                        function poll() {
                            if (!running || polling) return;
                            polling = true;
                            _system.default.fetch({
                                url: NOTIFICATION_API_URL + "?client=" + encodeURIComponent(clientId) + "&timestamp=" + Date.now(),
                                method: "GET",
                                responseType: "json",
                                success: (response)=>{
                                    polling = false;
                                    try {
                                        if (response && 200 === response.code) deliver(response.data);
                                    } catch (error) {
                                        console.log("parse computer notification failed", error);
                                    }
                                    schedule(POLL_INTERVAL_MS);
                                },
                                fail: ()=>{
                                    polling = false;
                                    schedule(2 * POLL_INTERVAL_MS);
                                }
                            });
                        }
                        function start(callback) {
                            receiver = callback;
                            if (running) return;
                            running = true;
                            clientId = createClientId();
                            poll();
                        }
                        function stop() {
                            running = false;
                            polling = false;
                            receiver = null;
                            if (timerId) {
                                clearTimeout(timerId);
                                timerId = null;
                            }
                        }
                        function resume() {
                            if (!running) return;
                            polling = false;
                            if (timerId) {
                                clearTimeout(timerId);
                                timerId = null;
                            }
                            schedule(0);
                        }
                        var _default = exports["default"] = {
                            start,
                            resume,
                            stop
                        };
                    },
                    "./src/common/notification-center.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports["default"] = void 0;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.storage"));
                        var _system2 = _interopRequireDefault($app_require$1("@app-module/system.vibrator"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const STORAGE_KEY = "notification_history_v1";
                        const EMPTY_STORAGE_VALUE = "__NO_NOTIFICATION_HISTORY__";
                        const HISTORY_LIMIT = 20;
                        let loaded = false;
                        let loading = false;
                        let history = [];
                        let activeNotification = null;
                        let listeners = [];
                        let readyCallbacks = [];
                        function pad(value) {
                            return value < 10 ? "0" + value : "" + value;
                        }
                        function formatTime(timestamp) {
                            const date = new Date(timestamp);
                            return pad(date.getHours()) + ":" + pad(date.getMinutes());
                        }
                        function formatDate(timestamp) {
                            const date = new Date(timestamp);
                            return pad(date.getMonth() + 1) + "月" + pad(date.getDate()) + "日";
                        }
                        function cloneNotification(item) {
                            return {
                                id: item.id,
                                type: item.type,
                                appName: item.appName,
                                sender: item.sender,
                                title: item.title,
                                body: item.body,
                                timestamp: item.timestamp,
                                timeText: item.timeText,
                                dateText: item.dateText,
                                unread: !!item.unread,
                                preview: !!item.preview,
                                previewIndex: Number(item.previewIndex) || 0,
                                previewTotal: Number(item.previewTotal) || 0
                            };
                        }
                        function normalizeType(value) {
                            if ("call" === value || "sms" === value || "app" === value) return value;
                            return "app";
                        }
                        function normalizeNotification(payload) {
                            const source = payload || {};
                            const timestamp = Math.max(0, Number(source.timestamp) || Date.now());
                            const type = normalizeType(source.type);
                            const typeDefaults = {
                                call: {
                                    appName: "电话",
                                    sender: "未知联系人",
                                    title: "来电提醒",
                                    body: "未知号码"
                                },
                                sms: {
                                    appName: "短信",
                                    sender: "新短信",
                                    title: "短信提醒",
                                    body: "收到一条新短信"
                                },
                                app: {
                                    appName: "应用通知",
                                    sender: "",
                                    title: "新消息",
                                    body: "收到一条应用通知"
                                }
                            };
                            const defaults = typeDefaults[type];
                            return {
                                id: source.id || type + "-" + timestamp,
                                type: type,
                                appName: source.appName || defaults.appName,
                                sender: source.sender || defaults.sender,
                                title: source.title || defaults.title,
                                body: source.body || defaults.body,
                                timestamp: timestamp,
                                timeText: formatTime(timestamp),
                                dateText: formatDate(timestamp),
                                unread: false !== source.unread,
                                preview: !!source.preview,
                                previewIndex: Number(source.previewIndex) || 0,
                                previewTotal: Number(source.previewTotal) || 0
                            };
                        }
                        function notifyListeners() {
                            const snapshot = {
                                active: activeNotification ? cloneNotification(activeNotification) : null,
                                history: history.map(cloneNotification)
                            };
                            const copied = listeners.slice();
                            for(let index = 0; index < copied.length; index += 1){
                                try {
                                    copied[index](snapshot);
                                } catch (error) {
                                    console.log("notification listener failed", error);
                                }
                            }
                        }
                        function flushReadyCallbacks() {
                            const callbacks = readyCallbacks.slice();
                            readyCallbacks = [];
                            for(let index = 0; index < callbacks.length; index += 1){
                                try {
                                    callbacks[index]();
                                } catch (error) {
                                    console.log("notification ready callback failed", error);
                                }
                            }
                        }
                        function finishLoading(nextHistory) {
                            history = Array.isArray(nextHistory) ? nextHistory.map(normalizeNotification).slice(0, HISTORY_LIMIT) : [];
                            loaded = true;
                            loading = false;
                            flushReadyCallbacks();
                            notifyListeners();
                        }
                        function initialize(callback) {
                            if (loaded) {
                                if (callback) callback();
                                return;
                            }
                            if (callback) readyCallbacks.push(callback);
                            if (loading) return;
                            loading = true;
                            try {
                                _system.default.get({
                                    key: STORAGE_KEY,
                                    default: EMPTY_STORAGE_VALUE,
                                    success: (value)=>{
                                        if (!value || value === EMPTY_STORAGE_VALUE) return void finishLoading([]);
                                        try {
                                            finishLoading(JSON.parse(value));
                                        } catch (error) {
                                            console.log("parse notification history failed", error);
                                            finishLoading([]);
                                        }
                                    },
                                    fail: (data, code)=>{
                                        console.log("load notification history failed", code, data);
                                        finishLoading([]);
                                    }
                                });
                            } catch (error) {
                                console.log("notification storage unavailable", error);
                                finishLoading([]);
                            }
                        }
                        function persistHistory() {
                            try {
                                _system.default.set({
                                    key: STORAGE_KEY,
                                    value: JSON.stringify(history),
                                    fail: (data, code)=>{
                                        console.log("save notification history failed", code, data);
                                    }
                                });
                            } catch (error) {
                                console.log("notification history persistence unavailable", error);
                            }
                        }
                        function triggerVibration(type) {
                            try {
                                _system2.default.vibrate({
                                    mode: "call" === type ? "long" : "short"
                                });
                                if ("sms" === type) setTimeout(()=>{
                                    try {
                                        _system2.default.vibrate({
                                            mode: "short"
                                        });
                                    } catch (error) {
                                        console.log("second sms vibration unavailable", error);
                                    }
                                }, 420);
                            } catch (error) {
                                console.log("notification vibration unavailable", error);
                            }
                        }
                        function storeInHistory(item) {
                            if (item.preview) return;
                            const nextHistory = [
                                item
                            ];
                            for(let index = 0; index < history.length; index += 1){
                                if (history[index].id !== item.id) nextHistory.push(history[index]);
                                if (nextHistory.length >= HISTORY_LIMIT) break;
                            }
                            history = nextHistory;
                            persistHistory();
                        }
                        function receive(payload) {
                            if (!loaded) {
                                initialize(()=>receive(payload));
                                return null;
                            }
                            const item = normalizeNotification(payload);
                            storeInHistory(item);
                            activeNotification = item;
                            triggerVibration(item.type);
                            notifyListeners();
                            return cloneNotification(item);
                        }
                        function record(payload) {
                            if (!loaded) {
                                initialize(()=>record(payload));
                                return null;
                            }
                            const item = normalizeNotification(payload);
                            storeInHistory(item);
                            notifyListeners();
                            return cloneNotification(item);
                        }
                        function dismiss(notificationId) {
                            if (activeNotification && (!notificationId || activeNotification.id === notificationId)) {
                                activeNotification = null;
                                notifyListeners();
                            }
                        }
                        function getActive() {
                            return activeNotification ? cloneNotification(activeNotification) : null;
                        }
                        function getHistory() {
                            return history.map(cloneNotification);
                        }
                        function subscribe(listener) {
                            if ("function" != typeof listener) return;
                            if (listeners.indexOf(listener) < 0) listeners.push(listener);
                            initialize(()=>{
                                listener({
                                    active: getActive(),
                                    history: getHistory()
                                });
                            });
                        }
                        function unsubscribe(listener) {
                            const index = listeners.indexOf(listener);
                            if (index >= 0) listeners.splice(index, 1);
                        }
                        function clearHistory() {
                            history = [];
                            persistHistory();
                            notifyListeners();
                        }
                        var _default = exports["default"] = {
                            initialize,
                            receive,
                            record,
                            dismiss,
                            getActive,
                            getHistory,
                            subscribe,
                            unsubscribe,
                            clearHistory
                        };
                    },
                    "./src/common/step-tracker.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports["default"] = void 0;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.device"));
                        var _system2 = _interopRequireDefault($app_require$1("@app-module/system.sensor"));
                        var _system3 = _interopRequireDefault($app_require$1("@app-module/system.storage"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const STORAGE_PREFIX = "daily_steps_v1_";
                        const STEP_GOAL_STORAGE_KEY = "step_goal_v1";
                        const CALORIE_GOAL_STORAGE_KEY = "calorie_goal_v1";
                        const DURATION_GOAL_STORAGE_KEY = "duration_goal_v1";
                        const SAVE_DELAY_MS = 900;
                        const HISTORY_RETENTION_DAYS = 30;
                        const DEMO_INTERVAL_MS = 2600;
                        const ENABLE_SIMULATOR_STEP_GROWTH = true;
                        const MIN_STEP_INTERVAL_MS = 280;
                        const MAX_STEP_INTERVAL_MS = 1800;
                        const DEMO_HISTORY_TOTALS = {
                            "2026-07-22": 11308,
                            "2026-07-23": 5421,
                            "2026-07-24": 8976,
                            "2026-07-25": 6230,
                            "2026-07-26": 10184,
                            "2026-07-27": 7352
                        };
                        const DEMO_HISTORY_GOALS = {
                            "2026-07-22": 12000,
                            "2026-07-23": 6000,
                            "2026-07-24": 10000,
                            "2026-07-25": 7500,
                            "2026-07-26": 11000,
                            "2026-07-27": 8000
                        };
                        const DEMO_HISTORY_CALORIE_GOALS = {
                            "2026-07-22": 450,
                            "2026-07-23": 300,
                            "2026-07-24": 400,
                            "2026-07-25": 350,
                            "2026-07-26": 500,
                            "2026-07-27": 400
                        };
                        const DEMO_HISTORY_DURATION_GOALS = {
                            "2026-07-22": 45,
                            "2026-07-23": 30,
                            "2026-07-24": 40,
                            "2026-07-25": 35,
                            "2026-07-26": 60,
                            "2026-07-27": 30
                        };
                        const DEFAULT_STEP_GOAL = 8000;
                        const DEFAULT_CALORIE_GOAL = 400;
                        const DEFAULT_DURATION_GOAL = 30;
                        function pad(value) {
                            return value < 10 ? "0" + value : "" + value;
                        }
                        function dateKey(date) {
                            return date.getFullYear() + "-" + pad(date.getMonth() + 1) + "-" + pad(date.getDate());
                        }
                        function todayKey() {
                            return dateKey(new Date());
                        }
                        function dateKeyBefore(daysBefore) {
                            const now = new Date();
                            const target = new Date(now.getFullYear(), now.getMonth(), now.getDate() - daysBefore);
                            return dateKey(target);
                        }
                        function recentDateKeys() {
                            const keys = [];
                            for(let offset = 0; offset < 7; offset += 1)keys.push(dateKeyBefore(offset));
                            return keys;
                        }
                        function emptyHours() {
                            const result = [];
                            for(let index = 0; index < 24; index += 1)result.push(0);
                            return result;
                        }
                        function cloneHours(source) {
                            const result = emptyHours();
                            if (!Array.isArray(source)) return result;
                            for(let index = 0; index < 24; index += 1){
                                const value = Number(source[index]);
                                result[index] = isFinite(value) && value >= 0 ? Math.round(value) : 0;
                            }
                            return result;
                        }
                        function createDemoHistoryRecord(key) {
                            const total = Number(DEMO_HISTORY_TOTALS[key]);
                            if (!isFinite(total) || total <= 0) return null;
                            const progress = [
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0.02,
                                0.07,
                                0.13,
                                0.19,
                                0.25,
                                0.31,
                                0.39,
                                0.46,
                                0.53,
                                0.61,
                                0.68,
                                0.75,
                                0.82,
                                0.88,
                                0.93,
                                0.97,
                                1,
                                1
                            ];
                            const hours = [];
                            for(let index = 0; index < 24; index += 1)hours.push(Math.round(total * progress[index]));
                            return {
                                dateKey: key,
                                steps: Math.round(total),
                                hours: hours,
                                goal: DEMO_HISTORY_GOALS[key] || DEFAULT_STEP_GOAL,
                                calorieGoal: DEMO_HISTORY_CALORIE_GOALS[key] || DEFAULT_CALORIE_GOAL,
                                durationGoal: DEMO_HISTORY_DURATION_GOALS[key] || DEFAULT_DURATION_GOAL
                            };
                        }
                        const tracker = {
                            started: false,
                            loaded: false,
                            loadingKey: "",
                            dateKey: "",
                            steps: 0,
                            goal: DEFAULT_STEP_GOAL,
                            calorieGoal: DEFAULT_CALORIE_GOAL,
                            durationGoal: DEFAULT_DURATION_GOAL,
                            hours: emptyHours(),
                            listeners: [],
                            dayClosedListeners: [],
                            sensorActive: false,
                            sensorEvents: 0,
                            isSimulator: false,
                            deviceChecked: false,
                            demoTimerId: null,
                            saveTimerId: null,
                            needsInitialSave: false,
                            lastMagnitude: null,
                            filteredMagnitude: null,
                            motionBaseline: 9.8,
                            abovePeak: false,
                            lastStepAt: 0,
                            start () {
                                if (this.started) return;
                                this.started = true;
                                this.ensureToday();
                                this.detectDevice();
                                this.startSensor();
                            },
                            stop () {
                                this.started = false;
                                this.stopDemo();
                                this.flushSave();
                                if (this.sensorActive) {
                                    try {
                                        _system2.default.unsubscribeAccelerometer();
                                    } catch (error) {
                                        console.log("step sensor unsubscribe failed", error);
                                    }
                                }
                                this.sensorActive = false;
                                this.resetMotionState();
                            },
                            onAppShow () {
                                const wasStarted = this.started;
                                this.start();
                                this.ensureToday();
                                if (!wasStarted) return;
                                if (this.sensorActive) {
                                    try {
                                        _system2.default.unsubscribeAccelerometer();
                                    } catch (error) {
                                        console.log("step sensor resume unsubscribe failed", error);
                                    }
                                }
                                this.sensorActive = false;
                                this.resetMotionState();
                                this.startSensor();
                                this.startDemoIfNeeded();
                            },
                            onAppHide () {
                                this.flushSave();
                            },
                            subscribe (callback) {
                                if (!callback) return;
                                let exists = false;
                                for(let index = 0; index < this.listeners.length; index += 1)if (this.listeners[index] === callback) exists = true;
                                if (!exists) this.listeners.push(callback);
                                this.start();
                                callback(this.snapshot());
                            },
                            unsubscribe (callback) {
                                const next = [];
                                for(let index = 0; index < this.listeners.length; index += 1)if (this.listeners[index] !== callback) next.push(this.listeners[index]);
                                this.listeners = next;
                            },
                            subscribeDayClosed (callback) {
                                if (!callback) return;
                                for(let index = 0; index < this.dayClosedListeners.length; index += 1)if (this.dayClosedListeners[index] === callback) return;
                                this.dayClosedListeners.push(callback);
                            },
                            unsubscribeDayClosed (callback) {
                                const next = [];
                                for(let index = 0; index < this.dayClosedListeners.length; index += 1)if (this.dayClosedListeners[index] !== callback) next.push(this.dayClosedListeners[index]);
                                this.dayClosedListeners = next;
                            },
                            notifyDayClosed (record) {
                                const listeners = this.dayClosedListeners.slice();
                                for(let index = 0; index < listeners.length; index += 1){
                                    try {
                                        listeners[index](record);
                                    } catch (error) {
                                        console.log("day closed listener failed", error);
                                    }
                                }
                            },
                            snapshot () {
                                this.ensureToday();
                                const now = new Date();
                                const currentHour = now.getHours();
                                const visibleHours = [];
                                for(let index = 0; index <= currentHour; index += 1)visibleHours.push(this.hours[index] || 0);
                                return {
                                    dateKey: this.dateKey,
                                    steps: this.steps,
                                    goal: this.goal,
                                    calorieGoal: this.calorieGoal,
                                    durationGoal: this.durationGoal,
                                    hours: this.hours.slice(),
                                    visibleHours: visibleHours,
                                    currentHour: currentHour,
                                    loaded: this.loaded,
                                    isSimulator: this.isSimulator,
                                    sensorActive: this.sensorActive
                                };
                            },
                            notify () {
                                const value = this.snapshot();
                                const listeners = this.listeners.slice();
                                for(let index = 0; index < listeners.length; index += 1){
                                    try {
                                        listeners[index](value);
                                    } catch (error) {
                                        console.log("step listener failed", error);
                                    }
                                }
                            },
                            ensureToday () {
                                const key = todayKey();
                                if (this.dateKey === key && (this.loaded || this.loadingKey === key)) return;
                                if (this.dateKey && this.loaded) {
                                    if (this.saveTimerId) {
                                        clearTimeout(this.saveTimerId);
                                        this.saveTimerId = null;
                                    }
                                    const closedRecord = {
                                        dateKey: this.dateKey,
                                        steps: this.steps,
                                        hours: cloneHours(this.hours),
                                        goal: this.goal,
                                        calorieGoal: this.calorieGoal,
                                        durationGoal: this.durationGoal
                                    };
                                    this.saveRecord(closedRecord.dateKey, closedRecord.steps, closedRecord.hours, closedRecord.goal, closedRecord.calorieGoal, closedRecord.durationGoal, ()=>this.notifyDayClosed(closedRecord));
                                }
                                this.dateKey = key;
                                this.steps = 0;
                                this.hours = emptyHours();
                                this.loaded = false;
                                this.deleteExpiredHistory();
                                this.loadDay(key);
                            },
                            loadRecentHistory (callback) {
                                const keys = recentDateKeys();
                                const rows = [];
                                let pending = keys.length;
                                const finishOne = ()=>{
                                    pending -= 1;
                                    if (pending <= 0 && callback) callback(rows);
                                };
                                for(let index = 0; index < keys.length; index += 1){
                                    const key = keys[index];
                                    rows.push({
                                        dateKey: key,
                                        steps: key === this.dateKey ? this.steps : 0,
                                        hours: key === this.dateKey ? cloneHours(this.hours) : emptyHours(),
                                        goal: key === this.dateKey ? this.goal : DEMO_HISTORY_GOALS[key] || DEFAULT_STEP_GOAL,
                                        calorieGoal: key === this.dateKey ? this.calorieGoal : DEMO_HISTORY_CALORIE_GOALS[key] || DEFAULT_CALORIE_GOAL,
                                        durationGoal: key === this.dateKey ? this.durationGoal : DEMO_HISTORY_DURATION_GOALS[key] || DEFAULT_DURATION_GOAL
                                    });
                                    if (key === this.dateKey) {
                                        finishOne();
                                        continue;
                                    }
                                    try {
                                        _system3.default.get({
                                            key: STORAGE_PREFIX + key,
                                            default: "",
                                            success: (value)=>{
                                                if (value) {
                                                    try {
                                                        const parsed = JSON.parse(value);
                                                        const storedSteps = Number(parsed.steps);
                                                        rows[index].steps = isFinite(storedSteps) && storedSteps >= 0 ? Math.round(storedSteps) : 0;
                                                        rows[index].hours = cloneHours(parsed.hours);
                                                        const storedGoal = Math.round(Number(parsed.goal) || 0);
                                                        rows[index].goal = storedGoal >= 1000 ? storedGoal : DEMO_HISTORY_GOALS[key] || DEFAULT_STEP_GOAL;
                                                        const storedCalorieGoal = Math.round(Number(parsed.calorieGoal) || 0);
                                                        const storedDurationGoal = Math.round(Number(parsed.durationGoal) || 0);
                                                        rows[index].calorieGoal = storedCalorieGoal >= 1 ? storedCalorieGoal : DEMO_HISTORY_CALORIE_GOALS[key] || DEFAULT_CALORIE_GOAL;
                                                        rows[index].durationGoal = storedDurationGoal >= 1 ? storedDurationGoal : DEMO_HISTORY_DURATION_GOALS[key] || DEFAULT_DURATION_GOAL;
                                                        if (storedGoal < 1000 || storedCalorieGoal < 1 || storedDurationGoal < 1) this.saveRecord(key, rows[index].steps, rows[index].hours, rows[index].goal, rows[index].calorieGoal, rows[index].durationGoal);
                                                    } catch (error) {
                                                        console.log("parse history steps failed", key, error);
                                                    }
                                                } else {
                                                    const demoRecord = createDemoHistoryRecord(key);
                                                    if (demoRecord) {
                                                        rows[index].steps = demoRecord.steps;
                                                        rows[index].hours = cloneHours(demoRecord.hours);
                                                        rows[index].goal = demoRecord.goal;
                                                        rows[index].calorieGoal = demoRecord.calorieGoal;
                                                        rows[index].durationGoal = demoRecord.durationGoal;
                                                        this.saveRecord(key, demoRecord.steps, demoRecord.hours, demoRecord.goal, demoRecord.calorieGoal, demoRecord.durationGoal);
                                                    }
                                                }
                                                finishOne();
                                            },
                                            fail: (data, code)=>{
                                                console.log("load history steps failed", key, code, data);
                                                finishOne();
                                            }
                                        });
                                    } catch (error) {
                                        console.log("history step storage unavailable", key, error);
                                        finishOne();
                                    }
                                }
                            },
                            deleteExpiredHistory () {
                                const expiredKey = dateKeyBefore(HISTORY_RETENTION_DAYS + 1);
                                try {
                                    _system3.default.delete({
                                        key: STORAGE_PREFIX + expiredKey,
                                        fail: (data, code)=>{
                                            console.log("delete expired steps failed", expiredKey, code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("delete expired steps unavailable", expiredKey, error);
                                }
                            },
                            loadDay (key) {
                                if (this.loadingKey === key) return;
                                this.loadingKey = key;
                                try {
                                    _system3.default.get({
                                        key: STORAGE_PREFIX + key,
                                        default: "",
                                        success: (value)=>{
                                            if (this.dateKey !== key) return;
                                            this.loadingKey = "";
                                            this.needsInitialSave = !value;
                                            if (value) {
                                                try {
                                                    const parsed = JSON.parse(value);
                                                    const storedSteps = Number(parsed.steps);
                                                    this.steps = isFinite(storedSteps) && storedSteps >= 0 ? Math.round(storedSteps) : 0;
                                                    this.hours = cloneHours(parsed.hours);
                                                } catch (error) {
                                                    console.log("parse daily steps failed", error);
                                                }
                                            }
                                            this.loadCurrentGoals(key, ()=>this.completeDayLoad(key));
                                        },
                                        fail: (data, code)=>{
                                            if (this.dateKey !== key) return;
                                            this.needsInitialSave = false;
                                            console.log("load daily steps failed", code, data);
                                            this.loadCurrentGoals(key, ()=>this.completeDayLoad(key));
                                        }
                                    });
                                } catch (error) {
                                    console.log("daily step storage unavailable", error);
                                    this.loadCurrentGoals(key, ()=>this.completeDayLoad(key));
                                }
                            },
                            loadCurrentGoals (key, callback) {
                                let pending = 3;
                                const finishOne = ()=>{
                                    pending -= 1;
                                    if (pending <= 0 && this.dateKey === key && callback) callback();
                                };
                                const readGoal = (storageKey, minimum, maximum, applyValue)=>{
                                    try {
                                        _system3.default.get({
                                            key: storageKey,
                                            default: "",
                                            success: (value)=>{
                                                if (this.dateKey === key) {
                                                    const storedGoal = Math.round(Number(value) || 0);
                                                    if (storedGoal >= minimum && storedGoal <= maximum) applyValue(storedGoal);
                                                }
                                                finishOne();
                                            },
                                            fail: (data, code)=>{
                                                console.log("load current goal failed", storageKey, code, data);
                                                finishOne();
                                            }
                                        });
                                    } catch (error) {
                                        console.log("current goal storage unavailable", storageKey, error);
                                        finishOne();
                                    }
                                };
                                readGoal(STEP_GOAL_STORAGE_KEY, 1000, 99999, (value)=>{
                                    this.goal = value;
                                });
                                readGoal(CALORIE_GOAL_STORAGE_KEY, 50, 9999, (value)=>{
                                    this.calorieGoal = value;
                                });
                                readGoal(DURATION_GOAL_STORAGE_KEY, 5, 1440, (value)=>{
                                    this.durationGoal = value;
                                });
                            },
                            completeDayLoad (key) {
                                if (this.dateKey !== key) return;
                                this.loadingKey = "";
                                this.loaded = true;
                                if (this.needsInitialSave) {
                                    this.needsInitialSave = false;
                                    this.save();
                                }
                                this.startDemoIfNeeded();
                                this.notify();
                            },
                            detectDevice () {
                                try {
                                    _system.default.getInfo({
                                        success: (info)=>{
                                            const signature = [
                                                info && info.brand,
                                                info && info.manufacturer,
                                                info && info.model,
                                                info && info.product
                                            ].join(" ").toLowerCase();
                                            this.isSimulator = signature.indexOf("virtual") >= 0 || signature.indexOf("emulator") >= 0 || signature.indexOf("goldfish") >= 0 || signature.indexOf("qemu") >= 0 || signature.indexOf("sdk") >= 0;
                                            this.deviceChecked = true;
                                            console.log("step device", signature, "simulator", this.isSimulator);
                                            this.startDemoIfNeeded();
                                            this.notify();
                                        },
                                        fail: (data, code)=>{
                                            this.deviceChecked = true;
                                            console.log("step device info failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    this.deviceChecked = true;
                                    console.log("step device info unavailable", error);
                                }
                            },
                            startSensor () {
                                if (this.sensorActive) return;
                                try {
                                    _system2.default.subscribeAccelerometer({
                                        interval: "normal",
                                        callback: (data)=>this.handleAcceleration(data),
                                        fail: (data, code)=>{
                                            this.sensorActive = false;
                                            console.log("step accelerometer failed", code, data);
                                        }
                                    });
                                    this.sensorActive = true;
                                } catch (error) {
                                    this.sensorActive = false;
                                    console.log("step accelerometer unavailable", error);
                                }
                            },
                            resetMotionState () {
                                this.lastMagnitude = null;
                                this.filteredMagnitude = null;
                                this.motionBaseline = 9.8;
                                this.abovePeak = false;
                                this.lastStepAt = 0;
                            },
                            handleAcceleration (data) {
                                if (!this.started || !data || this.isSimulator) return;
                                const x = Number(data.x);
                                const y = Number(data.y);
                                const z = Number(data.z);
                                if (!isFinite(x) || !isFinite(y) || !isFinite(z)) return;
                                this.sensorEvents += 1;
                                const magnitude = Math.sqrt(x * x + y * y + z * z);
                                if (null === this.filteredMagnitude) {
                                    this.filteredMagnitude = magnitude;
                                    this.motionBaseline = magnitude;
                                    this.lastMagnitude = magnitude;
                                    return;
                                }
                                this.filteredMagnitude = 0.72 * this.filteredMagnitude + 0.28 * magnitude;
                                this.motionBaseline = 0.985 * this.motionBaseline + 0.015 * this.filteredMagnitude;
                                const movement = this.filteredMagnitude - this.motionBaseline;
                                const now = Date.now();
                                const elapsed = now - this.lastStepAt;
                                if (movement > 1.05 && !this.abovePeak) {
                                    this.abovePeak = true;
                                    if (elapsed >= MIN_STEP_INTERVAL_MS && (0 === this.lastStepAt || elapsed <= MAX_STEP_INTERVAL_MS)) this.addSteps(1);
                                    else if (0 === this.lastStepAt) this.lastStepAt = now;
                                }
                                if (movement < 0.32) this.abovePeak = false;
                                this.lastMagnitude = magnitude;
                            },
                            startDemoIfNeeded () {
                                if (!ENABLE_SIMULATOR_STEP_GROWTH || !this.started || !this.loaded || !this.isSimulator || this.demoTimerId) return;
                                this.demoTimerId = setInterval(()=>{
                                    const now = new Date();
                                    const delta = 2 + (now.getSeconds() + now.getMinutes()) % 4;
                                    this.addSteps(delta);
                                }, DEMO_INTERVAL_MS);
                            },
                            stopDemo () {
                                if (!this.demoTimerId) return;
                                clearInterval(this.demoTimerId);
                                this.demoTimerId = null;
                            },
                            addSteps (count) {
                                this.ensureToday();
                                if (!this.loaded) return;
                                const delta = Math.max(1, Math.round(Number(count) || 1));
                                this.steps += delta;
                                const hour = new Date().getHours();
                                this.hours[hour] = this.steps;
                                this.lastStepAt = Date.now();
                                this.scheduleSave();
                                this.notify();
                            },
                            setGoal (value) {
                                const nextGoal = Math.max(1000, Math.round(Number(value) || DEFAULT_STEP_GOAL));
                                this.goal = nextGoal;
                                if (this.loaded) this.scheduleSave();
                                this.notify();
                            },
                            setMetricGoal (metric, value) {
                                if ("calories" === metric) this.calorieGoal = Math.max(1, Math.round(Number(value) || DEFAULT_CALORIE_GOAL));
                                else if ("duration" === metric) this.durationGoal = Math.max(1, Math.round(Number(value) || DEFAULT_DURATION_GOAL));
                                else this.goal = Math.max(1000, Math.round(Number(value) || DEFAULT_STEP_GOAL));
                                if (this.loaded) this.scheduleSave();
                                this.notify();
                            },
                            scheduleSave () {
                                if (this.saveTimerId) clearTimeout(this.saveTimerId);
                                this.saveTimerId = setTimeout(()=>{
                                    this.saveTimerId = null;
                                    this.save();
                                }, SAVE_DELAY_MS);
                            },
                            save () {
                                if (!this.loaded || !this.dateKey) return;
                                this.saveRecord(this.dateKey, this.steps, this.hours, this.goal, this.calorieGoal, this.durationGoal);
                            },
                            saveRecord (key, steps, hours, goal, calorieGoal, durationGoal, callback) {
                                if (!key) return;
                                try {
                                    _system3.default.set({
                                        key: STORAGE_PREFIX + key,
                                        value: JSON.stringify({
                                            dateKey: key,
                                            steps: Math.max(0, Math.round(Number(steps) || 0)),
                                            hours: cloneHours(hours),
                                            goal: Math.max(1000, Math.round(Number(goal) || DEFAULT_STEP_GOAL)),
                                            calorieGoal: Math.max(1, Math.round(Number(calorieGoal) || DEFAULT_CALORIE_GOAL)),
                                            durationGoal: Math.max(1, Math.round(Number(durationGoal) || DEFAULT_DURATION_GOAL))
                                        }),
                                        success: ()=>{
                                            if (callback) callback();
                                        },
                                        fail: (data, code)=>{
                                            console.log("save daily steps failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save daily steps unavailable", error);
                                }
                            },
                            flushSave () {
                                if (this.saveTimerId) {
                                    clearTimeout(this.saveTimerId);
                                    this.saveTimerId = null;
                                }
                                this.save();
                            }
                        };
                        var _default = exports["default"] = tracker;
                    },
                    "./src/common/weather-cities.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports.DEFAULT_CITY_ID = void 0;
                        exports.getCities = getCities;
                        exports.getCity = getCity;
                        exports.getDefaultCustomCities = getDefaultCustomCities;
                        const DEFAULT_CITY_ID = exports.DEFAULT_CITY_ID = "beijing";
                        const CITIES = [
                            {
                                id: "beijing",
                                locationId: "101010100",
                                name: "北京",
                                detailName: "北京市",
                                administrativeArea: "北京市",
                                country: "中国",
                                isBase: true
                            },
                            {
                                id: "shanghai",
                                locationId: "101020100",
                                name: "上海",
                                detailName: "上海市",
                                administrativeArea: "上海市",
                                country: "中国",
                                isBase: true
                            },
                            {
                                id: "guangzhou",
                                locationId: "101280101",
                                name: "广州",
                                detailName: "广州市",
                                administrativeArea: "广东省",
                                country: "中国",
                                isBase: true
                            },
                            {
                                id: "shenzhen",
                                locationId: "101280601",
                                name: "深圳",
                                detailName: "深圳市",
                                administrativeArea: "广东省",
                                country: "中国",
                                isBase: true
                            }
                        ];
                        const DEFAULT_CUSTOM_CITIES = [
                            {
                                id: "wuhan",
                                locationId: "101200101",
                                name: "武汉",
                                detailName: "武汉市",
                                administrativeArea: "湖北省",
                                country: "中国",
                                isBase: false,
                                weatherKey: "wuhan"
                            }
                        ];
                        function copyCity(city) {
                            return {
                                id: city.id,
                                locationId: city.locationId,
                                administrativeCode: city.administrativeCode || "",
                                name: city.name,
                                detailName: city.detailName,
                                administrativeArea: city.administrativeArea,
                                country: city.country,
                                isBase: city.isBase,
                                weatherKey: city.weatherKey || city.id,
                                weatherLocationName: city.weatherLocationName || "",
                                weatherAdmName: city.weatherAdmName || "",
                                weatherProvinceName: city.weatherProvinceName || "",
                                weatherCityName: city.weatherCityName || "",
                                isCurrentLocation: !!city.isCurrentLocation,
                                locationReady: false !== city.locationReady,
                                longitude: city.longitude,
                                latitude: city.latitude,
                                deletable: false !== city.deletable
                            };
                        }
                        function getCities() {
                            return CITIES.map(copyCity);
                        }
                        function getDefaultCustomCities() {
                            return DEFAULT_CUSTOM_CITIES.map(copyCity);
                        }
                        function getCity(id, customCities) {
                            for(let index = 0; index < CITIES.length; index += 1)if (CITIES[index].id === id) return copyCity(CITIES[index]);
                            const additions = customCities || [];
                            for(let index = 0; index < additions.length; index += 1)if (additions[index].id === id) return copyCity(additions[index]);
                            return copyCity(CITIES[0]);
                        }
                    },
                    "./src/common/weather-service.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports.fetchLiveWeather = fetchLiveWeather;
                        exports.fetchLiveWeatherByCoordinates = fetchLiveWeatherByCoordinates;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.fetch"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const WEATHER_API_URL = "http://10.0.2.2:8790/api/weather";
                        const WEATHER_ICON_ROOT = "/common/weather-icons/";
                        const SUPPORTED_ICON_CODES = "|100|101|102|103|104|150|151|152|153|154|300|301|302|303|304|305|306|307|308|309|310|311|312|313|314|315|316|317|318|350|351|399|400|401|402|403|404|405|406|407|408|409|410|456|457|499|500|501|502|503|504|507|508|509|510|511|512|513|514|515|900|901|999|";
                        function safeIconCode(value) {
                            const code = String(value || "");
                            return SUPPORTED_ICON_CODES.indexOf("|" + code + "|") >= 0 ? code : "999";
                        }
                        function splitAdministrativeArea(value) {
                            const parts = String(value || "").split("·");
                            const result = [];
                            for(let index = 0; index < parts.length; index += 1){
                                const part = parts[index].trim();
                                if (part) result.push(part);
                            }
                            return result;
                        }
                        function buildWeatherUrl(city) {
                            const areas = splitAdministrativeArea(city.administrativeArea);
                            const isDistrict = areas.length > 1;
                            const locationName = city.weatherLocationName || city.detailName || city.name;
                            const admName = city.weatherAdmName || (isDistrict ? areas[0] : areas[0] || city.administrativeArea);
                            const locationId = isDistrict ? "" : city.locationId || "";
                            return WEATHER_API_URL + "?locationId=" + encodeURIComponent(locationId) + "&location=" + encodeURIComponent(locationName) + "&adm=" + encodeURIComponent(admName);
                        }
                        function normalizeForecastItem(item) {
                            const icon = safeIconCode(item.icon);
                            return {
                                weekday: item.weekday || "--",
                                text: item.text || "--",
                                icon: icon,
                                iconSrc: WEATHER_ICON_ROOT + icon + ".png",
                                min: item.min || "--",
                                max: item.max || "--",
                                temperature: (item.min || "--") + "°~" + (item.max || "--") + "°"
                            };
                        }
                        function normalizeWeather(data) {
                            if (!data || "200" !== data.code || !data.live || !data.now) throw new Error("Invalid live weather response");
                            const forecast = [];
                            let highest = -100;
                            let lowest = 100;
                            const source = data.forecast || [];
                            for(let index = 0; index < source.length; index += 1){
                                const item = normalizeForecastItem(source[index]);
                                forecast.push(item);
                                highest = Math.max(highest, Number(item.max));
                                lowest = Math.min(lowest, Number(item.min));
                            }
                            while(forecast.length < 3)forecast.push(normalizeForecastItem({}));
                            const icon = safeIconCode(data.now.icon);
                            return {
                                live: true,
                                source: data.source || "QWeather",
                                updatedAt: data.updatedAt || "",
                                observedAt: data.observedAt || "",
                                location: data.location || {},
                                now: {
                                    temp: data.now.temp,
                                    feelsLike: data.now.feelsLike,
                                    humidity: data.now.humidity,
                                    visibility: data.now.visibility,
                                    text: data.now.text,
                                    icon: icon,
                                    iconSrc: WEATHER_ICON_ROOT + icon + ".png"
                                },
                                forecast: forecast.slice(0, 3),
                                highest: -100 === highest ? "--" : "" + highest,
                                lowest: 100 === lowest ? "--" : "" + lowest
                            };
                        }
                        function fetchLiveWeather(city, success, fail) {
                            _system.default.fetch({
                                url: buildWeatherUrl(city),
                                method: "GET",
                                responseType: "json",
                                success: (response)=>{
                                    try {
                                        if (!response || 200 !== response.code) throw new Error("Weather server HTTP error");
                                        success(normalizeWeather(response.data));
                                    } catch (error) {
                                        if (fail) fail(error);
                                    }
                                },
                                fail: (data, code)=>{
                                    if (fail) fail({
                                        data: data,
                                        code: code
                                    });
                                }
                            });
                        }
                        function fetchLiveWeatherByCoordinates(longitude, latitude, success, fail) {
                            const numericLongitude = Number(longitude);
                            const numericLatitude = Number(latitude);
                            if (!isFinite(numericLongitude) || !isFinite(numericLatitude)) {
                                if (fail) fail(new Error("Invalid location coordinates"));
                                return;
                            }
                            const coordinateText = numericLongitude.toFixed(5) + "," + numericLatitude.toFixed(5);
                            fetchLiveWeather({
                                id: "current-location",
                                locationId: "",
                                name: coordinateText,
                                detailName: coordinateText,
                                administrativeArea: "",
                                weatherLocationName: coordinateText,
                                weatherAdmName: ""
                            }, success, fail);
                        }
                    },
                    "./src/manifest.json" (module) {
                        "use strict";
                        module.exports = JSON.parse('{"package":"com.application.watch.redesign","name":"smart-band-ui-redesign","versionName":"1.0.12","versionCode":13,"minPlatformVersion":1000,"icon":"/common/background.png","deviceTypeList":["watch"],"features":[{"name":"system.app"},{"name":"system.router"},{"name":"system.battery"},{"name":"system.event"},{"name":"system.file"},{"name":"system.storage"},{"name":"system.fetch"},{"name":"system.geolocation"},{"name":"system.sensor"},{"name":"system.device"},{"name":"system.vibrator"},{"name":"system.interconnect"},{"name":"system.audio"},{"name":"system.record"},{"name":"system.uploadtask"}],"permissions":[{"name":"hapjs.permission.LOCATION"}],"config":{"logLevel":"log","designWidth":"device-width","background":{"features":["system.audio"]}},"router":{"entry":"pages/index","pages":{"pages/index":{"component":"index"},"pages/customize":{"component":"customize"},"pages/backgrounds":{"component":"backgrounds"},"pages/actions":{"component":"actions"},"pages/weather":{"component":"weather"},"pages/sport":{"component":"sport"},"pages/sportmode":{"component":"sportmode"},"pages/sportheartrate":{"component":"sportheartrate"},"pages/sportexit":{"component":"sportexit"},"pages/heartratealert":{"component":"heartratealert"},"pages/health":{"component":"health"},"pages/music":{"component":"music"},"pages/musicplayer":{"component":"musicplayer"},"pages/musiclist":{"component":"musiclist"},"pages/musicvolume":{"component":"musicvolume"},"pages/heartrate":{"component":"heartrate"},"pages/bloodpressure":{"component":"bloodpressure"},"pages/steps":{"component":"steps"},"pages/celebration":{"component":"celebration"},"pages/chargingnotice":{"component":"chargingnotice"},"pages/cities":{"component":"cities"},"pages/weatherdetail":{"component":"weatherdetail"},"pages/notificationpreview":{"component":"notificationpreview"},"pages/notificationhub":{"component":"notificationhub"},"pages/notification":{"component":"notification"},"pages/notifications":{"component":"notifications"},"pages/assistant":{"component":"assistant"}}}}');
                    }
                };
                var __webpack_module_cache__ = {};
                function __webpack_require__(moduleId) {
                    var cachedModule = __webpack_module_cache__[moduleId];
                    if (void 0 !== cachedModule) return cachedModule.exports;
                    var module = __webpack_module_cache__[moduleId] = {
                        exports: {}
                    };
                    __webpack_modules__[moduleId](module, module.exports, __webpack_require__);
                    return module.exports;
                }
                (()=>{
                    __webpack_require__.g = (()=>{
                        if ('object' == typeof globalThis) return globalThis;
                        try {
                            return this || new Function('return this')();
                        } catch (e) {
                            if ('object' == typeof window) return window;
                        }
                    })();
                })();
                (()=>{
                    __webpack_require__.rv = ()=>"1.7.12";
                })();
                (()=>{
                    __webpack_require__.ruid = "bundler=rspack@1.7.12";
                })();
                var __webpack_exports__ = {};
                (()=>{
                    var $app_style$ = [];
                    var $app_script$ = function __scriptModule__(module, exports, $app_require$1) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports.default = void 0;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.audio"));
                        var _system2 = _interopRequireDefault($app_require$1("@app-module/system.file"));
                        var _system3 = _interopRequireDefault($app_require$1("@app-module/system.geolocation"));
                        var _system4 = _interopRequireDefault($app_require$1("@app-module/system.router"));
                        var _system5 = _interopRequireDefault($app_require$1("@app-module/system.storage"));
                        var _customization = __webpack_require__("./src/common/customization.js");
                        var _weatherCities = __webpack_require__("./src/common/weather-cities.js");
                        var _stepTracker = _interopRequireDefault(__webpack_require__("./src/common/step-tracker.js"));
                        var _weatherService = __webpack_require__("./src/common/weather-service.js");
                        var _notificationCenter = _interopRequireDefault(__webpack_require__("./src/common/notification-center.js"));
                        var _notificationBridge = _interopRequireDefault(__webpack_require__("./src/common/notification-bridge.js"));
                        var _dailySync = _interopRequireDefault(__webpack_require__("./src/common/daily-sync.js"));
                        var _chargingMonitor = _interopRequireDefault(__webpack_require__("./src/common/charging-monitor.js"));
                        var _healthRecords = _interopRequireDefault(__webpack_require__("./src/common/health-records.js"));
                        var _musicPlayer = __webpack_require__("./src/common/music-player.js");
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const CUSTOM_CITIES_STORAGE_KEY = "weather_custom_cities_v1";
                        const EMPTY_STORAGE_VALUE = "__NO_CUSTOM_CITY_DATA__";
                        const LOCATION_REFRESH_MS = 60000;
                        const LOCATION_RESOLVE_MS = 180000;
                        const ALL_GOALS_CELEBRATION_DATE_KEY = "all_sport_goals_reward_last_date_v1";
                        const SPECIAL_SPORT_MODE_STORAGE_KEY = "special_sport_mode_active_v1";
                        const SPECIAL_SPORT_HEART_RATE_URI = "/common/emulator-heart-rate.json";
                        const SPECIAL_SPORT_HEART_RATE_POLL_MS = 1800;
                        const SPECIAL_SPORT_HEART_RATE_START_DELAY_MS = 1600;
                        const SPECIAL_SPORT_HEART_RATE_HIGH_BPM = 160;
                        const SPECIAL_SPORT_HIGH_SAMPLE_COUNT = 3;
                        const SPECIAL_SPORT_ALERT_COOLDOWN_MS = 300000;
                        const SPECIAL_SPORT_RECORD_INTERVAL_MS = 300000;
                        const SPECIAL_SPORT_METRICS = [
                            "steps",
                            "calories",
                            "duration"
                        ];
                        function createPendingLocation(name) {
                            return {
                                id: "current-location",
                                locationId: "",
                                name: name || "正在定位…",
                                detailName: name || "正在定位…",
                                administrativeArea: "当前位置",
                                country: "中国",
                                isBase: true,
                                isCurrentLocation: true,
                                locationReady: false,
                                deletable: false,
                                weatherLocationName: "",
                                weatherAdmName: "",
                                weatherProvinceName: "",
                                weatherCityName: ""
                            };
                        }
                        function copyLocationCity(city) {
                            const source = city || createPendingLocation();
                            return {
                                id: source.id,
                                locationId: source.locationId || "",
                                name: source.name,
                                detailName: source.detailName,
                                administrativeArea: source.administrativeArea,
                                country: source.country || "中国",
                                isBase: true,
                                isCurrentLocation: true,
                                locationReady: !!source.locationReady,
                                deletable: false,
                                longitude: source.longitude,
                                latitude: source.latitude,
                                weatherLocationName: source.weatherLocationName || "",
                                weatherAdmName: source.weatherAdmName || "",
                                weatherProvinceName: source.weatherProvinceName || "",
                                weatherCityName: source.weatherCityName || ""
                            };
                        }
                        var _default = exports.default = {
                            data: {
                                keepScreenOn: false,
                                customization: {
                                    backgroundId: _customization.DEFAULT_BACKGROUND_ID,
                                    actionId: _customization.DEFAULT_ACTION_ID,
                                    cityId: _weatherCities.DEFAULT_CITY_ID
                                },
                                customCities: (0, _weatherCities.getDefaultCustomCities)(),
                                customCitiesLoaded: false,
                                customCityReadyListeners: [],
                                currentLocation: createPendingLocation(),
                                currentLocationWeather: null,
                                currentLocationListeners: [],
                                locationTracking: false,
                                locationReading: false,
                                locationRequestSerial: 0,
                                locationRefreshTimerId: null,
                                lastLocationResolveAt: 0,
                                lastLongitude: null,
                                lastLatitude: null,
                                appVisible: false,
                                celebrationDateLoaded: false,
                                lastCelebrationDate: "",
                                celebrationShowing: false,
                                celebrationListener: null,
                                latestStepSnapshot: null,
                                notificationShowing: false,
                                chargingNoticeShowing: false,
                                dailySyncDayClosedListener: null,
                                specialSportModeActive: false,
                                specialSportModeLoaded: false,
                                specialSportMetric: "steps",
                                specialSportHeartRate: 78,
                                specialSportHeartRateReading: false,
                                specialSportHeartRateStartTimerId: null,
                                specialSportHeartRateTimerId: null,
                                specialSportHeartRateListeners: [],
                                specialSportHighSampleCount: 0,
                                specialSportHeartAlertShowing: false,
                                specialSportHeartAlertLastAt: 0,
                                specialSportHeartLastRecordAt: 0,
                                musicIndex: 0,
                                musicIsPlaying: false,
                                musicProgress: 0,
                                musicPrepared: false,
                                musicError: "",
                                musicListeners: [],
                                musicPauseTimerId: null,
                                musicVolumeRestoreTimerId: null,
                                musicPauseRestoreVolume: 1,
                                musicPausedMuted: false
                            },
                            onCreate () {
                                console.log("smart-band-ui-redesign created");
                                this.initializeMusicAudio();
                                this.loadSpecialSportMode();
                                _dailySync.default.start();
                                this.data.dailySyncDayClosedListener = (record)=>{
                                    _dailySync.default.enqueueClosedDay(record);
                                };
                                _stepTracker.default.subscribeDayClosed(this.data.dailySyncDayClosedListener);
                                this.loadCustomCities();
                                this.loadLastCelebrationDate();
                                this.data.celebrationListener = (snapshot)=>{
                                    this.handleCelebrationStepSnapshot(snapshot);
                                };
                                _stepTracker.default.subscribe(this.data.celebrationListener);
                                _notificationCenter.default.initialize(()=>{
                                    _notificationBridge.default.start((item)=>{
                                        this.handleComputerNotification(item);
                                    });
                                });
                                _chargingMonitor.default.start(()=>{
                                    this.handleChargingStarted();
                                });
                            },
                            onShow () {
                                this.data.appVisible = true;
                                this.resumeRuntime();
                                this.ensureWakeableScreen();
                                this.startLocationTracking();
                                this.evaluateAllGoalsCelebration();
                            },
                            onHide () {
                                this.data.appVisible = false;
                                this.prepareRuntimeForStandby();
                                this.stopLocationTracking();
                                this.cancelMusicPauseTimers(true);
                                try {
                                    _system.default.stop();
                                } catch (error) {
                                    console.log("stop music on destroy failed", error);
                                }
                                this.data.musicListeners = [];
                                this.releaseWakeableScreen();
                            },
                            resumeRuntime () {
                                _dailySync.default.onAppShow();
                                _stepTracker.default.onAppShow();
                                _notificationBridge.default.resume();
                                _chargingMonitor.default.resume();
                                if (this.data.specialSportModeActive) this.scheduleSpecialSportHeartRateMonitor(500);
                            },
                            prepareRuntimeForStandby () {
                                _stepTracker.default.onAppHide();
                                _dailySync.default.onAppHide();
                            },
                            onDestroy () {
                                this.data.appVisible = false;
                                if (this.data.dailySyncDayClosedListener) {
                                    _stepTracker.default.unsubscribeDayClosed(this.data.dailySyncDayClosedListener);
                                    this.data.dailySyncDayClosedListener = null;
                                }
                                _dailySync.default.stop();
                                if (this.data.celebrationListener) {
                                    _stepTracker.default.unsubscribe(this.data.celebrationListener);
                                    this.data.celebrationListener = null;
                                }
                                _notificationBridge.default.stop();
                                _chargingMonitor.default.stop();
                                this.stopSpecialSportHeartRateMonitor();
                                this.data.specialSportHeartRateListeners = [];
                                this.stopLocationTracking();
                                this.releaseWakeableScreen();
                            },
                            handleComputerNotification (payload) {
                                if (payload && ("call" === payload.type || "sms" === payload.type)) return void _notificationCenter.default.record(payload);
                                const item = _notificationCenter.default.receive(payload);
                                if (!item) return;
                                this.ensureWakeableScreen();
                                if (this.data.notificationShowing) return;
                                this.data.notificationShowing = true;
                                try {
                                    _system4.default.push({
                                        uri: "/pages/notification",
                                        params: {
                                            routeId: item.id,
                                            routeType: item.type,
                                            routeAppName: item.appName,
                                            routeSender: item.sender,
                                            routeTitle: item.title,
                                            routeBody: item.body,
                                            routeTimestamp: item.timestamp
                                        }
                                    });
                                } catch (error) {
                                    this.data.notificationShowing = false;
                                    console.log("open received notification failed", error);
                                }
                            },
                            finishNotificationDisplay () {
                                this.data.notificationShowing = false;
                            },
                            handleChargingStarted () {
                                if (this.data.chargingNoticeShowing) return;
                                this.data.chargingNoticeShowing = true;
                                this.ensureWakeableScreen();
                                try {
                                    _system4.default.push({
                                        uri: "/pages/chargingnotice"
                                    });
                                } catch (error) {
                                    this.data.chargingNoticeShowing = false;
                                    console.log("open charging notice failed", error);
                                }
                            },
                            finishChargingNotice () {
                                this.data.chargingNoticeShowing = false;
                            },
                            loadSpecialSportMode () {
                                try {
                                    _system5.default.get({
                                        key: SPECIAL_SPORT_MODE_STORAGE_KEY,
                                        default: "0",
                                        success: (value)=>{
                                            this.data.specialSportModeActive = "1" === value;
                                            this.data.specialSportModeLoaded = true;
                                            if (this.data.specialSportModeActive) {
                                                this.data.specialSportHeartLastRecordAt = Date.now();
                                                this.scheduleSpecialSportHeartRateMonitor();
                                            }
                                            this.notifySpecialSportHeartRateListeners();
                                        },
                                        fail: (data, code)=>{
                                            this.data.specialSportModeLoaded = true;
                                            console.log("load special sport mode failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    this.data.specialSportModeLoaded = true;
                                    console.log("special sport mode storage unavailable", error);
                                }
                            },
                            persistSpecialSportMode () {
                                try {
                                    _system5.default.set({
                                        key: SPECIAL_SPORT_MODE_STORAGE_KEY,
                                        value: this.data.specialSportModeActive ? "1" : "0",
                                        fail: (data, code)=>{
                                            console.log("save special sport mode failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save special sport mode unavailable", error);
                                }
                            },
                            startSpecialSportMode () {
                                this.data.specialSportModeActive = true;
                                this.data.specialSportModeLoaded = true;
                                this.data.specialSportMetric = "steps";
                                this.data.specialSportHighSampleCount = 0;
                                this.data.specialSportHeartLastRecordAt = Date.now();
                                this.persistSpecialSportMode();
                                this.scheduleSpecialSportHeartRateMonitor();
                                this.notifySpecialSportHeartRateListeners();
                                this.ensureWakeableScreen();
                            },
                            stopSpecialSportMode () {
                                this.data.specialSportModeActive = false;
                                this.data.specialSportMetric = "steps";
                                this.data.specialSportHighSampleCount = 0;
                                this.data.specialSportHeartAlertShowing = false;
                                this.persistSpecialSportMode();
                                this.stopSpecialSportHeartRateMonitor();
                                this.notifySpecialSportHeartRateListeners();
                            },
                            isSpecialSportMode () {
                                return !!this.data.specialSportModeActive;
                            },
                            setSpecialSportMetric (metric) {
                                if (SPECIAL_SPORT_METRICS.indexOf(metric) < 0) return;
                                this.data.specialSportMetric = metric;
                            },
                            getSpecialSportMetric () {
                                return this.data.specialSportMetric || "steps";
                            },
                            getSpecialSportHeartRateSnapshot () {
                                return {
                                    active: !!this.data.specialSportModeActive,
                                    bpm: Math.max(30, Math.round(Number(this.data.specialSportHeartRate) || 78)),
                                    threshold: SPECIAL_SPORT_HEART_RATE_HIGH_BPM
                                };
                            },
                            subscribeSpecialSportHeartRate (listener) {
                                if ("function" != typeof listener) return;
                                if (this.data.specialSportHeartRateListeners.indexOf(listener) < 0) this.data.specialSportHeartRateListeners.push(listener);
                                try {
                                    listener(this.getSpecialSportHeartRateSnapshot());
                                } catch (error) {
                                    console.log("special sport heart listener failed", error);
                                }
                            },
                            unsubscribeSpecialSportHeartRate (listener) {
                                const index = this.data.specialSportHeartRateListeners.indexOf(listener);
                                if (index >= 0) this.data.specialSportHeartRateListeners.splice(index, 1);
                            },
                            notifySpecialSportHeartRateListeners () {
                                const snapshot = this.getSpecialSportHeartRateSnapshot();
                                const listeners = this.data.specialSportHeartRateListeners.slice();
                                for(let index = 0; index < listeners.length; index += 1){
                                    try {
                                        listeners[index](snapshot);
                                    } catch (error) {
                                        console.log("special sport heart listener failed", error);
                                    }
                                }
                            },
                            startSpecialSportHeartRateMonitor () {
                                if (!this.data.specialSportModeActive) return;
                                if (this.data.specialSportHeartRateTimerId) return;
                                this.refreshSpecialSportHeartRate();
                                this.data.specialSportHeartRateTimerId = setInterval(()=>{
                                    this.refreshSpecialSportHeartRate();
                                }, SPECIAL_SPORT_HEART_RATE_POLL_MS);
                            },
                            scheduleSpecialSportHeartRateMonitor (delay) {
                                if (!this.data.specialSportModeActive) return;
                                if (this.data.specialSportHeartRateTimerId) return;
                                if (this.data.specialSportHeartRateStartTimerId) return;
                                const wait = Math.max(200, Math.round(Number(delay) || SPECIAL_SPORT_HEART_RATE_START_DELAY_MS));
                                this.data.specialSportHeartRateStartTimerId = setTimeout(()=>{
                                    this.data.specialSportHeartRateStartTimerId = null;
                                    this.startSpecialSportHeartRateMonitor();
                                }, wait);
                            },
                            stopSpecialSportHeartRateMonitor () {
                                if (this.data.specialSportHeartRateStartTimerId) {
                                    clearTimeout(this.data.specialSportHeartRateStartTimerId);
                                    this.data.specialSportHeartRateStartTimerId = null;
                                }
                                if (this.data.specialSportHeartRateTimerId) {
                                    clearInterval(this.data.specialSportHeartRateTimerId);
                                    this.data.specialSportHeartRateTimerId = null;
                                }
                                this.data.specialSportHeartRateReading = false;
                            },
                            refreshSpecialSportHeartRate () {
                                if (!this.data.specialSportModeActive || this.data.specialSportHeartRateReading) return;
                                this.data.specialSportHeartRateReading = true;
                                try {
                                    _system2.default.readText({
                                        uri: SPECIAL_SPORT_HEART_RATE_URI,
                                        success: (data)=>{
                                            try {
                                                const payload = JSON.parse(data && data.text ? data.text : "{}");
                                                const bpm = Math.round(Number(payload.bpm) || 0);
                                                if ("vela-emulator-heart-rate" === payload.source && bpm >= 30 && bpm <= 240) this.applySpecialSportHeartRate(bpm);
                                            } catch (error) {
                                                console.log("special sport heart rate parse failed", error);
                                            }
                                        },
                                        fail: (data, code)=>{
                                            console.log("special sport heart rate read failed", code, data);
                                        },
                                        complete: ()=>{
                                            this.data.specialSportHeartRateReading = false;
                                        }
                                    });
                                } catch (error) {
                                    this.data.specialSportHeartRateReading = false;
                                    console.log("special sport heart rate unavailable", error);
                                }
                            },
                            applySpecialSportHeartRate (bpm) {
                                if (!this.data.specialSportModeActive) return;
                                const now = Date.now();
                                const changed = bpm !== this.data.specialSportHeartRate;
                                this.data.specialSportHeartRate = bpm;
                                if (changed) this.notifySpecialSportHeartRateListeners();
                                if (now - this.data.specialSportHeartLastRecordAt >= SPECIAL_SPORT_RECORD_INTERVAL_MS) {
                                    this.data.specialSportHeartLastRecordAt = now;
                                    _healthRecords.default.recordHeartRate(bpm, now);
                                }
                                if (bpm >= SPECIAL_SPORT_HEART_RATE_HIGH_BPM) this.data.specialSportHighSampleCount += 1;
                                else this.data.specialSportHighSampleCount = 0;
                                if (this.data.specialSportHighSampleCount >= SPECIAL_SPORT_HIGH_SAMPLE_COUNT && !this.data.specialSportHeartAlertShowing && now - this.data.specialSportHeartAlertLastAt >= SPECIAL_SPORT_ALERT_COOLDOWN_MS) this.openSpecialSportHeartRateAlert(bpm);
                            },
                            openSpecialSportHeartRateAlert (bpm) {
                                if (!this.data.specialSportModeActive || this.data.specialSportHeartAlertShowing) return;
                                this.data.specialSportHeartAlertShowing = true;
                                this.data.specialSportHeartAlertLastAt = Date.now();
                                this.data.specialSportHighSampleCount = 0;
                                this.ensureWakeableScreen();
                                try {
                                    _system4.default.push({
                                        uri: "/pages/heartratealert",
                                        params: {
                                            routeBpm: bpm
                                        }
                                    });
                                } catch (error) {
                                    this.data.specialSportHeartAlertShowing = false;
                                    console.log("open special sport heart alert failed", error);
                                }
                            },
                            finishSpecialSportHeartRateAlert () {
                                this.data.specialSportHeartAlertShowing = false;
                            },
                            loadLastCelebrationDate () {
                                try {
                                    _system5.default.get({
                                        key: ALL_GOALS_CELEBRATION_DATE_KEY,
                                        default: "",
                                        success: (value)=>{
                                            this.data.lastCelebrationDate = value || "";
                                            this.data.celebrationDateLoaded = true;
                                            this.evaluateAllGoalsCelebration();
                                        },
                                        fail: (data, code)=>{
                                            console.log("load celebration date failed", code, data);
                                            this.data.celebrationDateLoaded = true;
                                            this.evaluateAllGoalsCelebration();
                                        }
                                    });
                                } catch (error) {
                                    console.log("celebration date storage unavailable", error);
                                    this.data.celebrationDateLoaded = true;
                                }
                            },
                            handleCelebrationStepSnapshot (snapshot) {
                                this.data.latestStepSnapshot = snapshot;
                                this.evaluateAllGoalsCelebration();
                            },
                            evaluateAllGoalsCelebration () {
                                const snapshot = this.data.latestStepSnapshot;
                                if (!this.data.appVisible || !this.data.celebrationDateLoaded || this.data.celebrationShowing || !snapshot || !snapshot.loaded || !snapshot.dateKey) return;
                                if (this.data.lastCelebrationDate === snapshot.dateKey) return;
                                const steps = Math.max(0, Math.round(Number(snapshot.steps) || 0));
                                const calories = Math.max(0, Math.round(0.04 * steps));
                                const duration = Math.max(0, Math.floor(steps / 100));
                                const stepGoal = Math.max(1, Math.round(Number(snapshot.goal) || 8000));
                                const calorieGoal = Math.max(1, Math.round(Number(snapshot.calorieGoal) || 400));
                                const durationGoal = Math.max(1, Math.round(Number(snapshot.durationGoal) || 30));
                                if (steps < stepGoal || calories < calorieGoal || duration < durationGoal) return;
                                this.data.celebrationShowing = true;
                                this.data.lastCelebrationDate = snapshot.dateKey;
                                this.persistCelebrationDate(snapshot.dateKey);
                                try {
                                    _system4.default.push({
                                        uri: "/pages/celebration"
                                    });
                                } catch (error) {
                                    this.data.celebrationShowing = false;
                                    console.log("open all goals celebration failed", error);
                                }
                            },
                            persistCelebrationDate (dateKey) {
                                try {
                                    _system5.default.set({
                                        key: ALL_GOALS_CELEBRATION_DATE_KEY,
                                        value: dateKey,
                                        fail: (data, code)=>{
                                            console.log("save celebration date failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save celebration date unavailable", error);
                                }
                            },
                            finishAllGoalsCelebration () {
                                this.data.celebrationShowing = false;
                            },
                            setKeepScreenOn (keepScreenOn) {
                                if (this.data.keepScreenOn === keepScreenOn) return;
                                this.data.keepScreenOn = keepScreenOn;
                            },
                            ensureWakeableScreen () {
                                return;
                            },
                            releaseWakeableScreen () {
                                return;
                            },
                            getCustomization () {
                                return this.data.customization;
                            },
                            loadCustomCities () {
                                try {
                                    _system5.default.get({
                                        key: CUSTOM_CITIES_STORAGE_KEY,
                                        default: EMPTY_STORAGE_VALUE,
                                        success: (value)=>{
                                            if (value === EMPTY_STORAGE_VALUE) {
                                                this.data.customCities = (0, _weatherCities.getDefaultCustomCities)();
                                                this.persistCustomCities();
                                            } else {
                                                try {
                                                    const parsed = JSON.parse(value || "[]");
                                                    this.data.customCities = Array.isArray(parsed) ? parsed : [];
                                                } catch (error) {
                                                    this.data.customCities = (0, _weatherCities.getDefaultCustomCities)();
                                                    this.persistCustomCities();
                                                }
                                            }
                                            this.finishCustomCityLoading();
                                        },
                                        fail: (data, code)=>{
                                            console.log("load custom cities failed", code, data);
                                            this.data.customCities = (0, _weatherCities.getDefaultCustomCities)();
                                            this.finishCustomCityLoading();
                                        }
                                    });
                                } catch (error) {
                                    console.log("custom city storage unavailable", error);
                                    this.data.customCities = (0, _weatherCities.getDefaultCustomCities)();
                                    this.finishCustomCityLoading();
                                }
                            },
                            finishCustomCityLoading () {
                                this.data.customCitiesLoaded = true;
                                const listeners = this.data.customCityReadyListeners.slice();
                                this.data.customCityReadyListeners = [];
                                for(let index = 0; index < listeners.length; index += 1){
                                    try {
                                        listeners[index]();
                                    } catch (error) {
                                        console.log("custom city listener failed", error);
                                    }
                                }
                            },
                            whenCustomCitiesReady (callback) {
                                if (!callback) return;
                                if (this.data.customCitiesLoaded) return void callback();
                                this.data.customCityReadyListeners.push(callback);
                            },
                            getCustomCities () {
                                return this.data.customCities.slice();
                            },
                            getCurrentLocation () {
                                return copyLocationCity(this.data.currentLocation);
                            },
                            getCurrentLocationWeather () {
                                return this.data.currentLocationWeather;
                            },
                            addCurrentLocationListener (callback) {
                                if (!callback) return;
                                for(let index = 0; index < this.data.currentLocationListeners.length; index += 1)if (this.data.currentLocationListeners[index] === callback) return void callback(this.getCurrentLocation(), this.data.currentLocationWeather);
                                this.data.currentLocationListeners.push(callback);
                                callback(this.getCurrentLocation(), this.data.currentLocationWeather);
                            },
                            removeCurrentLocationListener (callback) {
                                if (!callback) return;
                                const listeners = [];
                                for(let index = 0; index < this.data.currentLocationListeners.length; index += 1)if (this.data.currentLocationListeners[index] !== callback) listeners.push(this.data.currentLocationListeners[index]);
                                this.data.currentLocationListeners = listeners;
                            },
                            notifyCurrentLocationListeners () {
                                const listeners = this.data.currentLocationListeners.slice();
                                const city = this.getCurrentLocation();
                                const weather = this.data.currentLocationWeather;
                                for(let index = 0; index < listeners.length; index += 1){
                                    try {
                                        listeners[index](city, weather);
                                    } catch (error) {
                                        console.log("current location listener failed", error);
                                    }
                                }
                            },
                            startLocationTracking () {
                                if (this.data.locationTracking) return;
                                this.data.locationTracking = true;
                                this.refreshCurrentLocation(true);
                                try {
                                    _system3.default.subscribe({
                                        coordType: "wgs84",
                                        callback: (data)=>{
                                            this.handleLocationCoordinates(data, false);
                                        },
                                        fail: (data, code)=>{
                                            console.log("location subscribe failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("location subscribe unavailable", error);
                                }
                                this.data.locationRefreshTimerId = setInterval(()=>{
                                    this.refreshCurrentLocation(false);
                                }, LOCATION_REFRESH_MS);
                            },
                            stopLocationTracking () {
                                this.data.locationTracking = false;
                                this.data.locationRequestSerial += 1;
                                this.data.locationReading = false;
                                if (this.data.locationRefreshTimerId) {
                                    clearInterval(this.data.locationRefreshTimerId);
                                    this.data.locationRefreshTimerId = null;
                                }
                                try {
                                    _system3.default.unsubscribe();
                                } catch (error) {
                                    console.log("location unsubscribe unavailable", error);
                                }
                            },
                            refreshCurrentLocation (force) {
                                if (this.data.locationReading) return;
                                this.data.locationReading = true;
                                try {
                                    _system3.default.getLocation({
                                        coordType: "wgs84",
                                        timeout: 10000,
                                        success: (data)=>{
                                            this.data.locationReading = false;
                                            this.handleLocationCoordinates(data, !!force);
                                        },
                                        fail: (data, code)=>{
                                            this.data.locationReading = false;
                                            this.handleLocationFailure(data, code);
                                        }
                                    });
                                } catch (error) {
                                    this.data.locationReading = false;
                                    this.handleLocationFailure(error, -1);
                                }
                            },
                            handleLocationFailure (data, code) {
                                console.log("get current location failed", code, data);
                                if (this.data.currentLocation.locationReady) return;
                                this.data.currentLocation = createPendingLocation("定位失败，点击重试");
                                this.notifyCurrentLocationListeners();
                            },
                            handleLocationCoordinates (data, force) {
                                if (!data) return;
                                const longitude = Number(data.longitude);
                                const latitude = Number(data.latitude);
                                if (!isFinite(longitude) || !isFinite(latitude)) return void this.handleLocationFailure("invalid coordinates", -2);
                                const now = Date.now();
                                const sameCoordinates = null !== this.data.lastLongitude && Math.abs(longitude - this.data.lastLongitude) < 0.00001 && Math.abs(latitude - this.data.lastLatitude) < 0.00001;
                                if (!force && sameCoordinates && now - this.data.lastLocationResolveAt < LOCATION_RESOLVE_MS) return;
                                this.data.lastLongitude = longitude;
                                this.data.lastLatitude = latitude;
                                this.data.lastLocationResolveAt = now;
                                const requestSerial = this.data.locationRequestSerial + 1;
                                this.data.locationRequestSerial = requestSerial;
                                (0, _weatherService.fetchLiveWeatherByCoordinates)(longitude, latitude, (weather)=>{
                                    if (requestSerial !== this.data.locationRequestSerial) return;
                                    const location = weather.location || {};
                                    const districtName = location.displayName || location.name || "当前位置";
                                    const cityName = location.displayAdm2 || location.adm2 || districtName;
                                    const provinceName = location.displayAdm1 || location.adm1 || "";
                                    let administrativeArea = cityName;
                                    if (provinceName && cityName !== provinceName) administrativeArea += " · " + provinceName;
                                    else if (provinceName) administrativeArea = provinceName;
                                    this.data.currentLocation = {
                                        id: "current-location",
                                        locationId: location.id || "",
                                        name: districtName,
                                        detailName: districtName,
                                        administrativeArea: administrativeArea,
                                        country: location.country || "中国",
                                        isBase: true,
                                        isCurrentLocation: true,
                                        locationReady: true,
                                        deletable: false,
                                        longitude: longitude,
                                        latitude: latitude,
                                        weatherLocationName: location.name || districtName,
                                        weatherAdmName: location.adm2 || cityName,
                                        weatherProvinceName: provinceName,
                                        weatherCityName: cityName
                                    };
                                    this.data.currentLocationWeather = weather;
                                    this.notifyCurrentLocationListeners();
                                }, (error)=>{
                                    if (requestSerial !== this.data.locationRequestSerial) return;
                                    this.handleLocationFailure(error, -3);
                                });
                            },
                            persistCustomCities () {
                                try {
                                    _system5.default.set({
                                        key: CUSTOM_CITIES_STORAGE_KEY,
                                        value: JSON.stringify(this.data.customCities),
                                        fail: (data, code)=>{
                                            console.log("save custom cities failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save custom cities unavailable", error);
                                }
                            },
                            addCustomCity (city) {
                                if (!city || !city.id) return {
                                    status: "invalid"
                                };
                                const baseCities = (0, _weatherCities.getCities)();
                                for(let index = 0; index < baseCities.length; index += 1){
                                    const base = baseCities[index];
                                    if (base.detailName === city.detailName && base.administrativeArea === city.administrativeArea) return {
                                        status: "exists",
                                        city: base
                                    };
                                }
                                for(let index = 0; index < this.data.customCities.length; index += 1){
                                    const existing = this.data.customCities[index];
                                    if (existing.detailName === city.detailName && existing.administrativeArea === city.administrativeArea) return {
                                        status: "exists",
                                        city: existing
                                    };
                                }
                                this.data.customCities = this.data.customCities.concat([
                                    city
                                ]);
                                this.persistCustomCities();
                                return {
                                    status: "added",
                                    city: city
                                };
                            },
                            removeCustomCity (cityId) {
                                let removed = false;
                                const nextCities = [];
                                for(let index = 0; index < this.data.customCities.length; index += 1){
                                    const city = this.data.customCities[index];
                                    if (city.id === cityId) removed = true;
                                    else nextCities.push(city);
                                }
                                if (!removed) return false;
                                this.data.customCities = nextCities;
                                if (this.data.customization.cityId === cityId) this.data.customization.cityId = _weatherCities.DEFAULT_CITY_ID;
                                this.persistCustomCities();
                                return true;
                            },
                            initializeMusicAudio () {
                                _system.default.onplay = ()=>{
                                    this.data.musicIsPlaying = true;
                                    this.data.musicError = "";
                                    this.notifyMusicListeners();
                                };
                                _system.default.onpause = ()=>{
                                    this.data.musicIsPlaying = false;
                                    this.notifyMusicListeners();
                                };
                                _system.default.onstop = ()=>{
                                    this.data.musicIsPlaying = false;
                                    this.notifyMusicListeners();
                                };
                                _system.default.ontimeupdate = ()=>{
                                    const percent = Number(_system.default.percent);
                                    if (!isNaN(percent)) this.data.musicProgress = Math.max(0, Math.min(100, Math.round(percent)));
                                    this.notifyMusicListeners();
                                };
                                _system.default.onended = ()=>{
                                    this.changeMusicSong(1);
                                };
                                _system.default.onerror = (error)=>{
                                    this.data.musicIsPlaying = false;
                                    this.data.musicError = "音频暂时无法播放";
                                    console.log("music playback failed", error);
                                    this.notifyMusicListeners();
                                };
                                this.refreshMusicState();
                            },
                            musicStateSnapshot () {
                                return {
                                    index: this.data.musicIndex,
                                    song: _musicPlayer.MUSIC_LIBRARY[this.data.musicIndex],
                                    isPlaying: this.data.musicIsPlaying,
                                    progress: this.data.musicProgress,
                                    prepared: this.data.musicPrepared,
                                    error: this.data.musicError
                                };
                            },
                            notifyMusicListeners () {
                                const listeners = this.data.musicListeners.slice();
                                const snapshot = this.musicStateSnapshot();
                                for(let index = 0; index < listeners.length; index += 1){
                                    try {
                                        listeners[index](snapshot);
                                    } catch (error) {
                                        console.log("music state listener failed", error);
                                    }
                                }
                            },
                            subscribeMusic (listener) {
                                if ("function" != typeof listener) return ()=>{};
                                this.data.musicListeners.push(listener);
                                listener(this.musicStateSnapshot());
                                return ()=>{
                                    const index = this.data.musicListeners.indexOf(listener);
                                    if (index >= 0) this.data.musicListeners.splice(index, 1);
                                };
                            },
                            getMusicState () {
                                return this.musicStateSnapshot();
                            },
                            musicIndexForSource (source) {
                                const src = String(source || "");
                                if (!src) return -1;
                                for(let index = 0; index < _musicPlayer.MUSIC_LIBRARY.length; index += 1){
                                    const song = _musicPlayer.MUSIC_LIBRARY[index];
                                    const slash = song.playUrl.lastIndexOf("/");
                                    const filename = slash >= 0 ? song.playUrl.slice(slash + 1) : song.playUrl;
                                    if (src === song.playUrl || src.indexOf(filename) >= 0) return index;
                                }
                                return -1;
                            },
                            refreshMusicState (callback) {
                                try {
                                    _system.default.getPlayState({
                                        success: (state)=>{
                                            const source = state && state.src ? state.src : "";
                                            const index = this.musicIndexForSource(source);
                                            if (index >= 0) this.data.musicIndex = index;
                                            this.data.musicPrepared = !!source;
                                            this.data.musicIsPlaying = !!state && "play" === state.state;
                                            const percent = Number(state && state.percent);
                                            if (!isNaN(percent) && percent >= 0) this.data.musicProgress = Math.max(0, Math.min(100, Math.round(percent)));
                                            this.notifyMusicListeners();
                                            if ("function" == typeof callback) callback(this.musicStateSnapshot());
                                        },
                                        fail: (data, code)=>{
                                            console.log("read music state failed", code, data);
                                            if ("function" == typeof callback) callback(this.musicStateSnapshot());
                                        }
                                    });
                                } catch (error) {
                                    console.log("read music state unavailable", error);
                                    if ("function" == typeof callback) callback(this.musicStateSnapshot());
                                }
                            },
                            normalizeMusicIndex (index) {
                                const length = _musicPlayer.MUSIC_LIBRARY.length;
                                if (!length) return 0;
                                return (index + length) % length;
                            },
                            prepareMusicSong (autoplay) {
                                const song = _musicPlayer.MUSIC_LIBRARY[this.data.musicIndex];
                                if (!song) return;
                                this.cancelMusicPauseTimers(true);
                                this.data.musicError = "";
                                this.data.musicProgress = 0;
                                try {
                                    _system.default.stop();
                                    _system.default.src = song.playUrl;
                                    this.data.musicPrepared = true;
                                    this.notifyMusicListeners();
                                    if (autoplay) _system.default.play();
                                } catch (error) {
                                    this.data.musicIsPlaying = false;
                                    this.data.musicError = "播放失败";
                                    console.log("prepare music failed", error);
                                    this.notifyMusicListeners();
                                }
                            },
                            toggleMusic () {
                                try {
                                    if (!this.data.musicPrepared) return void this.prepareMusicSong(true);
                                    if (this.data.musicIsPlaying) this.softPauseMusic();
                                    else this.resumeMusicAfterPause();
                                } catch (error) {
                                    this.data.musicError = "播放控制失败";
                                    console.log("toggle music failed", error);
                                    this.notifyMusicListeners();
                                }
                            },
                            resumeMusicAfterPause () {
                                this.cancelMusicPauseTimers(false);
                                try {
                                    _system.default.muted = true;
                                    _system.default.volume = 0;
                                    _system.default.play();
                                    this.data.musicVolumeRestoreTimerId = setTimeout(()=>{
                                        this.data.musicVolumeRestoreTimerId = null;
                                        try {
                                            _system.default.volume = this.data.musicPauseRestoreVolume;
                                            _system.default.muted = false;
                                            this.data.musicPausedMuted = false;
                                        } catch (error) {
                                            console.log("restore music after play failed", error);
                                        }
                                    }, 260);
                                } catch (error) {
                                    this.data.musicError = "播放控制失败";
                                    console.log("resume music failed", error);
                                    this.notifyMusicListeners();
                                }
                            },
                            softPauseMusic () {
                                this.cancelMusicPauseTimers(true);
                                const currentVolume = Number(_system.default.volume);
                                this.data.musicPauseRestoreVolume = isNaN(currentVolume) ? 1 : Math.max(0, Math.min(1, currentVolume));
                                this.data.musicIsPlaying = false;
                                this.data.musicPausedMuted = true;
                                this.notifyMusicListeners();
                                try {
                                    _system.default.muted = true;
                                    _system.default.volume = 0;
                                } catch (error) {
                                    console.log("mute before pause failed", error);
                                }
                                this.data.musicPauseTimerId = setTimeout(()=>{
                                    this.data.musicPauseTimerId = null;
                                    try {
                                        _system.default.pause();
                                    } catch (error) {
                                        console.log("pause music failed", error);
                                    }
                                }, 180);
                            },
                            cancelMusicPauseTimers (restoreVolume) {
                                if (this.data.musicPauseTimerId) {
                                    clearTimeout(this.data.musicPauseTimerId);
                                    this.data.musicPauseTimerId = null;
                                }
                                if (this.data.musicVolumeRestoreTimerId) {
                                    clearTimeout(this.data.musicVolumeRestoreTimerId);
                                    this.data.musicVolumeRestoreTimerId = null;
                                }
                                if (!restoreVolume) return;
                                try {
                                    _system.default.volume = this.data.musicPauseRestoreVolume;
                                    _system.default.muted = false;
                                    this.data.musicPausedMuted = false;
                                } catch (error) {
                                    console.log("cancel music pause restore failed", error);
                                }
                            },
                            changeMusicSong (direction) {
                                this.data.musicIndex = this.normalizeMusicIndex(this.data.musicIndex + direction);
                                this.prepareMusicSong(true);
                            },
                            selectMusicSongById (songId, autoplay) {
                                for(let index = 0; index < _musicPlayer.MUSIC_LIBRARY.length; index += 1)if (_musicPlayer.MUSIC_LIBRARY[index].id === songId) {
                                    this.data.musicIndex = index;
                                    this.prepareMusicSong(false !== autoplay);
                                    return;
                                }
                            },
                            getMusicVolume () {
                                if (this.data.musicPausedMuted) return Math.max(0, Math.min(100, Math.round(100 * this.data.musicPauseRestoreVolume)));
                                const volume = Number(_system.default.volume);
                                if (isNaN(volume)) return 60;
                                return Math.max(0, Math.min(100, Math.round(100 * volume)));
                            },
                            setMusicVolume (percent) {
                                const next = Math.max(0, Math.min(100, Math.round(percent)));
                                try {
                                    this.data.musicPauseRestoreVolume = next / 100;
                                    if (this.data.musicPausedMuted) _system.default.volume = 0;
                                    else _system.default.volume = next / 100;
                                } catch (error) {
                                    console.log("set music volume failed", error);
                                }
                                return next;
                            },
                            setBackground (backgroundId) {
                                this.data.customization.backgroundId = backgroundId || _customization.DEFAULT_BACKGROUND_ID;
                            },
                            setAction (actionId) {
                                this.data.customization.actionId = actionId || _customization.DEFAULT_ACTION_ID;
                            },
                            setCity (cityId) {
                                this.data.customization.cityId = cityId || _weatherCities.DEFAULT_CITY_ID;
                            }
                        };
                    };
                    $app_script$({}, $app_exports$, $app_require$1);
                    $app_exports$.default.style = $app_style$;
                    $app_exports$.default.manifest = __webpack_require__("./src/manifest.json");
                    var $translateStyle$ = function(value) {
                        if ('string' == typeof value) return Object.fromEntries(value.split(';').filter((item)=>Boolean(item && item.trim())).map((item)=>{
                            const matchs = item.match(/([^:]+):(.*)/);
                            if (matchs && matchs.length > 2) return [
                                matchs[1].trim().replace(/-([a-z])/g, (_, match)=>match.toUpperCase()),
                                matchs[2].trim()
                            ];
                            return [];
                        }));
                        return value;
                    };
                    __webpack_require__.g.$translateStyle$ = $translateStyle$;
                })();
            })();
        };
        return createAppHandler();
    })(global, globalThis, window, $app_exports$, $app_evaluate$);
}
