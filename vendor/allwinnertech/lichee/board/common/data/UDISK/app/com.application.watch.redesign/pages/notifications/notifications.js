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
                    "./src/common/notification-center.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports["default"] = void 0;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.storage"));
                        var _system2 = _interopRequireDefault($app_require$1("@app-module/system.vibrator"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const STORAGE_KEY = "notification_history_v1";
                        const EMPTY_STORAGE_VALUE = "__NO_NOTIFICATION_HISTORY__";
                        const HISTORY_LIMIT = 20;
                        let loaded = false;
                        let loading = false;
                        let history = [];
                        let activeNotification = null;
                        let listeners = [];
                        let readyCallbacks = [];
                        function pad(value) {
                            return value < 10 ? "0" + value : "" + value;
                        }
                        function formatTime(timestamp) {
                            const date = new Date(timestamp);
                            return pad(date.getHours()) + ":" + pad(date.getMinutes());
                        }
                        function formatDate(timestamp) {
                            const date = new Date(timestamp);
                            return pad(date.getMonth() + 1) + "月" + pad(date.getDate()) + "日";
                        }
                        function cloneNotification(item) {
                            return {
                                id: item.id,
                                type: item.type,
                                appName: item.appName,
                                sender: item.sender,
                                title: item.title,
                                body: item.body,
                                timestamp: item.timestamp,
                                timeText: item.timeText,
                                dateText: item.dateText,
                                unread: !!item.unread,
                                preview: !!item.preview,
                                previewIndex: Number(item.previewIndex) || 0,
                                previewTotal: Number(item.previewTotal) || 0
                            };
                        }
                        function normalizeType(value) {
                            if ("call" === value || "sms" === value || "app" === value) return value;
                            return "app";
                        }
                        function normalizeNotification(payload) {
                            const source = payload || {};
                            const timestamp = Math.max(0, Number(source.timestamp) || Date.now());
                            const type = normalizeType(source.type);
                            const typeDefaults = {
                                call: {
                                    appName: "电话",
                                    sender: "未知联系人",
                                    title: "来电提醒",
                                    body: "未知号码"
                                },
                                sms: {
                                    appName: "短信",
                                    sender: "新短信",
                                    title: "短信提醒",
                                    body: "收到一条新短信"
                                },
                                app: {
                                    appName: "应用通知",
                                    sender: "",
                                    title: "新消息",
                                    body: "收到一条应用通知"
                                }
                            };
                            const defaults = typeDefaults[type];
                            return {
                                id: source.id || type + "-" + timestamp,
                                type: type,
                                appName: source.appName || defaults.appName,
                                sender: source.sender || defaults.sender,
                                title: source.title || defaults.title,
                                body: source.body || defaults.body,
                                timestamp: timestamp,
                                timeText: formatTime(timestamp),
                                dateText: formatDate(timestamp),
                                unread: false !== source.unread,
                                preview: !!source.preview,
                                previewIndex: Number(source.previewIndex) || 0,
                                previewTotal: Number(source.previewTotal) || 0
                            };
                        }
                        function notifyListeners() {
                            const snapshot = {
                                active: activeNotification ? cloneNotification(activeNotification) : null,
                                history: history.map(cloneNotification)
                            };
                            const copied = listeners.slice();
                            for(let index = 0; index < copied.length; index += 1){
                                try {
                                    copied[index](snapshot);
                                } catch (error) {
                                    console.log("notification listener failed", error);
                                }
                            }
                        }
                        function flushReadyCallbacks() {
                            const callbacks = readyCallbacks.slice();
                            readyCallbacks = [];
                            for(let index = 0; index < callbacks.length; index += 1){
                                try {
                                    callbacks[index]();
                                } catch (error) {
                                    console.log("notification ready callback failed", error);
                                }
                            }
                        }
                        function finishLoading(nextHistory) {
                            history = Array.isArray(nextHistory) ? nextHistory.map(normalizeNotification).slice(0, HISTORY_LIMIT) : [];
                            loaded = true;
                            loading = false;
                            flushReadyCallbacks();
                            notifyListeners();
                        }
                        function initialize(callback) {
                            if (loaded) {
                                if (callback) callback();
                                return;
                            }
                            if (callback) readyCallbacks.push(callback);
                            if (loading) return;
                            loading = true;
                            try {
                                _system.default.get({
                                    key: STORAGE_KEY,
                                    default: EMPTY_STORAGE_VALUE,
                                    success: (value)=>{
                                        if (!value || value === EMPTY_STORAGE_VALUE) return void finishLoading([]);
                                        try {
                                            finishLoading(JSON.parse(value));
                                        } catch (error) {
                                            console.log("parse notification history failed", error);
                                            finishLoading([]);
                                        }
                                    },
                                    fail: (data, code)=>{
                                        console.log("load notification history failed", code, data);
                                        finishLoading([]);
                                    }
                                });
                            } catch (error) {
                                console.log("notification storage unavailable", error);
                                finishLoading([]);
                            }
                        }
                        function persistHistory() {
                            try {
                                _system.default.set({
                                    key: STORAGE_KEY,
                                    value: JSON.stringify(history),
                                    fail: (data, code)=>{
                                        console.log("save notification history failed", code, data);
                                    }
                                });
                            } catch (error) {
                                console.log("notification history persistence unavailable", error);
                            }
                        }
                        function triggerVibration(type) {
                            try {
                                _system2.default.vibrate({
                                    mode: "call" === type ? "long" : "short"
                                });
                                if ("sms" === type) setTimeout(()=>{
                                    try {
                                        _system2.default.vibrate({
                                            mode: "short"
                                        });
                                    } catch (error) {
                                        console.log("second sms vibration unavailable", error);
                                    }
                                }, 420);
                            } catch (error) {
                                console.log("notification vibration unavailable", error);
                            }
                        }
                        function storeInHistory(item) {
                            if (item.preview) return;
                            const nextHistory = [
                                item
                            ];
                            for(let index = 0; index < history.length; index += 1){
                                if (history[index].id !== item.id) nextHistory.push(history[index]);
                                if (nextHistory.length >= HISTORY_LIMIT) break;
                            }
                            history = nextHistory;
                            persistHistory();
                        }
                        function receive(payload) {
                            if (!loaded) {
                                initialize(()=>receive(payload));
                                return null;
                            }
                            const item = normalizeNotification(payload);
                            storeInHistory(item);
                            activeNotification = item;
                            triggerVibration(item.type);
                            notifyListeners();
                            return cloneNotification(item);
                        }
                        function record(payload) {
                            if (!loaded) {
                                initialize(()=>record(payload));
                                return null;
                            }
                            const item = normalizeNotification(payload);
                            storeInHistory(item);
                            notifyListeners();
                            return cloneNotification(item);
                        }
                        function dismiss(notificationId) {
                            if (activeNotification && (!notificationId || activeNotification.id === notificationId)) {
                                activeNotification = null;
                                notifyListeners();
                            }
                        }
                        function getActive() {
                            return activeNotification ? cloneNotification(activeNotification) : null;
                        }
                        function getHistory() {
                            return history.map(cloneNotification);
                        }
                        function subscribe(listener) {
                            if ("function" != typeof listener) return;
                            if (listeners.indexOf(listener) < 0) listeners.push(listener);
                            initialize(()=>{
                                listener({
                                    active: getActive(),
                                    history: getHistory()
                                });
                            });
                        }
                        function unsubscribe(listener) {
                            const index = listeners.indexOf(listener);
                            if (index >= 0) listeners.splice(index, 1);
                        }
                        function clearHistory() {
                            history = [];
                            persistHistory();
                            notifyListeners();
                        }
                        var _default = exports["default"] = {
                            initialize,
                            receive,
                            record,
                            dismiss,
                            getActive,
                            getHistory,
                            subscribe,
                            unsubscribe,
                            clearHistory
                        };
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
                                backgroundColor: "#000000",
                                overflow: "hidden"
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
                                width: "260px",
                                height: "52px",
                                position: "absolute",
                                left: "86px",
                                top: "17px",
                                color: "#ffffff",
                                fontSize: "36px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-list"
                                ]
                            ],
                            {
                                width: "404px",
                                height: "354px",
                                position: "absolute",
                                left: "14px",
                                top: "76px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-item"
                                ]
                            ],
                            {
                                width: "404px",
                                height: "108px",
                                position: "relative",
                                marginBottom: "9px",
                                borderTopWidth: "2px",
                                borderRightWidth: "2px",
                                borderBottomWidth: "2px",
                                borderLeftWidth: "2px",
                                borderTopColor: "#344155",
                                borderRightColor: "#344155",
                                borderBottomColor: "#344155",
                                borderLeftColor: "#344155",
                                borderRadius: "25px",
                                backgroundColor: "#101721"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-icon"
                                ]
                            ],
                            {
                                width: "64px",
                                height: "64px",
                                position: "absolute",
                                left: "16px",
                                top: "20px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "icon-call"
                                ]
                            ],
                            {
                                borderRadius: "32px",
                                backgroundColor: "#00ca58"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "icon-sms"
                                ]
                            ],
                            {
                                borderRadius: "32px",
                                backgroundColor: "#0879f9"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "icon-app"
                                ]
                            ],
                            {
                                borderRadius: "18px",
                                backgroundColor: "#6547ef"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-icon-image"
                                ]
                            ],
                            {
                                width: "44px",
                                height: "44px",
                                position: "absolute",
                                left: "10px",
                                top: "10px",
                                objectFit: "contain"
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
                                width: "210px",
                                height: "34px",
                                position: "absolute",
                                left: "94px",
                                top: "12px",
                                color: "#ffffff",
                                fontSize: "25px",
                                fontWeight: "bold",
                                textOverflow: "ellipsis",
                                lines: 1
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-subtitle"
                                ]
                            ],
                            {
                                width: "270px",
                                height: "28px",
                                position: "absolute",
                                left: "94px",
                                top: "48px",
                                color: "#b9bdc6",
                                fontSize: "18px",
                                textOverflow: "ellipsis",
                                lines: 1
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-body"
                                ]
                            ],
                            {
                                width: "278px",
                                height: "26px",
                                position: "absolute",
                                left: "94px",
                                top: "75px",
                                color: "#a9adb6",
                                fontSize: "17px",
                                textOverflow: "ellipsis",
                                lines: 1
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
                                width: "82px",
                                height: "30px",
                                position: "absolute",
                                right: "14px",
                                top: "17px",
                                color: "#b7bac1",
                                fontSize: "19px",
                                textAlign: "right"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "count"
                                ]
                            ],
                            {
                                width: "250px",
                                height: "32px",
                                position: "absolute",
                                left: "91px",
                                bottom: "7px",
                                color: "#a6a6a6",
                                fontSize: "20px",
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
                        var _notificationCenter = _interopRequireDefault(__webpack_require__("./src/common/notification-center.js"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        var _default = exports.default = {
                            private: {
                                notifications: [],
                                notificationCount: 0,
                                notificationListener: null,
                                leaving: false
                            },
                            onInit () {
                                this.notificationListener = (snapshot)=>{
                                    this.applyHistory(snapshot.history);
                                };
                                _notificationCenter.default.subscribe(this.notificationListener);
                            },
                            onShow () {
                                this.$app.$def.ensureWakeableScreen();
                                this.leaving = false;
                                this.applyHistory(_notificationCenter.default.getHistory());
                            },
                            onDestroy () {
                                if (this.notificationListener) {
                                    _notificationCenter.default.unsubscribe(this.notificationListener);
                                    this.notificationListener = null;
                                }
                            },
                            isSameDay (left, right) {
                                return left.getFullYear() === right.getFullYear() && left.getMonth() === right.getMonth() && left.getDate() === right.getDate();
                            },
                            displayTime (item) {
                                const timestamp = Number(item.timestamp);
                                if (!isFinite(timestamp) || timestamp <= 0) return item.timeText || "";
                                const date = new Date(timestamp);
                                const now = new Date();
                                if (this.isSameDay(date, now)) return item.timeText || "";
                                const yesterday = new Date(now.getFullYear(), now.getMonth(), now.getDate() - 1);
                                if (this.isSameDay(date, yesterday)) return "昨天";
                                return item.dateText || "";
                            },
                            mapHistoryItem (item) {
                                if ("call" === item.type) return {
                                    id: item.id,
                                    iconClass: "history-icon icon-call",
                                    iconSrc: "/common/notifications/phone-answer.png",
                                    displayTitle: "未接来电",
                                    subtitle: (item.sender || "未知联系人") + (item.body ? "  " + item.body : ""),
                                    body: "",
                                    bodyVisible: false,
                                    displayTime: this.displayTime(item)
                                };
                                if ("sms" === item.type) return {
                                    id: item.id,
                                    iconClass: "history-icon icon-sms",
                                    iconSrc: "/common/notifications/message.png",
                                    displayTitle: "短信",
                                    subtitle: item.sender || "未知号码",
                                    body: item.body || "",
                                    bodyVisible: !!item.body,
                                    displayTime: this.displayTime(item)
                                };
                                return {
                                    id: item.id,
                                    iconClass: "history-icon icon-app",
                                    iconSrc: "/common/notifications/bell.png",
                                    displayTitle: item.appName || item.title || "应用通知",
                                    subtitle: item.sender || item.title || "",
                                    body: item.body || "",
                                    bodyVisible: !!item.body,
                                    displayTime: this.displayTime(item)
                                };
                            },
                            previewHistory () {
                                return [
                                    {
                                        id: "preview-call",
                                        iconClass: "history-icon icon-call",
                                        iconSrc: "/common/notifications/phone-answer.png",
                                        displayTitle: "未接来电",
                                        subtitle: "张三  138 0013 8000",
                                        body: "",
                                        bodyVisible: false,
                                        displayTime: "10:42"
                                    },
                                    {
                                        id: "preview-sms",
                                        iconClass: "history-icon icon-sms",
                                        iconSrc: "/common/notifications/message.png",
                                        displayTitle: "短信",
                                        subtitle: "138 0013 8000",
                                        body: "您的快递已到达驿站",
                                        bodyVisible: true,
                                        displayTime: "09:18"
                                    },
                                    {
                                        id: "preview-app",
                                        iconClass: "history-icon icon-app",
                                        iconSrc: "/common/notifications/bell.png",
                                        displayTitle: "健康助手",
                                        subtitle: "",
                                        body: "每日健康报告已生成",
                                        bodyVisible: true,
                                        displayTime: "昨天"
                                    }
                                ];
                            },
                            applyHistory (items) {
                                const source = Array.isArray(items) ? items : [];
                                const mapped = [];
                                for(let index = 0; index < source.length; index += 1)mapped.push(this.mapHistoryItem(source[index]));
                                this.notifications = mapped.length ? mapped : this.previewHistory();
                                this.notificationCount = this.notifications.length;
                            },
                            handleSwipe (event) {
                                if (this.leaving) return;
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if ("right" !== direction) return;
                                this.leaving = true;
                                try {
                                    _system.default.back();
                                } catch (error) {
                                    this.leaving = false;
                                    console.log("close notification history failed", error);
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
                                            }
                                        }
                                    }
                                }, [
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "title"
                                            ],
                                            value: "通知记录"
                                        }
                                    }, []),
                                    aiot.__ce__("list", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "history-list"
                                            ],
                                            bounces: "false"
                                        }
                                    }, [
                                        aiot.__cf__({
                                            __vm__: _vm_,
                                            __opts__: {
                                                exp: function() {
                                                    return {
                                                        __list__: _vm_.notifications,
                                                        __tid__: "id"
                                                    };
                                                },
                                                key: "$idx",
                                                value: "$item"
                                            }
                                        }, function($idx, $item) {
                                            return [
                                                aiot.__ce__("list-item", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        type: "notification",
                                                        classList: [
                                                            "history-item"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: function() {
                                                                const $classValue$ = $item.iconClass;
                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                return $classValue$;
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("image", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "history-icon-image"
                                                                ],
                                                                src: function() {
                                                                    return $item.iconSrc;
                                                                }
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "history-title"
                                                            ],
                                                            value: function() {
                                                                return $item.displayTitle;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "history-subtitle"
                                                            ],
                                                            value: function() {
                                                                return $item.subtitle;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ci__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            shown: function() {
                                                                return $item.bodyVisible;
                                                            }
                                                        }
                                                    }, function() {
                                                        return [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "history-body"
                                                                    ],
                                                                    value: function() {
                                                                        return $item.body;
                                                                    }
                                                                }
                                                            }, [])
                                                        ];
                                                    }),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "history-time"
                                                            ],
                                                            value: function() {
                                                                return $item.displayTime;
                                                            }
                                                        }
                                                    }, [])
                                                ])
                                            ];
                                        })
                                    ]),
                                    aiot.__ce__("text", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "count"
                                            ],
                                            value: function() {
                                                return "共 " + _vm_.notificationCount + " 条通知";
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
