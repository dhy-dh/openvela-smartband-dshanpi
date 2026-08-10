export default function(global, globalThis, window, $app_exports$, $app_evaluate$) {
    var org_app_require = $app_require$;
    (function(global, globalThis, window, $app_exports$, $app_evaluate$) {
        var setTimeout = global.setTimeout;
        var setInterval = global.setInterval;
        var clearTimeout = global.clearTimeout;
        var clearInterval = global.clearInterval;
        var $app_require$1 = global.$app_require$ || org_app_require;
        var createPageHandler = function() {
            return (()=>{
                var __webpack_modules__ = {
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
                    __webpack_require__.rv = ()=>"1.7.12";
                })();
                (()=>{
                    __webpack_require__.ruid = "bundler=rspack@1.7.12";
                })();
                var __webpack_exports__ = {};
                (()=>{
                    var $app_style$ = [
                        [
                            [
                                [
                                    0,
                                    "board-screen"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                backgroundColor: "#000000",
                                display: "flex",
                                justifyContent: "center",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "watch-area"
                                ]
                            ],
                            {
                                width: "432px",
                                height: "514px",
                                position: "relative",
                                overflow: "hidden"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "page"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                overflow: "hidden",
                                backgroundColor: "#030307"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "scene"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                objectFit: "cover"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "dark-overlay"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                backgroundColor: "rgba(0, 0, 0, 0.58)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "measurement-view"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                overflow: "hidden"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-view"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                overflow: "hidden"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "page-title"
                                ]
                            ],
                            {
                                width: "220px",
                                height: "58px",
                                position: "absolute",
                                left: "106px",
                                top: "22px",
                                color: "#ffffff",
                                fontSize: "42px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "pressure-card"
                                ]
                            ],
                            {
                                width: "386px",
                                height: "304px",
                                position: "absolute",
                                left: "23px",
                                top: "88px",
                                flexDirection: "column",
                                alignItems: "center",
                                borderRadius: "30px",
                                backgroundColor: "rgba(255, 255, 255, 0.95)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "measurement-state"
                                ]
                            ],
                            {
                                width: "220px",
                                height: "35px",
                                marginTop: "17px",
                                color: "#17366f",
                                fontSize: "24px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "pressure-row"
                                ]
                            ],
                            {
                                width: "330px",
                                height: "102px",
                                marginTop: "8px",
                                flexDirection: "row",
                                alignItems: "center",
                                justifyContent: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "pressure-value-block"
                                ]
                            ],
                            {
                                width: "128px",
                                height: "98px",
                                flexDirection: "column",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "pressure-value"
                                ]
                            ],
                            {
                                width: "128px",
                                height: "72px",
                                fontSize: "61px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "systolic"
                                ]
                            ],
                            {
                                color: "#ff3b30"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "diastolic"
                                ]
                            ],
                            {
                                color: "#2f80ed"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "pressure-label"
                                ]
                            ],
                            {
                                width: "128px",
                                height: "25px",
                                color: "#64748b",
                                fontSize: "17px",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "pressure-divider"
                                ]
                            ],
                            {
                                width: "42px",
                                height: "75px",
                                color: "#17366f",
                                fontSize: "56px",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "pressure-unit"
                                ]
                            ],
                            {
                                width: "120px",
                                height: "24px",
                                color: "#64748b",
                                fontSize: "17px",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "status-pill"
                                ]
                            ],
                            {
                                width: "168px",
                                height: "38px",
                                marginTop: "7px",
                                justifyContent: "center",
                                alignItems: "center",
                                borderRadius: "19px",
                                backgroundColor: "#e7f8ef"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "status-text"
                                ]
                            ],
                            {
                                width: "150px",
                                height: "27px",
                                color: "#14804a",
                                fontSize: "19px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "pulse-row"
                                ]
                            ],
                            {
                                width: "250px",
                                height: "38px",
                                marginTop: "9px",
                                flexDirection: "row",
                                justifyContent: "center",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "pulse-label"
                                ]
                            ],
                            {
                                height: "27px",
                                color: "#64748b",
                                fontSize: "18px",
                                width: "56px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "pulse-unit"
                                ]
                            ],
                            {
                                height: "27px",
                                color: "#64748b",
                                fontSize: "18px",
                                width: "55px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "pulse-value"
                                ]
                            ],
                            {
                                width: "70px",
                                height: "36px",
                                color: "#17366f",
                                fontSize: "29px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "measuring-dots"
                                ]
                            ],
                            {
                                width: "86px",
                                height: "20px",
                                position: "absolute",
                                left: "173px",
                                top: "404px",
                                flexDirection: "row",
                                justifyContent: "center",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "measure-dot"
                                ]
                            ],
                            {
                                width: "12px",
                                height: "12px",
                                borderRadius: "6px",
                                backgroundColor: "#ff3b30"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "dot-middle"
                                ]
                            ],
                            {
                                marginLeft: "12px",
                                marginRight: "12px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "measure-hint"
                                ]
                            ],
                            {
                                width: "330px",
                                height: "28px",
                                position: "absolute",
                                left: "51px",
                                top: "424px",
                                color: "#ffffff",
                                fontSize: "18px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-title"
                                ]
                            ],
                            {
                                width: "260px",
                                height: "55px",
                                position: "absolute",
                                left: "86px",
                                top: "22px",
                                color: "#ffffff",
                                fontSize: "39px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-card"
                                ]
                            ],
                            {
                                width: "404px",
                                height: "354px",
                                position: "absolute",
                                left: "14px",
                                top: "86px",
                                flexDirection: "column",
                                borderRadius: "28px",
                                backgroundColor: "rgba(255, 255, 255, 0.95)",
                                overflow: "hidden"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-row"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "50px",
                                position: "relative",
                                flexDirection: "row",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-dot"
                                ]
                            ],
                            {
                                width: "11px",
                                height: "11px",
                                marginLeft: "16px",
                                borderRadius: "6px",
                                backgroundColor: "#ff3b30"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-copy"
                                ]
                            ],
                            {
                                width: "104px",
                                height: "44px",
                                marginLeft: "10px",
                                flexDirection: "column",
                                justifyContent: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-date"
                                ]
                            ],
                            {
                                width: "104px",
                                height: "25px",
                                color: "#102b61",
                                fontSize: "19px",
                                fontWeight: "bold"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-time"
                                ]
                            ],
                            {
                                width: "104px",
                                height: "18px",
                                color: "#76839a",
                                fontSize: "14px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-pressure"
                                ]
                            ],
                            {
                                width: "91px",
                                height: "34px",
                                color: "#ff3030",
                                fontSize: "26px",
                                fontWeight: "bold",
                                textAlign: "right"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-pressure-unit"
                                ]
                            ],
                            {
                                width: "49px",
                                height: "22px",
                                marginLeft: "5px",
                                color: "#64748b",
                                fontSize: "14px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-pulse"
                                ]
                            ],
                            {
                                width: "95px",
                                height: "25px",
                                marginLeft: "5px",
                                color: "#17366f",
                                fontSize: "17px",
                                fontWeight: "bold",
                                textAlign: "right"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-separator"
                                ]
                            ],
                            {
                                width: "370px",
                                height: "1px",
                                position: "absolute",
                                left: "17px",
                                bottom: 0,
                                backgroundColor: "#d7e5f7"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-empty"
                                ]
                            ],
                            {
                                width: "320px",
                                height: "44px",
                                position: "absolute",
                                left: "56px",
                                top: "216px",
                                color: "#ffffff",
                                fontSize: "24px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ]
                    ];
                    var $app_script$ = function __scriptModule__(module, exports, $app_require$1) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports.default = void 0;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.router"));
                        var _system2 = _interopRequireDefault($app_require$1("@app-module/system.file"));
                        var _system3 = _interopRequireDefault($app_require$1("@app-module/system.storage"));
                        var _customization = __webpack_require__("./src/common/customization.js");
                        var _healthRecords = _interopRequireDefault(__webpack_require__("./src/common/health-records.js"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const HEART_RATE_URI = "/common/emulator-heart-rate.json";
                        const LAST_BLOOD_PRESSURE_KEY = "last_blood_pressure_measurement_v1";
                        const BLOOD_PRESSURE_HISTORY_KEY = "blood_pressure_history_v1";
                        const BLOOD_PRESSURE_HISTORY_LIMIT = 7;
                        const MEASUREMENT_DURATION_MS = 6000;
                        const SENSOR_POLL_MS = 650;
                        const SYSTOLIC_MIN = 105;
                        const SYSTOLIC_MAX = 145;
                        const DIASTOLIC_MIN = 65;
                        const DIASTOLIC_MAX = 95;
                        const PULSE_PRESSURE_MIN = 30;
                        const PULSE_PRESSURE_MAX = 60;
                        function randomInteger(minimum, maximum) {
                            return minimum + Math.floor(Math.random() * (maximum - minimum + 1));
                        }
                        function pad(value) {
                            return value < 10 ? "0" + value : "" + value;
                        }
                        var _default = exports.default = {
                            private: {
                                backgroundImage: "/common/backgrounds/dark-purple.png",
                                systolic: 128,
                                diastolic: 82,
                                pulse: 76,
                                sensorPulse: 76,
                                systolicText: "128",
                                diastolicText: "82",
                                pulseText: "76",
                                pressureStatus: "正常血压",
                                measurementState: "上次测量",
                                measureHint: "点击任意位置开始测量",
                                measuredAt: 0,
                                historyVisible: false,
                                historyRows: [],
                                historyHasRecords: false,
                                historyLoaded: false,
                                lastMeasurementLoaded: false,
                                measuring: false,
                                sensorReading: false,
                                sensorPollTimerId: null,
                                measurementTimerId: null,
                                touchX: 0,
                                touchY: 0,
                                leaving: false
                            },
                            onInit () {
                                this.syncCustomization();
                                this.loadLastMeasurement();
                                this.loadHistory();
                                this.refreshSensorValue();
                            },
                            onShow () {
                                this.$app.$def.ensureWakeableScreen();
                                this.leaving = false;
                                this.syncCustomization();
                                this.refreshSensorValue();
                            },
                            onHide () {
                                this.stopMeasurement();
                            },
                            onDestroy () {
                                this.stopMeasurement();
                            },
                            syncCustomization () {
                                const customization = this.$app.$def.getCustomization();
                                const background = (0, _customization.getBackground)(customization.backgroundId);
                                this.backgroundImage = background.src;
                            },
                            pressureLabel (systolic, diastolic) {
                                if (systolic >= 140 || diastolic >= 90) return "血压偏高";
                                if (systolic < 90 || diastolic < 60) return "血压偏低";
                                if (systolic >= 130 || diastolic >= 85) return "血压正常偏高";
                                return "正常血压";
                            },
                            loadLastMeasurement () {
                                try {
                                    _system3.default.get({
                                        key: LAST_BLOOD_PRESSURE_KEY,
                                        default: "",
                                        success: (value)=>{
                                            if (!value) {
                                                this.lastMeasurementLoaded = true;
                                                this.seedHistoryFromLastMeasurement();
                                                return;
                                            }
                                            try {
                                                const parsed = JSON.parse(value);
                                                const systolic = Math.round(Number(parsed.systolic) || 0);
                                                const diastolic = Math.round(Number(parsed.diastolic) || 0);
                                                const pulse = Math.round(Number(parsed.pulse) || 0);
                                                if (this.isValidPressure(systolic, diastolic)) this.applyPressure(systolic, diastolic);
                                                if (pulse >= 30 && pulse <= 240) {
                                                    this.pulse = pulse;
                                                    this.pulseText = String(pulse);
                                                }
                                                this.measuredAt = Math.max(0, Number(parsed.measuredAt) || 0);
                                            } catch (error) {
                                                console.log("parse last blood pressure failed", error);
                                            }
                                            this.lastMeasurementLoaded = true;
                                            this.seedHistoryFromLastMeasurement();
                                        },
                                        fail: (data, code)=>{
                                            console.log("load last blood pressure failed", code, data);
                                            this.lastMeasurementLoaded = true;
                                            this.seedHistoryFromLastMeasurement();
                                        }
                                    });
                                } catch (error) {
                                    console.log("last blood pressure storage unavailable", error);
                                    this.lastMeasurementLoaded = true;
                                    this.seedHistoryFromLastMeasurement();
                                }
                            },
                            loadHistory () {
                                try {
                                    _system3.default.get({
                                        key: BLOOD_PRESSURE_HISTORY_KEY,
                                        default: "",
                                        success: (value)=>{
                                            let records = [];
                                            if (value) {
                                                try {
                                                    const parsed = JSON.parse(value);
                                                    if (Array.isArray(parsed)) records = parsed;
                                                } catch (error) {
                                                    console.log("parse blood pressure history failed", error);
                                                }
                                            }
                                            this.setHistoryRecords(records);
                                            this.historyLoaded = true;
                                            this.seedHistoryFromLastMeasurement();
                                        },
                                        fail: (data, code)=>{
                                            console.log("load blood pressure history failed", code, data);
                                            this.setHistoryRecords([]);
                                            this.historyLoaded = true;
                                            this.seedHistoryFromLastMeasurement();
                                        }
                                    });
                                } catch (error) {
                                    console.log("blood pressure history storage unavailable", error);
                                    this.setHistoryRecords([]);
                                    this.historyLoaded = true;
                                    this.seedHistoryFromLastMeasurement();
                                }
                            },
                            setHistoryRecords (source) {
                                const records = [];
                                const list = Array.isArray(source) ? source : [];
                                for(let index = 0; index < list.length; index += 1){
                                    const systolic = Math.round(Number(list[index].systolic) || 0);
                                    const diastolic = Math.round(Number(list[index].diastolic) || 0);
                                    const pulse = Math.round(Number(list[index].pulse) || 0);
                                    const measuredAt = Math.max(0, Number(list[index].measuredAt) || 0);
                                    if (!this.isValidPressure(systolic, diastolic) || pulse < 30 || pulse > 240 || !measuredAt) continue;
                                    let duplicate = false;
                                    for(let rowIndex = 0; rowIndex < records.length; rowIndex += 1)if (records[rowIndex].measuredAt === measuredAt) duplicate = true;
                                    if (!duplicate) records.push({
                                        systolic: systolic,
                                        diastolic: diastolic,
                                        pulse: pulse,
                                        measuredAt: measuredAt
                                    });
                                    if (records.length >= BLOOD_PRESSURE_HISTORY_LIMIT) break;
                                }
                                this.historyRows = this.buildHistoryRows(records);
                                this.historyHasRecords = this.historyRows.length > 0;
                            },
                            buildHistoryRows (records) {
                                const rows = [];
                                for(let index = 0; index < records.length; index += 1){
                                    const record = records[index];
                                    const date = new Date(record.measuredAt);
                                    rows.push({
                                        id: String(record.measuredAt) + "-" + index,
                                        systolic: record.systolic,
                                        diastolic: record.diastolic,
                                        pulse: record.pulse,
                                        measuredAt: record.measuredAt,
                                        dateText: pad(date.getMonth() + 1) + "月" + pad(date.getDate()) + "日",
                                        timeText: pad(date.getHours()) + ":" + pad(date.getMinutes()),
                                        pressureText: record.systolic + "/" + record.diastolic,
                                        pulseText: record.pulse + " BPM",
                                        showSeparator: index < records.length - 1
                                    });
                                }
                                return rows;
                            },
                            seedHistoryFromLastMeasurement () {
                                if (!this.historyLoaded || !this.lastMeasurementLoaded || !this.measuredAt) return;
                                if (this.historyRows.length > 0) return;
                                this.addHistoryMeasurement();
                            },
                            addHistoryMeasurement () {
                                const records = [
                                    {
                                        systolic: this.systolic,
                                        diastolic: this.diastolic,
                                        pulse: this.pulse,
                                        measuredAt: this.measuredAt
                                    }
                                ];
                                for(let index = 0; index < this.historyRows.length; index += 1){
                                    const row = this.historyRows[index];
                                    if (row.measuredAt !== this.measuredAt) {
                                        records.push({
                                            systolic: row.systolic,
                                            diastolic: row.diastolic,
                                            pulse: row.pulse,
                                            measuredAt: row.measuredAt
                                        });
                                        if (records.length >= BLOOD_PRESSURE_HISTORY_LIMIT) break;
                                    }
                                }
                                this.setHistoryRecords(records);
                                this.persistHistory();
                            },
                            persistHistory () {
                                const records = [];
                                for(let index = 0; index < this.historyRows.length; index += 1){
                                    const row = this.historyRows[index];
                                    records.push({
                                        systolic: row.systolic,
                                        diastolic: row.diastolic,
                                        pulse: row.pulse,
                                        measuredAt: row.measuredAt
                                    });
                                }
                                try {
                                    _system3.default.set({
                                        key: BLOOD_PRESSURE_HISTORY_KEY,
                                        value: JSON.stringify(records),
                                        fail: (data, code)=>{
                                            console.log("save blood pressure history failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save blood pressure history unavailable", error);
                                }
                            },
                            isValidPressure (systolic, diastolic) {
                                const pulsePressure = systolic - diastolic;
                                return systolic >= SYSTOLIC_MIN && systolic <= SYSTOLIC_MAX && diastolic >= DIASTOLIC_MIN && diastolic <= DIASTOLIC_MAX && pulsePressure >= PULSE_PRESSURE_MIN && pulsePressure <= PULSE_PRESSURE_MAX;
                            },
                            applyPressure (systolic, diastolic) {
                                this.systolic = systolic;
                                this.diastolic = diastolic;
                                this.systolicText = String(systolic);
                                this.diastolicText = String(diastolic);
                                this.pressureStatus = this.pressureLabel(systolic, diastolic);
                            },
                            assignRandomPressure () {
                                const diastolic = randomInteger(DIASTOLIC_MIN, DIASTOLIC_MAX);
                                const minimumSystolic = Math.max(SYSTOLIC_MIN, diastolic + PULSE_PRESSURE_MIN);
                                const maximumSystolic = Math.min(SYSTOLIC_MAX, diastolic + PULSE_PRESSURE_MAX);
                                const systolic = randomInteger(minimumSystolic, maximumSystolic);
                                this.applyPressure(systolic, diastolic);
                            },
                            applyPulseReading (payload) {
                                const pulse = Math.round(Number(payload.bpm) || 0);
                                if ("vela-emulator-heart-rate" !== payload.source || pulse < 30 || pulse > 240) return;
                                this.sensorPulse = pulse;
                                if (this.measuring) {
                                    this.pulse = pulse;
                                    this.pulseText = String(pulse);
                                }
                            },
                            refreshSensorValue () {
                                if (this.sensorReading) return;
                                this.sensorReading = true;
                                try {
                                    _system2.default.readText({
                                        uri: HEART_RATE_URI,
                                        success: (data)=>{
                                            try {
                                                this.applyPulseReading(JSON.parse(data && data.text ? data.text : "{}"));
                                            } catch (error) {
                                                console.log("blood pressure pulse parse failed", error);
                                            }
                                        },
                                        fail: (data, code)=>{
                                            console.log("blood pressure pulse read failed", code, data);
                                        },
                                        complete: ()=>{
                                            this.sensorReading = false;
                                        }
                                    });
                                } catch (error) {
                                    this.sensorReading = false;
                                    console.log("blood pressure pulse unavailable", error);
                                }
                            },
                            startMeasurement () {
                                if (this.measuring || this.leaving) return;
                                this.measuring = true;
                                this.measurementState = "正在测量";
                                this.measureHint = "请佩戴好手表并保持静止";
                                this.pulse = this.sensorPulse;
                                this.pulseText = String(this.sensorPulse);
                                this.refreshSensorValue();
                                this.sensorPollTimerId = setInterval(()=>this.refreshSensorValue(), SENSOR_POLL_MS);
                                this.measurementTimerId = setTimeout(()=>{
                                    this.measurementTimerId = null;
                                    this.finishMeasurement();
                                }, MEASUREMENT_DURATION_MS);
                            },
                            finishMeasurement () {
                                if (!this.measuring) return;
                                this.stopSensorPolling();
                                this.assignRandomPressure();
                                this.measuring = false;
                                this.measuredAt = Date.now();
                                this.measurementState = "测量结果";
                                this.measureHint = "点击任意位置重新测量";
                                this.persistMeasurement();
                                this.addHistoryMeasurement();
                                _healthRecords.default.recordBloodPressure(this.systolic, this.diastolic, this.pulse, this.measuredAt);
                            },
                            persistMeasurement () {
                                try {
                                    _system3.default.set({
                                        key: LAST_BLOOD_PRESSURE_KEY,
                                        value: JSON.stringify({
                                            systolic: this.systolic,
                                            diastolic: this.diastolic,
                                            pulse: this.pulse,
                                            measuredAt: this.measuredAt
                                        }),
                                        fail: (data, code)=>{
                                            console.log("save blood pressure failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save blood pressure unavailable", error);
                                }
                            },
                            stopSensorPolling () {
                                if (!this.sensorPollTimerId) return;
                                clearInterval(this.sensorPollTimerId);
                                this.sensorPollTimerId = null;
                                this.sensorReading = false;
                            },
                            stopMeasurement () {
                                if (this.measurementTimerId) {
                                    clearTimeout(this.measurementTimerId);
                                    this.measurementTimerId = null;
                                }
                                this.stopSensorPolling();
                                this.measuring = false;
                            },
                            touchPoint (event, ending) {
                                if (!event) return {
                                    x: 0,
                                    y: 0
                                };
                                const list = ending ? event.changedTouches || event.touches : event.touches;
                                const point = list && list.length ? list[0] : event;
                                const x = void 0 !== point.clientX ? point.clientX : void 0 !== point.pageX ? point.pageX : point.x || 0;
                                const y = void 0 !== point.clientY ? point.clientY : void 0 !== point.pageY ? point.pageY : point.y || 0;
                                return {
                                    x: x,
                                    y: y
                                };
                            },
                            lockViewport (event) {
                                if (event && event.stop) event.stop();
                                return true;
                            },
                            onTouchStart (event) {
                                if (this.measuring) return;
                                const point = this.touchPoint(event, false);
                                this.touchX = point.x;
                                this.touchY = point.y;
                            },
                            onTouchEnd (event) {
                                if (this.measuring) return;
                                const point = this.touchPoint(event, true);
                                const deltaX = point.x - this.touchX;
                                const deltaY = point.y - this.touchY;
                                const vertical = Math.abs(deltaY) > 60 && Math.abs(deltaY) > 1.2 * Math.abs(deltaX);
                                if (vertical) return void this.returnToHealth();
                                const horizontal = Math.abs(deltaX) > 60 && Math.abs(deltaX) > 1.2 * Math.abs(deltaY);
                                if (horizontal && !this.measuring) {
                                    if (deltaX < 0 && !this.historyVisible) this.showHistory();
                                    if (deltaX > 0) if (this.historyVisible) this.hideHistory();
                                    else this.returnToHeartRate();
                                    return;
                                }
                                if (!this.historyVisible && !this.measuring && Math.abs(deltaX) < 18 && Math.abs(deltaY) < 18) this.startMeasurement();
                            },
                            handleSwipe (event) {
                                if (this.measuring) return;
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if ("up" === direction || "down" === direction) return void this.returnToHealth();
                                if ("left" === direction && !this.historyVisible) return void this.showHistory();
                                if ("right" === direction) if (this.historyVisible) this.hideHistory();
                                else this.returnToHeartRate();
                            },
                            showHistory () {
                                if (this.measuring || this.leaving) return;
                                this.historyVisible = true;
                            },
                            hideHistory () {
                                if (this.leaving) return;
                                this.historyVisible = false;
                            },
                            returnToHeartRate () {
                                if (this.leaving) return;
                                this.leaving = true;
                                this.stopMeasurement();
                                try {
                                    _system.default.back();
                                } catch (error) {
                                    this.leaving = false;
                                    console.log("close blood pressure failed", error);
                                }
                            },
                            returnToHealth () {
                                if (this.leaving) return;
                                this.leaving = true;
                                this.stopMeasurement();
                                try {
                                    _system.default.replace({
                                        uri: "/pages/health"
                                    });
                                } catch (error) {
                                    this.leaving = false;
                                    console.log("return blood pressure to health failed", error);
                                }
                            }
                        };
                        const moduleOwn = exports.default || module.exports;
                        const accessors = [
                            'public',
                            'protected',
                            'private'
                        ];
                        if (moduleOwn.data && accessors.some(function(acc) {
                            return moduleOwn[acc];
                        })) throw new Error('页面VM对象中的属性data不可与"' + accessors.join(',') + '"同时存在，请使用private替换data名称');
                        if (!moduleOwn.data) {
                            moduleOwn.data = {};
                            moduleOwn._descriptor = {};
                            accessors.forEach(function(acc) {
                                const accType = typeof moduleOwn[acc];
                                if ('object' === accType) {
                                    moduleOwn.data = Object.assign(moduleOwn.data, moduleOwn[acc]);
                                    for(const name in moduleOwn[acc])moduleOwn._descriptor[name] = {
                                        access: acc
                                    };
                                } else if ('function' === accType) console.warn('页面VM对象中的属性' + acc + '的值不能是函数，请使用对象');
                            });
                        }
                    };
                    var $app_template$ = function(vm) {
                        const _vm_ = vm || this;
                        return aiot.__ce__("div", {
                            __vm__: _vm_,
                            __opts__: {
                                classList: [
                                    "board-screen"
                                ]
                            }
                        }, [
                            aiot.__ce__("div", {
                                __vm__: _vm_,
                                __opts__: {
                                    classList: [
                                        "watch-area"
                                    ]
                                }
                            }, [
                                aiot.__ce__("div", {
                                    __vm__: _vm_,
                                    __opts__: {
                                        classList: [
                                            "page"
                                        ],
                                        events: {
                                            swipe: function(evt) {
                                                return _vm_.handleSwipe(evt);
                                            },
                                            touchstart: function(evt) {
                                                return _vm_.onTouchStart(evt);
                                            },
                                            touchmove: function(evt) {
                                                return _vm_.lockViewport(evt);
                                            },
                                            touchend: function(evt) {
                                                return _vm_.onTouchEnd(evt);
                                            }
                                        }
                                    }
                                }, [
                                    aiot.__ce__("image", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "scene"
                                            ],
                                            src: function() {
                                                return _vm_.backgroundImage;
                                            }
                                        }
                                    }, []),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "dark-overlay"
                                            ]
                                        }
                                    }, []),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return !_vm_.historyVisible;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "measurement-view"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "page-title"
                                                        ],
                                                        value: "血压"
                                                    }
                                                }, []),
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "pressure-card"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "measurement-state"
                                                            ],
                                                            value: function() {
                                                                return _vm_.measurementState;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "pressure-row"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "pressure-value-block"
                                                                ]
                                                            }
                                                        }, [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "pressure-value",
                                                                        "systolic"
                                                                    ],
                                                                    value: function() {
                                                                        return _vm_.systolicText;
                                                                    }
                                                                }
                                                            }, []),
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "pressure-label"
                                                                    ],
                                                                    value: "收缩压"
                                                                }
                                                            }, [])
                                                        ]),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "pressure-divider"
                                                                ],
                                                                value: "/"
                                                            }
                                                        }, []),
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "pressure-value-block"
                                                                ]
                                                            }
                                                        }, [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "pressure-value",
                                                                        "diastolic"
                                                                    ],
                                                                    value: function() {
                                                                        return _vm_.diastolicText;
                                                                    }
                                                                }
                                                            }, []),
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "pressure-label"
                                                                    ],
                                                                    value: "舒张压"
                                                                }
                                                            }, [])
                                                        ])
                                                    ]),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "pressure-unit"
                                                            ],
                                                            value: "mmHg"
                                                        }
                                                    }, []),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "status-pill"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "status-text"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.pressureStatus;
                                                                }
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "pulse-row"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "pulse-label"
                                                                ],
                                                                value: "脉搏"
                                                            }
                                                        }, []),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "pulse-value"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.pulseText;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "pulse-unit"
                                                                ],
                                                                value: "BPM"
                                                            }
                                                        }, [])
                                                    ])
                                                ]),
                                                aiot.__ci__({
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        shown: function() {
                                                            return _vm_.measuring;
                                                        }
                                                    }
                                                }, function() {
                                                    return [
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "measuring-dots"
                                                                ]
                                                            }
                                                        }, [
                                                            aiot.__ce__("div", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "measure-dot"
                                                                    ]
                                                                }
                                                            }, []),
                                                            aiot.__ce__("div", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "measure-dot",
                                                                        "dot-middle"
                                                                    ]
                                                                }
                                                            }, []),
                                                            aiot.__ce__("div", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "measure-dot"
                                                                    ]
                                                                }
                                                            }, [])
                                                        ])
                                                    ];
                                                }),
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "measure-hint"
                                                        ],
                                                        value: function() {
                                                            return _vm_.measureHint;
                                                        }
                                                    }
                                                }, [])
                                            ])
                                        ];
                                    }),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.historyVisible;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "history-view"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "history-title"
                                                        ],
                                                        value: "血压历史"
                                                    }
                                                }, []),
                                                aiot.__ci__({
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        shown: function() {
                                                            return _vm_.historyHasRecords;
                                                        }
                                                    }
                                                }, function() {
                                                    return [
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "history-card"
                                                                ]
                                                            }
                                                        }, [
                                                            aiot.__cf__({
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    exp: function() {
                                                                        return {
                                                                            __list__: _vm_.historyRows,
                                                                            __tid__: "id"
                                                                        };
                                                                    },
                                                                    key: "$idx",
                                                                    value: "$item"
                                                                }
                                                            }, function($idx, $item) {
                                                                return [
                                                                    aiot.__ce__("div", {
                                                                        __vm__: _vm_,
                                                                        __opts__: {
                                                                            classList: [
                                                                                "history-row"
                                                                            ]
                                                                        }
                                                                    }, [
                                                                        aiot.__ce__("div", {
                                                                            __vm__: _vm_,
                                                                            __opts__: {
                                                                                classList: [
                                                                                    "history-dot"
                                                                                ]
                                                                            }
                                                                        }, []),
                                                                        aiot.__ce__("div", {
                                                                            __vm__: _vm_,
                                                                            __opts__: {
                                                                                classList: [
                                                                                    "history-copy"
                                                                                ]
                                                                            }
                                                                        }, [
                                                                            aiot.__ce__("text", {
                                                                                __vm__: _vm_,
                                                                                __opts__: {
                                                                                    classList: [
                                                                                        "history-date"
                                                                                    ],
                                                                                    value: function() {
                                                                                        return $item.dateText;
                                                                                    }
                                                                                }
                                                                            }, []),
                                                                            aiot.__ce__("text", {
                                                                                __vm__: _vm_,
                                                                                __opts__: {
                                                                                    classList: [
                                                                                        "history-time"
                                                                                    ],
                                                                                    value: function() {
                                                                                        return $item.timeText;
                                                                                    }
                                                                                }
                                                                            }, [])
                                                                        ]),
                                                                        aiot.__ce__("text", {
                                                                            __vm__: _vm_,
                                                                            __opts__: {
                                                                                classList: [
                                                                                    "history-pressure"
                                                                                ],
                                                                                value: function() {
                                                                                    return $item.pressureText;
                                                                                }
                                                                            }
                                                                        }, []),
                                                                        aiot.__ce__("text", {
                                                                            __vm__: _vm_,
                                                                            __opts__: {
                                                                                classList: [
                                                                                    "history-pressure-unit"
                                                                                ],
                                                                                value: "mmHg"
                                                                            }
                                                                        }, []),
                                                                        aiot.__ce__("text", {
                                                                            __vm__: _vm_,
                                                                            __opts__: {
                                                                                classList: [
                                                                                    "history-pulse"
                                                                                ],
                                                                                value: function() {
                                                                                    return $item.pulseText;
                                                                                }
                                                                            }
                                                                        }, []),
                                                                        aiot.__ci__({
                                                                            __vm__: _vm_,
                                                                            __opts__: {
                                                                                shown: function() {
                                                                                    return $item.showSeparator;
                                                                                }
                                                                            }
                                                                        }, function() {
                                                                            return [
                                                                                aiot.__ce__("div", {
                                                                                    __vm__: _vm_,
                                                                                    __opts__: {
                                                                                        classList: [
                                                                                            "history-separator"
                                                                                        ]
                                                                                    }
                                                                                }, [])
                                                                            ];
                                                                        })
                                                                    ])
                                                                ];
                                                            })
                                                        ])
                                                    ];
                                                }),
                                                aiot.__ci__({
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        shown: function() {
                                                            return !_vm_.historyHasRecords;
                                                        }
                                                    }
                                                }, function() {
                                                    return [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "history-empty"
                                                                ],
                                                                value: "暂无血压测量记录"
                                                            }
                                                        }, [])
                                                    ];
                                                })
                                            ])
                                        ];
                                    })
                                ])
                            ])
                        ]);
                    };
                    $app_exports$['entry'] = function($app_exports$) {
                        $app_script$({}, $app_exports$, $app_require$1);
                        $app_exports$.default.template = $app_template$;
                        $app_exports$.default.style = $app_style$;
                    };
                })();
            })();
        };
        return createPageHandler();
    })(global, globalThis, window, $app_exports$, $app_evaluate$);
}
