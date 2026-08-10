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
                                    "viewport-lock"
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
                                backgroundColor: "rgba(0, 0, 0, 0.6)"
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
                                width: "180px",
                                height: "58px",
                                position: "absolute",
                                left: "126px",
                                top: "24px",
                                color: "#ffffff",
                                fontSize: "43px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "heart-visual"
                                ]
                            ],
                            {
                                width: "205px",
                                height: "205px",
                                position: "absolute",
                                left: "113px",
                                top: "59px",
                                objectFit: "contain"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "rate-label"
                                ]
                            ],
                            {
                                width: "286px",
                                height: "36px",
                                position: "absolute",
                                left: "73px",
                                top: "289px",
                                color: "#d7d9df",
                                fontSize: "23px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "rate-row"
                                ]
                            ],
                            {
                                width: "286px",
                                height: "85px",
                                position: "absolute",
                                left: "73px",
                                top: "314px",
                                flexDirection: "row",
                                alignItems: "flex-end",
                                justifyContent: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "rate-value"
                                ]
                            ],
                            {
                                width: "190px",
                                height: "85px",
                                color: "#ff3030",
                                fontSize: "76px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "rate-unit"
                                ]
                            ],
                            {
                                width: "78px",
                                height: "45px",
                                marginBottom: "7px",
                                color: "#d7d9df",
                                fontSize: "29px",
                                fontWeight: "bold"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "last-time"
                                ]
                            ],
                            {
                                width: "280px",
                                height: "25px",
                                position: "absolute",
                                left: "76px",
                                top: "398px",
                                color: "#d4d5da",
                                fontSize: "15px",
                                textAlign: "center"
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
                                width: "300px",
                                height: "25px",
                                position: "absolute",
                                left: "66px",
                                top: "260px",
                                color: "#ffffff",
                                fontSize: "16px",
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
                                width: "12px",
                                height: "12px",
                                marginLeft: "24px",
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
                                width: "168px",
                                height: "44px",
                                marginLeft: "15px",
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
                                width: "168px",
                                height: "25px",
                                color: "#102b61",
                                fontSize: "21px",
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
                                width: "168px",
                                height: "18px",
                                color: "#76839a",
                                fontSize: "14px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-value"
                                ]
                            ],
                            {
                                width: "95px",
                                height: "35px",
                                color: "#ff3030",
                                fontSize: "29px",
                                fontWeight: "bold",
                                textAlign: "right"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-unit"
                                ]
                            ],
                            {
                                width: "52px",
                                height: "25px",
                                marginLeft: "8px",
                                color: "#102b61",
                                fontSize: "16px",
                                fontWeight: "bold"
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
                                width: "354px",
                                height: "1px",
                                position: "absolute",
                                left: "25px",
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
                                width: "310px",
                                height: "44px",
                                position: "absolute",
                                left: "61px",
                                top: "216px",
                                color: "#ffffff",
                                fontSize: "24px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "standby-cover"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                zIndex: 100,
                                backgroundColor: "#000000"
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
                        const APP_RESOURCE_ROOT = "/data/app/com.application.watch.redesign";
                        const HEART_RATE_URI = "/common/emulator-heart-rate.json";
                        const LAST_HEART_RATE_KEY = "last_heart_rate_measurement_v1";
                        const HEART_RATE_HISTORY_KEY = "heart_rate_history_v1";
                        const HEART_RATE_HISTORY_LIMIT = 7;
                        const MEASUREMENT_DURATION_MS = 8000;
                        const SENSOR_POLL_MS = 650;
                        const STANDBY_DELAY_MS = 60000;
                        function pad(value) {
                            return value < 10 ? "0" + value : "" + value;
                        }
                        function heartFrames() {
                            const frames = [];
                            for(let index = 1; index <= 12; index += 1){
                                const number = index < 10 ? "0" + index : "" + index;
                                frames.push({
                                    src: APP_RESOURCE_ROOT + "/common/heart-rate/frame-" + number + ".png"
                                });
                            }
                            return frames;
                        }
                        var _default = exports.default = {
                            private: {
                                backgroundImage: "/common/backgrounds/dark-purple.png",
                                heartFrames: heartFrames(),
                                measuring: false,
                                heartRate: 78,
                                sensorHeartRate: 78,
                                heartRateText: "78",
                                rateLabel: "上次心率",
                                lastMeasuredText: "上次测量：暂无时间记录",
                                measuredAt: 0,
                                historyVisible: false,
                                historyRows: [],
                                historyHasRecords: false,
                                historyLoaded: false,
                                lastMeasurementLoaded: false,
                                sensorReading: false,
                                sensorPollTimerId: null,
                                measurementTimerId: null,
                                animatorStartTimerId: null,
                                standbyTimerId: null,
                                screenStandby: false,
                                ignoreTouchEnd: false,
                                touchX: 0,
                                touchY: 0,
                                leaving: false,
                                returnTimerId: null
                            },
                            onInit () {
                                this.syncCustomization();
                                this.loadLastMeasurement();
                                this.loadHistory();
                            },
                            onShow () {
                                this.$app.$def.ensureWakeableScreen();
                                this.leaving = false;
                                this.screenStandby = false;
                                this.ignoreTouchEnd = false;
                                this.syncCustomization();
                                this.refreshSensorValue();
                                this.startStandbyTimer();
                            },
                            onHide () {
                                this.stopMeasurement(false);
                                this.stopStandbyTimer();
                                this.cancelReturn();
                            },
                            onDestroy () {
                                this.stopMeasurement(false);
                                this.stopStandbyTimer();
                                this.cancelReturn();
                            },
                            syncCustomization () {
                                const customization = this.$app.$def.getCustomization();
                                const background = (0, _customization.getBackground)(customization.backgroundId);
                                this.backgroundImage = background.src;
                            },
                            loadLastMeasurement () {
                                try {
                                    _system3.default.get({
                                        key: LAST_HEART_RATE_KEY,
                                        default: "",
                                        success: (value)=>{
                                            if (!value) {
                                                this.updateLastMeasuredText();
                                                this.lastMeasurementLoaded = true;
                                                this.seedHistoryFromLastMeasurement();
                                                return;
                                            }
                                            try {
                                                const parsed = JSON.parse(value);
                                                const bpm = Math.round(Number(parsed.bpm) || 0);
                                                if (bpm >= 30 && bpm <= 240) {
                                                    this.heartRate = bpm;
                                                    this.heartRateText = String(bpm);
                                                }
                                                this.measuredAt = Math.max(0, Number(parsed.measuredAt) || 0);
                                            } catch (error) {
                                                console.log("parse last heart rate failed", error);
                                            }
                                            this.updateLastMeasuredText();
                                            this.lastMeasurementLoaded = true;
                                            this.seedHistoryFromLastMeasurement();
                                        },
                                        fail: (data, code)=>{
                                            console.log("load last heart rate failed", code, data);
                                            this.updateLastMeasuredText();
                                            this.lastMeasurementLoaded = true;
                                            this.seedHistoryFromLastMeasurement();
                                        }
                                    });
                                } catch (error) {
                                    console.log("last heart rate storage unavailable", error);
                                    this.updateLastMeasuredText();
                                    this.lastMeasurementLoaded = true;
                                    this.seedHistoryFromLastMeasurement();
                                }
                            },
                            loadHistory () {
                                try {
                                    _system3.default.get({
                                        key: HEART_RATE_HISTORY_KEY,
                                        default: "",
                                        success: (value)=>{
                                            let records = [];
                                            if (value) {
                                                try {
                                                    const parsed = JSON.parse(value);
                                                    if (Array.isArray(parsed)) records = parsed;
                                                } catch (error) {
                                                    console.log("parse heart rate history failed", error);
                                                }
                                            }
                                            this.setHistoryRecords(records);
                                            this.historyLoaded = true;
                                            this.seedHistoryFromLastMeasurement();
                                        },
                                        fail: (data, code)=>{
                                            console.log("load heart rate history failed", code, data);
                                            this.setHistoryRecords([]);
                                            this.historyLoaded = true;
                                            this.seedHistoryFromLastMeasurement();
                                        }
                                    });
                                } catch (error) {
                                    console.log("heart rate history storage unavailable", error);
                                    this.setHistoryRecords([]);
                                    this.historyLoaded = true;
                                    this.seedHistoryFromLastMeasurement();
                                }
                            },
                            setHistoryRecords (source) {
                                const records = [];
                                const list = Array.isArray(source) ? source : [];
                                for(let index = 0; index < list.length; index += 1){
                                    const bpm = Math.round(Number(list[index].bpm) || 0);
                                    const measuredAt = Math.max(0, Number(list[index].measuredAt) || 0);
                                    if (bpm < 30 || bpm > 240 || !measuredAt) continue;
                                    let duplicate = false;
                                    for(let rowIndex = 0; rowIndex < records.length; rowIndex += 1)if (records[rowIndex].measuredAt === measuredAt) duplicate = true;
                                    if (!duplicate) records.push({
                                        bpm: bpm,
                                        measuredAt: measuredAt
                                    });
                                    if (records.length >= HEART_RATE_HISTORY_LIMIT) break;
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
                                        bpm: record.bpm,
                                        measuredAt: record.measuredAt,
                                        dateText: pad(date.getMonth() + 1) + "月" + pad(date.getDate()) + "日",
                                        timeText: pad(date.getHours()) + ":" + pad(date.getMinutes()),
                                        showSeparator: index < records.length - 1
                                    });
                                }
                                return rows;
                            },
                            seedHistoryFromLastMeasurement () {
                                if (!this.historyLoaded || !this.lastMeasurementLoaded || !this.measuredAt) return;
                                if (this.historyRows.length > 0) return;
                                this.addHistoryMeasurement(this.heartRate, this.measuredAt);
                            },
                            addHistoryMeasurement (bpm, measuredAt) {
                                const records = [
                                    {
                                        bpm: bpm,
                                        measuredAt: measuredAt
                                    }
                                ];
                                for(let index = 0; index < this.historyRows.length; index += 1){
                                    const row = this.historyRows[index];
                                    if (row.measuredAt !== measuredAt) {
                                        records.push({
                                            bpm: row.bpm,
                                            measuredAt: row.measuredAt
                                        });
                                        if (records.length >= HEART_RATE_HISTORY_LIMIT) break;
                                    }
                                }
                                this.setHistoryRecords(records);
                                this.persistHistory();
                            },
                            persistHistory () {
                                const records = [];
                                for(let index = 0; index < this.historyRows.length; index += 1)records.push({
                                    bpm: this.historyRows[index].bpm,
                                    measuredAt: this.historyRows[index].measuredAt
                                });
                                try {
                                    _system3.default.set({
                                        key: HEART_RATE_HISTORY_KEY,
                                        value: JSON.stringify(records),
                                        fail: (data, code)=>{
                                            console.log("save heart rate history failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save heart rate history unavailable", error);
                                }
                            },
                            updateLastMeasuredText () {
                                if (!this.measuredAt) {
                                    this.lastMeasuredText = "上次测量：暂无时间记录";
                                    return;
                                }
                                const date = new Date(this.measuredAt);
                                this.lastMeasuredText = "上次测量：" + pad(date.getMonth() + 1) + "-" + pad(date.getDate()) + " " + pad(date.getHours()) + ":" + pad(date.getMinutes());
                            },
                            startMeasurement () {
                                if (this.measuring || this.leaving || this.screenStandby) return;
                                this.registerActivity();
                                this.measuring = true;
                                this.rateLabel = "当前心率";
                                this.refreshSensorValue();
                                this.startSensorPolling();
                                this.queueAnimatorStart();
                                this.measurementTimerId = setTimeout(()=>{
                                    this.measurementTimerId = null;
                                    this.finishMeasurement();
                                }, MEASUREMENT_DURATION_MS);
                            },
                            finishMeasurement () {
                                if (!this.measuring) return;
                                this.stopSensorPolling();
                                this.stopAnimator();
                                this.measuring = false;
                                this.rateLabel = "上次心率";
                                this.measuredAt = Date.now();
                                this.updateLastMeasuredText();
                                this.persistMeasurement();
                                this.addHistoryMeasurement(this.heartRate, this.measuredAt);
                                _healthRecords.default.recordHeartRate(this.heartRate, this.measuredAt);
                                this.startStandbyTimer();
                            },
                            stopMeasurement (saveResult) {
                                if (this.measurementTimerId) {
                                    clearTimeout(this.measurementTimerId);
                                    this.measurementTimerId = null;
                                }
                                this.stopSensorPolling();
                                this.cancelAnimatorStart();
                                this.stopAnimator();
                                if (saveResult && this.measuring) this.persistMeasurement();
                                this.measuring = false;
                                this.rateLabel = "上次心率";
                            },
                            persistMeasurement () {
                                try {
                                    _system3.default.set({
                                        key: LAST_HEART_RATE_KEY,
                                        value: JSON.stringify({
                                            bpm: this.heartRate,
                                            measuredAt: this.measuredAt
                                        }),
                                        fail: (data, code)=>{
                                            console.log("save heart rate failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save heart rate unavailable", error);
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
                                                const payload = JSON.parse(data && data.text ? data.text : "{}");
                                                const bpm = Math.round(Number(payload.bpm) || 0);
                                                if ("vela-emulator-heart-rate" === payload.source && bpm >= 30 && bpm <= 240) {
                                                    this.sensorHeartRate = bpm;
                                                    if (this.measuring) {
                                                        this.heartRate = bpm;
                                                        this.heartRateText = String(bpm);
                                                    }
                                                }
                                            } catch (error) {
                                                console.log("heart rate bridge parse failed", error);
                                            }
                                        },
                                        fail: (data, code)=>{
                                            console.log("heart rate bridge read failed", code, data);
                                        },
                                        complete: ()=>{
                                            this.sensorReading = false;
                                        }
                                    });
                                } catch (error) {
                                    this.sensorReading = false;
                                    console.log("heart rate bridge unavailable", error);
                                }
                            },
                            startSensorPolling () {
                                this.stopSensorPolling();
                                this.refreshSensorValue();
                                this.sensorPollTimerId = setInterval(()=>this.refreshSensorValue(), SENSOR_POLL_MS);
                            },
                            stopSensorPolling () {
                                if (!this.sensorPollTimerId) return;
                                clearInterval(this.sensorPollTimerId);
                                this.sensorPollTimerId = null;
                                this.sensorReading = false;
                            },
                            queueAnimatorStart () {
                                this.cancelAnimatorStart();
                                this.animatorStartTimerId = setTimeout(()=>{
                                    this.animatorStartTimerId = null;
                                    const animator = this.$element("heartRateAnimator");
                                    if (!animator || !this.measuring) return;
                                    try {
                                        animator.start();
                                    } catch (error) {
                                        console.log("heart rate animator start failed", error);
                                    }
                                }, 100);
                            },
                            cancelAnimatorStart () {
                                if (!this.animatorStartTimerId) return;
                                clearTimeout(this.animatorStartTimerId);
                                this.animatorStartTimerId = null;
                            },
                            stopAnimator () {
                                this.cancelAnimatorStart();
                                const animator = this.$element("heartRateAnimator");
                                if (!animator) return;
                                try {
                                    animator.stop();
                                } catch (error) {
                                    console.log("heart rate animator stop failed", error);
                                }
                            },
                            startStandbyTimer () {
                                this.stopStandbyTimer();
                                if (this.screenStandby || this.measuring) return;
                                this.standbyTimerId = setTimeout(()=>{
                                    this.standbyTimerId = null;
                                    this.enterStandby();
                                }, STANDBY_DELAY_MS);
                            },
                            stopStandbyTimer () {
                                if (!this.standbyTimerId) return;
                                clearTimeout(this.standbyTimerId);
                                this.standbyTimerId = null;
                            },
                            registerActivity () {
                                if (!this.screenStandby && !this.measuring) this.startStandbyTimer();
                            },
                            enterStandby () {
                                if (this.screenStandby || this.measuring) return;
                                this.screenStandby = true;
                            },
                            wakeScreen () {
                                if (!this.screenStandby) return void this.registerActivity();
                                this.screenStandby = false;
                                this.ignoreTouchEnd = true;
                                this.leaving = false;
                                this.syncCustomization();
                                this.refreshSensorValue();
                                this.startStandbyTimer();
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
                                if (this.screenStandby) {
                                    this.ignoreTouchEnd = true;
                                    this.wakeScreen();
                                    return;
                                }
                                if (this.ignoreTouchEnd) return void this.registerActivity();
                                if (this.measuring) return;
                                this.ignoreTouchEnd = false;
                                this.registerActivity();
                                const point = this.touchPoint(event, false);
                                this.touchX = point.x;
                                this.touchY = point.y;
                            },
                            onTouchEnd (event) {
                                if (this.ignoreTouchEnd) {
                                    this.ignoreTouchEnd = false;
                                    return;
                                }
                                if (this.measuring) return;
                                this.registerActivity();
                                const point = this.touchPoint(event, true);
                                const deltaX = point.x - this.touchX;
                                const deltaY = point.y - this.touchY;
                                const vertical = Math.abs(deltaY) > 60 && Math.abs(deltaY) > 1.2 * Math.abs(deltaX);
                                if (vertical) return void this.returnToHealth();
                                const horizontal = Math.abs(deltaX) > 60 && Math.abs(deltaX) > 1.2 * Math.abs(deltaY);
                                if (horizontal && !this.measuring) {
                                    if (deltaX < 0) if (this.historyVisible) this.openBloodPressure();
                                    else this.showHistory();
                                    if (deltaX > 0 && this.historyVisible) this.hideHistory();
                                    return;
                                }
                                if (!this.historyVisible && !this.measuring && Math.abs(deltaX) < 18 && Math.abs(deltaY) < 18) this.startMeasurement();
                            },
                            handleSwipe (event) {
                                if (this.screenStandby) return void this.wakeScreen();
                                if (this.measuring) return;
                                this.registerActivity();
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if ("up" === direction || "down" === direction) return void this.returnToHealth();
                                if ("left" === direction) {
                                    if (this.historyVisible) this.openBloodPressure();
                                    else this.showHistory();
                                    return;
                                }
                                if ("right" === direction && this.historyVisible) this.hideHistory();
                            },
                            showHistory () {
                                if (this.measuring || this.leaving) return;
                                this.historyVisible = true;
                                this.startStandbyTimer();
                            },
                            hideHistory () {
                                if (this.leaving) return;
                                this.historyVisible = false;
                                this.startStandbyTimer();
                            },
                            openBloodPressure () {
                                if (this.measuring || this.leaving) return;
                                this.leaving = true;
                                this.stopStandbyTimer();
                                try {
                                    _system.default.push({
                                        uri: "/pages/bloodpressure"
                                    });
                                } catch (error) {
                                    this.leaving = false;
                                    console.log("open blood pressure failed", error);
                                }
                            },
                            queueReturn () {
                                this.cancelReturn();
                                this.returnTimerId = setTimeout(()=>{
                                    this.returnTimerId = null;
                                    try {
                                        _system.default.back();
                                    } catch (error) {
                                        this.leaving = false;
                                        console.log("close heart rate failed", error);
                                    }
                                }, 120);
                            },
                            cancelReturn () {
                                if (!this.returnTimerId) return;
                                clearTimeout(this.returnTimerId);
                                this.returnTimerId = null;
                            },
                            returnToHealth () {
                                if (this.leaving) return;
                                this.leaving = true;
                                this.stopMeasurement(false);
                                this.queueReturn();
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
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "viewport-lock"
                                            ]
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
                                                            value: "心率"
                                                        }
                                                    }, []),
                                                    aiot.__ci__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            shown: function() {
                                                                return !_vm_.measuring;
                                                            }
                                                        }
                                                    }, function() {
                                                        return [
                                                            aiot.__ce__("image", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "heart-visual"
                                                                    ],
                                                                    src: "/common/heart-rate/frame-01.png"
                                                                }
                                                            }, [])
                                                        ];
                                                    }),
                                                    aiot.__ci__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            shown: function() {
                                                                return _vm_.measuring;
                                                            }
                                                        }
                                                    }, function() {
                                                        return [
                                                            aiot.__ce__("image-animator", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "heart-visual"
                                                                    ],
                                                                    id: "heartRateAnimator",
                                                                    images: function() {
                                                                        return _vm_.heartFrames;
                                                                    },
                                                                    duration: "85ms",
                                                                    iteration: "infinite",
                                                                    fixedsize: "true"
                                                                }
                                                            }, [])
                                                        ];
                                                    }),
                                                    aiot.__ci__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            shown: function() {
                                                                return !_vm_.measuring;
                                                            }
                                                        }
                                                    }, function() {
                                                        return [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "measure-hint"
                                                                    ],
                                                                    value: "请佩戴好手表并保持静止"
                                                                }
                                                            }, [])
                                                        ];
                                                    }),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "rate-label"
                                                            ],
                                                            value: function() {
                                                                return _vm_.rateLabel;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "rate-row"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "rate-value"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.heartRateText;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "rate-unit"
                                                                ],
                                                                value: "BPM"
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ci__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            shown: function() {
                                                                return !_vm_.measuring;
                                                            }
                                                        }
                                                    }, function() {
                                                        return [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "last-time"
                                                                    ],
                                                                    value: function() {
                                                                        return _vm_.lastMeasuredText;
                                                                    }
                                                                }
                                                            }, [])
                                                        ];
                                                    })
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
                                                            value: "心率历史"
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
                                                                                        "history-value"
                                                                                    ],
                                                                                    value: function() {
                                                                                        return $item.bpm;
                                                                                    }
                                                                                }
                                                                            }, []),
                                                                            aiot.__ce__("text", {
                                                                                __vm__: _vm_,
                                                                                __opts__: {
                                                                                    classList: [
                                                                                        "history-unit"
                                                                                    ],
                                                                                    value: "BPM"
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
                                                                    value: "暂无心率测量记录"
                                                                }
                                                            }, [])
                                                        ];
                                                    })
                                                ])
                                            ];
                                        })
                                    ]),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.screenStandby;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "standby-cover"
                                                    ],
                                                    events: {
                                                        touchstart: function(evt) {
                                                            return _vm_.wakeScreen(evt);
                                                        },
                                                        click: function(evt) {
                                                            return _vm_.wakeScreen(evt);
                                                        }
                                                    }
                                                }
                                            }, [])
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
