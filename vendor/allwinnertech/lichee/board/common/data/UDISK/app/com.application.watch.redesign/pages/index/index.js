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
                                position: "relative",
                                backgroundColor: "#08153b"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "gesture-layer"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                zIndex: 90,
                                backgroundColor: "transparent"
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
                                    "cat-animation"
                                ]
                            ],
                            {
                                width: "200px",
                                height: "205px",
                                position: "absolute",
                                left: "106px",
                                top: "245px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "clock"
                                ]
                            ],
                            {
                                width: "242px",
                                height: "92px",
                                position: "absolute",
                                left: "31px",
                                top: "42px",
                                color: "#ffffff",
                                fontSize: "68px",
                                fontWeight: "normal"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "date"
                                ]
                            ],
                            {
                                width: "142px",
                                height: "42px",
                                position: "absolute",
                                left: "34px",
                                top: "132px",
                                color: "#ffffff",
                                fontSize: "27px",
                                fontWeight: "normal"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "weekday"
                                ]
                            ],
                            {
                                width: "72px",
                                height: "42px",
                                position: "absolute",
                                left: "184px",
                                top: "132px",
                                color: "#ffffff",
                                fontSize: "27px",
                                fontWeight: "normal"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "home-weather-icon"
                                ]
                            ],
                            {
                                width: "72px",
                                height: "72px",
                                position: "absolute",
                                left: "255px",
                                top: "96px",
                                objectFit: "contain"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "home-temperature"
                                ]
                            ],
                            {
                                width: "98px",
                                height: "66px",
                                position: "absolute",
                                left: "329px",
                                top: "100px",
                                color: "#ffffff",
                                fontSize: "48px",
                                fontWeight: "normal",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "home-location-weather"
                                ]
                            ],
                            {
                                width: "138px",
                                height: "34px",
                                position: "absolute",
                                left: "289px",
                                top: "159px",
                                color: "#ffffff",
                                fontSize: "17px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "home-administrative"
                                ]
                            ],
                            {
                                width: "142px",
                                height: "30px",
                                position: "absolute",
                                left: "286px",
                                top: "188px",
                                color: "#ffffff",
                                fontSize: "14px",
                                fontWeight: "normal",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "battery-row"
                                ]
                            ],
                            {
                                width: "130px",
                                height: "34px",
                                position: "absolute",
                                left: "294px",
                                top: "45px",
                                flexDirection: "row",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "battery-charging-slot"
                                ]
                            ],
                            {
                                width: "28px",
                                height: "32px",
                                justifyContent: "center",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "battery-charging"
                                ]
                            ],
                            {
                                width: "24px",
                                height: "24px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "battery-icon"
                                ]
                            ],
                            {
                                width: "40px",
                                height: "22px",
                                paddingTop: "3px",
                                paddingRight: "3px",
                                paddingBottom: "3px",
                                paddingLeft: "3px",
                                borderTopWidth: "3px",
                                borderRightWidth: "3px",
                                borderBottomWidth: "3px",
                                borderLeftWidth: "3px",
                                borderTopColor: "#ffffff",
                                borderRightColor: "#ffffff",
                                borderBottomColor: "#ffffff",
                                borderLeftColor: "#ffffff",
                                borderRadius: "5px",
                                backgroundColor: "transparent"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "battery-level"
                                ]
                            ],
                            {
                                width: "28px",
                                height: "10px",
                                borderRadius: "2px",
                                backgroundColor: "#ffffff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "battery-cap"
                                ]
                            ],
                            {
                                width: "4px",
                                height: "10px",
                                marginLeft: "2px",
                                borderRadius: "2px",
                                backgroundColor: "#ffffff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "battery-text"
                                ]
                            ],
                            {
                                width: "50px",
                                marginLeft: "6px",
                                color: "#ffffff",
                                fontSize: "20px",
                                fontWeight: "bold"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "text-light"
                                ]
                            ],
                            {
                                color: "#ffffff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "text-sky"
                                ]
                            ],
                            {
                                color: "#142a65"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "text-brown"
                                ]
                            ],
                            {
                                color: "#5b362c"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "text-plum"
                                ]
                            ],
                            {
                                color: "#7a3156"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "border-light"
                                ]
                            ],
                            {
                                borderTopColor: "#ffffff",
                                borderRightColor: "#ffffff",
                                borderBottomColor: "#ffffff",
                                borderLeftColor: "#ffffff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "border-sky"
                                ]
                            ],
                            {
                                borderTopColor: "#142a65",
                                borderRightColor: "#142a65",
                                borderBottomColor: "#142a65",
                                borderLeftColor: "#142a65"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "border-brown"
                                ]
                            ],
                            {
                                borderTopColor: "#5b362c",
                                borderRightColor: "#5b362c",
                                borderBottomColor: "#5b362c",
                                borderLeftColor: "#5b362c"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "border-plum"
                                ]
                            ],
                            {
                                borderTopColor: "#7a3156",
                                borderRightColor: "#7a3156",
                                borderBottomColor: "#7a3156",
                                borderLeftColor: "#7a3156"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "fill-light"
                                ]
                            ],
                            {
                                backgroundColor: "#ffffff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "fill-sky"
                                ]
                            ],
                            {
                                backgroundColor: "#142a65"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "fill-brown"
                                ]
                            ],
                            {
                                backgroundColor: "#5b362c"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "fill-plum"
                                ]
                            ],
                            {
                                backgroundColor: "#7a3156"
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
                        var _system2 = _interopRequireDefault($app_require$1("@app-module/system.battery"));
                        var _system3 = _interopRequireDefault($app_require$1("@app-module/system.event"));
                        var _system4 = _interopRequireDefault($app_require$1("@app-module/system.file"));
                        var _customization = __webpack_require__("./src/common/customization.js");
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const APP_RESOURCE_ROOT = "/data/app/com.application.watch.redesign";
                        const BATTERY_FALLBACK_POLL_MS = 300000;
                        const BATTERY_BRIDGE_POLL_MS = 1000;
                        const BATTERY_BRIDGE_URI = "/common/emulator-battery.json";
                        const STANDBY_DELAY_MS = 60000;
                        const WEATHER_REFRESH_MS = 180000;
                        var _default = exports.default = {
                            private: {
                                timeText: "12:44",
                                dateText: "07月23日",
                                weekdayText: "周四",
                                batteryPercent: 82,
                                batteryCharging: false,
                                backgroundImage: "/common/background.png",
                                clockClass: "clock text-light",
                                dateClass: "date text-light",
                                weekdayClass: "weekday text-light",
                                batteryIconClass: "battery-icon border-light",
                                batteryLevelClass: "battery-level fill-light",
                                batteryLevelStyle: {
                                    width: "23px"
                                },
                                batteryCapClass: "battery-cap fill-light",
                                batteryTextClass: "battery-text text-light",
                                batteryChargingIcon: "/common/icons/charging-light.png",
                                homeWeatherIcon: "/common/weather-icons/999.png",
                                homeTemperature: "--°",
                                homeLocationWeather: "定位中",
                                homeAdministrative: "正在获取当前位置",
                                homeTemperatureClass: "home-temperature text-light",
                                homeLocationWeatherClass: "home-location-weather text-light",
                                homeAdministrativeClass: "home-administrative text-light",
                                weatherTimerId: null,
                                weatherRequestSerial: 0,
                                locationListener: null,
                                timerId: null,
                                batteryPollTimerId: null,
                                batteryBridgeTimerId: null,
                                batteryEventId: null,
                                batteryReading: false,
                                batteryBridgeReading: false,
                                batterySensorReady: false,
                                catFrames: [],
                                catDuration: "3400ms",
                                catHealthTimerId: null,
                                catStartTimerId: null,
                                catAnimatorReady: false,
                                activeActionId: "",
                                touchX: 0,
                                touchY: 0,
                                standbyTimerId: null,
                                screenStandby: false,
                                ignoreTouchEnd: false,
                                selectorOpening: false,
                                specialSportMode: false,
                                specialSportListener: null
                            },
                            onInit () {
                                this.syncCustomization();
                                this.updateClock();
                            },
                            onReady () {
                                this.catAnimatorReady = true;
                                this.queueCatAnimationStart();
                            },
                            onShow () {
                                this.$app.$def.ensureWakeableScreen();
                                this.selectorOpening = false;
                                this.specialSportMode = this.$app.$def.isSpecialSportMode();
                                this.specialSportListener = (snapshot)=>{
                                    this.specialSportMode = !!(snapshot && snapshot.active);
                                };
                                this.$app.$def.subscribeSpecialSportHeartRate(this.specialSportListener);
                                this.screenStandby = false;
                                this.ignoreTouchEnd = false;
                                this.syncCustomization();
                                this.startClock();
                                this.startBatteryMonitoring();
                                this.startWeatherMonitoring();
                                if (this.catAnimatorReady) this.queueCatAnimationStart();
                                this.startCatHealthCheck();
                                this.startStandbyTimer();
                            },
                            onHide () {
                                this.unsubscribeSpecialSportMode();
                                this.stopStandbyTimer();
                                this.stopClock();
                                this.stopWeatherMonitoring();
                                this.stopBatteryMonitoring(true);
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.stopCatHealthCheck();
                            },
                            onDestroy () {
                                this.unsubscribeSpecialSportMode();
                                this.stopStandbyTimer();
                                this.stopClock();
                                this.stopWeatherMonitoring();
                                this.stopBatteryMonitoring();
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.stopCatHealthCheck();
                                this.catAnimatorReady = false;
                            },
                            unsubscribeSpecialSportMode () {
                                if (!this.specialSportListener) return;
                                this.$app.$def.unsubscribeSpecialSportHeartRate(this.specialSportListener);
                                this.specialSportListener = null;
                            },
                            startStandbyTimer () {
                                this.stopStandbyTimer();
                                if (this.screenStandby) return;
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
                                if (!this.screenStandby) this.startStandbyTimer();
                            },
                            enterStandby () {
                                if (this.screenStandby) return;
                                this.$app.$def.prepareRuntimeForStandby();
                                this.screenStandby = true;
                                this.stopClock();
                                this.stopBatteryMonitoring();
                                this.stopWeatherMonitoring();
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.stopCatHealthCheck();
                            },
                            wakeScreen () {
                                if (!this.screenStandby) return void this.registerActivity();
                                this.screenStandby = false;
                                this.$app.$def.resumeRuntime();
                                this.ignoreTouchEnd = true;
                                this.selectorOpening = false;
                                this.syncCustomization();
                                this.updateClock();
                                this.startClock();
                                this.startBatteryMonitoring();
                                this.startWeatherMonitoring();
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.queueCatAnimationStart();
                                this.startCatHealthCheck();
                                this.startStandbyTimer();
                            },
                            pad (value) {
                                return value < 10 ? "0" + value : "" + value;
                            },
                            updateClock () {
                                const now = new Date();
                                const nextTime = this.pad(now.getHours()) + ":" + this.pad(now.getMinutes());
                                const nextDate = this.pad(now.getMonth() + 1) + "月" + this.pad(now.getDate()) + "日";
                                const weekdays = [
                                    "周日",
                                    "周一",
                                    "周二",
                                    "周三",
                                    "周四",
                                    "周五",
                                    "周六"
                                ];
                                const nextWeekday = weekdays[now.getDay()];
                                if (this.timeText !== nextTime) this.timeText = nextTime;
                                if (this.dateText !== nextDate) this.dateText = nextDate;
                                if (this.weekdayText !== nextWeekday) this.weekdayText = nextWeekday;
                            },
                            startClock () {
                                if (this.timerId) return;
                                this.updateClock();
                                this.scheduleNextClockTick();
                            },
                            scheduleNextClockTick () {
                                const now = new Date();
                                const delay = Math.max(1000, (60 - now.getSeconds()) * 1000);
                                this.timerId = setTimeout(()=>{
                                    this.timerId = null;
                                    this.updateClock();
                                    this.scheduleNextClockTick();
                                }, delay);
                            },
                            stopClock () {
                                if (!this.timerId) return;
                                clearTimeout(this.timerId);
                                this.timerId = null;
                            },
                            showHomeWeatherLoading (cityName) {
                                this.homeWeatherIcon = "/common/weather-icons/999.png";
                                this.homeTemperature = "--°";
                                this.homeLocationWeather = (cityName || "定位中") + "   更新中";
                                this.homeAdministrative = "正在获取当前位置";
                            },
                            showHomeWeatherFailure (cityName) {
                                this.homeWeatherIcon = "/common/weather-icons/999.png";
                                this.homeTemperature = "--°";
                                this.homeLocationWeather = (cityName || "当前位置") + "   获取失败";
                                this.homeAdministrative = "请检查定位与天气服务";
                            },
                            applyHomeWeather (city, weather) {
                                if (!city || !weather || !weather.now) return;
                                this.homeWeatherIcon = weather.now.iconSrc;
                                this.homeTemperature = weather.now.temp + "°";
                                const locationName = city.name || city.detailName || weather.location.name || "当前位置";
                                this.homeLocationWeather = locationName + "   " + weather.now.text;
                                const administrativeArea = String(city.administrativeArea || "").split("·").map((part)=>part.trim()).filter((part)=>!!part).join("，");
                                this.homeAdministrative = (administrativeArea ? administrativeArea + "，" : "") + (city.country || weather.location.country || "中国");
                            },
                            observeCurrentLocation () {
                                if (this.locationListener) return;
                                this.locationListener = (city, weather)=>{
                                    if (!city || !city.locationReady) return void this.showHomeWeatherLoading(city ? city.name : "定位中");
                                    if (weather) this.applyHomeWeather(city, weather);
                                    else this.refreshHomeWeather();
                                };
                                this.$app.$def.addCurrentLocationListener(this.locationListener);
                            },
                            stopObservingCurrentLocation () {
                                if (!this.locationListener) return;
                                this.$app.$def.removeCurrentLocationListener(this.locationListener);
                                this.locationListener = null;
                            },
                            refreshHomeWeather () {
                                const city = this.$app.$def.getCurrentLocation();
                                const weather = this.$app.$def.getCurrentLocationWeather();
                                this.observeCurrentLocation();
                                if (!city.locationReady || !weather) {
                                    this.showHomeWeatherLoading(city.name);
                                    this.$app.$def.refreshCurrentLocation(true);
                                    return;
                                }
                                this.applyHomeWeather(city, weather);
                            },
                            startWeatherMonitoring () {
                                this.refreshHomeWeather();
                                if (this.weatherTimerId) return;
                                this.weatherTimerId = setInterval(()=>this.refreshHomeWeather(), WEATHER_REFRESH_MS);
                            },
                            stopWeatherMonitoring () {
                                this.weatherRequestSerial += 1;
                                if (this.weatherTimerId) {
                                    clearInterval(this.weatherTimerId);
                                    this.weatherTimerId = null;
                                }
                                this.stopObservingCurrentLocation();
                            },
                            applyBatteryLevel (level) {
                                const numericLevel = Number(level);
                                if (!isFinite(numericLevel) || numericLevel < 0) {
                                    console.log("battery ignored unsupported level", numericLevel);
                                    return false;
                                }
                                const percent = numericLevel <= 1 ? Math.round(100 * numericLevel) : Math.round(numericLevel);
                                const nextPercent = Math.max(0, Math.min(100, percent));
                                this.batterySensorReady = true;
                                if (nextPercent === this.batteryPercent) return true;
                                this.batteryPercent = nextPercent;
                                const fillWidth = this.batteryPercent > 0 ? Math.max(1, Math.round(28 * this.batteryPercent / 100)) : 0;
                                this.batteryLevelStyle = {
                                    width: fillWidth + "px"
                                };
                                return true;
                            },
                            applyBatteryCharging (charging) {
                                let nextCharging = charging;
                                if ("string" == typeof nextCharging) {
                                    const normalized = nextCharging.toLowerCase();
                                    nextCharging = "true" === normalized || "charging" === normalized || "full" === normalized;
                                }
                                nextCharging = !!nextCharging;
                                if (nextCharging === this.batteryCharging) return true;
                                this.batteryCharging = nextCharging;
                                return true;
                            },
                            batteryChargingFromStatus (data) {
                                if (!data) return null;
                                if (void 0 !== data.charging) return !!data.charging;
                                if (void 0 !== data.isCharging) return !!data.isCharging;
                                if (void 0 !== data.status) {
                                    const normalized = String(data.status).toLowerCase();
                                    if ("charging" === normalized || "full" === normalized) return true;
                                    if ("discharging" === normalized || "not charging" === normalized || "not_charging" === normalized) return false;
                                }
                                return null;
                            },
                            handleSystemBatteryStatus (data) {
                                if (data && void 0 !== data.level) this.applyBatteryLevel(data.level);
                                const charging = this.batteryChargingFromStatus(data);
                                if (null !== charging) this.applyBatteryCharging(charging);
                                this.startBatteryBridgeMonitoring();
                            },
                            refreshBattery () {
                                if (this.batteryReading) return;
                                this.batteryReading = true;
                                try {
                                    _system2.default.getStatus({
                                        success: (data)=>{
                                            this.handleSystemBatteryStatus(data);
                                        },
                                        fail: (data, code)=>{
                                            console.log("battery getStatus failed", code, data);
                                            this.startBatteryBridgeMonitoring();
                                        },
                                        complete: ()=>{
                                            this.batteryReading = false;
                                        }
                                    });
                                } catch (error) {
                                    this.batteryReading = false;
                                    console.log("battery getStatus unavailable", error);
                                    this.startBatteryBridgeMonitoring();
                                }
                            },
                            refreshEmulatorBattery () {
                                if (this.batteryBridgeReading) return;
                                this.batteryBridgeReading = true;
                                try {
                                    _system4.default.readText({
                                        uri: BATTERY_BRIDGE_URI,
                                        success: (data)=>{
                                            try {
                                                const payload = JSON.parse(data && data.text ? data.text : "{}");
                                                if (payload && "vela-emulator" === payload.source) {
                                                    this.applyBatteryLevel(payload.level);
                                                    this.applyBatteryCharging(payload.charging);
                                                }
                                            } catch (error) {
                                                console.log("battery bridge parse failed", error);
                                            }
                                        },
                                        fail: (data, code)=>{
                                            console.log("battery bridge read failed", code, data);
                                        },
                                        complete: ()=>{
                                            this.batteryBridgeReading = false;
                                        }
                                    });
                                } catch (error) {
                                    this.batteryBridgeReading = false;
                                    console.log("battery bridge unavailable", error);
                                }
                            },
                            startBatteryBridgeMonitoring () {
                                this.refreshEmulatorBattery();
                                if (this.batteryBridgeTimerId) return;
                                this.batteryBridgeTimerId = setInterval(()=>this.refreshEmulatorBattery(), BATTERY_BRIDGE_POLL_MS);
                            },
                            stopBatteryBridgeMonitoring () {
                                if (this.batteryBridgeTimerId) {
                                    clearInterval(this.batteryBridgeTimerId);
                                    this.batteryBridgeTimerId = null;
                                }
                                this.batteryBridgeReading = false;
                            },
                            subscribeBatteryEvent () {
                                if (null !== this.batteryEventId) return;
                                try {
                                    const id = _system3.default.subscribe({
                                        eventName: "usual.event.BATTERY_CHANGED",
                                        callback: (result)=>{
                                            const params = result && result.params ? result.params : result;
                                            if (params) this.handleSystemBatteryStatus(params);
                                            else this.refreshBattery();
                                        }
                                    });
                                    if (void 0 !== id) this.batteryEventId = id;
                                } catch (error) {
                                    console.log("battery event unavailable", error);
                                }
                            },
                            startBatteryMonitoring () {
                                this.refreshBattery();
                                this.subscribeBatteryEvent();
                                if (this.batteryPollTimerId) return;
                                this.batteryPollTimerId = setInterval(()=>this.refreshBattery(), BATTERY_FALLBACK_POLL_MS);
                            },
                            stopBatteryMonitoring (keepEmulatorBridge) {
                                if (this.batteryPollTimerId) {
                                    clearInterval(this.batteryPollTimerId);
                                    this.batteryPollTimerId = null;
                                }
                                if (!keepEmulatorBridge) this.stopBatteryBridgeMonitoring();
                                if (null !== this.batteryEventId) {
                                    try {
                                        _system3.default.unsubscribe({
                                            id: this.batteryEventId
                                        });
                                    } catch (error) {
                                        console.log("battery event unsubscribe failed", error);
                                    }
                                    this.batteryEventId = null;
                                }
                                this.batteryReading = false;
                            },
                            startCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("catAnimator");
                                if (!animator) return;
                                try {
                                    animator.start();
                                } catch (error) {
                                    console.log("cat animator start failed", error);
                                }
                            },
                            queueCatAnimationStart () {
                                if (!this.catAnimatorReady) return;
                                this.cancelCatAnimationStart();
                                this.catStartTimerId = setTimeout(()=>{
                                    this.catStartTimerId = null;
                                    this.startCatAnimation();
                                }, 500);
                            },
                            cancelCatAnimationStart () {
                                if (!this.catStartTimerId) return;
                                clearTimeout(this.catStartTimerId);
                                this.catStartTimerId = null;
                            },
                            pauseCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("catAnimator");
                                if (!animator) return;
                                try {
                                    animator.pause();
                                } catch (error) {
                                    console.log("cat animator pause failed", error);
                                }
                            },
                            stopCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("catAnimator");
                                if (!animator) return;
                                try {
                                    animator.stop();
                                } catch (error) {
                                    console.log("cat animator stop failed", error);
                                }
                            },
                            startCatHealthCheck () {
                                if (this.catHealthTimerId) return;
                                this.catHealthTimerId = setInterval(()=>{
                                    const animator = this.$element("catAnimator");
                                    if (!this.catAnimatorReady || !animator) return;
                                    try {
                                        const state = animator.getState();
                                        if ("paused" === state) animator.resume();
                                        if ("stopped" === state) animator.start();
                                    } catch (error) {
                                        console.log("cat animator health check failed", error);
                                    }
                                }, 10000);
                            },
                            stopCatHealthCheck () {
                                if (!this.catHealthTimerId) return;
                                clearInterval(this.catHealthTimerId);
                                this.catHealthTimerId = null;
                            },
                            syncCustomization () {
                                const customization = this.$app.$def.getCustomization();
                                const background = (0, _customization.getBackground)(customization.backgroundId);
                                const action = (0, _customization.getAction)(customization.actionId);
                                this.backgroundImage = background.src;
                                this.clockClass = "clock text-" + background.foreground;
                                this.dateClass = "date text-" + background.foreground;
                                this.weekdayClass = "weekday text-" + background.foreground;
                                this.batteryIconClass = "battery-icon border-" + background.foreground;
                                this.batteryLevelClass = "battery-level fill-" + background.foreground;
                                this.batteryCapClass = "battery-cap fill-" + background.foreground;
                                this.batteryTextClass = "battery-text text-" + background.foreground;
                                this.batteryChargingIcon = "/common/icons/charging-" + background.foreground + ".png";
                                this.homeTemperatureClass = "home-temperature text-" + background.foreground;
                                this.homeLocationWeatherClass = "home-location-weather text-" + background.foreground;
                                this.homeAdministrativeClass = "home-administrative text-" + background.foreground;
                                if (this.activeActionId !== action.id) {
                                    const animatorFrames = [];
                                    for(let index = 0; index < action.frames.length; index += 1)animatorFrames.push({
                                        src: APP_RESOURCE_ROOT + action.frames[index]
                                    });
                                    this.activeActionId = action.id;
                                    this.catFrames = animatorFrames;
                                    const frameDuration = Math.max(50, Math.round(action.duration / action.frames.length));
                                    this.catDuration = frameDuration + "ms";
                                }
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
                            onTouchStart (event) {
                                console.log("INDEX_TOUCH_START");
                                if (this.screenStandby) {
                                    this.ignoreTouchEnd = true;
                                    this.wakeScreen();
                                    return;
                                }
                                if (this.ignoreTouchEnd) return void this.registerActivity();
                                this.ignoreTouchEnd = false;
                                this.registerActivity();
                                const point = this.touchPoint(event, false);
                                this.touchX = point.x;
                                this.touchY = point.y;
                            },
                            onTouchEnd (event) {
                                console.log("INDEX_TOUCH_END");
                                if (this.ignoreTouchEnd) {
                                    this.ignoreTouchEnd = false;
                                    return;
                                }
                                this.registerActivity();
                                const point = this.touchPoint(event, true);
                                const deltaX = point.x - this.touchX;
                                const deltaY = point.y - this.touchY;
                                if (this.specialSportMode) {
                                    if (deltaX < -34 && Math.abs(deltaX) > 1.04 * Math.abs(deltaY)) this.openSpecialSportStatistics();
                                    if (deltaX > 34 && Math.abs(deltaX) > 1.04 * Math.abs(deltaY)) this.openSpecialSportExit();
                                    return;
                                }
                                if (deltaY > 60 && Math.abs(deltaY) > 1.2 * Math.abs(deltaX)) this.openSelector();
                                if (deltaX < -60 && Math.abs(deltaX) > 1.2 * Math.abs(deltaY)) this.openWeather();
                                if (deltaX > 60 && Math.abs(deltaX) > 1.2 * Math.abs(deltaY)) this.openAssistant();
                            },
                            handleSwipe (event) {
                                console.log("INDEX_SWIPE " + JSON.stringify(event || {}));
                                if (this.screenStandby) return void this.wakeScreen();
                                this.registerActivity();
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if (this.specialSportMode) {
                                    if ("left" === direction) this.openSpecialSportStatistics();
                                    if ("right" === direction) this.openSpecialSportExit();
                                    return;
                                }
                                if ("down" === direction) this.openSelector();
                                if ("left" === direction) this.openWeather();
                                if ("right" === direction) this.openAssistant();
                            },
                            openSelector () {
                                if (this.selectorOpening || this.screenStandby) return;
                                this.selectorOpening = true;
                                this.cancelCatAnimationStart();
                                this.stopCatHealthCheck();
                                this.stopCatAnimation();
                                setTimeout(()=>{
                                    try {
                                        _system.default.push({
                                            uri: "/pages/customize"
                                        });
                                    } catch (error) {
                                        this.selectorOpening = false;
                                        this.queueCatAnimationStart();
                                        this.startCatHealthCheck();
                                        console.log("open selector failed", error);
                                    }
                                }, 120);
                            },
                            openSpecialSportStatistics () {
                                if (this.selectorOpening || this.screenStandby) return;
                                this.selectorOpening = true;
                                this.$app.$def.setSpecialSportMetric("steps");
                                this.cancelCatAnimationStart();
                                this.stopCatHealthCheck();
                                this.stopCatAnimation();
                                setTimeout(()=>{
                                    try {
                                        _system.default.replace({
                                            uri: "/pages/steps"
                                        });
                                    } catch (error) {
                                        this.selectorOpening = false;
                                        this.queueCatAnimationStart();
                                        this.startCatHealthCheck();
                                        console.log("open special sport statistics failed", error);
                                    }
                                }, 120);
                            },
                            openSpecialSportExit () {
                                if (this.selectorOpening || this.screenStandby) return;
                                this.selectorOpening = true;
                                this.cancelCatAnimationStart();
                                this.stopCatHealthCheck();
                                this.stopCatAnimation();
                                setTimeout(()=>{
                                    try {
                                        _system.default.replace({
                                            uri: "/pages/sportexit"
                                        });
                                    } catch (error) {
                                        this.selectorOpening = false;
                                        this.queueCatAnimationStart();
                                        this.startCatHealthCheck();
                                        console.log("open special sport exit failed", error);
                                    }
                                }, 120);
                            },
                            openWeather () {
                                if (this.selectorOpening || this.screenStandby) return;
                                this.selectorOpening = true;
                                this.cancelCatAnimationStart();
                                this.stopCatHealthCheck();
                                this.stopCatAnimation();
                                setTimeout(()=>{
                                    try {
                                        _system.default.replace({
                                            uri: "/pages/weather"
                                        });
                                    } catch (error) {
                                        this.selectorOpening = false;
                                        this.queueCatAnimationStart();
                                        this.startCatHealthCheck();
                                        console.log("open weather failed", error);
                                    }
                                }, 120);
                            },
                            openAssistant () {
                                if (this.selectorOpening || this.screenStandby) return;
                                this.selectorOpening = true;
                                this.cancelCatAnimationStart();
                                this.stopCatHealthCheck();
                                this.stopCatAnimation();
                                setTimeout(()=>{
                                    try {
                                        _system.default.replace({
                                            uri: "/pages/assistant"
                                        });
                                    } catch (error) {
                                        this.selectorOpening = false;
                                        this.queueCatAnimationStart();
                                        this.startCatHealthCheck();
                                        console.log("open assistant failed", error);
                                    }
                                }, 120);
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
                                    aiot.__ce__("image-animator", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "cat-animation"
                                            ],
                                            id: "catAnimator",
                                            images: function() {
                                                return _vm_.catFrames;
                                            },
                                            duration: function() {
                                                return _vm_.catDuration;
                                            },
                                            iteration: "infinite",
                                            fixedsize: "true"
                                        }
                                    }, []),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: function() {
                                                const $classValue$ = _vm_.clockClass;
                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                return $classValue$;
                                            },
                                            value: function() {
                                                return _vm_.timeText;
                                            }
                                        }
                                    }, []),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: function() {
                                                const $classValue$ = _vm_.dateClass;
                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                return $classValue$;
                                            },
                                            value: function() {
                                                return _vm_.dateText;
                                            }
                                        }
                                    }, []),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: function() {
                                                const $classValue$ = _vm_.weekdayClass;
                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                return $classValue$;
                                            },
                                            value: function() {
                                                return _vm_.weekdayText;
                                            }
                                        }
                                    }, []),
                                    aiot.__ce__("image", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "home-weather-icon"
                                            ],
                                            src: function() {
                                                return _vm_.homeWeatherIcon;
                                            }
                                        }
                                    }, []),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: function() {
                                                const $classValue$ = _vm_.homeTemperatureClass;
                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                return $classValue$;
                                            },
                                            value: function() {
                                                return _vm_.homeTemperature;
                                            }
                                        }
                                    }, []),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: function() {
                                                const $classValue$ = _vm_.homeLocationWeatherClass;
                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                return $classValue$;
                                            },
                                            value: function() {
                                                return _vm_.homeLocationWeather;
                                            }
                                        }
                                    }, []),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: function() {
                                                const $classValue$ = _vm_.homeAdministrativeClass;
                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                return $classValue$;
                                            },
                                            value: function() {
                                                return _vm_.homeAdministrative;
                                            }
                                        }
                                    }, []),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "battery-row"
                                            ]
                                        }
                                    }, [
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "battery-charging-slot"
                                                ]
                                            }
                                        }, [
                                            aiot.__ci__({
                                                __vm__: _vm_,
                                                __opts__: {
                                                    shown: function() {
                                                        return _vm_.batteryCharging;
                                                    }
                                                }
                                            }, function() {
                                                return [
                                                    aiot.__ce__("image", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "battery-charging"
                                                            ],
                                                            src: function() {
                                                                return _vm_.batteryChargingIcon;
                                                            }
                                                        }
                                                    }, [])
                                                ];
                                            })
                                        ]),
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: function() {
                                                    const $classValue$ = _vm_.batteryIconClass;
                                                    if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                    return $classValue$;
                                                }
                                            }
                                        }, [
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: function() {
                                                        const $classValue$ = _vm_.batteryLevelClass;
                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                        return $classValue$;
                                                    },
                                                    style: function() {
                                                        return __webpack_require__.g.$translateStyle$(_vm_.batteryLevelStyle);
                                                    }
                                                }
                                            }, [])
                                        ]),
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: function() {
                                                    const $classValue$ = _vm_.batteryCapClass;
                                                    if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                    return $classValue$;
                                                }
                                            }
                                        }, []),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: function() {
                                                    const $classValue$ = _vm_.batteryTextClass;
                                                    if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                    return $classValue$;
                                                },
                                                value: function() {
                                                    return _vm_.batteryPercent + "%";
                                                }
                                            }
                                        }, [])
                                    ]),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "gesture-layer"
                                            ],
                                            events: {
                                                swipe: function(evt) {
                                                    return _vm_.handleSwipe(evt);
                                                },
                                                touchstart: function(evt) {
                                                    return _vm_.onTouchStart(evt);
                                                },
                                                touchend: function(evt) {
                                                    return _vm_.onTouchEnd(evt);
                                                }
                                            }
                                        }
                                    }, []),
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
