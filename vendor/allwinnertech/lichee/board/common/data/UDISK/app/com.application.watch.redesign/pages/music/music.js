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
                                overflow: "hidden",
                                backgroundColor: "#08153b"
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
                                    "music-title"
                                ]
                            ],
                            {
                                width: "220px",
                                height: "72px",
                                position: "absolute",
                                left: "106px",
                                top: "34px",
                                color: "#142a65",
                                fontSize: "52px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "music-card"
                                ]
                            ],
                            {
                                width: "132px",
                                height: "132px",
                                position: "absolute",
                                left: "150px",
                                top: "105px",
                                borderTopWidth: "4px",
                                borderRightWidth: "4px",
                                borderBottomWidth: "4px",
                                borderLeftWidth: "4px",
                                borderTopColor: "#2b9de0",
                                borderRightColor: "#2b9de0",
                                borderBottomColor: "#2b9de0",
                                borderLeftColor: "#2b9de0",
                                borderRadius: "66px",
                                backgroundColor: "rgba(255, 255, 255, 0.94)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "music-icon"
                                ]
                            ],
                            {
                                width: "100px",
                                height: "100px",
                                position: "absolute",
                                left: "12px",
                                top: "12px",
                                objectFit: "contain"
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
                                width: "174px",
                                height: "178px",
                                position: "absolute",
                                left: "129px",
                                top: "270px"
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
                        var _default = exports.default = {
                            private: {
                                backgroundImage: "/common/backgrounds/dark-purple.png",
                                titleClass: "music-title text-light",
                                catFrames: [],
                                catDuration: "3400ms",
                                activeActionId: "",
                                catAnimatorReady: false,
                                catStartTimerId: null,
                                catHealthTimerId: null,
                                standbyTimerId: null,
                                screenStandby: false,
                                ignoreTouchEnd: false,
                                touchX: 0,
                                touchY: 0,
                                routeGuardTimerId: null,
                                leaving: false
                            },
                            onInit () {
                                this.syncCustomization();
                            },
                            onReady () {
                                this.catAnimatorReady = true;
                                this.queueCatAnimationStart();
                            },
                            onShow () {
                                this.$app.$def.ensureWakeableScreen();
                                this.clearRouteGuard();
                                this.leaving = false;
                                this.screenStandby = false;
                                this.ignoreTouchEnd = false;
                                this.syncCustomization();
                                if (this.catAnimatorReady) this.queueCatAnimationStart();
                                this.startCatHealthCheck();
                                this.startStandbyTimer();
                            },
                            onHide () {
                                this.stopStandbyTimer();
                                this.clearRouteGuard();
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.stopCatHealthCheck();
                            },
                            onDestroy () {
                                this.stopStandbyTimer();
                                this.clearRouteGuard();
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.stopCatHealthCheck();
                                this.catAnimatorReady = false;
                            },
                            syncCustomization () {
                                const customization = this.$app.$def.getCustomization();
                                const background = (0, _customization.getBackground)(customization.backgroundId);
                                const action = (0, _customization.getAction)(customization.actionId);
                                this.backgroundImage = background.src;
                                this.titleClass = "music-title text-" + background.foreground;
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
                            startCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("musicCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.start();
                                } catch (error) {
                                    console.log("music cat animator start failed", error);
                                }
                            },
                            queueCatAnimationStart () {
                                if (!this.catAnimatorReady || this.screenStandby) return;
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
                            stopCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("musicCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.stop();
                                } catch (error) {
                                    console.log("music cat animator stop failed", error);
                                }
                            },
                            startCatHealthCheck () {
                                if (this.catHealthTimerId || this.screenStandby) return;
                                this.catHealthTimerId = setInterval(()=>{
                                    const animator = this.$element("musicCatAnimator");
                                    if (!this.catAnimatorReady || !animator) return;
                                    try {
                                        const state = animator.getState();
                                        if ("paused" === state) animator.resume();
                                        if ("stopped" === state) animator.start();
                                    } catch (error) {
                                        console.log("music cat animator health check failed", error);
                                    }
                                }, 10000);
                            },
                            stopCatHealthCheck () {
                                if (!this.catHealthTimerId) return;
                                clearInterval(this.catHealthTimerId);
                                this.catHealthTimerId = null;
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
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.stopCatHealthCheck();
                            },
                            wakeScreen () {
                                if (!this.screenStandby) return void this.registerActivity();
                                this.screenStandby = false;
                                this.ignoreTouchEnd = true;
                                this.leaving = false;
                                this.syncCustomization();
                                this.queueCatAnimationStart();
                                this.startCatHealthCheck();
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
                                if (deltaX > 60 && Math.abs(deltaX) > 1.2 * Math.abs(deltaY)) this.openHealth();
                                if (deltaX < -60 && Math.abs(deltaX) > 1.2 * Math.abs(deltaY)) this.openNotificationHub();
                                if (Math.abs(deltaY) > 60 && Math.abs(deltaY) > 1.2 * Math.abs(deltaX)) this.openPlayer();
                            },
                            handleSwipe (event) {
                                if (this.screenStandby) return void this.wakeScreen();
                                this.registerActivity();
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if ("right" === direction) this.openHealth();
                                if ("left" === direction) this.openNotificationHub();
                                if ("up" === direction || "down" === direction) this.openPlayer();
                            },
                            replacePage (uri) {
                                if (this.leaving || this.screenStandby) return;
                                this.leaving = true;
                                this.armRouteGuard();
                                this.cancelCatAnimationStart();
                                this.stopCatHealthCheck();
                                this.stopCatAnimation();
                                setTimeout(()=>{
                                    try {
                                        _system.default.replace({
                                            uri: uri
                                        });
                                    } catch (error) {
                                        this.clearRouteGuard();
                                        this.leaving = false;
                                        this.queueCatAnimationStart();
                                        this.startCatHealthCheck();
                                        console.log("replace music module failed", error);
                                    }
                                }, 120);
                            },
                            armRouteGuard () {
                                this.clearRouteGuard();
                                this.routeGuardTimerId = setTimeout(()=>{
                                    this.routeGuardTimerId = null;
                                    if (!this.leaving) return;
                                    this.leaving = false;
                                    console.log("music route guard released");
                                }, 1200);
                            },
                            clearRouteGuard () {
                                if (!this.routeGuardTimerId) return;
                                clearTimeout(this.routeGuardTimerId);
                                this.routeGuardTimerId = null;
                            },
                            openHealth () {
                                this.replacePage("/pages/health");
                            },
                            openNotificationHub () {
                                this.replacePage("/pages/notificationhub");
                            },
                            openPlayer () {
                                if (this.leaving || this.screenStandby) return;
                                this.leaving = true;
                                this.armRouteGuard();
                                this.cancelCatAnimationStart();
                                this.stopCatHealthCheck();
                                this.stopCatAnimation();
                                setTimeout(()=>{
                                    try {
                                        _system.default.push({
                                            uri: "/pages/musicplayer"
                                        });
                                    } catch (error) {
                                        this.clearRouteGuard();
                                        this.leaving = false;
                                        this.queueCatAnimationStart();
                                        this.startCatHealthCheck();
                                        console.log("open music player failed", error);
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
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: function() {
                                                const $classValue$ = _vm_.titleClass;
                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                return $classValue$;
                                            },
                                            value: "音乐",
                                            static: true
                                        }
                                    }, []),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "music-card"
                                            ],
                                            events: {
                                                click: function(evt) {
                                                    return _vm_.openPlayer(evt);
                                                }
                                            }
                                        }
                                    }, [
                                        aiot.__ce__("image", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "music-icon"
                                                ],
                                                src: "/common/music/player-logo.png"
                                            }
                                        }, [])
                                    ]),
                                    aiot.__ce__("image-animator", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "cat-animation"
                                            ],
                                            id: "musicCatAnimator",
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
