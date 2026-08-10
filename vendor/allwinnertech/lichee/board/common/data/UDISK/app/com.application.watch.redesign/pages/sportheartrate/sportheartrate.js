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
                                backgroundColor: "#02030a",
                                opacity: 0.76
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
                                width: "100%",
                                height: "58px",
                                position: "absolute",
                                left: 0,
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
                                left: "114px",
                                top: "86px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "monitor-status"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "32px",
                                position: "absolute",
                                left: 0,
                                top: "286px",
                                color: "#ffb9bd",
                                fontSize: "22px",
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
                                width: "330px",
                                height: "102px",
                                position: "absolute",
                                left: "51px",
                                top: "320px",
                                justifyContent: "center",
                                alignItems: "flex-end"
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
                                height: "100px",
                                color: "#ff3038",
                                fontSize: "92px",
                                fontWeight: "bold"
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
                                width: "72px",
                                height: "55px",
                                marginLeft: "10px",
                                color: "#f4e9ea",
                                fontSize: "29px",
                                fontWeight: "bold"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "threshold-hint"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "34px",
                                position: "absolute",
                                left: 0,
                                top: "430px",
                                color: "#e1d6db",
                                fontSize: "21px",
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
                                zIndex: 50,
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
                        var _customization = __webpack_require__("./src/common/customization.js");
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const APP_RESOURCE_ROOT = "/data/app/com.application.watch.redesign";
                        const STANDBY_DELAY_MS = 60000;
                        function createHeartFrames() {
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
                                heartFrames: createHeartFrames(),
                                heartRateText: "78",
                                thresholdText: "160",
                                heartRateListener: null,
                                animatorReady: false,
                                animatorStartTimerId: null,
                                animatorHealthTimerId: null,
                                standbyTimerId: null,
                                screenStandby: false,
                                ignoreTouchEnd: false,
                                touchX: 0,
                                touchY: 0,
                                leaving: false
                            },
                            onInit () {
                                this.syncCustomization();
                                const snapshot = this.$app.$def.getSpecialSportHeartRateSnapshot();
                                this.applyHeartRateSnapshot(snapshot);
                            },
                            onReady () {
                                this.animatorReady = true;
                                this.queueAnimatorStart();
                            },
                            onShow () {
                                if (!this.$app.$def.isSpecialSportMode()) return void this.returnToNormalHome();
                                this.$app.$def.ensureWakeableScreen();
                                this.leaving = false;
                                this.screenStandby = false;
                                this.ignoreTouchEnd = false;
                                this.syncCustomization();
                                this.heartRateListener = (snapshot)=>this.applyHeartRateSnapshot(snapshot);
                                this.$app.$def.subscribeSpecialSportHeartRate(this.heartRateListener);
                                if (this.animatorReady) this.queueAnimatorStart();
                                this.startAnimatorHealthCheck();
                                this.startStandbyTimer();
                            },
                            onHide () {
                                this.unsubscribeHeartRate();
                                this.stopStandbyTimer();
                                this.cancelAnimatorStart();
                                this.stopAnimator();
                                this.stopAnimatorHealthCheck();
                            },
                            onDestroy () {
                                this.unsubscribeHeartRate();
                                this.stopStandbyTimer();
                                this.cancelAnimatorStart();
                                this.stopAnimator();
                                this.stopAnimatorHealthCheck();
                                this.animatorReady = false;
                            },
                            syncCustomization () {
                                const customization = this.$app.$def.getCustomization();
                                this.backgroundImage = (0, _customization.getBackground)(customization.backgroundId).src;
                            },
                            applyHeartRateSnapshot (snapshot) {
                                if (!snapshot) return;
                                this.heartRateText = String(Math.max(30, Math.round(Number(snapshot.bpm) || 78)));
                                this.thresholdText = String(Math.max(30, Math.round(Number(snapshot.threshold) || 160)));
                            },
                            unsubscribeHeartRate () {
                                if (!this.heartRateListener) return;
                                this.$app.$def.unsubscribeSpecialSportHeartRate(this.heartRateListener);
                                this.heartRateListener = null;
                            },
                            queueAnimatorStart () {
                                if (!this.animatorReady || this.screenStandby) return;
                                this.cancelAnimatorStart();
                                this.animatorStartTimerId = setTimeout(()=>{
                                    this.animatorStartTimerId = null;
                                    const animator = this.$element("sportHeartAnimator");
                                    if (!animator || this.screenStandby) return;
                                    try {
                                        animator.start();
                                    } catch (error) {
                                        console.log("special sport heart animator start failed", error);
                                    }
                                }, 120);
                            },
                            cancelAnimatorStart () {
                                if (!this.animatorStartTimerId) return;
                                clearTimeout(this.animatorStartTimerId);
                                this.animatorStartTimerId = null;
                            },
                            stopAnimator () {
                                if (!this.animatorReady) return;
                                const animator = this.$element("sportHeartAnimator");
                                if (!animator) return;
                                try {
                                    animator.stop();
                                } catch (error) {
                                    console.log("special sport heart animator stop failed", error);
                                }
                            },
                            startAnimatorHealthCheck () {
                                if (this.animatorHealthTimerId || this.screenStandby) return;
                                this.animatorHealthTimerId = setInterval(()=>{
                                    const animator = this.$element("sportHeartAnimator");
                                    if (!animator || this.screenStandby) return;
                                    try {
                                        const state = animator.getState();
                                        if ("paused" === state) animator.resume();
                                        if ("stopped" === state) animator.start();
                                    } catch (error) {
                                        console.log("special sport heart animator health check failed", error);
                                    }
                                }, 10000);
                            },
                            stopAnimatorHealthCheck () {
                                if (!this.animatorHealthTimerId) return;
                                clearInterval(this.animatorHealthTimerId);
                                this.animatorHealthTimerId = null;
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
                                this.cancelAnimatorStart();
                                this.stopAnimator();
                                this.stopAnimatorHealthCheck();
                            },
                            wakeScreen () {
                                if (!this.screenStandby) return void this.registerActivity();
                                this.screenStandby = false;
                                this.ignoreTouchEnd = true;
                                this.leaving = false;
                                this.$app.$def.resumeRuntime();
                                this.queueAnimatorStart();
                                this.startAnimatorHealthCheck();
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
                            onTouchStart (event) {
                                if (this.screenStandby) {
                                    this.ignoreTouchEnd = true;
                                    this.wakeScreen();
                                    return;
                                }
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
                                this.registerActivity();
                                const point = this.touchPoint(event, true);
                                const deltaX = point.x - this.touchX;
                                const deltaY = point.y - this.touchY;
                                const horizontal = Math.abs(deltaX) > 34 && Math.abs(deltaX) > 1.04 * Math.abs(deltaY);
                                if (!horizontal) return;
                                if (deltaX < 0) this.openExitPage();
                                if (deltaX > 0) this.openDurationStatistics();
                            },
                            handleSwipe (event) {
                                if (this.screenStandby) return void this.wakeScreen();
                                this.registerActivity();
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if ("left" === direction) this.openExitPage();
                                if ("right" === direction) this.openDurationStatistics();
                            },
                            openExitPage () {
                                if (this.leaving) return;
                                this.leaving = true;
                                try {
                                    _system.default.replace({
                                        uri: "/pages/sportexit"
                                    });
                                } catch (error) {
                                    this.leaving = false;
                                    console.log("open special sport exit failed", error);
                                }
                            },
                            openDurationStatistics () {
                                if (this.leaving) return;
                                this.leaving = true;
                                this.$app.$def.setSpecialSportMetric("duration");
                                try {
                                    _system.default.replace({
                                        uri: "/pages/steps"
                                    });
                                } catch (error) {
                                    this.leaving = false;
                                    console.log("return special duration statistics failed", error);
                                }
                            },
                            returnToNormalHome () {
                                if (this.leaving) return;
                                this.leaving = true;
                                try {
                                    _system.default.replace({
                                        uri: "/pages/index"
                                    });
                                } catch (error) {
                                    this.leaving = false;
                                    console.log("return normal home failed", error);
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
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "page-title"
                                            ],
                                            value: "持续心率"
                                        }
                                    }, []),
                                    aiot.__ce__("image-animator", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "heart-visual"
                                            ],
                                            id: "sportHeartAnimator",
                                            images: function() {
                                                return _vm_.heartFrames;
                                            },
                                            duration: "85ms",
                                            iteration: "infinite",
                                            fixedsize: "true"
                                        }
                                    }, []),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "monitor-status"
                                            ],
                                            value: "运动模式 · 长时间监测中"
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
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "threshold-hint"
                                            ],
                                            value: function() {
                                                return "超过 " + _vm_.thresholdText + " BPM 将自动提醒";
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
