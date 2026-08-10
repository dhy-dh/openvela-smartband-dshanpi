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
                                backgroundColor: "rgba(0, 10, 40, 0.55)"
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
                                height: "54px",
                                position: "absolute",
                                left: "86px",
                                top: "24px",
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
                                    "song-list"
                                ]
                            ],
                            {
                                width: "372px",
                                height: "318px",
                                position: "absolute",
                                left: "30px",
                                top: "88px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "song-item"
                                ]
                            ],
                            {
                                width: "372px",
                                height: "98px",
                                marginBottom: "8px",
                                borderRadius: "25px",
                                backgroundColor: "rgba(255, 255, 255, 0.94)",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "index-badge"
                                ]
                            ],
                            {
                                width: "56px",
                                height: "56px",
                                marginLeft: "14px",
                                borderRadius: "28px",
                                backgroundColor: "#3b8fff",
                                justifyContent: "center",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "index-text"
                                ]
                            ],
                            {
                                width: "56px",
                                height: "38px",
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
                                    "song-copy"
                                ]
                            ],
                            {
                                width: "238px",
                                height: "78px",
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
                                width: "238px",
                                height: "42px",
                                color: "#11275d",
                                fontSize: "28px",
                                fontWeight: "bold",
                                lines: 1,
                                textOverflow: "ellipsis"
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
                                width: "238px",
                                height: "30px",
                                color: "#71809e",
                                fontSize: "20px",
                                lines: 1,
                                textOverflow: "ellipsis"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "active-mark"
                                ]
                            ],
                            {
                                width: "38px",
                                height: "44px",
                                color: "#2d8cff",
                                fontSize: "27px",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "hint"
                                ]
                            ],
                            {
                                width: "340px",
                                height: "30px",
                                position: "absolute",
                                left: "46px",
                                top: "426px",
                                color: "rgba(255, 255, 255, 0.82)",
                                fontSize: "18px",
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
                        var _musicPlayer = __webpack_require__("./src/common/music-player.js");
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        var _default = exports.default = {
                            private: {
                                backgroundImage: "/common/backgrounds/dark-purple.png",
                                songs: _musicPlayer.MUSIC_LIBRARY,
                                activeId: 1,
                                touchX: 0,
                                touchY: 0,
                                leaving: false,
                                routeGuardTimerId: null
                            },
                            onInit () {
                                this.syncState();
                            },
                            onShow () {
                                this.$app.$def.ensureWakeableScreen();
                                this.leaving = false;
                                this.clearRouteGuard();
                                this.syncState();
                            },
                            onHide () {
                                this.clearRouteGuard();
                            },
                            onDestroy () {
                                this.clearRouteGuard();
                            },
                            syncState () {
                                const customization = this.$app.$def.getCustomization();
                                this.backgroundImage = (0, _customization.getBackground)(customization.backgroundId).src;
                                const state = this.$app.$def.getMusicState();
                                this.activeId = state.song ? state.song.id : 1;
                                this.$app.$def.refreshMusicState((latest)=>{
                                    this.activeId = latest.song ? latest.song.id : this.activeId;
                                });
                            },
                            selectSong (item) {
                                if (!item || this.leaving) return;
                                this.activeId = item.id;
                                this.$app.$def.selectMusicSongById(item.id, true);
                                this.returnToPlayer();
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
                                if (Math.abs(deltaY) > 60 && Math.abs(deltaY) > 1.2 * Math.abs(deltaX)) this.returnToPlayer();
                            },
                            handleSwipe (event) {
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if ("up" === direction || "down" === direction) this.returnToPlayer();
                            },
                            returnToPlayer () {
                                if (this.leaving) return;
                                this.leaving = true;
                                this.armRouteGuard();
                                try {
                                    _system.default.replace({
                                        uri: "/pages/musicplayer"
                                    });
                                } catch (error) {
                                    this.clearRouteGuard();
                                    this.leaving = false;
                                    console.log("close music list failed", error);
                                }
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
                                            value: "播放列表"
                                        }
                                    }, []),
                                    aiot.__ce__("list", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "song-list"
                                            ]
                                        }
                                    }, [
                                        aiot.__cf__({
                                            __vm__: _vm_,
                                            __opts__: {
                                                exp: function() {
                                                    return {
                                                        __list__: _vm_.songs,
                                                        __tid__: "id"
                                                    };
                                                },
                                                key: "index",
                                                value: "item"
                                            }
                                        }, function(index, item) {
                                            return [
                                                aiot.__ce__("list-item", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "song-item"
                                                        ],
                                                        type: "song",
                                                        events: {
                                                            click: function(evt) {
                                                                return _vm_.selectSong(item, evt);
                                                            }
                                                        }
                                                    }
                                                }, [
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "index-badge"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "index-text"
                                                                ],
                                                                value: function() {
                                                                    return index + 1;
                                                                }
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "song-copy"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "song-name"
                                                                ],
                                                                value: function() {
                                                                    return item.name;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "artist-name"
                                                                ],
                                                                value: function() {
                                                                    return item.artists;
                                                                }
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ci__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            shown: function() {
                                                                return item.id === _vm_.activeId;
                                                            }
                                                        }
                                                    }, function() {
                                                        return [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "active-mark"
                                                                    ],
                                                                    value: "▶"
                                                                }
                                                            }, [])
                                                        ];
                                                    })
                                                ])
                                            ];
                                        })
                                    ]),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "hint"
                                            ],
                                            value: "点击歌曲播放 · 上下滑返回"
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
