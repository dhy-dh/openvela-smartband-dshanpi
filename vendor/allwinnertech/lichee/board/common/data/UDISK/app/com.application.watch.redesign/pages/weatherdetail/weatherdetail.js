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
                    "./src/common/weather-cities.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports.DEFAULT_CITY_ID = void 0;
                        exports.getCities = getCities;
                        exports.getCity = getCity;
                        exports.getDefaultCustomCities = getDefaultCustomCities;
                        const DEFAULT_CITY_ID = exports.DEFAULT_CITY_ID = "beijing";
                        const CITIES = [
                            {
                                id: "beijing",
                                locationId: "101010100",
                                name: "北京",
                                detailName: "北京市",
                                administrativeArea: "北京市",
                                country: "中国",
                                isBase: true
                            },
                            {
                                id: "shanghai",
                                locationId: "101020100",
                                name: "上海",
                                detailName: "上海市",
                                administrativeArea: "上海市",
                                country: "中国",
                                isBase: true
                            },
                            {
                                id: "guangzhou",
                                locationId: "101280101",
                                name: "广州",
                                detailName: "广州市",
                                administrativeArea: "广东省",
                                country: "中国",
                                isBase: true
                            },
                            {
                                id: "shenzhen",
                                locationId: "101280601",
                                name: "深圳",
                                detailName: "深圳市",
                                administrativeArea: "广东省",
                                country: "中国",
                                isBase: true
                            }
                        ];
                        const DEFAULT_CUSTOM_CITIES = [
                            {
                                id: "wuhan",
                                locationId: "101200101",
                                name: "武汉",
                                detailName: "武汉市",
                                administrativeArea: "湖北省",
                                country: "中国",
                                isBase: false,
                                weatherKey: "wuhan"
                            }
                        ];
                        function copyCity(city) {
                            return {
                                id: city.id,
                                locationId: city.locationId,
                                administrativeCode: city.administrativeCode || "",
                                name: city.name,
                                detailName: city.detailName,
                                administrativeArea: city.administrativeArea,
                                country: city.country,
                                isBase: city.isBase,
                                weatherKey: city.weatherKey || city.id,
                                weatherLocationName: city.weatherLocationName || "",
                                weatherAdmName: city.weatherAdmName || "",
                                weatherProvinceName: city.weatherProvinceName || "",
                                weatherCityName: city.weatherCityName || "",
                                isCurrentLocation: !!city.isCurrentLocation,
                                locationReady: false !== city.locationReady,
                                longitude: city.longitude,
                                latitude: city.latitude,
                                deletable: false !== city.deletable
                            };
                        }
                        function getCities() {
                            return CITIES.map(copyCity);
                        }
                        function getDefaultCustomCities() {
                            return DEFAULT_CUSTOM_CITIES.map(copyCity);
                        }
                        function getCity(id, customCities) {
                            for(let index = 0; index < CITIES.length; index += 1)if (CITIES[index].id === id) return copyCity(CITIES[index]);
                            const additions = customCities || [];
                            for(let index = 0; index < additions.length; index += 1)if (additions[index].id === id) return copyCity(additions[index]);
                            return copyCity(CITIES[0]);
                        }
                    },
                    "./src/common/weather-service.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports.fetchLiveWeather = fetchLiveWeather;
                        exports.fetchLiveWeatherByCoordinates = fetchLiveWeatherByCoordinates;
                        var _system = _interopRequireDefault($app_require$1("@app-module/system.fetch"));
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const WEATHER_API_URL = "http://10.0.2.2:8790/api/weather";
                        const WEATHER_ICON_ROOT = "/common/weather-icons/";
                        const SUPPORTED_ICON_CODES = "|100|101|102|103|104|150|151|152|153|154|300|301|302|303|304|305|306|307|308|309|310|311|312|313|314|315|316|317|318|350|351|399|400|401|402|403|404|405|406|407|408|409|410|456|457|499|500|501|502|503|504|507|508|509|510|511|512|513|514|515|900|901|999|";
                        function safeIconCode(value) {
                            const code = String(value || "");
                            return SUPPORTED_ICON_CODES.indexOf("|" + code + "|") >= 0 ? code : "999";
                        }
                        function splitAdministrativeArea(value) {
                            const parts = String(value || "").split("·");
                            const result = [];
                            for(let index = 0; index < parts.length; index += 1){
                                const part = parts[index].trim();
                                if (part) result.push(part);
                            }
                            return result;
                        }
                        function buildWeatherUrl(city) {
                            const areas = splitAdministrativeArea(city.administrativeArea);
                            const isDistrict = areas.length > 1;
                            const locationName = city.weatherLocationName || city.detailName || city.name;
                            const admName = city.weatherAdmName || (isDistrict ? areas[0] : areas[0] || city.administrativeArea);
                            const locationId = isDistrict ? "" : city.locationId || "";
                            return WEATHER_API_URL + "?locationId=" + encodeURIComponent(locationId) + "&location=" + encodeURIComponent(locationName) + "&adm=" + encodeURIComponent(admName);
                        }
                        function normalizeForecastItem(item) {
                            const icon = safeIconCode(item.icon);
                            return {
                                weekday: item.weekday || "--",
                                text: item.text || "--",
                                icon: icon,
                                iconSrc: WEATHER_ICON_ROOT + icon + ".png",
                                min: item.min || "--",
                                max: item.max || "--",
                                temperature: (item.min || "--") + "°~" + (item.max || "--") + "°"
                            };
                        }
                        function normalizeWeather(data) {
                            if (!data || "200" !== data.code || !data.live || !data.now) throw new Error("Invalid live weather response");
                            const forecast = [];
                            let highest = -100;
                            let lowest = 100;
                            const source = data.forecast || [];
                            for(let index = 0; index < source.length; index += 1){
                                const item = normalizeForecastItem(source[index]);
                                forecast.push(item);
                                highest = Math.max(highest, Number(item.max));
                                lowest = Math.min(lowest, Number(item.min));
                            }
                            while(forecast.length < 3)forecast.push(normalizeForecastItem({}));
                            const icon = safeIconCode(data.now.icon);
                            return {
                                live: true,
                                source: data.source || "QWeather",
                                updatedAt: data.updatedAt || "",
                                observedAt: data.observedAt || "",
                                location: data.location || {},
                                now: {
                                    temp: data.now.temp,
                                    feelsLike: data.now.feelsLike,
                                    humidity: data.now.humidity,
                                    visibility: data.now.visibility,
                                    text: data.now.text,
                                    icon: icon,
                                    iconSrc: WEATHER_ICON_ROOT + icon + ".png"
                                },
                                forecast: forecast.slice(0, 3),
                                highest: -100 === highest ? "--" : "" + highest,
                                lowest: 100 === lowest ? "--" : "" + lowest
                            };
                        }
                        function fetchLiveWeather(city, success, fail) {
                            _system.default.fetch({
                                url: buildWeatherUrl(city),
                                method: "GET",
                                responseType: "json",
                                success: (response)=>{
                                    try {
                                        if (!response || 200 !== response.code) throw new Error("Weather server HTTP error");
                                        success(normalizeWeather(response.data));
                                    } catch (error) {
                                        if (fail) fail(error);
                                    }
                                },
                                fail: (data, code)=>{
                                    if (fail) fail({
                                        data: data,
                                        code: code
                                    });
                                }
                            });
                        }
                        function fetchLiveWeatherByCoordinates(longitude, latitude, success, fail) {
                            const numericLongitude = Number(longitude);
                            const numericLatitude = Number(latitude);
                            if (!isFinite(numericLongitude) || !isFinite(numericLatitude)) {
                                if (fail) fail(new Error("Invalid location coordinates"));
                                return;
                            }
                            const coordinateText = numericLongitude.toFixed(5) + "," + numericLatitude.toFixed(5);
                            fetchLiveWeather({
                                id: "current-location",
                                locationId: "",
                                name: coordinateText,
                                detailName: coordinateText,
                                administrativeArea: "",
                                weatherLocationName: coordinateText,
                                weatherAdmName: ""
                            }, success, fail);
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
                                position: "absolute",
                                left: 0,
                                top: 0,
                                overflow: "hidden",
                                backgroundColor: "#0f0d2c"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "viewport-lock"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                overflow: "hidden"
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
                                height: "528px",
                                position: "absolute",
                                left: 0,
                                top: "-7px",
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
                                top: 0
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "veil-light"
                                ]
                            ],
                            {
                                backgroundColor: "rgba(255, 255, 255, 0.02)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "veil-dark"
                                ]
                            ],
                            {
                                backgroundColor: "rgba(5, 9, 31, 0.08)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "city-title"
                                ]
                            ],
                            {
                                width: "250px",
                                height: "50px",
                                position: "absolute",
                                left: "91px",
                                top: "28px",
                                fontSize: "38px",
                                fontWeight: "bold",
                                lineHeight: "47px",
                                textAlign: "center",
                                zIndex: 10
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "city-meta"
                                ]
                            ],
                            {
                                width: "250px",
                                height: "30px",
                                position: "absolute",
                                left: "91px",
                                top: "70px",
                                fontSize: "18px",
                                fontWeight: "normal",
                                textAlign: "center",
                                zIndex: 10
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "current-page"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forecast-page"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "current-icon"
                                ]
                            ],
                            {
                                width: "132px",
                                height: "132px",
                                position: "absolute",
                                left: "59px",
                                top: "91px",
                                objectFit: "contain"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "current-temp"
                                ]
                            ],
                            {
                                width: "205px",
                                height: "126px",
                                position: "absolute",
                                left: "217px",
                                top: "101px",
                                fontSize: "86px",
                                fontWeight: "normal",
                                lineHeight: "112px",
                                textAlign: "left"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "weather-text"
                                ]
                            ],
                            {
                                width: "300px",
                                height: "43px",
                                position: "absolute",
                                left: "66px",
                                top: "216px",
                                fontSize: "25px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "detail-card"
                                ]
                            ],
                            {
                                width: "378px",
                                height: "88px",
                                position: "absolute",
                                left: "27px",
                                top: "252px",
                                borderRadius: "17px",
                                backgroundColor: "rgba(255, 255, 255, 0.95)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "metric"
                                ]
                            ],
                            {
                                width: "125px",
                                height: "84px",
                                position: "absolute",
                                top: "2px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "metric-left"
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
                                    "metric-center"
                                ]
                            ],
                            {
                                left: "126px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "metric-right"
                                ]
                            ],
                            {
                                left: "252px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "metric-value"
                                ]
                            ],
                            {
                                width: "125px",
                                height: "48px",
                                position: "absolute",
                                left: 0,
                                top: "8px",
                                color: "#102758",
                                fontSize: "33px",
                                fontWeight: "normal",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "metric-visibility"
                                ]
                            ],
                            {
                                fontSize: "31px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "metric-label"
                                ]
                            ],
                            {
                                width: "125px",
                                height: "29px",
                                position: "absolute",
                                left: 0,
                                top: "53px",
                                color: "#102758",
                                fontSize: "17px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "metric-divider"
                                ]
                            ],
                            {
                                width: "1px",
                                height: "54px",
                                position: "absolute",
                                top: "17px",
                                backgroundColor: "#d7dfed"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "divider-one"
                                ]
                            ],
                            {
                                left: "126px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "divider-two"
                                ]
                            ],
                            {
                                left: "252px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forecast-heading"
                                ]
                            ],
                            {
                                width: "280px",
                                height: "43px",
                                position: "absolute",
                                left: "76px",
                                top: "106px",
                                fontSize: "27px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forecast-range"
                                ]
                            ],
                            {
                                width: "280px",
                                height: "31px",
                                position: "absolute",
                                left: "76px",
                                top: "143px",
                                fontSize: "18px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forecast-card"
                                ]
                            ],
                            {
                                width: "117px",
                                height: "178px",
                                position: "absolute",
                                top: "177px",
                                borderRadius: "14px",
                                backgroundColor: "rgba(255, 255, 255, 0.94)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forecast-card-one"
                                ]
                            ],
                            {
                                left: "32px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forecast-card-two"
                                ]
                            ],
                            {
                                left: "157px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forecast-card-three"
                                ]
                            ],
                            {
                                left: "282px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forecast-weekday"
                                ]
                            ],
                            {
                                width: "117px",
                                height: "36px",
                                position: "absolute",
                                left: 0,
                                top: "13px",
                                color: "#102758",
                                fontSize: "23px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forecast-icon"
                                ]
                            ],
                            {
                                width: "68px",
                                height: "68px",
                                position: "absolute",
                                left: "24px",
                                top: "43px",
                                objectFit: "contain"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forecast-text"
                                ]
                            ],
                            {
                                width: "111px",
                                height: "32px",
                                position: "absolute",
                                left: "3px",
                                top: "113px",
                                color: "#102758",
                                fontSize: "19px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "forecast-temp"
                                ]
                            ],
                            {
                                width: "111px",
                                height: "31px",
                                position: "absolute",
                                left: "3px",
                                top: "145px",
                                color: "#102758",
                                fontSize: "18px",
                                fontWeight: "normal",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "update-text"
                                ]
                            ],
                            {
                                width: "300px",
                                height: "29px",
                                position: "absolute",
                                left: "66px",
                                top: "371px",
                                fontSize: "14px",
                                fontWeight: "normal",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "corner-cat"
                                ]
                            ],
                            {
                                width: "82px",
                                height: "86px",
                                position: "absolute",
                                left: "318px",
                                top: "352px"
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
                        var _weatherCities = __webpack_require__("./src/common/weather-cities.js");
                        var _weatherService = __webpack_require__("./src/common/weather-service.js");
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const APP_RESOURCE_ROOT = "/data/app/com.application.watch.redesign";
                        const STANDBY_DELAY_MS = 60000;
                        const SWIPE_DISTANCE = 55;
                        var _default = exports.default = {
                            private: {
                                pageIndex: 0,
                                backgroundImage: "/common/backgrounds/dark-purple.png",
                                pageStyle: {
                                    backgroundColor: "#0f0d2c"
                                },
                                veilClass: "veil veil-dark",
                                cityTitleClass: "city-title text-light",
                                cityMetaClass: "city-meta text-light",
                                currentTempClass: "current-temp text-light",
                                weatherTextClass: "weather-text text-light",
                                forecastTitleClass: "forecast-heading text-light",
                                forecastRangeClass: "forecast-range text-light",
                                updateClass: "update-text text-light",
                                cityName: "北京市",
                                cityMeta: "北京市 · 中国",
                                currentIcon: "/common/weather-icons/103.png",
                                currentTemp: "27°",
                                currentText: "晴转多云",
                                feelsLike: "29°",
                                humidity: "62%",
                                visibility: "10km",
                                forecastRange: "最高33°  最低23°",
                                forecastOne: {},
                                forecastTwo: {},
                                forecastThree: {},
                                updateText: "07-27 12:30 更新",
                                catFrames: [],
                                catDuration: "100ms",
                                activeActionId: "",
                                catAnimatorReady: false,
                                catStartTimerId: null,
                                catHealthTimerId: null,
                                standbyTimerId: null,
                                screenStandby: false,
                                ignoreTouchEnd: false,
                                touchX: 0,
                                touchY: 0,
                                leaving: false,
                                weatherRequestSerial: 0,
                                locationListener: null,
                                hasShown: false
                            },
                            onInit () {
                                this.loadPage();
                            },
                            onReady () {
                                this.catAnimatorReady = true;
                                this.queueCatAnimationStart();
                            },
                            onShow () {
                                this.$app.$def.ensureWakeableScreen();
                                this.leaving = false;
                                this.screenStandby = false;
                                this.ignoreTouchEnd = false;
                                if (this.hasShown) this.loadPage();
                                else this.hasShown = true;
                                this.observeCurrentLocation();
                                if (this.catAnimatorReady) this.queueCatAnimationStart();
                                this.startCatHealthCheck();
                                this.startStandbyTimer();
                            },
                            onHide () {
                                this.stopObservingCurrentLocation();
                                this.weatherRequestSerial += 1;
                                this.stopStandbyTimer();
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.stopCatHealthCheck();
                            },
                            onDestroy () {
                                this.stopObservingCurrentLocation();
                                this.weatherRequestSerial += 1;
                                this.stopStandbyTimer();
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.stopCatHealthCheck();
                                this.catAnimatorReady = false;
                            },
                            loadPage () {
                                const customization = this.$app.$def.getCustomization();
                                const background = (0, _customization.getBackground)(customization.backgroundId);
                                const action = (0, _customization.getAction)(customization.actionId);
                                const additions = [
                                    this.$app.$def.getCurrentLocation()
                                ].concat(this.$app.$def.getCustomCities());
                                const city = (0, _weatherCities.getCity)(customization.cityId, additions);
                                const textClass = "text-" + background.foreground;
                                this.backgroundImage = background.src;
                                this.pageStyle = {
                                    backgroundColor: background.edgeColor || "#0f0d2c"
                                };
                                this.veilClass = "night-stage" === background.id ? "veil veil-dark" : "veil veil-light";
                                this.cityTitleClass = "city-title " + textClass;
                                this.cityMetaClass = "city-meta " + textClass;
                                this.currentTempClass = "current-temp " + textClass;
                                this.weatherTextClass = "weather-text " + textClass;
                                this.forecastTitleClass = "forecast-heading " + textClass;
                                this.forecastRangeClass = "forecast-range " + textClass;
                                this.updateClass = "update-text " + textClass;
                                this.cityName = city.detailName;
                                this.cityMeta = city.administrativeArea + " · " + city.country;
                                if (city.isCurrentLocation && !city.locationReady) {
                                    this.showWeatherLoading();
                                    this.$app.$def.refreshCurrentLocation(true);
                                } else if (city.isCurrentLocation) {
                                    const currentWeather = this.$app.$def.getCurrentLocationWeather();
                                    if (currentWeather) this.applyLiveWeather(currentWeather);
                                    this.loadLiveWeather(city);
                                } else this.loadLiveWeather(city);
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
                            observeCurrentLocation () {
                                const customization = this.$app.$def.getCustomization();
                                if ("current-location" !== customization.cityId || this.locationListener) return;
                                this.locationListener = (city, weather)=>{
                                    if (this.leaving || !city || !city.locationReady) return;
                                    this.cityName = city.detailName;
                                    this.cityMeta = city.administrativeArea + " · " + city.country;
                                    if (weather) this.applyLiveWeather(weather);
                                };
                                this.$app.$def.addCurrentLocationListener(this.locationListener);
                            },
                            stopObservingCurrentLocation () {
                                if (!this.locationListener) return;
                                this.$app.$def.removeCurrentLocationListener(this.locationListener);
                                this.locationListener = null;
                            },
                            emptyForecastItem () {
                                return {
                                    weekday: "--",
                                    text: "--",
                                    iconSrc: "/common/weather-icons/999.png",
                                    temperature: "--°~--°"
                                };
                            },
                            showWeatherLoading () {
                                this.currentIcon = "/common/weather-icons/999.png";
                                this.currentTemp = "--°";
                                this.currentText = "正在更新";
                                this.feelsLike = "--°";
                                this.humidity = "--%";
                                this.visibility = "--km";
                                this.forecastRange = "正在获取实时预报";
                                this.forecastOne = this.emptyForecastItem();
                                this.forecastTwo = this.emptyForecastItem();
                                this.forecastThree = this.emptyForecastItem();
                                this.updateText = "正在获取实时天气";
                            },
                            showWeatherFailure () {
                                this.currentIcon = "/common/weather-icons/999.png";
                                this.currentTemp = "--°";
                                this.currentText = "获取失败";
                                this.feelsLike = "--°";
                                this.humidity = "--%";
                                this.visibility = "--km";
                                this.forecastRange = "实时预报暂不可用";
                                this.forecastOne = this.emptyForecastItem();
                                this.forecastTwo = this.emptyForecastItem();
                                this.forecastThree = this.emptyForecastItem();
                                this.updateText = "请检查网络或天气服务";
                            },
                            applyLiveWeather (weather) {
                                this.currentIcon = weather.now.iconSrc;
                                this.currentTemp = weather.now.temp + "°";
                                this.currentText = weather.now.text;
                                this.feelsLike = weather.now.feelsLike + "°";
                                this.humidity = weather.now.humidity + "%";
                                this.visibility = weather.now.visibility + "km";
                                this.forecastRange = "最高" + weather.highest + "°  最低" + weather.lowest + "°";
                                this.forecastOne = weather.forecast[0];
                                this.forecastTwo = weather.forecast[1];
                                this.forecastThree = weather.forecast[2];
                                this.updateText = weather.updatedAt + " 更新 · 和风天气";
                            },
                            loadLiveWeather (city) {
                                const requestSerial = this.weatherRequestSerial + 1;
                                this.weatherRequestSerial = requestSerial;
                                this.showWeatherLoading();
                                (0, _weatherService.fetchLiveWeather)(city, (weather)=>{
                                    if (requestSerial !== this.weatherRequestSerial || this.leaving) return;
                                    this.applyLiveWeather(weather);
                                }, (error)=>{
                                    if (requestSerial !== this.weatherRequestSerial || this.leaving) return;
                                    console.log("load live weather failed", error);
                                    this.showWeatherFailure();
                                });
                            },
                            setPage (index) {
                                if (this.leaving || this.screenStandby) return;
                                this.pageIndex = index < 1 ? 0 : 1;
                                this.registerActivity();
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
                            lockViewport (event) {
                                if (event && event.stop) event.stop();
                                return true;
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
                                const horizontal = Math.abs(deltaX) > 1.15 * Math.abs(deltaY);
                                const vertical = Math.abs(deltaY) > 1.15 * Math.abs(deltaX);
                                if (vertical && Math.abs(deltaY) > SWIPE_DISTANCE) return void this.returnToCities();
                                if (horizontal && deltaX < -SWIPE_DISTANCE) this.setPage(1);
                                if (horizontal && deltaX > SWIPE_DISTANCE) this.setPage(0);
                            },
                            handleSwipe (event) {
                                if (this.screenStandby) return void this.wakeScreen();
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if ("up" === direction || "down" === direction) return void this.returnToCities();
                                if ("left" === direction) this.setPage(1);
                                if ("right" === direction) this.setPage(0);
                            },
                            returnToCities () {
                                if (this.leaving || this.screenStandby) return;
                                this.leaving = true;
                                try {
                                    _system.default.replace({
                                        uri: "/pages/cities"
                                    });
                                } catch (error) {
                                    this.leaving = false;
                                    console.log("close weather details failed", error);
                                }
                            },
                            startCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("weatherDetailCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.start();
                                } catch (error) {
                                    console.log("weather detail cat start failed", error);
                                }
                            },
                            queueCatAnimationStart () {
                                if (!this.catAnimatorReady || this.screenStandby) return;
                                this.cancelCatAnimationStart();
                                this.catStartTimerId = setTimeout(()=>{
                                    this.catStartTimerId = null;
                                    this.startCatAnimation();
                                }, 400);
                            },
                            cancelCatAnimationStart () {
                                if (!this.catStartTimerId) return;
                                clearTimeout(this.catStartTimerId);
                                this.catStartTimerId = null;
                            },
                            pauseCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("weatherDetailCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.pause();
                                } catch (error) {
                                    console.log("weather detail cat pause failed", error);
                                }
                            },
                            stopCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("weatherDetailCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.stop();
                                } catch (error) {
                                    console.log("weather detail cat stop failed", error);
                                }
                            },
                            startCatHealthCheck () {
                                if (this.catHealthTimerId || this.screenStandby) return;
                                this.catHealthTimerId = setInterval(()=>{
                                    const animator = this.$element("weatherDetailCatAnimator");
                                    if (!this.catAnimatorReady || !animator) return;
                                    try {
                                        const state = animator.getState();
                                        if ("paused" === state) animator.resume();
                                        if ("stopped" === state) animator.start();
                                    } catch (error) {
                                        console.log("weather detail cat health check failed", error);
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
                                this.loadPage();
                                this.queueCatAnimationStart();
                                this.startCatHealthCheck();
                                this.startStandbyTimer();
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
                                        style: function() {
                                            return __webpack_require__.g.$translateStyle$(_vm_.pageStyle);
                                        },
                                        events: {
                                            swipe: function(evt) {
                                                return _vm_.handleSwipe(evt);
                                            },
                                            touchstart: function(evt) {
                                                return _vm_.onTouchStart(evt);
                                            },
                                            touchmove: function(evt) {
                                                return _vm_.lockViewport(evt);
                                            },
                                            touchend: function(evt) {
                                                return _vm_.onTouchEnd(evt);
                                            }
                                        }
                                    }
                                }, [
                                    aiot.__ce__("div", {
                                        __vm__: _vm_,
                                        __opts__: {
                                            classList: [
                                                "viewport-lock"
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
                                        aiot.__ce__("div", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: function() {
                                                    const $classValue$ = _vm_.veilClass;
                                                    if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                    return $classValue$;
                                                },
                                                static: true
                                            }
                                        }, []),
                                        aiot.__ci__({
                                            __vm__: _vm_,
                                            __opts__: {
                                                shown: function() {
                                                    return 0 === _vm_.pageIndex;
                                                }
                                            }
                                        }, function() {
                                            return [
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "current-page"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("image", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "current-icon"
                                                            ],
                                                            src: function() {
                                                                return _vm_.currentIcon;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: function() {
                                                                const $classValue$ = _vm_.currentTempClass;
                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                return $classValue$;
                                                            },
                                                            value: function() {
                                                                return _vm_.currentTemp;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: function() {
                                                                const $classValue$ = _vm_.weatherTextClass;
                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                return $classValue$;
                                                            },
                                                            value: function() {
                                                                return _vm_.currentText;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "detail-card"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "metric",
                                                                    "metric-left"
                                                                ]
                                                            }
                                                        }, [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "metric-value"
                                                                    ],
                                                                    value: function() {
                                                                        return _vm_.feelsLike;
                                                                    }
                                                                }
                                                            }, []),
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "metric-label"
                                                                    ],
                                                                    value: "体感温度"
                                                                }
                                                            }, [])
                                                        ]),
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "metric-divider",
                                                                    "divider-one"
                                                                ]
                                                            }
                                                        }, []),
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "metric",
                                                                    "metric-center"
                                                                ]
                                                            }
                                                        }, [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "metric-value"
                                                                    ],
                                                                    value: function() {
                                                                        return _vm_.humidity;
                                                                    }
                                                                }
                                                            }, []),
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "metric-label"
                                                                    ],
                                                                    value: "湿度"
                                                                }
                                                            }, [])
                                                        ]),
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "metric-divider",
                                                                    "divider-two"
                                                                ]
                                                            }
                                                        }, []),
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "metric",
                                                                    "metric-right"
                                                                ]
                                                            }
                                                        }, [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "metric-value",
                                                                        "metric-visibility"
                                                                    ],
                                                                    value: function() {
                                                                        return _vm_.visibility;
                                                                    }
                                                                }
                                                            }, []),
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "metric-label"
                                                                    ],
                                                                    value: "能见度"
                                                                }
                                                            }, [])
                                                        ])
                                                    ])
                                                ])
                                            ];
                                        }),
                                        aiot.__ci__({
                                            __vm__: _vm_,
                                            __opts__: {
                                                shown: function() {
                                                    return 1 === _vm_.pageIndex;
                                                }
                                            }
                                        }, function() {
                                            return [
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "forecast-page"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: function() {
                                                                const $classValue$ = _vm_.forecastTitleClass;
                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                return $classValue$;
                                                            },
                                                            value: "未来3天预报",
                                                            static: true
                                                        }
                                                    }, []),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: function() {
                                                                const $classValue$ = _vm_.forecastRangeClass;
                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                return $classValue$;
                                                            },
                                                            value: function() {
                                                                return _vm_.forecastRange;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "forecast-card",
                                                                "forecast-card-one"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-weekday"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.forecastOne.weekday;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("image", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-icon"
                                                                ],
                                                                src: function() {
                                                                    return _vm_.forecastOne.iconSrc;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-text"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.forecastOne.text;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-temp"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.forecastOne.temperature;
                                                                }
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "forecast-card",
                                                                "forecast-card-two"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-weekday"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.forecastTwo.weekday;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("image", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-icon"
                                                                ],
                                                                src: function() {
                                                                    return _vm_.forecastTwo.iconSrc;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-text"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.forecastTwo.text;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-temp"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.forecastTwo.temperature;
                                                                }
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "forecast-card",
                                                                "forecast-card-three"
                                                            ]
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-weekday"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.forecastThree.weekday;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("image", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-icon"
                                                                ],
                                                                src: function() {
                                                                    return _vm_.forecastThree.iconSrc;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-text"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.forecastThree.text;
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "forecast-temp"
                                                                ],
                                                                value: function() {
                                                                    return _vm_.forecastThree.temperature;
                                                                }
                                                            }
                                                        }, [])
                                                    ])
                                                ])
                                            ];
                                        }),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: function() {
                                                    const $classValue$ = _vm_.cityTitleClass;
                                                    if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                    return $classValue$;
                                                },
                                                value: function() {
                                                    return _vm_.cityName;
                                                }
                                            }
                                        }, []),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: function() {
                                                    const $classValue$ = _vm_.cityMetaClass;
                                                    if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                    return $classValue$;
                                                },
                                                value: function() {
                                                    return _vm_.cityMeta;
                                                }
                                            }
                                        }, []),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: function() {
                                                    const $classValue$ = _vm_.updateClass;
                                                    if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                    return $classValue$;
                                                },
                                                value: function() {
                                                    return _vm_.updateText;
                                                }
                                            }
                                        }, []),
                                        aiot.__ce__("image-animator", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: [
                                                    "corner-cat"
                                                ],
                                                id: "weatherDetailCatAnimator",
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
