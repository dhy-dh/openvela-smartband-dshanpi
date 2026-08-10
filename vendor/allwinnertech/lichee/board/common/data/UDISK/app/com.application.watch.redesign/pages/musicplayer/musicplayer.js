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
                                    "shade"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                backgroundColor: "rgba(0, 10, 40, 0.48)"
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
                                width: "260px",
                                height: "52px",
                                position: "absolute",
                                left: "86px",
                                top: "18px",
                                color: "#ffffff",
                                fontSize: "38px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "song-card"
                                ]
                            ],
                            {
                                width: "344px",
                                height: "102px",
                                position: "absolute",
                                left: "44px",
                                top: "76px",
                                borderRadius: "28px",
                                backgroundColor: "rgba(255, 255, 255, 0.94)",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "album"
                                ]
                            ],
                            {
                                width: "78px",
                                height: "78px",
                                marginLeft: "12px",
                                objectFit: "contain"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "song-copy"
                                ]
                            ],
                            {
                                width: "230px",
                                height: "80px",
                                marginLeft: "12px",
                                flexDirection: "column",
                                justifyContent: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "song-name"
                                ]
                            ],
                            {
                                width: "224px",
                                height: "44px",
                                color: "#11275d",
                                fontSize: "29px",
                                fontWeight: "bold"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "artist-name"
                                ]
                            ],
                            {
                                width: "224px",
                                height: "30px",
                                color: "#71809e",
                                fontSize: "20px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "controls"
                                ]
                            ],
                            {
                                width: "324px",
                                height: "148px",
                                position: "absolute",
                                left: "54px",
                                top: "184px",
                                alignItems: "center",
                                justifyContent: "space-between"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "side-button"
                                ]
                            ],
                            {
                                width: "76px",
                                height: "76px",
                                borderRadius: "38px",
                                backgroundColor: "rgba(255, 255, 255, 0.18)",
                                justifyContent: "center",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "side-icon"
                                ]
                            ],
                            {
                                width: "48px",
                                height: "48px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "play-button"
                                ]
                            ],
                            {
                                width: "132px",
                                height: "132px",
                                justifyContent: "center",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "play-progress"
                                ]
                            ],
                            {
                                width: "132px",
                                height: "132px",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                radius: "54px",
                                strokeWidth: "9px",
                                startAngle: "4deg",
                                totalAngle: "360deg",
                                color: "#4ea0ff",
                                layerColor: "rgba(255, 255, 255, 0.2)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "play-icon"
                                ]
                            ],
                            {
                                width: "58px",
                                height: "58px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "progress-label"
                                ]
                            ],
                            {
                                position: "absolute",
                                color: "#ffffff",
                                textAlign: "center",
                                width: "90px",
                                height: "28px",
                                left: "171px",
                                top: "315px",
                                fontSize: "19px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "status"
                                ]
                            ],
                            {
                                position: "absolute",
                                color: "#ffffff",
                                textAlign: "center",
                                width: "210px",
                                height: "30px",
                                left: "111px",
                                top: "340px",
                                fontSize: "20px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "footer-button"
                                ]
                            ],
                            {
                                width: "76px",
                                height: "76px",
                                position: "absolute",
                                top: "377px",
                                borderRadius: "38px",
                                backgroundColor: "rgba(255, 255, 255, 0.2)",
                                justifyContent: "center",
                                alignItems: "center",
                                zIndex: 20
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "volume-button"
                                ]
                            ],
                            {
                                left: "60px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "list-button"
                                ]
                            ],
                            {
                                left: "296px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "footer-icon"
                                ]
                            ],
                            {
                                width: "44px",
                                height: "44px"
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
                                width: "76px",
                                height: "78px",
                                position: "absolute",
                                left: "178px",
                                top: "375px",
                                zIndex: 10
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "footer-hit"
                                ]
                            ],
                            {
                                width: "104px",
                                height: "104px",
                                position: "absolute",
                                top: "360px",
                                borderRadius: "52px",
                                backgroundColor: "rgba(255, 255, 255, 0.01)",
                                zIndex: 100
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "volume-hit"
                                ]
                            ],
                            {
                                left: "46px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "list-hit"
                                ]
                            ],
                            {
                                left: "282px"
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
                                songName: "未知歌曲",
                                artistName: "未知歌手",
                                isPlaying: false,
                                progress: 0,
                                progressLabel: "0%",
                                statusText: "点击播放",
                                unsubscribeMusic: null,
                                catFrames: [],
                                catDuration: "3400ms",
                                catAnimatorReady: false,
                                catStartTimerId: null,
                                catHealthTimerId: null,
                                touchX: 0,
                                touchY: 0,
                                leaving: false,
                                routeGuardTimerId: null
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
                                this.leaving = false;
                                this.clearRouteGuard();
                                this.syncCustomization();
                                this.unsubscribeFromMusic();
                                this.unsubscribeMusic = this.$app.$def.subscribeMusic((state)=>{
                                    const song = state.song || {};
                                    this.songName = song.name || "未知歌曲";
                                    this.artistName = song.artists || "未知歌手";
                                    this.isPlaying = state.isPlaying;
                                    this.progress = state.progress;
                                    this.progressLabel = state.progress + "%";
                                    this.statusText = state.error || (state.isPlaying ? "正在播放" : "点击播放");
                                });
                                this.$app.$def.refreshMusicState();
                                this.queueCatAnimationStart();
                                this.startCatHealthCheck();
                            },
                            onHide () {
                                this.unsubscribeFromMusic();
                                this.clearRouteGuard();
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.stopCatHealthCheck();
                            },
                            onDestroy () {
                                this.unsubscribeFromMusic();
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
                                const frames = [];
                                for(let index = 0; index < action.frames.length; index += 1)frames.push({
                                    src: APP_RESOURCE_ROOT + action.frames[index]
                                });
                                this.catFrames = frames;
                                const frameDuration = Math.max(50, Math.round(action.duration / action.frames.length));
                                this.catDuration = frameDuration + "ms";
                            },
                            queueCatAnimationStart () {
                                if (!this.catAnimatorReady) return;
                                this.cancelCatAnimationStart();
                                this.catStartTimerId = setTimeout(()=>{
                                    this.catStartTimerId = null;
                                    const animator = this.$element("musicPlayerCatAnimator");
                                    if (!animator) return;
                                    try {
                                        animator.start();
                                    } catch (error) {
                                        console.log("music player cat start failed", error);
                                    }
                                }, 350);
                            },
                            cancelCatAnimationStart () {
                                if (!this.catStartTimerId) return;
                                clearTimeout(this.catStartTimerId);
                                this.catStartTimerId = null;
                            },
                            stopCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("musicPlayerCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.stop();
                                } catch (error) {
                                    console.log("music player cat stop failed", error);
                                }
                            },
                            startCatHealthCheck () {
                                if (this.catHealthTimerId) return;
                                this.catHealthTimerId = setInterval(()=>{
                                    const animator = this.$element("musicPlayerCatAnimator");
                                    if (!this.catAnimatorReady || !animator) return;
                                    try {
                                        const state = animator.getState();
                                        if ("paused" === state) animator.resume();
                                        if ("stopped" === state) animator.start();
                                    } catch (error) {
                                        console.log("music player cat health check failed", error);
                                    }
                                }, 10000);
                            },
                            stopCatHealthCheck () {
                                if (!this.catHealthTimerId) return;
                                clearInterval(this.catHealthTimerId);
                                this.catHealthTimerId = null;
                            },
                            unsubscribeFromMusic () {
                                if (!this.unsubscribeMusic) return;
                                this.unsubscribeMusic();
                                this.unsubscribeMusic = null;
                            },
                            playOrPause () {
                                this.$app.$def.toggleMusic();
                            },
                            previousSong () {
                                this.$app.$def.changeMusicSong(-1);
                            },
                            nextSong () {
                                this.$app.$def.changeMusicSong(1);
                            },
                            openVolume () {
                                console.log("music player open volume");
                                this.replacePage("/pages/musicvolume");
                            },
                            openList () {
                                console.log("music player open list");
                                this.replacePage("/pages/musiclist");
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
                                const point = this.touchPoint(event, false);
                                this.touchX = point.x;
                                this.touchY = point.y;
                            },
                            onTouchEnd (event) {
                                const point = this.touchPoint(event, true);
                                const deltaX = point.x - this.touchX;
                                const deltaY = point.y - this.touchY;
                                if (Math.abs(deltaX) < 24 && Math.abs(deltaY) < 24 && point.y >= 360) {
                                    if (point.x <= 170) {
                                        console.log("music player coordinate tap: volume");
                                        this.openVolume();
                                        return;
                                    }
                                    if (point.x >= 262) {
                                        console.log("music player coordinate tap: list");
                                        this.openList();
                                        return;
                                    }
                                }
                                if (Math.abs(deltaY) > 60 && Math.abs(deltaY) > 1.2 * Math.abs(deltaX)) this.backToMusic();
                            },
                            handleSwipe (event) {
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if ("up" === direction || "down" === direction) this.backToMusic();
                            },
                            replacePage (uri) {
                                if (this.leaving) return;
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
                                        console.log("replace music player page failed", error);
                                    }
                                }, 120);
                            },
                            backToMusic () {
                                if (this.leaving) return;
                                this.leaving = true;
                                this.armRouteGuard();
                                this.cancelCatAnimationStart();
                                this.stopCatHealthCheck();
                                this.stopCatAnimation();
                                setTimeout(()=>{
                                    try {
                                        _system.default.back();
                                    } catch (error) {
                                        this.clearRouteGuard();
                                        this.leaving = false;
                                        this.queueCatAnimationStart();
                                        this.startCatHealthCheck();
                                        console.log("close music player failed", error);
                                    }
                                }, 120);
                            },
                            armRouteGuard () {
                                this.clearRouteGuard();
                                this.routeGuardTimerId = setTimeout(()=>{
                                    this.routeGuardTimerId = null;
                                    this.leaving = false;
                                }, 1200);
                            },
                            clearRouteGuard () {
                                if (!this.routeGuardTimerId) return;
                                clearTimeout(this.routeGuardTimerId);
                                this.routeGuardTimerId = null;
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
                                                "shade"
                                            ]
                                        }
                                    }, []),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "page-title"
                                            ],
                                            value: "音乐播放"
                                        }
                                    }, []),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "song-card"
                                            ]
                                        }
                                    }, [
                                        aiot.__ce__("image", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "album"
                                                ],
                                                src: "/common/music/player-logo.png"
                                            }
                                        }, []),
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "song-copy"
                                                ]
                                            }
                                        }, [
                                            aiot.__ce__("marquee", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "song-name"
                                                    ],
                                                    scrollamount: function() {
                                                        return 28;
                                                    },
                                                    value: function() {
                                                        return _vm_.songName;
                                                    }
                                                }
                                            }, []),
                                            aiot.__ce__("marquee", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "artist-name"
                                                    ],
                                                    scrollamount: function() {
                                                        return 24;
                                                    },
                                                    value: function() {
                                                        return _vm_.artistName;
                                                    }
                                                }
                                            }, [])
                                        ])
                                    ]),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "controls"
                                            ]
                                        }
                                    }, [
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "side-button"
                                                ],
                                                events: {
                                                    click: function(evt) {
                                                        return _vm_.previousSong(evt);
                                                    }
                                                }
                                            }
                                        }, [
                                            aiot.__ce__("image", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "side-icon"
                                                    ],
                                                    src: "/common/music/icons/prev.png"
                                                }
                                            }, [])
                                        ]),
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "play-button"
                                                ],
                                                events: {
                                                    click: function(evt) {
                                                        return _vm_.playOrPause(evt);
                                                    }
                                                }
                                            }
                                        }, [
                                            aiot.__ce__("progress", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "play-progress"
                                                    ],
                                                    type: "arc",
                                                    percent: function() {
                                                        return _vm_.progress;
                                                    }
                                                }
                                            }, []),
                                            aiot.__ci__({
                                                __vm__: _vm_,
                                                __opts__: {
                                                    shown: function() {
                                                        return _vm_.isPlaying;
                                                    }
                                                }
                                            }, function() {
                                                return [
                                                    aiot.__ce__("image", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "play-icon"
                                                            ],
                                                            src: "/common/music/icons/pause.png"
                                                        }
                                                    }, [])
                                                ];
                                            }),
                                            aiot.__ci__({
                                                __vm__: _vm_,
                                                __opts__: {
                                                    shown: function() {
                                                        return !_vm_.isPlaying;
                                                    }
                                                }
                                            }, function() {
                                                return [
                                                    aiot.__ce__("image", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "play-icon"
                                                            ],
                                                            src: "/common/music/icons/play.png"
                                                        }
                                                    }, [])
                                                ];
                                            })
                                        ]),
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "side-button"
                                                ],
                                                events: {
                                                    click: function(evt) {
                                                        return _vm_.nextSong(evt);
                                                    }
                                                }
                                            }
                                        }, [
                                            aiot.__ce__("image", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "side-icon"
                                                    ],
                                                    src: "/common/music/icons/next.png"
                                                }
                                            }, [])
                                        ])
                                    ]),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "progress-label"
                                            ],
                                            value: function() {
                                                return _vm_.progressLabel;
                                            }
                                        }
                                    }, []),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.statusText;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "status"
                                                    ],
                                                    value: function() {
                                                        return _vm_.statusText;
                                                    }
                                                }
                                            }, [])
                                        ];
                                    }),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "footer-button",
                                                "volume-button"
                                            ],
                                            events: {
                                                click: function(evt) {
                                                    return _vm_.openVolume(evt);
                                                }
                                            }
                                        }
                                    }, [
                                        aiot.__ce__("image", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "footer-icon"
                                                ],
                                                src: "/common/music/icons/volume.png",
                                                events: {
                                                    click: function(evt) {
                                                        return _vm_.openVolume(evt);
                                                    }
                                                }
                                            }
                                        }, [])
                                    ]),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "footer-button",
                                                "list-button"
                                            ],
                                            events: {
                                                click: function(evt) {
                                                    return _vm_.openList(evt);
                                                }
                                            }
                                        }
                                    }, [
                                        aiot.__ce__("image", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "footer-icon"
                                                ],
                                                src: "/common/music/icons/play-list.png",
                                                events: {
                                                    click: function(evt) {
                                                        return _vm_.openList(evt);
                                                    }
                                                }
                                            }
                                        }, [])
                                    ]),
                                    aiot.__ce__("image-animator", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "cat-animation"
                                            ],
                                            id: "musicPlayerCatAnimator",
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
                                                "footer-hit",
                                                "volume-hit"
                                            ],
                                            events: {
                                                touchend: function(evt) {
                                                    return _vm_.openVolume(evt);
                                                },
                                                click: function(evt) {
                                                    return _vm_.openVolume(evt);
                                                }
                                            }
                                        }
                                    }, []),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "footer-hit",
                                                "list-hit"
                                            ],
                                            events: {
                                                touchend: function(evt) {
                                                    return _vm_.openList(evt);
                                                },
                                                click: function(evt) {
                                                    return _vm_.openList(evt);
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
