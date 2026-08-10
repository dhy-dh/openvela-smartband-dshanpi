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
                                    "call-page"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                backgroundColor: "#000000",
                                overflow: "hidden",
                                left: 0,
                                top: 0
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
                                backgroundColor: "#000000",
                                overflow: "hidden",
                                left: 0,
                                top: 0
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
                            public: {
                                routeId: "",
                                routeType: "",
                                routeAppName: "",
                                routeSender: "",
                                routeTitle: "",
                                routeBody: "",
                                routeTimestamp: 0
                            },
                            private: {
                                notificationId: "",
                                notificationType: "app",
                                timeText: "12:44",
                                typeLabel: "App 通知",
                                iconText: "讯",
                                sender: "",
                                notificationBody: "",
                                actionText: "知道了",
                                topIconClass: "top-icon app-top-icon",
                                kindClass: "message-kind app-kind",
                                clockTimerId: null,
                                closing: false,
                                notificationListener: null
                            },
                            onInit () {
                                this.updateTime();
                                const routed = this.getRoutedNotification();
                                if (routed) this.applyNotification(routed);
                                this.notificationListener = (snapshot)=>{
                                    if (snapshot.active) this.applyNotification(snapshot.active);
                                };
                                _notificationCenter.default.subscribe(this.notificationListener);
                                if (!routed) {
                                    const active = _notificationCenter.default.getActive();
                                    if (active) this.applyNotification(active);
                                }
                            },
                            onShow () {
                                this.$app.$def.ensureWakeableScreen();
                                this.closing = false;
                                this.startClock();
                                const routed = this.getRoutedNotification();
                                if (routed) return void this.applyNotification(routed);
                                const active = _notificationCenter.default.getActive();
                                if (active) return void this.applyNotification(active);
                                this.closeEmptyNotificationPage();
                            },
                            onHide () {
                                this.stopClock();
                                this.$app.$def.finishNotificationDisplay();
                            },
                            onDestroy () {
                                this.stopClock();
                                this.$app.$def.finishNotificationDisplay();
                                if (this.notificationListener) {
                                    _notificationCenter.default.unsubscribe(this.notificationListener);
                                    this.notificationListener = null;
                                }
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
                            getRoutedNotification () {
                                if (!this.routeId) return null;
                                return {
                                    id: this.routeId,
                                    type: this.routeType || "app",
                                    appName: this.routeAppName || "",
                                    sender: this.routeSender || "",
                                    title: this.routeTitle || "",
                                    body: this.routeBody || "",
                                    timestamp: Number(this.routeTimestamp) || Date.now()
                                };
                            },
                            applyNotification (item) {
                                this.notificationId = item.id;
                                this.notificationType = item.type;
                                this.sender = item.sender || item.appName || "未知联系人";
                                this.notificationBody = item.body || item.title || "";
                                if ("sms" === item.type) {
                                    this.typeLabel = "短信";
                                    this.iconText = "短";
                                    this.actionText = "回复";
                                    this.topIconClass = "top-icon sms-top-icon";
                                    this.kindClass = "message-kind";
                                } else if ("app" === item.type) {
                                    this.typeLabel = item.appName || "App 通知";
                                    this.iconText = "讯";
                                    this.actionText = "知道了";
                                    this.topIconClass = "top-icon app-top-icon";
                                    this.kindClass = "message-kind app-kind";
                                }
                            },
                            closeEmptyNotificationPage () {
                                this.$app.$def.finishNotificationDisplay();
                                setTimeout(()=>{
                                    try {
                                        _system.default.back();
                                    } catch (error) {
                                        console.log("close empty notification page failed", error);
                                    }
                                }, 80);
                            },
                            closeNotification () {
                                if (this.closing) return;
                                this.closing = true;
                                _notificationCenter.default.dismiss(this.notificationId);
                                this.$app.$def.finishNotificationDisplay();
                                setTimeout(()=>{
                                    try {
                                        _system.default.back();
                                    } catch (error) {
                                        this.closing = false;
                                        console.log("close notification failed", error);
                                    }
                                }, 80);
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
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return "call" === _vm_.notificationType;
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
                                                        value: function() {
                                                            return _vm_.sender;
                                                        }
                                                    }
                                                }, []),
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "call-number"
                                                        ],
                                                        value: function() {
                                                            return _vm_.notificationBody;
                                                        }
                                                    }
                                                }, []),
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "call-action",
                                                            "answer"
                                                        ],
                                                        events: {
                                                            click: function(evt) {
                                                                return _vm_.closeNotification(evt);
                                                            }
                                                        }
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
                                                        ],
                                                        events: {
                                                            click: function(evt) {
                                                                return _vm_.closeNotification(evt);
                                                            }
                                                        }
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
                                                        ],
                                                        events: {
                                                            click: function(evt) {
                                                                return _vm_.closeNotification(evt);
                                                            }
                                                        }
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
                                                return "call" !== _vm_.notificationType;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "message-page"
                                                    ],
                                                    events: {
                                                        click: function(evt) {
                                                            return _vm_.closeNotification(evt);
                                                        }
                                                    }
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
                                                        classList: function() {
                                                            const $classValue$ = _vm_.topIconClass;
                                                            if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                            return $classValue$;
                                                        }
                                                    }
                                                }, [
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "top-icon-text"
                                                            ],
                                                            value: function() {
                                                                return _vm_.iconText;
                                                            }
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
                                                            classList: function() {
                                                                const $classValue$ = _vm_.kindClass;
                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                return $classValue$;
                                                            },
                                                            value: function() {
                                                                return _vm_.typeLabel;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "message-sender"
                                                            ],
                                                            value: function() {
                                                                return _vm_.sender;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "message-body"
                                                            ],
                                                            value: function() {
                                                                return _vm_.notificationBody;
                                                            }
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
                                                            value: function() {
                                                                return _vm_.actionText;
                                                            }
                                                        }
                                                    }, [])
                                                ])
                                            ])
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
