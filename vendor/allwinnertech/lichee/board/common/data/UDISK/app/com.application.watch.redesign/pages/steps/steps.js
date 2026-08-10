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
                    "./src/common/step-tracker.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports["default"] = void 0;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.device"));
                        var _system2 = _interopRequireDefault($app_require$1("@app-module/system.sensor"));
                        var _system3 = _interopRequireDefault($app_require$1("@app-module/system.storage"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const STORAGE_PREFIX = "daily_steps_v1_";
                        const STEP_GOAL_STORAGE_KEY = "step_goal_v1";
                        const CALORIE_GOAL_STORAGE_KEY = "calorie_goal_v1";
                        const DURATION_GOAL_STORAGE_KEY = "duration_goal_v1";
                        const SAVE_DELAY_MS = 900;
                        const HISTORY_RETENTION_DAYS = 30;
                        const DEMO_INTERVAL_MS = 2600;
                        const ENABLE_SIMULATOR_STEP_GROWTH = true;
                        const MIN_STEP_INTERVAL_MS = 280;
                        const MAX_STEP_INTERVAL_MS = 1800;
                        const DEMO_HISTORY_TOTALS = {
                            "2026-07-22": 11308,
                            "2026-07-23": 5421,
                            "2026-07-24": 8976,
                            "2026-07-25": 6230,
                            "2026-07-26": 10184,
                            "2026-07-27": 7352
                        };
                        const DEMO_HISTORY_GOALS = {
                            "2026-07-22": 12000,
                            "2026-07-23": 6000,
                            "2026-07-24": 10000,
                            "2026-07-25": 7500,
                            "2026-07-26": 11000,
                            "2026-07-27": 8000
                        };
                        const DEMO_HISTORY_CALORIE_GOALS = {
                            "2026-07-22": 450,
                            "2026-07-23": 300,
                            "2026-07-24": 400,
                            "2026-07-25": 350,
                            "2026-07-26": 500,
                            "2026-07-27": 400
                        };
                        const DEMO_HISTORY_DURATION_GOALS = {
                            "2026-07-22": 45,
                            "2026-07-23": 30,
                            "2026-07-24": 40,
                            "2026-07-25": 35,
                            "2026-07-26": 60,
                            "2026-07-27": 30
                        };
                        const DEFAULT_STEP_GOAL = 8000;
                        const DEFAULT_CALORIE_GOAL = 400;
                        const DEFAULT_DURATION_GOAL = 30;
                        function pad(value) {
                            return value < 10 ? "0" + value : "" + value;
                        }
                        function dateKey(date) {
                            return date.getFullYear() + "-" + pad(date.getMonth() + 1) + "-" + pad(date.getDate());
                        }
                        function todayKey() {
                            return dateKey(new Date());
                        }
                        function dateKeyBefore(daysBefore) {
                            const now = new Date();
                            const target = new Date(now.getFullYear(), now.getMonth(), now.getDate() - daysBefore);
                            return dateKey(target);
                        }
                        function recentDateKeys() {
                            const keys = [];
                            for(let offset = 0; offset < 7; offset += 1)keys.push(dateKeyBefore(offset));
                            return keys;
                        }
                        function emptyHours() {
                            const result = [];
                            for(let index = 0; index < 24; index += 1)result.push(0);
                            return result;
                        }
                        function cloneHours(source) {
                            const result = emptyHours();
                            if (!Array.isArray(source)) return result;
                            for(let index = 0; index < 24; index += 1){
                                const value = Number(source[index]);
                                result[index] = isFinite(value) && value >= 0 ? Math.round(value) : 0;
                            }
                            return result;
                        }
                        function createDemoHistoryRecord(key) {
                            const total = Number(DEMO_HISTORY_TOTALS[key]);
                            if (!isFinite(total) || total <= 0) return null;
                            const progress = [
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0.02,
                                0.07,
                                0.13,
                                0.19,
                                0.25,
                                0.31,
                                0.39,
                                0.46,
                                0.53,
                                0.61,
                                0.68,
                                0.75,
                                0.82,
                                0.88,
                                0.93,
                                0.97,
                                1,
                                1
                            ];
                            const hours = [];
                            for(let index = 0; index < 24; index += 1)hours.push(Math.round(total * progress[index]));
                            return {
                                dateKey: key,
                                steps: Math.round(total),
                                hours: hours,
                                goal: DEMO_HISTORY_GOALS[key] || DEFAULT_STEP_GOAL,
                                calorieGoal: DEMO_HISTORY_CALORIE_GOALS[key] || DEFAULT_CALORIE_GOAL,
                                durationGoal: DEMO_HISTORY_DURATION_GOALS[key] || DEFAULT_DURATION_GOAL
                            };
                        }
                        const tracker = {
                            started: false,
                            loaded: false,
                            loadingKey: "",
                            dateKey: "",
                            steps: 0,
                            goal: DEFAULT_STEP_GOAL,
                            calorieGoal: DEFAULT_CALORIE_GOAL,
                            durationGoal: DEFAULT_DURATION_GOAL,
                            hours: emptyHours(),
                            listeners: [],
                            dayClosedListeners: [],
                            sensorActive: false,
                            sensorEvents: 0,
                            isSimulator: false,
                            deviceChecked: false,
                            demoTimerId: null,
                            saveTimerId: null,
                            needsInitialSave: false,
                            lastMagnitude: null,
                            filteredMagnitude: null,
                            motionBaseline: 9.8,
                            abovePeak: false,
                            lastStepAt: 0,
                            start () {
                                if (this.started) return;
                                this.started = true;
                                this.ensureToday();
                                this.detectDevice();
                                this.startSensor();
                            },
                            stop () {
                                this.started = false;
                                this.stopDemo();
                                this.flushSave();
                                if (this.sensorActive) {
                                    try {
                                        _system2.default.unsubscribeAccelerometer();
                                    } catch (error) {
                                        console.log("step sensor unsubscribe failed", error);
                                    }
                                }
                                this.sensorActive = false;
                                this.resetMotionState();
                            },
                            onAppShow () {
                                const wasStarted = this.started;
                                this.start();
                                this.ensureToday();
                                if (!wasStarted) return;
                                if (this.sensorActive) {
                                    try {
                                        _system2.default.unsubscribeAccelerometer();
                                    } catch (error) {
                                        console.log("step sensor resume unsubscribe failed", error);
                                    }
                                }
                                this.sensorActive = false;
                                this.resetMotionState();
                                this.startSensor();
                                this.startDemoIfNeeded();
                            },
                            onAppHide () {
                                this.flushSave();
                            },
                            subscribe (callback) {
                                if (!callback) return;
                                let exists = false;
                                for(let index = 0; index < this.listeners.length; index += 1)if (this.listeners[index] === callback) exists = true;
                                if (!exists) this.listeners.push(callback);
                                this.start();
                                callback(this.snapshot());
                            },
                            unsubscribe (callback) {
                                const next = [];
                                for(let index = 0; index < this.listeners.length; index += 1)if (this.listeners[index] !== callback) next.push(this.listeners[index]);
                                this.listeners = next;
                            },
                            subscribeDayClosed (callback) {
                                if (!callback) return;
                                for(let index = 0; index < this.dayClosedListeners.length; index += 1)if (this.dayClosedListeners[index] === callback) return;
                                this.dayClosedListeners.push(callback);
                            },
                            unsubscribeDayClosed (callback) {
                                const next = [];
                                for(let index = 0; index < this.dayClosedListeners.length; index += 1)if (this.dayClosedListeners[index] !== callback) next.push(this.dayClosedListeners[index]);
                                this.dayClosedListeners = next;
                            },
                            notifyDayClosed (record) {
                                const listeners = this.dayClosedListeners.slice();
                                for(let index = 0; index < listeners.length; index += 1){
                                    try {
                                        listeners[index](record);
                                    } catch (error) {
                                        console.log("day closed listener failed", error);
                                    }
                                }
                            },
                            snapshot () {
                                this.ensureToday();
                                const now = new Date();
                                const currentHour = now.getHours();
                                const visibleHours = [];
                                for(let index = 0; index <= currentHour; index += 1)visibleHours.push(this.hours[index] || 0);
                                return {
                                    dateKey: this.dateKey,
                                    steps: this.steps,
                                    goal: this.goal,
                                    calorieGoal: this.calorieGoal,
                                    durationGoal: this.durationGoal,
                                    hours: this.hours.slice(),
                                    visibleHours: visibleHours,
                                    currentHour: currentHour,
                                    loaded: this.loaded,
                                    isSimulator: this.isSimulator,
                                    sensorActive: this.sensorActive
                                };
                            },
                            notify () {
                                const value = this.snapshot();
                                const listeners = this.listeners.slice();
                                for(let index = 0; index < listeners.length; index += 1){
                                    try {
                                        listeners[index](value);
                                    } catch (error) {
                                        console.log("step listener failed", error);
                                    }
                                }
                            },
                            ensureToday () {
                                const key = todayKey();
                                if (this.dateKey === key && (this.loaded || this.loadingKey === key)) return;
                                if (this.dateKey && this.loaded) {
                                    if (this.saveTimerId) {
                                        clearTimeout(this.saveTimerId);
                                        this.saveTimerId = null;
                                    }
                                    const closedRecord = {
                                        dateKey: this.dateKey,
                                        steps: this.steps,
                                        hours: cloneHours(this.hours),
                                        goal: this.goal,
                                        calorieGoal: this.calorieGoal,
                                        durationGoal: this.durationGoal
                                    };
                                    this.saveRecord(closedRecord.dateKey, closedRecord.steps, closedRecord.hours, closedRecord.goal, closedRecord.calorieGoal, closedRecord.durationGoal, ()=>this.notifyDayClosed(closedRecord));
                                }
                                this.dateKey = key;
                                this.steps = 0;
                                this.hours = emptyHours();
                                this.loaded = false;
                                this.deleteExpiredHistory();
                                this.loadDay(key);
                            },
                            loadRecentHistory (callback) {
                                const keys = recentDateKeys();
                                const rows = [];
                                let pending = keys.length;
                                const finishOne = ()=>{
                                    pending -= 1;
                                    if (pending <= 0 && callback) callback(rows);
                                };
                                for(let index = 0; index < keys.length; index += 1){
                                    const key = keys[index];
                                    rows.push({
                                        dateKey: key,
                                        steps: key === this.dateKey ? this.steps : 0,
                                        hours: key === this.dateKey ? cloneHours(this.hours) : emptyHours(),
                                        goal: key === this.dateKey ? this.goal : DEMO_HISTORY_GOALS[key] || DEFAULT_STEP_GOAL,
                                        calorieGoal: key === this.dateKey ? this.calorieGoal : DEMO_HISTORY_CALORIE_GOALS[key] || DEFAULT_CALORIE_GOAL,
                                        durationGoal: key === this.dateKey ? this.durationGoal : DEMO_HISTORY_DURATION_GOALS[key] || DEFAULT_DURATION_GOAL
                                    });
                                    if (key === this.dateKey) {
                                        finishOne();
                                        continue;
                                    }
                                    try {
                                        _system3.default.get({
                                            key: STORAGE_PREFIX + key,
                                            default: "",
                                            success: (value)=>{
                                                if (value) {
                                                    try {
                                                        const parsed = JSON.parse(value);
                                                        const storedSteps = Number(parsed.steps);
                                                        rows[index].steps = isFinite(storedSteps) && storedSteps >= 0 ? Math.round(storedSteps) : 0;
                                                        rows[index].hours = cloneHours(parsed.hours);
                                                        const storedGoal = Math.round(Number(parsed.goal) || 0);
                                                        rows[index].goal = storedGoal >= 1000 ? storedGoal : DEMO_HISTORY_GOALS[key] || DEFAULT_STEP_GOAL;
                                                        const storedCalorieGoal = Math.round(Number(parsed.calorieGoal) || 0);
                                                        const storedDurationGoal = Math.round(Number(parsed.durationGoal) || 0);
                                                        rows[index].calorieGoal = storedCalorieGoal >= 1 ? storedCalorieGoal : DEMO_HISTORY_CALORIE_GOALS[key] || DEFAULT_CALORIE_GOAL;
                                                        rows[index].durationGoal = storedDurationGoal >= 1 ? storedDurationGoal : DEMO_HISTORY_DURATION_GOALS[key] || DEFAULT_DURATION_GOAL;
                                                        if (storedGoal < 1000 || storedCalorieGoal < 1 || storedDurationGoal < 1) this.saveRecord(key, rows[index].steps, rows[index].hours, rows[index].goal, rows[index].calorieGoal, rows[index].durationGoal);
                                                    } catch (error) {
                                                        console.log("parse history steps failed", key, error);
                                                    }
                                                } else {
                                                    const demoRecord = createDemoHistoryRecord(key);
                                                    if (demoRecord) {
                                                        rows[index].steps = demoRecord.steps;
                                                        rows[index].hours = cloneHours(demoRecord.hours);
                                                        rows[index].goal = demoRecord.goal;
                                                        rows[index].calorieGoal = demoRecord.calorieGoal;
                                                        rows[index].durationGoal = demoRecord.durationGoal;
                                                        this.saveRecord(key, demoRecord.steps, demoRecord.hours, demoRecord.goal, demoRecord.calorieGoal, demoRecord.durationGoal);
                                                    }
                                                }
                                                finishOne();
                                            },
                                            fail: (data, code)=>{
                                                console.log("load history steps failed", key, code, data);
                                                finishOne();
                                            }
                                        });
                                    } catch (error) {
                                        console.log("history step storage unavailable", key, error);
                                        finishOne();
                                    }
                                }
                            },
                            deleteExpiredHistory () {
                                const expiredKey = dateKeyBefore(HISTORY_RETENTION_DAYS + 1);
                                try {
                                    _system3.default.delete({
                                        key: STORAGE_PREFIX + expiredKey,
                                        fail: (data, code)=>{
                                            console.log("delete expired steps failed", expiredKey, code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("delete expired steps unavailable", expiredKey, error);
                                }
                            },
                            loadDay (key) {
                                if (this.loadingKey === key) return;
                                this.loadingKey = key;
                                try {
                                    _system3.default.get({
                                        key: STORAGE_PREFIX + key,
                                        default: "",
                                        success: (value)=>{
                                            if (this.dateKey !== key) return;
                                            this.loadingKey = "";
                                            this.needsInitialSave = !value;
                                            if (value) {
                                                try {
                                                    const parsed = JSON.parse(value);
                                                    const storedSteps = Number(parsed.steps);
                                                    this.steps = isFinite(storedSteps) && storedSteps >= 0 ? Math.round(storedSteps) : 0;
                                                    this.hours = cloneHours(parsed.hours);
                                                } catch (error) {
                                                    console.log("parse daily steps failed", error);
                                                }
                                            }
                                            this.loadCurrentGoals(key, ()=>this.completeDayLoad(key));
                                        },
                                        fail: (data, code)=>{
                                            if (this.dateKey !== key) return;
                                            this.needsInitialSave = false;
                                            console.log("load daily steps failed", code, data);
                                            this.loadCurrentGoals(key, ()=>this.completeDayLoad(key));
                                        }
                                    });
                                } catch (error) {
                                    console.log("daily step storage unavailable", error);
                                    this.loadCurrentGoals(key, ()=>this.completeDayLoad(key));
                                }
                            },
                            loadCurrentGoals (key, callback) {
                                let pending = 3;
                                const finishOne = ()=>{
                                    pending -= 1;
                                    if (pending <= 0 && this.dateKey === key && callback) callback();
                                };
                                const readGoal = (storageKey, minimum, maximum, applyValue)=>{
                                    try {
                                        _system3.default.get({
                                            key: storageKey,
                                            default: "",
                                            success: (value)=>{
                                                if (this.dateKey === key) {
                                                    const storedGoal = Math.round(Number(value) || 0);
                                                    if (storedGoal >= minimum && storedGoal <= maximum) applyValue(storedGoal);
                                                }
                                                finishOne();
                                            },
                                            fail: (data, code)=>{
                                                console.log("load current goal failed", storageKey, code, data);
                                                finishOne();
                                            }
                                        });
                                    } catch (error) {
                                        console.log("current goal storage unavailable", storageKey, error);
                                        finishOne();
                                    }
                                };
                                readGoal(STEP_GOAL_STORAGE_KEY, 1000, 99999, (value)=>{
                                    this.goal = value;
                                });
                                readGoal(CALORIE_GOAL_STORAGE_KEY, 50, 9999, (value)=>{
                                    this.calorieGoal = value;
                                });
                                readGoal(DURATION_GOAL_STORAGE_KEY, 5, 1440, (value)=>{
                                    this.durationGoal = value;
                                });
                            },
                            completeDayLoad (key) {
                                if (this.dateKey !== key) return;
                                this.loadingKey = "";
                                this.loaded = true;
                                if (this.needsInitialSave) {
                                    this.needsInitialSave = false;
                                    this.save();
                                }
                                this.startDemoIfNeeded();
                                this.notify();
                            },
                            detectDevice () {
                                try {
                                    _system.default.getInfo({
                                        success: (info)=>{
                                            const signature = [
                                                info && info.brand,
                                                info && info.manufacturer,
                                                info && info.model,
                                                info && info.product
                                            ].join(" ").toLowerCase();
                                            this.isSimulator = signature.indexOf("virtual") >= 0 || signature.indexOf("emulator") >= 0 || signature.indexOf("goldfish") >= 0 || signature.indexOf("qemu") >= 0 || signature.indexOf("sdk") >= 0;
                                            this.deviceChecked = true;
                                            console.log("step device", signature, "simulator", this.isSimulator);
                                            this.startDemoIfNeeded();
                                            this.notify();
                                        },
                                        fail: (data, code)=>{
                                            this.deviceChecked = true;
                                            console.log("step device info failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    this.deviceChecked = true;
                                    console.log("step device info unavailable", error);
                                }
                            },
                            startSensor () {
                                if (this.sensorActive) return;
                                try {
                                    _system2.default.subscribeAccelerometer({
                                        interval: "normal",
                                        callback: (data)=>this.handleAcceleration(data),
                                        fail: (data, code)=>{
                                            this.sensorActive = false;
                                            console.log("step accelerometer failed", code, data);
                                        }
                                    });
                                    this.sensorActive = true;
                                } catch (error) {
                                    this.sensorActive = false;
                                    console.log("step accelerometer unavailable", error);
                                }
                            },
                            resetMotionState () {
                                this.lastMagnitude = null;
                                this.filteredMagnitude = null;
                                this.motionBaseline = 9.8;
                                this.abovePeak = false;
                                this.lastStepAt = 0;
                            },
                            handleAcceleration (data) {
                                if (!this.started || !data || this.isSimulator) return;
                                const x = Number(data.x);
                                const y = Number(data.y);
                                const z = Number(data.z);
                                if (!isFinite(x) || !isFinite(y) || !isFinite(z)) return;
                                this.sensorEvents += 1;
                                const magnitude = Math.sqrt(x * x + y * y + z * z);
                                if (null === this.filteredMagnitude) {
                                    this.filteredMagnitude = magnitude;
                                    this.motionBaseline = magnitude;
                                    this.lastMagnitude = magnitude;
                                    return;
                                }
                                this.filteredMagnitude = 0.72 * this.filteredMagnitude + 0.28 * magnitude;
                                this.motionBaseline = 0.985 * this.motionBaseline + 0.015 * this.filteredMagnitude;
                                const movement = this.filteredMagnitude - this.motionBaseline;
                                const now = Date.now();
                                const elapsed = now - this.lastStepAt;
                                if (movement > 1.05 && !this.abovePeak) {
                                    this.abovePeak = true;
                                    if (elapsed >= MIN_STEP_INTERVAL_MS && (0 === this.lastStepAt || elapsed <= MAX_STEP_INTERVAL_MS)) this.addSteps(1);
                                    else if (0 === this.lastStepAt) this.lastStepAt = now;
                                }
                                if (movement < 0.32) this.abovePeak = false;
                                this.lastMagnitude = magnitude;
                            },
                            startDemoIfNeeded () {
                                if (!ENABLE_SIMULATOR_STEP_GROWTH || !this.started || !this.loaded || !this.isSimulator || this.demoTimerId) return;
                                this.demoTimerId = setInterval(()=>{
                                    const now = new Date();
                                    const delta = 2 + (now.getSeconds() + now.getMinutes()) % 4;
                                    this.addSteps(delta);
                                }, DEMO_INTERVAL_MS);
                            },
                            stopDemo () {
                                if (!this.demoTimerId) return;
                                clearInterval(this.demoTimerId);
                                this.demoTimerId = null;
                            },
                            addSteps (count) {
                                this.ensureToday();
                                if (!this.loaded) return;
                                const delta = Math.max(1, Math.round(Number(count) || 1));
                                this.steps += delta;
                                const hour = new Date().getHours();
                                this.hours[hour] = this.steps;
                                this.lastStepAt = Date.now();
                                this.scheduleSave();
                                this.notify();
                            },
                            setGoal (value) {
                                const nextGoal = Math.max(1000, Math.round(Number(value) || DEFAULT_STEP_GOAL));
                                this.goal = nextGoal;
                                if (this.loaded) this.scheduleSave();
                                this.notify();
                            },
                            setMetricGoal (metric, value) {
                                if ("calories" === metric) this.calorieGoal = Math.max(1, Math.round(Number(value) || DEFAULT_CALORIE_GOAL));
                                else if ("duration" === metric) this.durationGoal = Math.max(1, Math.round(Number(value) || DEFAULT_DURATION_GOAL));
                                else this.goal = Math.max(1000, Math.round(Number(value) || DEFAULT_STEP_GOAL));
                                if (this.loaded) this.scheduleSave();
                                this.notify();
                            },
                            scheduleSave () {
                                if (this.saveTimerId) clearTimeout(this.saveTimerId);
                                this.saveTimerId = setTimeout(()=>{
                                    this.saveTimerId = null;
                                    this.save();
                                }, SAVE_DELAY_MS);
                            },
                            save () {
                                if (!this.loaded || !this.dateKey) return;
                                this.saveRecord(this.dateKey, this.steps, this.hours, this.goal, this.calorieGoal, this.durationGoal);
                            },
                            saveRecord (key, steps, hours, goal, calorieGoal, durationGoal, callback) {
                                if (!key) return;
                                try {
                                    _system3.default.set({
                                        key: STORAGE_PREFIX + key,
                                        value: JSON.stringify({
                                            dateKey: key,
                                            steps: Math.max(0, Math.round(Number(steps) || 0)),
                                            hours: cloneHours(hours),
                                            goal: Math.max(1000, Math.round(Number(goal) || DEFAULT_STEP_GOAL)),
                                            calorieGoal: Math.max(1, Math.round(Number(calorieGoal) || DEFAULT_CALORIE_GOAL)),
                                            durationGoal: Math.max(1, Math.round(Number(durationGoal) || DEFAULT_DURATION_GOAL))
                                        }),
                                        success: ()=>{
                                            if (callback) callback();
                                        },
                                        fail: (data, code)=>{
                                            console.log("save daily steps failed", code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save daily steps unavailable", error);
                                }
                            },
                            flushSave () {
                                if (this.saveTimerId) {
                                    clearTimeout(this.saveTimerId);
                                    this.saveTimerId = null;
                                }
                                this.save();
                            }
                        };
                        var _default = exports["default"] = tracker;
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
                                    "soft-veil"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                backgroundColor: "rgba(255, 255, 255, 0.1)"
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
                                top: "17px",
                                color: "#142a65",
                                fontSize: "38px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "stats-layer"
                                ]
                            ],
                            {
                                width: "432px",
                                height: "480px",
                                position: "absolute",
                                left: 0,
                                top: 0
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "stats-layer-hidden"
                                ]
                            ],
                            {
                                width: 0,
                                height: 0,
                                left: "-1000px",
                                top: "-1000px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "trend-card"
                                ]
                            ],
                            {
                                width: "384px",
                                height: "260px",
                                position: "absolute",
                                left: "24px",
                                top: "76px",
                                borderRadius: "30px",
                                backgroundColor: "rgba(255, 255, 255, 0.95)",
                                flexDirection: "column"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "trend-title"
                                ]
                            ],
                            {
                                width: "180px",
                                height: "42px",
                                position: "absolute",
                                left: "29px",
                                top: "13px",
                                color: "#142a65",
                                fontSize: "28px",
                                fontWeight: "bold"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-goal-label"
                                ]
                            ],
                            {
                                width: "166px",
                                height: "30px",
                                position: "absolute",
                                right: "22px",
                                top: "20px",
                                color: "#142a65",
                                fontSize: "17px",
                                fontWeight: "bold",
                                textAlign: "right"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "chart-scroll"
                                ]
                            ],
                            {
                                width: "338px",
                                height: "171px",
                                position: "absolute",
                                left: "23px",
                                top: "54px",
                                flexDirection: "row"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "chart-content"
                                ]
                            ],
                            {
                                width: "1125px",
                                height: "171px",
                                position: "relative",
                                flexShrink: 0
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "line-dot"
                                ]
                            ],
                            {
                                width: "7px",
                                height: "7px",
                                position: "absolute",
                                zIndex: 3,
                                borderRadius: "4px",
                                backgroundColor: "#347ff0"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "step-point"
                                ]
                            ],
                            {
                                width: "10px",
                                height: "10px",
                                position: "absolute",
                                zIndex: 4,
                                borderTopWidth: "2px",
                                borderRightWidth: "2px",
                                borderBottomWidth: "2px",
                                borderLeftWidth: "2px",
                                borderTopColor: "#347ff0",
                                borderRightColor: "#347ff0",
                                borderBottomColor: "#347ff0",
                                borderLeftColor: "#347ff0",
                                borderRadius: "50%",
                                backgroundColor: "#ffd24d"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "step-bubble"
                                ]
                            ],
                            {
                                width: "132px",
                                height: "34px",
                                position: "absolute",
                                zIndex: 30,
                                borderRadius: "17px",
                                backgroundColor: "#142a65",
                                justifyContent: "center",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "bubble-text"
                                ]
                            ],
                            {
                                width: "126px",
                                height: "30px",
                                color: "#ffffff",
                                fontSize: "16px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "grid-line"
                                ]
                            ],
                            {
                                width: "1125px",
                                height: "2px",
                                position: "absolute",
                                left: 0,
                                backgroundColor: "#d8e9fb"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "grid-one"
                                ]
                            ],
                            {
                                top: "16px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "grid-two"
                                ]
                            ],
                            {
                                top: "53px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "grid-three"
                                ]
                            ],
                            {
                                top: "90px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "grid-four"
                                ]
                            ],
                            {
                                top: "127px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-goal-line"
                                ]
                            ],
                            {
                                width: "1125px",
                                height: "4px",
                                position: "absolute",
                                left: 0,
                                zIndex: 2,
                                borderRadius: "2px",
                                backgroundColor: "#ffd020"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "hour-row"
                                ]
                            ],
                            {
                                width: "1125px",
                                height: "29px",
                                position: "absolute",
                                left: 0,
                                top: "141px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "hour-touch-row"
                                ]
                            ],
                            {
                                width: "1125px",
                                height: "171px",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                zIndex: 12
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "hour-touch"
                                ]
                            ],
                            {
                                width: "45px",
                                height: "171px",
                                flexShrink: 0
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "hour-label"
                                ]
                            ],
                            {
                                width: "45px",
                                height: "28px",
                                color: "#61708d",
                                fontSize: "17px",
                                textAlign: "center",
                                flexShrink: 0
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "hour-major"
                                ]
                            ],
                            {
                                color: "#142a65",
                                fontWeight: "bold"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "hour-minor"
                                ]
                            ],
                            {
                                color: "#8b98ae"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "scroll-hint"
                                ]
                            ],
                            {
                                width: "220px",
                                height: "25px",
                                position: "absolute",
                                left: "82px",
                                top: "229px",
                                color: "#8292aa",
                                fontSize: "16px",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "count-label"
                                ]
                            ],
                            {
                                width: "200px",
                                height: "38px",
                                position: "absolute",
                                left: "116px",
                                top: "348px",
                                color: "#142a65",
                                fontSize: "26px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "count-row"
                                ]
                            ],
                            {
                                width: "352px",
                                height: "91px",
                                position: "absolute",
                                left: "27px",
                                top: "379px",
                                alignItems: "flex-end",
                                justifyContent: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "count-value"
                                ]
                            ],
                            {
                                width: "252px",
                                height: "91px",
                                color: "#142a65",
                                fontSize: "72px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "count-unit"
                                ]
                            ],
                            {
                                width: "88px",
                                height: "60px",
                                marginBottom: "3px",
                                color: "#142a65",
                                fontSize: "32px",
                                fontWeight: "bold",
                                textAlign: "left"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-card"
                                ]
                            ],
                            {
                                width: "384px",
                                height: "374px",
                                position: "absolute",
                                left: "24px",
                                top: "76px",
                                paddingTop: "5px",
                                borderRadius: "30px",
                                backgroundColor: "#ffffff",
                                flexDirection: "column"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-row"
                                ]
                            ],
                            {
                                width: "344px",
                                height: "52px",
                                marginLeft: "20px",
                                position: "relative",
                                flexShrink: 0
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-dot"
                                ]
                            ],
                            {
                                width: "11px",
                                height: "11px",
                                position: "absolute",
                                left: "4px",
                                top: "20px",
                                borderRadius: "50%"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-dot-today"
                                ]
                            ],
                            {
                                backgroundColor: "#ffca18"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-dot-hidden"
                                ]
                            ],
                            {
                                backgroundColor: "transparent"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-date"
                                ]
                            ],
                            {
                                width: "132px",
                                height: "38px",
                                position: "absolute",
                                left: "28px",
                                top: "7px",
                                color: "#142a65",
                                fontSize: "24px",
                                fontWeight: "bold",
                                textAlign: "left"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-steps"
                                ]
                            ],
                            {
                                width: "112px",
                                height: "38px",
                                position: "absolute",
                                right: "48px",
                                top: "7px",
                                color: "#142a65",
                                fontSize: "23px",
                                fontWeight: "bold",
                                textAlign: "right"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-unit"
                                ]
                            ],
                            {
                                width: "48px",
                                height: "32px",
                                position: "absolute",
                                right: 0,
                                top: "11px",
                                color: "#142a65",
                                fontSize: "16px",
                                fontWeight: "bold",
                                textAlign: "left"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "history-separator"
                                ]
                            ],
                            {
                                width: "316px",
                                height: "1px",
                                position: "absolute",
                                left: "14px",
                                bottom: 0,
                                borderBottomWidth: "1px",
                                borderBottomStyle: "dashed",
                                borderBottomColor: "#b8d8f7"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-card"
                                ]
                            ],
                            {
                                width: "384px",
                                height: "326px",
                                position: "absolute",
                                left: "24px",
                                top: "76px",
                                borderRadius: "30px",
                                backgroundColor: "#ffffff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-label"
                                ]
                            ],
                            {
                                width: "180px",
                                height: "36px",
                                position: "absolute",
                                left: "102px",
                                top: "14px",
                                color: "#142a65",
                                fontSize: "25px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-target-row"
                                ]
                            ],
                            {
                                width: "300px",
                                height: "52px",
                                position: "absolute",
                                left: "42px",
                                top: "47px",
                                justifyContent: "center",
                                alignItems: "flex-end"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-target-value"
                                ]
                            ],
                            {
                                width: "210px",
                                height: "52px",
                                color: "#142a65",
                                fontSize: "44px",
                                fontWeight: "bold",
                                textAlign: "right"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-target-unit"
                                ]
                            ],
                            {
                                width: "66px",
                                height: "39px",
                                marginLeft: "8px",
                                color: "#142a65",
                                fontSize: "20px",
                                fontWeight: "bold",
                                textAlign: "left"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-ring"
                                ]
                            ],
                            {
                                width: "220px",
                                height: "220px",
                                position: "absolute",
                                left: "82px",
                                top: "87px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-dot"
                                ]
                            ],
                            {
                                width: "10px",
                                height: "10px",
                                position: "absolute",
                                borderRadius: "50%"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-dot-inactive"
                                ]
                            ],
                            {
                                backgroundColor: "#d8e8fb"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-dot-active"
                                ]
                            ],
                            {
                                backgroundColor: "#2d82f5"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-dot-end"
                                ]
                            ],
                            {
                                width: "14px",
                                height: "14px",
                                marginLeft: "-2px",
                                marginTop: "-2px",
                                borderTopWidth: "2px",
                                borderRightWidth: "2px",
                                borderBottomWidth: "2px",
                                borderLeftWidth: "2px",
                                borderTopColor: "#2d82f5",
                                borderRightColor: "#2d82f5",
                                borderBottomColor: "#2d82f5",
                                borderLeftColor: "#2d82f5",
                                backgroundColor: "#ffd020"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-percent"
                                ]
                            ],
                            {
                                width: "180px",
                                height: "78px",
                                position: "absolute",
                                left: "20px",
                                top: "72px",
                                color: "#142a65",
                                fontSize: "61px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-completed"
                                ]
                            ],
                            {
                                width: "280px",
                                height: "34px",
                                position: "absolute",
                                left: "52px",
                                bottom: "7px",
                                color: "#142a65",
                                fontSize: "22px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-edit-button"
                                ]
                            ],
                            {
                                width: "330px",
                                height: "58px",
                                position: "absolute",
                                left: "51px",
                                top: "411px",
                                borderRadius: "24px",
                                backgroundColor: "#287ff0",
                                justifyContent: "center",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-edit-text"
                                ]
                            ],
                            {
                                width: "280px",
                                height: "42px",
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
                                    "goal-editor-card"
                                ]
                            ],
                            {
                                width: "384px",
                                height: "394px",
                                position: "absolute",
                                left: "24px",
                                top: "76px",
                                borderRadius: "30px",
                                backgroundColor: "#ffffff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-input-label"
                                ]
                            ],
                            {
                                width: "140px",
                                height: "30px",
                                position: "absolute",
                                left: "122px",
                                top: "8px",
                                color: "#142a65",
                                fontSize: "21px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-input-display"
                                ]
                            ],
                            {
                                width: "310px",
                                height: "48px",
                                position: "absolute",
                                left: "37px",
                                top: "34px",
                                borderTopWidth: "2px",
                                borderRightWidth: "2px",
                                borderBottomWidth: "2px",
                                borderLeftWidth: "2px",
                                borderTopColor: "#9fc9f6",
                                borderRightColor: "#9fc9f6",
                                borderBottomColor: "#9fc9f6",
                                borderLeftColor: "#9fc9f6",
                                borderRadius: "15px",
                                backgroundColor: "#f6faff",
                                justifyContent: "center",
                                alignItems: "flex-end"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-input-value"
                                ]
                            ],
                            {
                                width: "214px",
                                height: "46px",
                                color: "#142a65",
                                fontSize: "37px",
                                fontWeight: "bold",
                                textAlign: "right"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-input-unit"
                                ]
                            ],
                            {
                                width: "58px",
                                height: "35px",
                                marginLeft: "7px",
                                color: "#142a65",
                                fontSize: "18px",
                                fontWeight: "bold",
                                textAlign: "left"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-input-hint"
                                ]
                            ],
                            {
                                width: "300px",
                                height: "24px",
                                position: "absolute",
                                left: "42px",
                                top: "82px",
                                color: "#71829e",
                                fontSize: "15px",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "keypad-row"
                                ]
                            ],
                            {
                                width: "316px",
                                height: "51px",
                                position: "absolute",
                                left: "34px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "keypad-row-one"
                                ]
                            ],
                            {
                                top: "108px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "keypad-row-two"
                                ]
                            ],
                            {
                                top: "164px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "keypad-row-three"
                                ]
                            ],
                            {
                                top: "220px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "keypad-row-four"
                                ]
                            ],
                            {
                                top: "276px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "keypad-key"
                                ]
                            ],
                            {
                                width: "92px",
                                height: "51px",
                                borderRadius: "15px",
                                backgroundColor: "#edf5ff",
                                justifyContent: "center",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "keypad-key-middle"
                                ]
                            ],
                            {
                                marginLeft: "20px",
                                marginRight: "20px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "keypad-key-action"
                                ]
                            ],
                            {
                                backgroundColor: "#e3effd"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "keypad-number"
                                ]
                            ],
                            {
                                width: "72px",
                                height: "46px",
                                color: "#142a65",
                                fontSize: "32px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "keypad-action-text"
                                ]
                            ],
                            {
                                width: "76px",
                                height: "40px",
                                color: "#315c97",
                                fontSize: "19px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-confirm-button"
                                ]
                            ],
                            {
                                width: "260px",
                                height: "48px",
                                position: "absolute",
                                left: "62px",
                                top: "337px",
                                borderRadius: "18px",
                                backgroundColor: "#287ff0",
                                justifyContent: "center",
                                alignItems: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "goal-confirm-text"
                                ]
                            ],
                            {
                                width: "210px",
                                height: "38px",
                                color: "#ffffff",
                                fontSize: "24px",
                                fontWeight: "bold",
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
                        var _system2 = _interopRequireDefault($app_require$1("@app-module/system.storage"));
                        var _customization = __webpack_require__("./src/common/customization.js");
                        var _stepTracker = _interopRequireDefault(__webpack_require__("./src/common/step-tracker.js"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const STANDBY_DELAY_MS = 60000;
                        const BUBBLE_HIDE_DELAY_MS = 2600;
                        const CHART_WIDTH = 1125;
                        const DEFAULT_STEP_GOAL = 8000;
                        const STEP_GOAL_STORAGE_KEY = "step_goal_v1";
                        const METRIC_TYPES = [
                            "steps",
                            "calories",
                            "duration"
                        ];
                        const METRIC_CONFIGS = {
                            steps: {
                                statsTitle: "步数统计",
                                historyTitle: "步数历史",
                                goalTitle: "步数目标",
                                currentLabel: "当前步数",
                                unit: "步",
                                editLabel: "修改目标步数",
                                inputLabel: "目标步数",
                                storageKey: STEP_GOAL_STORAGE_KEY,
                                defaultGoal: DEFAULT_STEP_GOAL,
                                minGoal: 1000,
                                maxGoal: 99999
                            },
                            calories: {
                                statsTitle: "卡路里统计",
                                historyTitle: "卡路里历史",
                                goalTitle: "卡路里目标",
                                currentLabel: "当前卡路里",
                                unit: "千卡",
                                editLabel: "修改卡路里目标",
                                inputLabel: "目标卡路里",
                                storageKey: "calorie_goal_v1",
                                defaultGoal: 400,
                                minGoal: 50,
                                maxGoal: 9999
                            },
                            duration: {
                                statsTitle: "运动时长统计",
                                historyTitle: "运动时长历史",
                                goalTitle: "运动时长目标",
                                currentLabel: "当前运动时长",
                                unit: "分钟",
                                editLabel: "修改时长目标",
                                inputLabel: "目标时长",
                                storageKey: "duration_goal_v1",
                                defaultGoal: 30,
                                minGoal: 5,
                                maxGoal: 1440
                            }
                        };
                        function metricConfig(type) {
                            return METRIC_CONFIGS[type] || METRIC_CONFIGS.steps;
                        }
                        function metricValue(type, steps) {
                            const safeSteps = Math.max(0, Number(steps) || 0);
                            if ("calories" === type) return Math.round(0.04 * safeSteps);
                            if ("duration" === type) return Math.floor(safeSteps / 100);
                            return Math.round(safeSteps);
                        }
                        function metricHours(type, source) {
                            const result = [];
                            if (!Array.isArray(source)) return result;
                            for(let index = 0; index < source.length; index += 1)result.push(metricValue(type, source[index]));
                            return result;
                        }
                        function metricGoal(type, source, fallback) {
                            const config = metricConfig(type);
                            let value = 0;
                            value = "calories" === type ? Number(source && source.calorieGoal) : "duration" === type ? Number(source && source.durationGoal) : Number(source && source.goal);
                            const candidate = Math.round(value || Number(fallback) || config.defaultGoal);
                            return Math.max(config.minGoal, Math.min(config.maxGoal, candidate));
                        }
                        function goalRangeText(type) {
                            const config = metricConfig(type);
                            return "请输入 " + formatNumber(config.minGoal) + "～" + formatNumber(config.maxGoal);
                        }
                        function convertMetricSnapshot(type, source, fallbackGoal) {
                            const raw = source || {};
                            const hours = metricHours(type, raw.hours);
                            const currentHour = Math.max(0, Math.min(23, Number(raw.currentHour) || 0));
                            return {
                                dateKey: raw.dateKey || "",
                                steps: metricValue(type, raw.steps),
                                goal: metricGoal(type, raw, fallbackGoal),
                                hours: hours,
                                currentHour: currentHour,
                                loaded: raw.loaded,
                                isSimulator: raw.isSimulator,
                                sensorActive: raw.sensorActive
                            };
                        }
                        function formatNumber(value) {
                            const source = String(Math.max(0, Math.round(Number(value) || 0)));
                            let result = "";
                            for(let index = 0; index < source.length; index += 1){
                                if (index > 0 && (source.length - index) % 3 === 0) result += ",";
                                result += source[index];
                            }
                            return result;
                        }
                        function formatCompactSteps(value) {
                            const steps = Math.max(0, Math.round(Number(value) || 0));
                            if (steps >= 10000) return (steps / 10000).toFixed(1) + "万";
                            return formatNumber(steps);
                        }
                        function formatHistoryDate(key) {
                            const parts = String(key || "").split("-");
                            if (3 !== parts.length) return key || "";
                            return padDatePart(parts[1]) + "月" + padDatePart(parts[2]) + "日";
                        }
                        function padDatePart(value) {
                            const number = Math.max(0, Math.round(Number(value) || 0));
                            return number < 10 ? "0" + number : "" + number;
                        }
                        function makeHourLabels() {
                            const result = [];
                            for(let hour = 0; hour <= 24; hour += 1)result.push({
                                id: "hour-" + hour,
                                label: hour < 10 ? "0" + hour : "" + hour,
                                className: hour % 3 === 0 ? "hour-label hour-major" : "hour-label hour-minor"
                            });
                            return result;
                        }
                        function chartMaximum(values) {
                            let highest = 0;
                            for(let index = 0; index < values.length; index += 1)highest = Math.max(highest, Number(values[index]) || 0);
                            if (highest <= 100) return 100;
                            if (highest <= 500) return 500;
                            if (highest <= 1000) return 1000;
                            if (highest <= 2000) return 2000;
                            if (highest <= 5000) return 5000;
                            if (highest <= 10000) return 10000;
                            if (highest <= 20000) return 20000;
                            if (highest <= 50000) return 50000;
                            return 50000 * Math.ceil(highest / 50000);
                        }
                        function finalizeHistoryHours(source, totalSteps) {
                            const result = [
                                0
                            ];
                            let runningTotal = 0;
                            for(let boundary = 1; boundary <= 24; boundary += 1){
                                const sourceIndex = boundary - 1;
                                const value = source && source.length > sourceIndex ? Math.max(0, Math.round(Number(source[sourceIndex]) || 0)) : 0;
                                runningTotal = Math.max(runningTotal, value);
                                result.push(runningTotal);
                            }
                            const finalTotal = Math.max(0, Math.round(Number(totalSteps) || 0));
                            if (result[24] < finalTotal) result[24] = finalTotal;
                            return result;
                        }
                        function createLiveStepSeries(snapshot) {
                            const currentHour = Math.max(0, Math.min(23, Number(snapshot.currentHour) || 0));
                            const source = snapshot.hours || [];
                            const boundaryValues = [
                                0
                            ];
                            const series = [
                                {
                                    id: "boundary-0",
                                    position: 0,
                                    value: 0,
                                    showPoint: true
                                }
                            ];
                            let runningTotal = 0;
                            for(let boundary = 1; boundary <= currentHour; boundary += 1){
                                const sourceValue = Math.max(0, Math.round(Number(source[boundary - 1]) || 0));
                                runningTotal = Math.max(runningTotal, sourceValue);
                                boundaryValues.push(runningTotal);
                                series.push({
                                    id: "boundary-" + boundary,
                                    position: boundary,
                                    value: runningTotal,
                                    showPoint: true
                                });
                            }
                            const now = new Date();
                            const minuteProgress = (60 * now.getMinutes() + now.getSeconds()) / 3600;
                            const livePosition = Math.min(24, currentHour + minuteProgress);
                            const liveSteps = Math.max(runningTotal, Math.round(Number(snapshot.steps) || 0));
                            if (livePosition > currentHour + 0.001) series.push({
                                id: "live-" + currentHour,
                                position: livePosition,
                                value: liveSteps,
                                showPoint: false
                            });
                            while(boundaryValues.length < 25)boundaryValues.push(0);
                            return {
                                series: series,
                                boundaryValues: boundaryValues
                            };
                        }
                        function createGoalDots(percent) {
                            const dots = [];
                            const count = 44;
                            const safePercent = Math.max(0, Math.min(100, Number(percent) || 0));
                            const activeCount = Math.round(safePercent / 100 * count);
                            for(let index = 0; index < count; index += 1){
                                const angle = (-90 + 360 * index / count) * Math.PI / 180;
                                const left = Math.round(110 + 94 * Math.cos(angle) - 5);
                                const top = Math.round(110 + 94 * Math.sin(angle) - 5);
                                let className = index < activeCount ? "goal-dot goal-dot-active" : "goal-dot goal-dot-inactive";
                                if (activeCount > 0 && index === activeCount - 1) className = "goal-dot goal-dot-end";
                                dots.push({
                                    id: "goal-dot-" + index,
                                    left: left,
                                    top: top,
                                    className: className
                                });
                            }
                            return dots;
                        }
                        var _default = exports.default = {
                            private: {
                                backgroundImage: "/common/backgrounds/dark-purple.png",
                                titleClass: "page-title text-light",
                                countLabelClass: "count-label text-light",
                                countClass: "count-value text-light",
                                unitClass: "count-unit text-light",
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
                                chartTouching: false,
                                returnTimerId: null,
                                leaving: false,
                                specialSportMode: false,
                                metricType: "steps",
                                metricStatsTitle: "步数统计",
                                metricHistoryTitle: "步数历史",
                                metricGoalTitle: "步数目标",
                                metricCurrentLabel: "当前步数",
                                metricUnit: "步",
                                metricEditLabel: "修改目标步数",
                                metricInputLabel: "目标步数",
                                metricGoalMin: 1000,
                                metricGoalMax: 99999,
                                metricStorageKey: STEP_GOAL_STORAGE_KEY,
                                metricSwitchAt: 0,
                                formattedSteps: "0",
                                statsVisible: true,
                                statsLayerClass: "stats-layer",
                                todayTitleVisible: true,
                                historyVisible: false,
                                historyDetailVisible: false,
                                goalVisible: false,
                                goalEditorVisible: false,
                                stepGoal: DEFAULT_STEP_GOAL,
                                stepGoalText: formatNumber(DEFAULT_STEP_GOAL),
                                goalInput: String(DEFAULT_STEP_GOAL),
                                goalInputText: formatNumber(DEFAULT_STEP_GOAL),
                                goalInputHint: "请输入 1,000～99,999",
                                goalInputFresh: true,
                                goalCompletedText: "0",
                                goalPercentText: "0%",
                                goalDots: createGoalDots(0),
                                currentStepValue: 0,
                                selectedHistoryDate: "",
                                selectedHistoryKey: "",
                                selectedHistoryGoal: DEFAULT_STEP_GOAL,
                                selectedHistoryGoalText: formatNumber(DEFAULT_STEP_GOAL),
                                historyGoalLineVisible: false,
                                historyGoalLineTop: 0,
                                historyRows: [],
                                historyDateKey: "",
                                historyLoadSerial: 0,
                                historyDateTimerId: null,
                                hourLabels: makeHourLabels(),
                                chartReady: true,
                                chartMax: 100,
                                chartOptions: {
                                    xAxis: {
                                        min: 0,
                                        max: 23,
                                        axisTick: 12,
                                        display: false
                                    },
                                    yAxis: {
                                        min: 0,
                                        max: 100,
                                        axisTick: 4,
                                        display: false
                                    },
                                    series: {
                                        lineStyle: {
                                            width: "5px",
                                            smooth: true
                                        }
                                    }
                                },
                                chartDatasets: [
                                    {
                                        strokeColor: "#347ff0",
                                        fillColor: "#cfe7ff",
                                        gradient: true,
                                        data: [
                                            0
                                        ]
                                    }
                                ],
                                stepPoints: [],
                                lineDots: [],
                                currentHour: 0,
                                latestHours: [],
                                bubbleVisible: false,
                                bubbleHour: -1,
                                bubbleLeft: 0,
                                bubbleTop: 0,
                                bubbleText: "",
                                bubbleTimerId: null,
                                stepListener: null,
                                chartRefreshTimerId: null
                            },
                            onInit () {
                                this.stepListener = (snapshot)=>this.applyStepSnapshot(snapshot);
                                this.specialSportMode = this.$app.$def.isSpecialSportMode();
                                const initialMetric = this.specialSportMode ? this.$app.$def.getSpecialSportMetric() : "steps";
                                this.applyMetricConfig(initialMetric);
                                this.loadStepGoal();
                                this.syncCustomization();
                            },
                            onReady () {
                                this.scrollToCurrentHour();
                            },
                            onShow () {
                                this.$app.$def.ensureWakeableScreen();
                                this.leaving = false;
                                this.screenStandby = false;
                                this.ignoreTouchEnd = false;
                                this.chartTouching = false;
                                this.syncCustomization();
                                _stepTracker.default.subscribe(this.stepListener);
                                this.startHistoryDateTimer();
                                this.startStandbyTimer();
                                setTimeout(()=>this.scrollToCurrentHour(), 360);
                            },
                            onHide () {
                                _stepTracker.default.unsubscribe(this.stepListener);
                                this.hideHourBubble();
                                this.stopHistoryDateTimer();
                                this.stopStandbyTimer();
                                this.cancelReturn();
                                this.cancelChartRefresh();
                            },
                            onDestroy () {
                                _stepTracker.default.unsubscribe(this.stepListener);
                                this.hideHourBubble();
                                this.stopHistoryDateTimer();
                                this.stopStandbyTimer();
                                this.cancelReturn();
                                this.cancelChartRefresh();
                            },
                            applyMetricConfig (type) {
                                const config = metricConfig(type);
                                this.metricType = type;
                                this.metricStatsTitle = config.statsTitle;
                                this.metricHistoryTitle = config.historyTitle;
                                this.metricGoalTitle = config.goalTitle;
                                this.metricCurrentLabel = config.currentLabel;
                                this.metricUnit = config.unit;
                                this.metricEditLabel = config.editLabel;
                                this.metricInputLabel = config.inputLabel;
                                this.metricGoalMin = config.minGoal;
                                this.metricGoalMax = config.maxGoal;
                                this.metricStorageKey = config.storageKey;
                                this.stepGoal = config.defaultGoal;
                                this.stepGoalText = formatNumber(config.defaultGoal);
                                this.goalInputHint = goalRangeText(type);
                            },
                            applyStepSnapshot (snapshot) {
                                if (!snapshot) return;
                                snapshot = convertMetricSnapshot(this.metricType, snapshot, this.stepGoal);
                                this.currentStepValue = Math.max(0, Math.round(Number(snapshot.steps) || 0));
                                this.updateGoalProgress();
                                if (this.historyDateKey && this.historyDateKey !== snapshot.dateKey) this.loadHistoryRows();
                                else this.updateTodayHistoryRow(snapshot);
                                if (this.historyDetailVisible) return;
                                if (this.goalVisible || this.goalEditorVisible) return;
                                this.formattedSteps = formatNumber(snapshot.steps);
                                const liveChart = createLiveStepSeries(snapshot);
                                const chartValues = [];
                                for(let index = 0; index < liveChart.series.length; index += 1)chartValues.push(liveChart.series[index].value);
                                const nextMax = chartMaximum(chartValues);
                                this.currentHour = snapshot.currentHour;
                                this.latestHours = liveChart.boundaryValues;
                                this.chartMax = nextMax;
                                this.rebuildStepPoints(liveChart.series, nextMax);
                                if (this.bubbleVisible && this.bubbleHour <= this.currentHour) this.showHourBubble(this.bubbleHour, false);
                            },
                            loadHistoryRows () {
                                const loadSerial = this.historyLoadSerial + 1;
                                this.historyLoadSerial = loadSerial;
                                _stepTracker.default.loadRecentHistory((records)=>{
                                    if (loadSerial !== this.historyLoadSerial || !records) return;
                                    const snapshot = convertMetricSnapshot(this.metricType, _stepTracker.default.snapshot(), this.stepGoal);
                                    const rows = [];
                                    for(let index = 0; index < records.length; index += 1){
                                        const record = records[index];
                                        const isToday = record.dateKey === snapshot.dateKey;
                                        const steps = isToday ? snapshot.steps : metricValue(this.metricType, record.steps);
                                        const recordHours = isToday ? snapshot.hours ? snapshot.hours.slice() : [] : metricHours(this.metricType, record.hours);
                                        rows.push({
                                            dateKey: record.dateKey,
                                            dateLabel: formatHistoryDate(record.dateKey),
                                            steps: Math.max(0, Math.round(Number(steps) || 0)),
                                            stepsText: formatNumber(steps),
                                            goal: Math.max(this.metricGoalMin, Math.round(Number(isToday ? snapshot.goal : metricGoal(this.metricType, record, this.stepGoal)) || this.stepGoal)),
                                            hours: recordHours,
                                            isToday: isToday,
                                            dotClass: isToday ? "history-dot history-dot-today" : "history-dot history-dot-hidden",
                                            showSeparator: index < records.length - 1
                                        });
                                    }
                                    this.historyDateKey = snapshot.dateKey;
                                    this.historyRows = rows;
                                });
                            },
                            updateTodayHistoryRow (snapshot) {
                                if (!snapshot || !this.historyRows.length) return;
                                const rows = [];
                                let updated = false;
                                for(let index = 0; index < this.historyRows.length; index += 1){
                                    const source = this.historyRows[index];
                                    if (source.dateKey === snapshot.dateKey) {
                                        const steps = Math.max(0, Math.round(Number(snapshot.steps) || 0));
                                        rows.push({
                                            dateKey: source.dateKey,
                                            dateLabel: source.dateLabel,
                                            steps: steps,
                                            stepsText: formatNumber(steps),
                                            goal: Math.max(this.metricGoalMin, Math.round(Number(snapshot.goal) || Number(this.stepGoal))),
                                            hours: snapshot.hours ? snapshot.hours.slice() : [],
                                            isToday: true,
                                            dotClass: "history-dot history-dot-today",
                                            showSeparator: source.showSeparator
                                        });
                                        updated = true;
                                    } else rows.push(source);
                                }
                                if (updated) this.historyRows = rows;
                            },
                            startHistoryDateTimer () {
                                this.stopHistoryDateTimer();
                                this.historyDateTimerId = setInterval(()=>{
                                    const snapshot = _stepTracker.default.snapshot();
                                    this.applyStepSnapshot(snapshot);
                                }, 60000);
                            },
                            stopHistoryDateTimer () {
                                if (!this.historyDateTimerId) return;
                                clearInterval(this.historyDateTimerId);
                                this.historyDateTimerId = null;
                            },
                            loadStepGoal () {
                                const config = metricConfig(this.metricType);
                                const requestedType = this.metricType;
                                try {
                                    _system2.default.get({
                                        key: config.storageKey,
                                        default: "",
                                        success: (value)=>{
                                            if (requestedType !== this.metricType) return;
                                            const storedGoal = Math.round(Number(value) || 0);
                                            this.stepGoal = storedGoal >= config.minGoal && storedGoal <= config.maxGoal ? storedGoal : config.defaultGoal;
                                            this.stepGoalText = formatNumber(this.stepGoal);
                                            _stepTracker.default.setMetricGoal(this.metricType, this.stepGoal);
                                            this.updateGoalProgress();
                                            if (storedGoal < config.minGoal || storedGoal > config.maxGoal) this.persistStepGoal();
                                        },
                                        fail: (data, code)=>{
                                            if (requestedType !== this.metricType) return;
                                            console.log("load metric goal failed", this.metricType, code, data);
                                            this.stepGoal = config.defaultGoal;
                                            this.stepGoalText = formatNumber(this.stepGoal);
                                            _stepTracker.default.setMetricGoal(this.metricType, this.stepGoal);
                                            this.updateGoalProgress();
                                        }
                                    });
                                } catch (error) {
                                    console.log("metric goal storage unavailable", this.metricType, error);
                                    this.stepGoal = config.defaultGoal;
                                    this.stepGoalText = formatNumber(this.stepGoal);
                                    _stepTracker.default.setMetricGoal(this.metricType, this.stepGoal);
                                    this.updateGoalProgress();
                                }
                            },
                            persistStepGoal () {
                                try {
                                    _system2.default.set({
                                        key: this.metricStorageKey,
                                        value: String(this.stepGoal),
                                        fail: (data, code)=>{
                                            console.log("save metric goal failed", this.metricType, code, data);
                                        }
                                    });
                                } catch (error) {
                                    console.log("save metric goal unavailable", this.metricType, error);
                                }
                            },
                            updateGoalProgress () {
                                const goal = Math.max(1, Number(this.stepGoal) || metricConfig(this.metricType).defaultGoal);
                                const completed = Math.max(0, Number(this.currentStepValue) || 0);
                                const percent = Math.max(0, Math.min(100, Math.floor(completed / goal * 100)));
                                this.stepGoalText = formatNumber(goal);
                                this.goalCompletedText = formatNumber(completed);
                                this.goalPercentText = percent + "%";
                                if (this.goalVisible) this.goalDots = createGoalDots(percent);
                            },
                            requestGoalEdit () {
                                if (!this.goalVisible) return;
                                this.goalInput = String(this.stepGoal);
                                this.goalInputText = formatNumber(this.stepGoal);
                                this.goalInputHint = goalRangeText(this.metricType);
                                this.goalInputFresh = true;
                                this.goalVisible = false;
                                this.goalEditorVisible = true;
                                this.historyGoalLineVisible = false;
                                this.setStatsVisible(false);
                                this.todayTitleVisible = false;
                                this.registerActivity();
                            },
                            pressGoalDigit (digit) {
                                const value = String(Math.max(0, Math.min(9, Math.round(Number(digit) || 0))));
                                let next = this.goalInput;
                                if (this.goalInputFresh || "0" === next) next = value;
                                else if (next.length < String(this.metricGoalMax).length) next += value;
                                this.goalInputFresh = false;
                                this.goalInput = next;
                                this.goalInputText = formatNumber(next);
                                this.goalInputHint = goalRangeText(this.metricType);
                                this.registerActivity();
                            },
                            clearGoalInput () {
                                this.goalInputFresh = false;
                                this.goalInput = "0";
                                this.goalInputText = "0";
                                this.goalInputHint = goalRangeText(this.metricType);
                                this.registerActivity();
                            },
                            deleteGoalDigit () {
                                this.goalInputFresh = false;
                                const current = String(this.goalInput || "");
                                const next = current.length > 1 ? current.slice(0, current.length - 1) : "0";
                                this.goalInput = next;
                                this.goalInputText = formatNumber(next);
                                this.goalInputHint = goalRangeText(this.metricType);
                                this.registerActivity();
                            },
                            confirmGoalInput () {
                                const nextGoal = Math.round(Number(this.goalInput) || 0);
                                if (nextGoal < this.metricGoalMin || nextGoal > this.metricGoalMax) {
                                    this.goalInputHint = "目标需在 " + formatNumber(this.metricGoalMin) + "～" + formatNumber(this.metricGoalMax) + " 之间";
                                    this.registerActivity();
                                    return;
                                }
                                this.stepGoal = nextGoal;
                                this.stepGoalText = formatNumber(nextGoal);
                                _stepTracker.default.setMetricGoal(this.metricType, nextGoal);
                                this.persistStepGoal();
                                this.goalEditorVisible = false;
                                this.goalVisible = true;
                                this.updateGoalProgress();
                                this.registerActivity();
                            },
                            closeGoalEditor () {
                                if (!this.goalEditorVisible) return;
                                this.goalEditorVisible = false;
                                this.goalVisible = true;
                                this.goalInputHint = goalRangeText(this.metricType);
                                this.updateGoalProgress();
                                this.registerActivity();
                            },
                            showHistory () {
                                if (this.historyVisible) return;
                                this.hideHourBubble();
                                this.historyGoalLineVisible = false;
                                this.setStatsVisible(false);
                                this.todayTitleVisible = false;
                                this.goalVisible = false;
                                this.goalEditorVisible = false;
                                this.historyDetailVisible = false;
                                this.historyVisible = true;
                                this.loadHistoryRows();
                                this.registerActivity();
                            },
                            hideHistory () {
                                if (!this.historyVisible) return;
                                this.historyVisible = false;
                                this.historyGoalLineVisible = false;
                                this.historyDetailVisible = false;
                                this.goalEditorVisible = false;
                                this.setStatsVisible(true);
                                this.todayTitleVisible = true;
                                this.selectedHistoryDate = "";
                                this.selectedHistoryKey = "";
                                this.applyStepSnapshot(_stepTracker.default.snapshot());
                                this.registerActivity();
                            },
                            openHistoricalDay (item) {
                                if (!item || item.isToday) return;
                                const hours = finalizeHistoryHours(item.hours, item.steps);
                                const historyGoal = Math.max(this.metricGoalMin, Math.round(Number(item.goal) || this.stepGoal));
                                const scaleValues = hours.slice();
                                scaleValues.push(historyGoal);
                                const maximum = chartMaximum(scaleValues);
                                this.hideHourBubble();
                                this.selectedHistoryDate = item.dateLabel;
                                this.selectedHistoryKey = item.dateKey;
                                this.selectedHistoryGoal = historyGoal;
                                this.selectedHistoryGoalText = formatNumber(historyGoal);
                                this.historyGoalLineTop = Math.max(11, Math.min(127, Math.round(127 - historyGoal / maximum * 112)));
                                this.historyGoalLineVisible = true;
                                this.historyVisible = false;
                                this.historyDetailVisible = true;
                                this.goalVisible = false;
                                this.goalEditorVisible = false;
                                this.setStatsVisible(true);
                                this.todayTitleVisible = false;
                                this.formattedSteps = formatNumber(item.steps);
                                this.currentHour = 24;
                                this.latestHours = hours;
                                this.chartMax = maximum;
                                this.rebuildStepPoints(hours, maximum);
                                this.registerActivity();
                                setTimeout(()=>this.scrollToHour(24), 120);
                            },
                            closeHistoricalDay () {
                                if (!this.historyDetailVisible) return;
                                this.hideHourBubble();
                                this.historyDetailVisible = false;
                                this.historyVisible = true;
                                this.goalVisible = false;
                                this.goalEditorVisible = false;
                                this.setStatsVisible(false);
                                this.todayTitleVisible = false;
                                this.selectedHistoryDate = "";
                                this.selectedHistoryKey = "";
                                this.historyGoalLineVisible = false;
                                this.loadHistoryRows();
                                this.registerActivity();
                            },
                            showGoal () {
                                if (!this.historyVisible || this.goalVisible) return;
                                this.hideHourBubble();
                                this.historyGoalLineVisible = false;
                                this.historyVisible = false;
                                this.historyDetailVisible = false;
                                this.goalEditorVisible = false;
                                this.goalVisible = true;
                                this.setStatsVisible(false);
                                this.todayTitleVisible = false;
                                this.updateGoalProgress();
                                this.registerActivity();
                            },
                            closeGoal () {
                                if (!this.goalVisible) return;
                                this.goalVisible = false;
                                this.goalEditorVisible = false;
                                this.historyVisible = true;
                                this.historyDetailVisible = false;
                                this.setStatsVisible(false);
                                this.todayTitleVisible = false;
                                this.loadHistoryRows();
                                this.registerActivity();
                            },
                            rebuildStepPoints (values, maximum) {
                                const points = [];
                                const pathPoints = [];
                                const dots = [];
                                const safeMax = Math.max(1, maximum);
                                for(let index = 0; index < values.length; index += 1){
                                    const source = values[index];
                                    const isSeriesPoint = source && "object" == typeof source;
                                    const value = Math.max(0, Number(isSeriesPoint ? source.value : source) || 0);
                                    const position = isSeriesPoint ? Math.max(0, Math.min(24, Number(source.position) || 0)) : index;
                                    const top = Math.round(122 - value / safeMax * 112);
                                    const point = {
                                        id: isSeriesPoint ? source.id : "point-" + index,
                                        hour: position,
                                        value: value,
                                        left: Math.round(45 * position + 18),
                                        top: Math.max(6, Math.min(122, top))
                                    };
                                    pathPoints.push(point);
                                    if (!isSeriesPoint || false !== source.showPoint) points.push(point);
                                }
                                for(let index = 0; index < pathPoints.length - 1; index += 1){
                                    const start = pathPoints[index];
                                    const end = pathPoints[index + 1];
                                    const startX = start.left + 5;
                                    const startY = start.top + 5;
                                    const endX = end.left + 5;
                                    const endY = end.top + 5;
                                    const deltaX = endX - startX;
                                    const deltaY = endY - startY;
                                    const distance = Math.sqrt(deltaX * deltaX + deltaY * deltaY);
                                    const dotCount = Math.max(1, Math.ceil(distance / 4));
                                    for(let dotIndex = 0; dotIndex <= dotCount; dotIndex += 1){
                                        const progress = dotIndex / dotCount;
                                        dots.push({
                                            id: "line-" + index + "-" + dotIndex,
                                            left: Math.round(startX + deltaX * progress - 3),
                                            top: Math.round(startY + deltaY * progress - 3)
                                        });
                                    }
                                }
                                this.stepPoints = points;
                                this.lineDots = dots;
                            },
                            selectHour (item) {
                                if (!item) return;
                                const hour = Number(String(item.id || "").replace("hour-", ""));
                                if (!isFinite(hour) || hour < 0 || hour > this.currentHour) return;
                                this.showHourBubble(hour, true);
                                this.registerActivity();
                            },
                            showHourBubble (hour, restartTimer) {
                                const value = Math.max(0, Number(this.latestHours[hour]) || 0);
                                const safeMax = Math.max(1, this.chartMax);
                                const pointTop = Math.round(122 - value / safeMax * 112);
                                this.bubbleHour = hour;
                                this.bubbleText = (hour < 10 ? "0" + hour : "" + hour) + ":00  " + formatCompactSteps(value) + this.metricUnit;
                                this.bubbleLeft = Math.max(0, Math.min(CHART_WIDTH - 132, 45 * hour - 41));
                                this.bubbleTop = Math.max(0, Math.min(100, pointTop - 39));
                                this.bubbleVisible = true;
                                if (restartTimer) this.scheduleBubbleHide();
                            },
                            scheduleBubbleHide () {
                                this.cancelBubbleHide();
                                this.bubbleTimerId = setTimeout(()=>{
                                    this.bubbleTimerId = null;
                                    this.bubbleVisible = false;
                                    this.bubbleHour = -1;
                                }, BUBBLE_HIDE_DELAY_MS);
                            },
                            cancelBubbleHide () {
                                if (!this.bubbleTimerId) return;
                                clearTimeout(this.bubbleTimerId);
                                this.bubbleTimerId = null;
                            },
                            hideHourBubble () {
                                this.cancelBubbleHide();
                                this.bubbleVisible = false;
                                this.bubbleHour = -1;
                            },
                            recreateChart (maximum) {
                                this.chartMax = maximum;
                                this.chartReady = false;
                                this.chartOptions = {
                                    xAxis: {
                                        min: 0,
                                        max: 23,
                                        axisTick: 12,
                                        display: false
                                    },
                                    yAxis: {
                                        min: 0,
                                        max: maximum,
                                        axisTick: 4,
                                        display: false
                                    },
                                    series: {
                                        lineStyle: {
                                            width: "5px",
                                            smooth: true
                                        }
                                    }
                                };
                                this.cancelChartRefresh();
                                this.chartRefreshTimerId = setTimeout(()=>{
                                    this.chartRefreshTimerId = null;
                                    this.chartReady = true;
                                }, 40);
                            },
                            cancelChartRefresh () {
                                if (!this.chartRefreshTimerId) return;
                                clearTimeout(this.chartRefreshTimerId);
                                this.chartRefreshTimerId = null;
                            },
                            scrollToCurrentHour () {
                                this.scrollToHour(this.currentHour);
                            },
                            scrollToHour (hour) {
                                const chartScroll = this.$element("hourChartScroll");
                                if (!chartScroll) return;
                                const target = Math.max(0, Math.min(CHART_WIDTH - 338, 45 * hour - 270));
                                try {
                                    chartScroll.scrollTo({
                                        left: target,
                                        behavior: "instant"
                                    });
                                } catch (error) {
                                    console.log("step chart initial scroll failed", error);
                                }
                            },
                            syncCustomization () {
                                const customization = this.$app.$def.getCustomization();
                                const background = (0, _customization.getBackground)(customization.backgroundId);
                                this.backgroundImage = background.src;
                                const foreground = "text-" + background.foreground;
                                this.titleClass = "page-title " + foreground;
                                this.countLabelClass = "count-label " + foreground;
                                this.countClass = "count-value " + foreground;
                                this.unitClass = "count-unit " + foreground;
                            },
                            startCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("stepsCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.start();
                                } catch (error) {
                                    console.log("steps cat animator start failed", error);
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
                            pauseCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("stepsCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.pause();
                                } catch (error) {
                                    console.log("steps cat animator pause failed", error);
                                }
                            },
                            stopCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("stepsCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.stop();
                                } catch (error) {
                                    console.log("steps cat animator stop failed", error);
                                }
                            },
                            startCatHealthCheck () {
                                if (this.catHealthTimerId || this.screenStandby) return;
                                this.catHealthTimerId = setInterval(()=>{
                                    const animator = this.$element("stepsCatAnimator");
                                    if (!this.catAnimatorReady || !animator) return;
                                    try {
                                        const state = animator.getState();
                                        if ("paused" === state) animator.resume();
                                        if ("stopped" === state) animator.start();
                                    } catch (error) {
                                        console.log("steps cat animator health check failed", error);
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
                            },
                            wakeScreen () {
                                if (!this.screenStandby) return void this.registerActivity();
                                this.screenStandby = false;
                                this.ignoreTouchEnd = true;
                                this.leaving = false;
                                this.syncCustomization();
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
                            onChartTouchStart () {
                                this.chartTouching = true;
                                this.registerActivity();
                            },
                            onChartTouchEnd () {
                                setTimeout(()=>{
                                    this.chartTouching = false;
                                }, 80);
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
                                if (this.specialSportMode) {
                                    if (this.chartTouching) return;
                                    const specialHorizontal = Math.abs(deltaX) > 32 && Math.abs(deltaX) > 1.02 * Math.abs(deltaY);
                                    if (specialHorizontal && this.canSwitchMetric()) this.switchSpecialSportPage(deltaX < 0 ? 1 : -1);
                                    return;
                                }
                                const vertical = Math.abs(deltaY) > 45 && Math.abs(deltaY) > 1.08 * Math.abs(deltaX);
                                if (vertical) {
                                    if (this.canSwitchMetric()) this.switchMetric(deltaY < 0 ? 1 : -1);
                                    return;
                                }
                                if (this.chartTouching) return;
                                const horizontal = Math.abs(deltaX) > 45 && Math.abs(deltaX) > 1.08 * Math.abs(deltaY);
                                if (!horizontal) return;
                                if (deltaX > 0 && this.canSwitchMetric()) return void this.openSportModeEntry();
                                if (deltaX < 0 && !this.historyVisible && !this.historyDetailVisible && !this.goalVisible && !this.goalEditorVisible) return void this.showHistory();
                                if (deltaX < 0 && this.historyVisible) return void this.showGoal();
                                if (deltaX > 0 && this.goalVisible) return void this.closeGoal();
                                if (deltaX > 0 && this.goalEditorVisible) return void this.closeGoalEditor();
                                if (deltaX > 0 && this.historyDetailVisible) return void this.closeHistoricalDay();
                                if (deltaX > 0 && this.historyVisible) this.hideHistory();
                            },
                            handleSwipe (event) {
                                if (this.screenStandby) return void this.wakeScreen();
                                this.registerActivity();
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if (this.specialSportMode) {
                                    if (this.chartTouching) return;
                                    if ("left" === direction && this.canSwitchMetric()) this.switchSpecialSportPage(1);
                                    if ("right" === direction && this.canSwitchMetric()) this.switchSpecialSportPage(-1);
                                    return;
                                }
                                if ("up" === direction) {
                                    if (this.canSwitchMetric()) this.switchMetric(1);
                                    return;
                                }
                                if ("down" === direction) {
                                    if (this.canSwitchMetric()) this.switchMetric(-1);
                                    return;
                                }
                                if (this.chartTouching) return;
                                if ("right" === direction && this.canSwitchMetric()) return void this.openSportModeEntry();
                                if ("left" === direction && !this.historyVisible && !this.historyDetailVisible && !this.goalVisible && !this.goalEditorVisible) return void this.showHistory();
                                if ("left" === direction && this.historyVisible) return void this.showGoal();
                                if ("right" === direction && this.goalVisible) return void this.closeGoal();
                                if ("right" === direction && this.goalEditorVisible) return void this.closeGoalEditor();
                                if ("right" === direction && this.historyDetailVisible) return void this.closeHistoricalDay();
                                if ("right" === direction && this.historyVisible) this.hideHistory();
                            },
                            canSwitchMetric () {
                                return this.statsVisible && this.todayTitleVisible && !this.historyVisible && !this.historyDetailVisible && !this.goalVisible && !this.goalEditorVisible;
                            },
                            openSportModeEntry () {
                                if (this.leaving || this.screenStandby) return;
                                this.leaving = true;
                                try {
                                    _system.default.replace({
                                        uri: "/pages/sportmode"
                                    });
                                } catch (error) {
                                    this.leaving = false;
                                    console.log("open sport mode entry failed", error);
                                }
                            },
                            queueReturn () {
                                this.cancelReturn();
                                this.returnTimerId = setTimeout(()=>{
                                    this.returnTimerId = null;
                                    try {
                                        _system.default.back();
                                    } catch (error) {
                                        this.leaving = false;
                                        console.log("close step statistics failed", error);
                                    }
                                }, 120);
                            },
                            cancelReturn () {
                                if (!this.returnTimerId) return;
                                clearTimeout(this.returnTimerId);
                                this.returnTimerId = null;
                            },
                            resetMetricView () {
                                this.hideHourBubble();
                                this.historyLoadSerial += 1;
                                this.historyVisible = false;
                                this.historyDetailVisible = false;
                                this.goalVisible = false;
                                this.goalEditorVisible = false;
                                this.historyGoalLineVisible = false;
                                this.setStatsVisible(true);
                                this.todayTitleVisible = true;
                                this.selectedHistoryDate = "";
                                this.selectedHistoryKey = "";
                            },
                            setStatsVisible (visible) {
                                this.statsVisible = !!visible;
                                this.statsLayerClass = visible ? "stats-layer" : "stats-layer stats-layer-hidden";
                            },
                            switchMetric (offset) {
                                const now = Date.now();
                                if (now - this.metricSwitchAt < 180) return;
                                this.metricSwitchAt = now;
                                const currentIndex = METRIC_TYPES.indexOf(this.metricType);
                                const nextIndex = currentIndex + offset;
                                if (nextIndex < 0) return void this.returnToSport();
                                if (nextIndex >= METRIC_TYPES.length) return void this.registerActivity();
                                this.resetMetricView();
                                this.applyMetricConfig(METRIC_TYPES[nextIndex]);
                                this.loadStepGoal();
                                this.applyStepSnapshot(_stepTracker.default.snapshot());
                                setTimeout(()=>this.scrollToCurrentHour(), 120);
                                this.registerActivity();
                            },
                            switchSpecialSportPage (offset) {
                                const now = Date.now();
                                if (now - this.metricSwitchAt < 180 || this.leaving) return;
                                this.metricSwitchAt = now;
                                const currentIndex = METRIC_TYPES.indexOf(this.metricType);
                                const nextIndex = currentIndex + offset;
                                if (nextIndex < 0) {
                                    this.leaving = true;
                                    try {
                                        _system.default.replace({
                                            uri: "/pages/index"
                                        });
                                    } catch (error) {
                                        this.leaving = false;
                                        console.log("return special sport home failed", error);
                                    }
                                    return;
                                }
                                if (nextIndex >= METRIC_TYPES.length) {
                                    this.leaving = true;
                                    try {
                                        _system.default.replace({
                                            uri: "/pages/sportheartrate"
                                        });
                                    } catch (error) {
                                        this.leaving = false;
                                        console.log("open special sport heart rate failed", error);
                                    }
                                    return;
                                }
                                const nextMetric = METRIC_TYPES[nextIndex];
                                this.$app.$def.setSpecialSportMetric(nextMetric);
                                this.resetMetricView();
                                this.applyMetricConfig(nextMetric);
                                this.loadStepGoal();
                                this.applyStepSnapshot(_stepTracker.default.snapshot());
                                setTimeout(()=>this.scrollToCurrentHour(), 120);
                                this.registerActivity();
                            },
                            returnToSport () {
                                if (this.leaving) return;
                                this.leaving = true;
                                this.queueReturn();
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
                                                "soft-veil"
                                            ]
                                        }
                                    }, []),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.todayTitleVisible;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: function() {
                                                        const $classValue$ = _vm_.titleClass;
                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                        return $classValue$;
                                                    },
                                                    value: function() {
                                                        return _vm_.metricStatsTitle;
                                                    }
                                                }
                                            }, [])
                                        ];
                                    }),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.historyVisible;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: function() {
                                                        const $classValue$ = _vm_.titleClass;
                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                        return $classValue$;
                                                    },
                                                    value: function() {
                                                        return _vm_.metricHistoryTitle;
                                                    }
                                                }
                                            }, [])
                                        ];
                                    }),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.historyDetailVisible;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: function() {
                                                        const $classValue$ = _vm_.titleClass;
                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                        return $classValue$;
                                                    },
                                                    value: function() {
                                                        return _vm_.selectedHistoryDate;
                                                    }
                                                }
                                            }, [])
                                        ];
                                    }),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.goalVisible;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: function() {
                                                        const $classValue$ = _vm_.titleClass;
                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                        return $classValue$;
                                                    },
                                                    value: function() {
                                                        return _vm_.metricGoalTitle;
                                                    }
                                                }
                                            }, [])
                                        ];
                                    }),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.goalEditorVisible;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: function() {
                                                        const $classValue$ = _vm_.titleClass;
                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                        return $classValue$;
                                                    },
                                                    value: "修改目标"
                                                }
                                            }, [])
                                        ];
                                    }),
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: function() {
                                                const $classValue$ = _vm_.statsLayerClass;
                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                return $classValue$;
                                            }
                                        }
                                    }, [
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "trend-card"
                                                ]
                                            }
                                        }, [
                                            aiot.__ci__({
                                                __vm__: _vm_,
                                                __opts__: {
                                                    shown: function() {
                                                        return _vm_.statsVisible;
                                                    }
                                                }
                                            }, function() {
                                                return [
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "trend-title"
                                                            ],
                                                            value: "今日趋势"
                                                        }
                                                    }, [])
                                                ];
                                            }),
                                            aiot.__ci__({
                                                __vm__: _vm_,
                                                __opts__: {
                                                    shown: function() {
                                                        return _vm_.historyDetailVisible;
                                                    }
                                                }
                                            }, function() {
                                                return [
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "history-goal-label"
                                                            ],
                                                            value: function() {
                                                                return "目标：" + _vm_.selectedHistoryGoalText + _vm_.metricUnit;
                                                            }
                                                        }
                                                    }, [])
                                                ];
                                            }),
                                            aiot.__ce__("scroll", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    id: "hourChartScroll",
                                                    classList: [
                                                        "chart-scroll"
                                                    ],
                                                    scrollX: "true",
                                                    scrollY: "false",
                                                    bounces: "false",
                                                    events: {
                                                        touchstart: function(evt) {
                                                            return _vm_.onChartTouchStart(evt);
                                                        },
                                                        touchend: function(evt) {
                                                            return _vm_.onChartTouchEnd(evt);
                                                        }
                                                    }
                                                }
                                            }, [
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "chart-content"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "grid-line",
                                                                "grid-one"
                                                            ]
                                                        }
                                                    }, []),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "grid-line",
                                                                "grid-two"
                                                            ]
                                                        }
                                                    }, []),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "grid-line",
                                                                "grid-three"
                                                            ]
                                                        }
                                                    }, []),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "grid-line",
                                                                "grid-four"
                                                            ]
                                                        }
                                                    }, []),
                                                    aiot.__ci__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            shown: function() {
                                                                return _vm_.historyGoalLineVisible;
                                                            }
                                                        }
                                                    }, function() {
                                                        return [
                                                            aiot.__ce__("div", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "history-goal-line"
                                                                    ],
                                                                    style: function() {
                                                                        return __webpack_require__.g.$translateStyle$("top: " + _vm_.historyGoalLineTop + "px;");
                                                                    }
                                                                }
                                                            }, [])
                                                        ];
                                                    }),
                                                    aiot.__cf__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            exp: function() {
                                                                return {
                                                                    __list__: _vm_.lineDots,
                                                                    __tid__: "id"
                                                                };
                                                            },
                                                            key: "$idx",
                                                            value: "$item"
                                                        }
                                                    }, function($idx, $item) {
                                                        return [
                                                            aiot.__ce__("div", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "line-dot"
                                                                    ],
                                                                    style: function() {
                                                                        return __webpack_require__.g.$translateStyle$("left: " + $item.left + "px; top: " + $item.top + "px;");
                                                                    }
                                                                }
                                                            }, [])
                                                        ];
                                                    }),
                                                    aiot.__cf__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            exp: function() {
                                                                return {
                                                                    __list__: _vm_.stepPoints,
                                                                    __tid__: "id"
                                                                };
                                                            },
                                                            key: "$idx",
                                                            value: "$item"
                                                        }
                                                    }, function($idx, $item) {
                                                        return [
                                                            aiot.__ce__("div", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "step-point"
                                                                    ],
                                                                    style: function() {
                                                                        return __webpack_require__.g.$translateStyle$("left: " + $item.left + "px; top: " + $item.top + "px;");
                                                                    }
                                                                }
                                                            }, [])
                                                        ];
                                                    }),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "hour-row"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__cf__({
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                exp: function() {
                                                                    return {
                                                                        __list__: _vm_.hourLabels,
                                                                        __tid__: "id"
                                                                    };
                                                                },
                                                                key: "$idx",
                                                                value: "$item"
                                                            }
                                                        }, function($idx, $item) {
                                                            return [
                                                                aiot.__ce__("text", {
                                                                    __vm__: _vm_,
                                                                    __opts__: {
                                                                        classList: function() {
                                                                            const $classValue$ = $item.className;
                                                                            if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                            return $classValue$;
                                                                        },
                                                                        value: function() {
                                                                            return $item.label;
                                                                        }
                                                                    }
                                                                }, [])
                                                            ];
                                                        })
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "hour-touch-row"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__cf__({
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                exp: function() {
                                                                    return {
                                                                        __list__: _vm_.hourLabels,
                                                                        __tid__: "id"
                                                                    };
                                                                },
                                                                key: "$idx",
                                                                value: "$item"
                                                            }
                                                        }, function($idx, $item) {
                                                            return [
                                                                aiot.__ce__("div", {
                                                                    __vm__: _vm_,
                                                                    __opts__: {
                                                                        classList: [
                                                                            "hour-touch"
                                                                        ],
                                                                        events: {
                                                                            click: function(evt) {
                                                                                return _vm_.selectHour($item, evt);
                                                                            }
                                                                        }
                                                                    }
                                                                }, [])
                                                            ];
                                                        })
                                                    ]),
                                                    aiot.__ci__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            shown: function() {
                                                                return _vm_.bubbleVisible;
                                                            }
                                                        }
                                                    }, function() {
                                                        return [
                                                            aiot.__ce__("div", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "step-bubble"
                                                                    ],
                                                                    style: function() {
                                                                        return __webpack_require__.g.$translateStyle$("left: " + _vm_.bubbleLeft + "px; top: " + _vm_.bubbleTop + "px;");
                                                                    }
                                                                }
                                                            }, [
                                                                aiot.__ce__("text", {
                                                                    __vm__: _vm_,
                                                                    __opts__: {
                                                                        classList: [
                                                                            "bubble-text"
                                                                        ],
                                                                        value: function() {
                                                                            return _vm_.bubbleText;
                                                                        }
                                                                    }
                                                                }, [])
                                                            ])
                                                        ];
                                                    })
                                                ])
                                            ]),
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "scroll-hint"
                                                    ],
                                                    value: "左右滑动查看全天"
                                                }
                                            }, [])
                                        ]),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: function() {
                                                    const $classValue$ = _vm_.countLabelClass;
                                                    if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                    return $classValue$;
                                                },
                                                value: function() {
                                                    return _vm_.metricCurrentLabel;
                                                }
                                            }
                                        }, []),
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "count-row"
                                                ]
                                            }
                                        }, [
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: function() {
                                                        const $classValue$ = _vm_.countClass;
                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                        return $classValue$;
                                                    },
                                                    value: function() {
                                                        return _vm_.formattedSteps;
                                                    }
                                                }
                                            }, []),
                                            aiot.__ce__("text", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: function() {
                                                        const $classValue$ = _vm_.unitClass;
                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                        return $classValue$;
                                                    },
                                                    value: function() {
                                                        return _vm_.metricUnit;
                                                    }
                                                }
                                            }, [])
                                        ])
                                    ]),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.historyVisible;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "history-card"
                                                    ]
                                                }
                                            }, [
                                                aiot.__cf__({
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        exp: function() {
                                                            return {
                                                                __list__: _vm_.historyRows,
                                                                __tid__: "dateKey"
                                                            };
                                                        },
                                                        key: "$idx",
                                                        value: "$item"
                                                    }
                                                }, function($idx, $item) {
                                                    return [
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "history-row"
                                                                ],
                                                                events: {
                                                                    click: function(evt) {
                                                                        return _vm_.openHistoricalDay($item, evt);
                                                                    }
                                                                }
                                                            }
                                                        }, [
                                                            aiot.__ce__("div", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: function() {
                                                                        const $classValue$ = $item.dotClass;
                                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                        return $classValue$;
                                                                    }
                                                                }
                                                            }, []),
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "history-date"
                                                                    ],
                                                                    value: function() {
                                                                        return $item.dateLabel;
                                                                    }
                                                                }
                                                            }, []),
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "history-steps"
                                                                    ],
                                                                    value: function() {
                                                                        return $item.stepsText;
                                                                    }
                                                                }
                                                            }, []),
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "history-unit"
                                                                    ],
                                                                    value: function() {
                                                                        return _vm_.metricUnit;
                                                                    }
                                                                }
                                                            }, []),
                                                            aiot.__ci__({
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    shown: function() {
                                                                        return $item.showSeparator;
                                                                    }
                                                                }
                                                            }, function() {
                                                                return [
                                                                    aiot.__ce__("div", {
                                                                        __vm__: _vm_,
                                                                        __opts__: {
                                                                            classList: [
                                                                                "history-separator"
                                                                            ]
                                                                        }
                                                                    }, [])
                                                                ];
                                                            })
                                                        ])
                                                    ];
                                                })
                                            ])
                                        ];
                                    }),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.goalVisible;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "goal-card"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "goal-label"
                                                        ],
                                                        value: "今日目标"
                                                    }
                                                }, []),
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "goal-target-row"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "goal-target-value"
                                                            ],
                                                            value: function() {
                                                                return _vm_.stepGoalText;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "goal-target-unit"
                                                            ],
                                                            value: function() {
                                                                return _vm_.metricUnit;
                                                            }
                                                        }
                                                    }, [])
                                                ]),
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "goal-ring"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__cf__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            exp: function() {
                                                                return {
                                                                    __list__: _vm_.goalDots,
                                                                    __tid__: "id"
                                                                };
                                                            },
                                                            key: "$idx",
                                                            value: "$item"
                                                        }
                                                    }, function($idx, $item) {
                                                        return [
                                                            aiot.__ce__("div", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: function() {
                                                                        const $classValue$ = $item.className;
                                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                        return $classValue$;
                                                                    },
                                                                    style: function() {
                                                                        return __webpack_require__.g.$translateStyle$("left: " + $item.left + "px; top: " + $item.top + "px;");
                                                                    }
                                                                }
                                                            }, [])
                                                        ];
                                                    }),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "goal-percent"
                                                            ],
                                                            value: function() {
                                                                return _vm_.goalPercentText;
                                                            }
                                                        }
                                                    }, [])
                                                ]),
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "goal-completed"
                                                        ],
                                                        value: function() {
                                                            return "已完成 " + _vm_.goalCompletedText + " " + _vm_.metricUnit;
                                                        }
                                                    }
                                                }, [])
                                            ])
                                        ];
                                    }),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.goalVisible;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "goal-edit-button"
                                                    ],
                                                    events: {
                                                        click: function(evt) {
                                                            return _vm_.requestGoalEdit(evt);
                                                        }
                                                    }
                                                }
                                            }, [
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "goal-edit-text"
                                                        ],
                                                        value: function() {
                                                            return _vm_.metricEditLabel;
                                                        }
                                                    }
                                                }, [])
                                            ])
                                        ];
                                    }),
                                    aiot.__ci__({
                                        __vm__: _vm_,
                                        __opts__: {
                                            shown: function() {
                                                return _vm_.goalEditorVisible;
                                            }
                                        }
                                    }, function() {
                                        return [
                                            aiot.__ce__("div", {
                                                __vm__: _vm_,
                                                __opts__: {
                                                    classList: [
                                                        "goal-editor-card"
                                                    ]
                                                }
                                            }, [
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "goal-input-label"
                                                        ],
                                                        value: function() {
                                                            return _vm_.metricInputLabel;
                                                        }
                                                    }
                                                }, []),
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "goal-input-display"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "goal-input-value"
                                                            ],
                                                            value: function() {
                                                                return _vm_.goalInputText;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "goal-input-unit"
                                                            ],
                                                            value: function() {
                                                                return _vm_.metricUnit;
                                                            }
                                                        }
                                                    }, [])
                                                ]),
                                                aiot.__ce__("text", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "goal-input-hint"
                                                        ],
                                                        value: function() {
                                                            return _vm_.goalInputHint;
                                                        }
                                                    }
                                                }, []),
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "keypad-row",
                                                            "keypad-row-one"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.pressGoalDigit(1, evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-number"
                                                                ],
                                                                value: "1"
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key",
                                                                "keypad-key-middle"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.pressGoalDigit(2, evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-number"
                                                                ],
                                                                value: "2"
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.pressGoalDigit(3, evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-number"
                                                                ],
                                                                value: "3"
                                                            }
                                                        }, [])
                                                    ])
                                                ]),
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "keypad-row",
                                                            "keypad-row-two"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.pressGoalDigit(4, evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-number"
                                                                ],
                                                                value: "4"
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key",
                                                                "keypad-key-middle"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.pressGoalDigit(5, evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-number"
                                                                ],
                                                                value: "5"
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.pressGoalDigit(6, evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-number"
                                                                ],
                                                                value: "6"
                                                            }
                                                        }, [])
                                                    ])
                                                ]),
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "keypad-row",
                                                            "keypad-row-three"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.pressGoalDigit(7, evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-number"
                                                                ],
                                                                value: "7"
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key",
                                                                "keypad-key-middle"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.pressGoalDigit(8, evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-number"
                                                                ],
                                                                value: "8"
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.pressGoalDigit(9, evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-number"
                                                                ],
                                                                value: "9"
                                                            }
                                                        }, [])
                                                    ])
                                                ]),
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "keypad-row",
                                                            "keypad-row-four"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key",
                                                                "keypad-key-action"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.clearGoalInput(evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-action-text"
                                                                ],
                                                                value: "清空"
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key",
                                                                "keypad-key-middle"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.pressGoalDigit(0, evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-number"
                                                                ],
                                                                value: "0"
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "keypad-key",
                                                                "keypad-key-action"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.deleteGoalDigit(evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "keypad-action-text"
                                                                ],
                                                                value: "删除"
                                                            }
                                                        }, [])
                                                    ])
                                                ]),
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "goal-confirm-button"
                                                        ],
                                                        events: {
                                                            click: function(evt) {
                                                                return _vm_.confirmGoalInput(evt);
                                                            }
                                                        }
                                                    }
                                                }, [
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "goal-confirm-text"
                                                            ],
                                                            value: "确认修改"
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
