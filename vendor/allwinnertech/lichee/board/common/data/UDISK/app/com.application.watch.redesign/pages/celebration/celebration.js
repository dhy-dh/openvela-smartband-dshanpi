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
                                backgroundColor: "#07112f"
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
                                    "scene-soft"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                objectFit: "cover",
                                opacity: 0.12
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "scene-soft-left"
                                ]
                            ],
                            {
                                left: 0
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "scene-soft-right"
                                ]
                            ],
                            {
                                left: 0
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "background-veil"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                backgroundColor: "rgba(7, 14, 48, 0.66)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "firework"
                                ]
                            ],
                            {
                                width: "124px",
                                height: "124px",
                                position: "absolute"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "firework-top-left"
                                ]
                            ],
                            {
                                left: "7px",
                                top: "11px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "firework-top-right"
                                ]
                            ],
                            {
                                right: "7px",
                                top: "20px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "firework-side-left"
                                ]
                            ],
                            {
                                left: 0,
                                top: "207px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "firework-side-right"
                                ]
                            ],
                            {
                                right: "1px",
                                top: "224px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "message-panel"
                                ]
                            ],
                            {
                                width: "304px",
                                height: "176px",
                                position: "absolute",
                                left: "64px",
                                top: "51px",
                                borderTopWidth: "2px",
                                borderRightWidth: "2px",
                                borderBottomWidth: "2px",
                                borderLeftWidth: "2px",
                                borderTopColor: "rgba(255,231,145,0.68)",
                                borderRightColor: "rgba(255,231,145,0.68)",
                                borderBottomColor: "rgba(255,231,145,0.68)",
                                borderLeftColor: "rgba(255,231,145,0.68)",
                                borderRadius: "34px",
                                backgroundColor: "rgba(10, 25, 70, 0.78)",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "congratulations"
                                ]
                            ],
                            {
                                width: "268px",
                                height: "58px",
                                position: "absolute",
                                left: "18px",
                                top: "16px",
                                color: "#ffd958",
                                fontSize: "43px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "message-line-one"
                                ]
                            ],
                            {
                                width: "276px",
                                height: "36px",
                                position: "absolute",
                                left: "14px",
                                top: "82px",
                                color: "#ffffff",
                                fontSize: "25px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "message-line-two"
                                ]
                            ],
                            {
                                width: "276px",
                                height: "36px",
                                position: "absolute",
                                left: "14px",
                                top: "120px",
                                color: "#ffffff",
                                fontSize: "25px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "cat-glow"
                                ]
                            ],
                            {
                                width: "238px",
                                height: "82px",
                                position: "absolute",
                                left: "97px",
                                bottom: "18px",
                                borderRadius: "50%",
                                backgroundColor: "rgba(145, 181, 255, 0.24)"
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
                                width: "214px",
                                height: "214px",
                                position: "absolute",
                                left: "109px",
                                bottom: "22px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "exit-layer"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                zIndex: 50,
                                backgroundColor: "rgba(0, 0, 0, 0)"
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
                        var _default = exports.default = {
                            private: {
                                backgroundImage: "/common/backgrounds/dark-purple.png",
                                fireworkFrames: [],
                                catFrames: [],
                                catDuration: "3400ms",
                                ready: false,
                                exiting: false,
                                startTimerIds: [],
                                healthTimerId: null
                            },
                            onInit () {
                                this.syncContent();
                            },
                            onReady () {
                                this.ready = true;
                                this.queueAnimations();
                            },
                            onShow () {
                                this.$app.$def.ensureWakeableScreen();
                                this.exiting = false;
                                this.syncContent();
                                if (this.ready) this.queueAnimations();
                                this.startHealthCheck();
                            },
                            onHide () {
                                this.$app.$def.finishAllGoalsCelebration();
                                this.clearStartTimers();
                                this.stopHealthCheck();
                                this.stopAnimations();
                            },
                            onDestroy () {
                                this.clearStartTimers();
                                this.stopHealthCheck();
                                this.stopAnimations();
                                this.ready = false;
                            },
                            syncContent () {
                                const customization = this.$app.$def.getCustomization();
                                const background = (0, _customization.getBackground)(customization.backgroundId);
                                const action = (0, _customization.getAction)(customization.actionId);
                                const fireworks = [];
                                const cats = [];
                                for(let index = 1; index <= 16; index += 1){
                                    const number = index < 10 ? "0" + index : "" + index;
                                    fireworks.push({
                                        src: APP_RESOURCE_ROOT + "/common/celebration/fireworks/frame-" + number + ".png"
                                    });
                                }
                                for(let index = 0; index < action.frames.length; index += 1)cats.push({
                                    src: APP_RESOURCE_ROOT + action.frames[index]
                                });
                                this.backgroundImage = background.src;
                                this.fireworkFrames = fireworks;
                                this.catFrames = cats;
                                this.catDuration = Math.max(50, Math.round(action.duration / action.frames.length)) + "ms";
                            },
                            startAnimator (id) {
                                if (!this.ready) return;
                                const animator = this.$element(id);
                                if (!animator) return;
                                try {
                                    animator.start();
                                } catch (error) {
                                    console.log("celebration animator start failed", id, error);
                                }
                            },
                            queueStart (id, delay) {
                                const timerId = setTimeout(()=>{
                                    this.startAnimator(id);
                                }, delay);
                                this.startTimerIds.push(timerId);
                            },
                            queueAnimations () {
                                if (!this.ready) return;
                                this.clearStartTimers();
                                this.stopAnimations();
                                this.queueStart("fireworkOne", 80);
                                this.queueStart("fireworkTwo", 360);
                                this.queueStart("fireworkThree", 650);
                                this.queueStart("fireworkFour", 930);
                                this.queueStart("celebrationCatAnimator", 260);
                            },
                            clearStartTimers () {
                                for(let index = 0; index < this.startTimerIds.length; index += 1)clearTimeout(this.startTimerIds[index]);
                                this.startTimerIds = [];
                            },
                            stopAnimator (id) {
                                if (!this.ready) return;
                                const animator = this.$element(id);
                                if (!animator) return;
                                try {
                                    animator.stop();
                                } catch (error) {
                                    console.log("celebration animator stop failed", id, error);
                                }
                            },
                            stopAnimations () {
                                this.stopAnimator("fireworkOne");
                                this.stopAnimator("fireworkTwo");
                                this.stopAnimator("fireworkThree");
                                this.stopAnimator("fireworkFour");
                                this.stopAnimator("celebrationCatAnimator");
                            },
                            startHealthCheck () {
                                if (this.healthTimerId) return;
                                this.healthTimerId = setInterval(()=>{
                                    const ids = [
                                        "fireworkOne",
                                        "fireworkTwo",
                                        "fireworkThree",
                                        "fireworkFour",
                                        "celebrationCatAnimator"
                                    ];
                                    for(let index = 0; index < ids.length; index += 1){
                                        const animator = this.$element(ids[index]);
                                        if (animator) try {
                                            const state = animator.getState();
                                            if ("paused" === state) animator.resume();
                                            if ("stopped" === state) animator.start();
                                        } catch (error) {
                                            console.log("celebration animator health check failed", ids[index], error);
                                        }
                                    }
                                }, 8000);
                            },
                            stopHealthCheck () {
                                if (!this.healthTimerId) return;
                                clearInterval(this.healthTimerId);
                                this.healthTimerId = null;
                            },
                            exitCelebration () {
                                if (this.exiting) return;
                                this.exiting = true;
                                this.$app.$def.finishAllGoalsCelebration();
                                try {
                                    _system.default.back();
                                } catch (error) {
                                    this.exiting = false;
                                    console.log("close celebration failed", error);
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
                                    aiot.__ce__("image", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "scene-soft",
                                                "scene-soft-left"
                                            ],
                                            src: function() {
                                                return _vm_.backgroundImage;
                                            }
                                        }
                                    }, []),
                                    aiot.__ce__("image", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "scene-soft",
                                                "scene-soft-right"
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
                                                "background-veil"
                                            ]
                                        }
                                    }, []),
                                    aiot.__ce__("image-animator", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "firework",
                                                "firework-top-left"
                                            ],
                                            id: "fireworkOne",
                                            images: function() {
                                                return _vm_.fireworkFrames;
                                            },
                                            duration: "1300ms",
                                            iteration: "infinite",
                                            fixedsize: "true"
                                        }
                                    }, []),
                                    aiot.__ce__("image-animator", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "firework",
                                                "firework-top-right"
                                            ],
                                            id: "fireworkTwo",
                                            images: function() {
                                                return _vm_.fireworkFrames;
                                            },
                                            duration: "1300ms",
                                            iteration: "infinite",
                                            fixedsize: "true"
                                        }
                                    }, []),
                                    aiot.__ce__("image-animator", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "firework",
                                                "firework-side-left"
                                            ],
                                            id: "fireworkThree",
                                            images: function() {
                                                return _vm_.fireworkFrames;
                                            },
                                            duration: "1300ms",
                                            iteration: "infinite",
                                            fixedsize: "true"
                                        }
                                    }, []),
                                    aiot.__ce__("image-animator", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "firework",
                                                "firework-side-right"
                                            ],
                                            id: "fireworkFour",
                                            images: function() {
                                                return _vm_.fireworkFrames;
                                            },
                                            duration: "1300ms",
                                            iteration: "infinite",
                                            fixedsize: "true"
                                        }
                                    }, []),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "message-panel"
                                            ]
                                        }
                                    }, [
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "congratulations"
                                                ],
                                                value: "恭喜！"
                                            }
                                        }, []),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "message-line-one"
                                                ],
                                                value: "今天所有运动目标"
                                            }
                                        }, []),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "message-line-two"
                                                ],
                                                value: "已经完成！"
                                            }
                                        }, [])
                                    ]),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "cat-glow"
                                            ]
                                        }
                                    }, []),
                                    aiot.__ce__("image-animator", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "cat-animation"
                                            ],
                                            id: "celebrationCatAnimator",
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
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "exit-layer"
                                            ],
                                            events: {
                                                click: function(evt) {
                                                    return _vm_.exitCelebration(evt);
                                                }
                                            }
                                        }
                                    }, [])
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
