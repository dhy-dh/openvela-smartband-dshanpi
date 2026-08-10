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
                var __webpack_modules__ = {};
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
                            backgroundColor: "#000000",
                            overflow: "hidden"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "call-page"
                            ]
                        ],
                        {
                            width: "100%",
                            height: "100%",
                            position: "absolute",
                            left: 0,
                            top: 0,
                            backgroundColor: "#000000"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "message-page"
                            ]
                        ],
                        {
                            width: "100%",
                            height: "100%",
                            position: "absolute",
                            left: 0,
                            top: 0,
                            backgroundColor: "#000000"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "call-time"
                            ]
                        ],
                        {
                            width: "180px",
                            height: "48px",
                            position: "absolute",
                            left: "126px",
                            top: "22px",
                            color: "#ffffff",
                            fontSize: "34px",
                            fontWeight: "bold",
                            textAlign: "center"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "call-quality"
                            ]
                        ],
                        {
                            width: "100px",
                            height: "34px",
                            position: "absolute",
                            left: "166px",
                            top: "65px",
                            color: "#ffffff",
                            fontSize: "24px",
                            textAlign: "center"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "call-name"
                            ]
                        ],
                        {
                            width: "360px",
                            height: "52px",
                            position: "absolute",
                            left: "36px",
                            top: "142px",
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
                                "call-number"
                            ]
                        ],
                        {
                            width: "360px",
                            height: "40px",
                            position: "absolute",
                            left: "36px",
                            top: "197px",
                            color: "#c7c7c7",
                            fontSize: "27px",
                            textAlign: "center"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "call-action"
                            ]
                        ],
                        {
                            width: "92px",
                            height: "92px",
                            position: "absolute",
                            top: "319px",
                            borderRadius: "46px"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "answer"
                            ]
                        ],
                        {
                            left: "28px",
                            backgroundColor: "#00d05f"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "reject"
                            ]
                        ],
                        {
                            left: "170px",
                            backgroundColor: "#ff1424"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "message"
                            ]
                        ],
                        {
                            left: "312px",
                            backgroundColor: "#0879f9"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "call-action-icon"
                            ]
                        ],
                        {
                            width: "54px",
                            height: "54px",
                            position: "absolute",
                            left: "19px",
                            top: "19px",
                            objectFit: "contain"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "message-time"
                            ]
                        ],
                        {
                            width: "150px",
                            height: "38px",
                            position: "absolute",
                            left: "141px",
                            top: "18px",
                            color: "#ffffff",
                            fontSize: "28px",
                            fontWeight: "bold",
                            textAlign: "center"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "top-icon"
                            ]
                        ],
                        {
                            width: "84px",
                            height: "84px",
                            position: "absolute",
                            left: "174px",
                            top: "54px",
                            borderRadius: "42px"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "sms-top-icon"
                            ]
                        ],
                        {
                            backgroundColor: "#ffb43d"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "app-top-icon"
                            ]
                        ],
                        {
                            backgroundColor: "#6d52d9"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "top-icon-text"
                            ]
                        ],
                        {
                            width: "84px",
                            height: "84px",
                            color: "#ffffff",
                            fontSize: "34px",
                            fontWeight: "bold",
                            textAlign: "center",
                            lineHeight: "84px"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "message-card"
                            ]
                        ],
                        {
                            width: "406px",
                            height: "246px",
                            position: "absolute",
                            left: "13px",
                            top: "151px",
                            borderRadius: "30px",
                            backgroundColor: "#242424"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "message-kind"
                            ]
                        ],
                        {
                            width: "310px",
                            height: "38px",
                            position: "absolute",
                            left: "28px",
                            top: "23px",
                            color: "#bdbdbd",
                            fontSize: "27px",
                            fontWeight: "bold"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "app-kind"
                            ]
                        ],
                        {
                            color: "#bcaeff"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "message-sender"
                            ]
                        ],
                        {
                            width: "350px",
                            height: "43px",
                            position: "absolute",
                            left: "28px",
                            top: "67px",
                            color: "#ffffff",
                            fontSize: "31px",
                            fontWeight: "bold",
                            textOverflow: "ellipsis",
                            lines: 1
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "message-body"
                            ]
                        ],
                        {
                            width: "350px",
                            height: "82px",
                            position: "absolute",
                            left: "28px",
                            top: "116px",
                            color: "#dedede",
                            fontSize: "25px",
                            lineHeight: "36px",
                            textOverflow: "ellipsis",
                            lines: 2
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "message-received"
                            ]
                        ],
                        {
                            width: "150px",
                            height: "28px",
                            position: "absolute",
                            left: "28px",
                            bottom: "17px",
                            color: "#8d8d8d",
                            fontSize: "19px"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "reply-button"
                            ]
                        ],
                        {
                            width: "250px",
                            height: "62px",
                            position: "absolute",
                            left: "91px",
                            top: "409px",
                            borderRadius: "31px",
                            backgroundColor: "#252525"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "reply-text"
                            ]
                        ],
                        {
                            width: "250px",
                            height: "62px",
                            color: "#ffffff",
                            fontSize: "27px",
                            fontWeight: "bold",
                            textAlign: "center",
                            lineHeight: "62px"
                        }
                    ],
                    [
                        [
                            [
                                0,
                                "preview-hint"
                            ]
                        ],
                        {
                            width: "310px",
                            height: "22px",
                            position: "absolute",
                            left: "61px",
                            bottom: "3px",
                            color: "#777777",
                            fontSize: "14px",
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
                    var _default = exports.default = {
                        private: {
                            previewIndex: 0,
                            previewNumber: 1,
                            timeText: "12:44",
                            clockTimerId: null
                        },
                        onInit () {
                            this.updateTime();
                        },
                        onShow () {
                            this.$app.$def.ensureWakeableScreen();
                            this.startClock();
                        },
                        onHide () {
                            this.stopClock();
                        },
                        onDestroy () {
                            this.stopClock();
                        },
                        pad (value) {
                            return value < 10 ? "0" + value : "" + value;
                        },
                        updateTime () {
                            const now = new Date();
                            this.timeText = this.pad(now.getHours()) + ":" + this.pad(now.getMinutes());
                        },
                        startClock () {
                            this.stopClock();
                            this.updateTime();
                            this.clockTimerId = setInterval(()=>this.updateTime(), 30000);
                        },
                        stopClock () {
                            if (!this.clockTimerId) return;
                            clearInterval(this.clockTimerId);
                            this.clockTimerId = null;
                        },
                        nextPreview () {
                            this.previewIndex = (this.previewIndex + 1) % 3;
                            this.previewNumber = this.previewIndex + 1;
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
                                        click: function(evt) {
                                            return _vm_.nextPreview(evt);
                                        }
                                    }
                                }
                            }, [
                                aiot.__ci__({
                                    __vm__: _vm_,
                                    __opts__: {
                                        shown: function() {
                                            return 0 === _vm_.previewIndex;
                                        }
                                    }
                                }, function() {
                                    return [
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "call-page"
                                                ]
                                            }
                                        }, [
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "call-time"
                                                    ],
                                                    value: function() {
                                                        return _vm_.timeText;
                                                    }
                                                }
                                            }, []),
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "call-quality"
                                                    ],
                                                    value: "HD"
                                                }
                                            }, []),
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "call-name"
                                                    ],
                                                    value: "张三"
                                                }
                                            }, []),
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "call-number"
                                                    ],
                                                    value: "138 **** 8888"
                                                }
                                            }, []),
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "call-action",
                                                        "answer"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("image", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "call-action-icon"
                                                        ],
                                                        src: "/common/notifications/phone-answer.png"
                                                    }
                                                }, [])
                                            ]),
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "call-action",
                                                        "reject"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("image", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "call-action-icon"
                                                        ],
                                                        src: "/common/notifications/phone-reject.png"
                                                    }
                                                }, [])
                                            ]),
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "call-action",
                                                        "message"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("image", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "call-action-icon"
                                                        ],
                                                        src: "/common/notifications/message.png"
                                                    }
                                                }, [])
                                            ])
                                        ])
                                    ];
                                }),
                                aiot.__ci__({
                                    __vm__: _vm_,
                                    __opts__: {
                                        shown: function() {
                                            return 1 === _vm_.previewIndex;
                                        }
                                    }
                                }, function() {
                                    return [
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "message-page"
                                                ]
                                            }
                                        }, [
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "message-time"
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
                                                        "top-icon",
                                                        "sms-top-icon"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "top-icon-text"
                                                        ],
                                                        value: "短"
                                                    }
                                                }, [])
                                            ]),
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "message-card"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "message-kind"
                                                        ],
                                                        value: "短信"
                                                    }
                                                }, []),
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "message-sender"
                                                        ],
                                                        value: "妈妈"
                                                    }
                                                }, []),
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "message-body"
                                                        ],
                                                        value: "晚上记得早点回家，路上注意安全。"
                                                    }
                                                }, []),
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "message-received"
                                                        ],
                                                        value: "刚刚"
                                                    }
                                                }, [])
                                            ]),
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "reply-button"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "reply-text"
                                                        ],
                                                        value: "回复"
                                                    }
                                                }, [])
                                            ])
                                        ])
                                    ];
                                }),
                                aiot.__ci__({
                                    __vm__: _vm_,
                                    __opts__: {
                                        shown: function() {
                                            return 2 === _vm_.previewIndex;
                                        }
                                    }
                                }, function() {
                                    return [
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "message-page"
                                                ]
                                            }
                                        }, [
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "message-time"
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
                                                        "top-icon",
                                                        "app-top-icon"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "top-icon-text"
                                                        ],
                                                        value: "微"
                                                    }
                                                }, [])
                                            ]),
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "message-card",
                                                        "app-message-card"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "message-kind",
                                                            "app-kind"
                                                        ],
                                                        value: "微信通知"
                                                    }
                                                }, []),
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "message-sender"
                                                        ],
                                                        value: "项目小组"
                                                    }
                                                }, []),
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "message-body"
                                                        ],
                                                        value: "新的设计稿已经发到群里，请查收。"
                                                    }
                                                }, []),
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "message-received"
                                                        ],
                                                        value: "刚刚"
                                                    }
                                                }, [])
                                            ]),
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "reply-button",
                                                        "app-reply-button"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "reply-text"
                                                        ],
                                                        value: "知道了"
                                                    }
                                                }, [])
                                            ])
                                        ])
                                    ];
                                }),
                                aiot.__ce__("text", {
                                    __vm__: _vm_,
                                    __opts__: {
                                        classList: [
                                            "preview-hint"
                                        ],
                                        value: function() {
                                            return "样式预览 " + _vm_.previewNumber + "/3 · 点击查看下一种";
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
        };
        return createPageHandler();
    })(global, globalThis, window, $app_exports$, $app_evaluate$);
}
