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
                                position: "relative",
                                backgroundColor: "#f7f4ee"
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
                                    "veil"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                backgroundColor: "rgba(250, 248, 244, 0.95)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "title"
                                ]
                            ],
                            {
                                width: "230px",
                                height: "50px",
                                position: "absolute",
                                left: "30px",
                                top: "28px",
                                color: "#202020",
                                fontSize: "33px",
                                fontWeight: "bold"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "time"
                                ]
                            ],
                            {
                                width: "70px",
                                height: "36px",
                                position: "absolute",
                                right: "68px",
                                top: "34px",
                                color: "#262626",
                                fontSize: "21px",
                                fontWeight: "bold",
                                textAlign: "right"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "item"
                                ]
                            ],
                            {
                                width: "372px",
                                height: "106px",
                                position: "absolute",
                                left: "30px",
                                borderTopWidth: "1px",
                                borderRightWidth: "1px",
                                borderBottomWidth: "1px",
                                borderLeftWidth: "1px",
                                borderTopColor: "rgba(178,174,168,0.72)",
                                borderRightColor: "rgba(178,174,168,0.72)",
                                borderBottomColor: "rgba(178,174,168,0.72)",
                                borderLeftColor: "rgba(178,174,168,0.72)",
                                borderRadius: "26px",
                                backgroundColor: "rgba(255, 255, 255, 0.92)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "item-background"
                                ]
                            ],
                            {
                                top: "130px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "item-action"
                                ]
                            ],
                            {
                                top: "258px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "background-icon"
                                ]
                            ],
                            {
                                width: "64px",
                                height: "64px",
                                position: "absolute",
                                left: "24px",
                                top: "21px",
                                borderRadius: "50%",
                                objectFit: "cover"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "action-icon"
                                ]
                            ],
                            {
                                width: "64px",
                                height: "64px",
                                position: "absolute",
                                left: "24px",
                                top: "21px",
                                borderRadius: "50%",
                                objectFit: "contain",
                                backgroundColor: "#f1ece5"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "item-name"
                                ]
                            ],
                            {
                                width: "210px",
                                height: "48px",
                                position: "absolute",
                                left: "112px",
                                top: "31px",
                                color: "#242424",
                                fontSize: "29px",
                                fontWeight: "bold"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forward-mark"
                                ]
                            ],
                            {
                                width: "34px",
                                height: "58px",
                                position: "absolute",
                                right: "18px",
                                top: "20px",
                                color: "#9c9a96",
                                fontSize: "50px",
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
                        var _customization = __webpack_require__("./src/common/customization.js");
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const STANDBY_DELAY_MS = 60000;
                        var _default = exports.default = {
                            private: {
                                backgroundImage: "/common/backgrounds/dark-purple.png",
                                actionPreview: "/common/cat/frame-18.png",
                                timeText: "12:44",
                                clockTimerId: null,
                                navigationTimerId: null,
                                touchX: 0,
                                touchY: 0,
                                standbyTimerId: null,
                                screenStandby: false,
                                ignoreTouchEnd: false,
                                leaving: false
                            },
                            onInit () {
                                this.syncCustomization();
                                this.updateClock();
                            },
                            onShow () {
                                this.$app.$def.ensureWakeableScreen();
                                this.leaving = false;
                                this.screenStandby = false;
                                this.ignoreTouchEnd = false;
                                this.syncCustomization();
                                this.startClock();
                                this.startStandbyTimer();
                            },
                            onHide () {
                                this.stopStandbyTimer();
                                this.stopClock();
                                this.cancelNavigation();
                            },
                            onDestroy () {
                                this.stopStandbyTimer();
                                this.stopClock();
                                this.cancelNavigation();
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
                                this.screenStandby = true;
                                this.stopClock();
                            },
                            wakeScreen () {
                                if (!this.screenStandby) return void this.registerActivity();
                                this.screenStandby = false;
                                this.ignoreTouchEnd = true;
                                this.leaving = false;
                                this.syncCustomization();
                                this.updateClock();
                                this.startClock();
                                this.startStandbyTimer();
                            },
                            pad (value) {
                                return value < 10 ? "0" + value : "" + value;
                            },
                            updateClock () {
                                const now = new Date();
                                const nextTime = this.pad(now.getHours()) + ":" + this.pad(now.getMinutes());
                                if (this.timeText !== nextTime) this.timeText = nextTime;
                            },
                            startClock () {
                                if (this.clockTimerId) return;
                                this.updateClock();
                                this.scheduleNextClockTick();
                            },
                            scheduleNextClockTick () {
                                const now = new Date();
                                const delay = Math.max(1000, (60 - now.getSeconds()) * 1000);
                                this.clockTimerId = setTimeout(()=>{
                                    this.clockTimerId = null;
                                    this.updateClock();
                                    this.scheduleNextClockTick();
                                }, delay);
                            },
                            stopClock () {
                                if (!this.clockTimerId) return;
                                clearTimeout(this.clockTimerId);
                                this.clockTimerId = null;
                            },
                            syncCustomization () {
                                const customization = this.$app.$def.getCustomization();
                                this.backgroundImage = (0, _customization.getBackground)(customization.backgroundId).src;
                                this.actionPreview = (0, _customization.getAction)(customization.actionId).preview;
                            },
                            queueNavigation (uri) {
                                this.cancelNavigation();
                                this.navigationTimerId = setTimeout(()=>{
                                    this.navigationTimerId = null;
                                    try {
                                        _system.default.push({
                                            uri: uri
                                        });
                                    } catch (error) {
                                        this.leaving = false;
                                        console.log("open customization page failed", error);
                                    }
                                }, 120);
                            },
                            queueBack () {
                                this.cancelNavigation();
                                this.navigationTimerId = setTimeout(()=>{
                                    this.navigationTimerId = null;
                                    try {
                                        _system.default.back();
                                    } catch (error) {
                                        this.leaving = false;
                                        console.log("close customization page failed", error);
                                    }
                                }, 120);
                            },
                            cancelNavigation () {
                                if (!this.navigationTimerId) return;
                                clearTimeout(this.navigationTimerId);
                                this.navigationTimerId = null;
                            },
                            openBackgrounds () {
                                if (this.leaving) return;
                                this.registerActivity();
                                this.leaving = true;
                                this.queueNavigation("/pages/backgrounds");
                            },
                            openActions () {
                                if (this.leaving) return;
                                this.registerActivity();
                                this.leaving = true;
                                this.queueNavigation("/pages/actions");
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
                                if (this.ignoreTouchEnd) return void this.registerActivity();
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
                                if (deltaY < -60 && Math.abs(deltaY) > 1.15 * Math.abs(deltaX)) this.returnToMain();
                            },
                            handleSwipe (event) {
                                if (this.screenStandby) return void this.wakeScreen();
                                this.registerActivity();
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if ("up" === direction) this.returnToMain();
                            },
                            returnToMain () {
                                if (this.leaving) return;
                                this.leaving = true;
                                this.queueBack();
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
                                                "veil"
                                            ]
                                        }
                                    }, []),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "title"
                                            ],
                                            value: "外观选择"
                                        }
                                    }, []),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "time"
                                            ],
                                            value: function() {
                                                return _vm_.timeText;
                                            }
                                        }
                                    }, []),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "item",
                                                "item-background"
                                            ],
                                            events: {
                                                click: function(evt) {
                                                    return _vm_.openBackgrounds(evt);
                                                }
                                            }
                                        }
                                    }, [
                                        aiot.__ce__("image", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "background-icon"
                                                ],
                                                src: function() {
                                                    return _vm_.backgroundImage;
                                                }
                                            }
                                        }, []),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "item-name"
                                                ],
                                                value: "背景选择"
                                            }
                                        }, []),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "forward-mark"
                                                ],
                                                value: "›"
                                            }
                                        }, [])
                                    ]),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "item",
                                                "item-action"
                                            ],
                                            events: {
                                                click: function(evt) {
                                                    return _vm_.openActions(evt);
                                                }
                                            }
                                        }
                                    }, [
                                        aiot.__ce__("image", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "action-icon"
                                                ],
                                                src: function() {
                                                    return _vm_.actionPreview;
                                                }
                                            }
                                        }, []),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "item-name"
                                                ],
                                                value: "动作选择"
                                            }
                                        }, []),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "forward-mark"
                                                ],
                                                value: "›"
                                            }
                                        }, [])
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
