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
                    "./src/common/city-picker-data.js" (__unused_rspack_module, exports) {
                        "use strict";
                        Object.defineProperty(exports, "__esModule", {
                            value: true
                        });
                        exports.buildCustomCity = buildCustomCity;
                        exports.getCityOption = getCityOption;
                        exports.getCityOptions = getCityOptions;
                        exports.getDistrictOptions = getDistrictOptions;
                        exports.getProvince = getProvince;
                        exports.getProvinceOptions = getProvinceOptions;
                        exports.resolveAdministrativeLocation = resolveAdministrativeLocation;
                        const PROVINCES = [
                            {
                                id: "beijing",
                                administrativeCode: "110000",
                                name: "北京市",
                                cities: [
                                    {
                                        id: "beijing",
                                        administrativeCode: "110100",
                                        name: "北京市",
                                        shortName: "北京",
                                        locationId: "101010100",
                                        districts: [
                                            "东城区",
                                            "西城区",
                                            "朝阳区",
                                            "丰台区",
                                            "石景山区",
                                            "海淀区",
                                            "门头沟区",
                                            "房山区",
                                            "通州区",
                                            "顺义区",
                                            "昌平区",
                                            "大兴区",
                                            "怀柔区",
                                            "平谷区",
                                            "密云区",
                                            "延庆区"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-12",
                                administrativeCode: "120000",
                                name: "天津市",
                                cities: [
                                    {
                                        id: "c-1201",
                                        administrativeCode: "120100",
                                        name: "天津市",
                                        shortName: "天津",
                                        locationId: "",
                                        districts: [
                                            "和平区",
                                            "河东区",
                                            "河西区",
                                            "南开区",
                                            "河北区",
                                            "红桥区",
                                            "东丽区",
                                            "西青区",
                                            "津南区",
                                            "北辰区",
                                            "武清区",
                                            "宝坻区",
                                            "滨海新区",
                                            "宁河区",
                                            "静海区",
                                            "蓟州区"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-13",
                                administrativeCode: "130000",
                                name: "河北省",
                                cities: [
                                    {
                                        id: "c-1301",
                                        administrativeCode: "130100",
                                        name: "石家庄市",
                                        shortName: "石家庄",
                                        locationId: "",
                                        districts: [
                                            "长安区",
                                            "桥西区",
                                            "新华区",
                                            "井陉矿区",
                                            "裕华区",
                                            "藁城区",
                                            "鹿泉区",
                                            "栾城区",
                                            "井陉县",
                                            "正定县",
                                            "行唐县",
                                            "灵寿县",
                                            "高邑县",
                                            "深泽县",
                                            "赞皇县",
                                            "无极县",
                                            "平山县",
                                            "元氏县",
                                            "赵县",
                                            "辛集市",
                                            "晋州市",
                                            "新乐市"
                                        ]
                                    },
                                    {
                                        id: "c-1302",
                                        administrativeCode: "130200",
                                        name: "唐山市",
                                        shortName: "唐山",
                                        locationId: "",
                                        districts: [
                                            "路南区",
                                            "路北区",
                                            "古冶区",
                                            "开平区",
                                            "丰南区",
                                            "丰润区",
                                            "曹妃甸区",
                                            "滦南县",
                                            "乐亭县",
                                            "迁西县",
                                            "玉田县",
                                            "遵化市",
                                            "迁安市",
                                            "滦州市"
                                        ]
                                    },
                                    {
                                        id: "c-1303",
                                        administrativeCode: "130300",
                                        name: "秦皇岛市",
                                        shortName: "秦皇岛",
                                        locationId: "",
                                        districts: [
                                            "海港区",
                                            "山海关区",
                                            "北戴河区",
                                            "抚宁区",
                                            "青龙满族自治县",
                                            "昌黎县",
                                            "卢龙县"
                                        ]
                                    },
                                    {
                                        id: "c-1304",
                                        administrativeCode: "130400",
                                        name: "邯郸市",
                                        shortName: "邯郸",
                                        locationId: "",
                                        districts: [
                                            "邯山区",
                                            "丛台区",
                                            "复兴区",
                                            "峰峰矿区",
                                            "肥乡区",
                                            "永年区",
                                            "临漳县",
                                            "成安县",
                                            "大名县",
                                            "涉县",
                                            "磁县",
                                            "邱县",
                                            "鸡泽县",
                                            "广平县",
                                            "馆陶县",
                                            "魏县",
                                            "曲周县",
                                            "武安市"
                                        ]
                                    },
                                    {
                                        id: "c-1305",
                                        administrativeCode: "130500",
                                        name: "邢台市",
                                        shortName: "邢台",
                                        locationId: "",
                                        districts: [
                                            "襄都区",
                                            "信都区",
                                            "任泽区",
                                            "南和区",
                                            "临城县",
                                            "内丘县",
                                            "柏乡县",
                                            "隆尧县",
                                            "宁晋县",
                                            "巨鹿县",
                                            "新河县",
                                            "广宗县",
                                            "平乡县",
                                            "威县",
                                            "清河县",
                                            "临西县",
                                            "南宫市",
                                            "沙河市"
                                        ]
                                    },
                                    {
                                        id: "c-1306",
                                        administrativeCode: "130600",
                                        name: "保定市",
                                        shortName: "保定",
                                        locationId: "",
                                        districts: [
                                            "竞秀区",
                                            "莲池区",
                                            "满城区",
                                            "清苑区",
                                            "徐水区",
                                            "涞水县",
                                            "阜平县",
                                            "定兴县",
                                            "唐县",
                                            "高阳县",
                                            "容城县",
                                            "涞源县",
                                            "望都县",
                                            "安新县",
                                            "易县",
                                            "曲阳县",
                                            "蠡县",
                                            "顺平县",
                                            "博野县",
                                            "雄县",
                                            "涿州市",
                                            "定州市",
                                            "安国市",
                                            "高碑店市"
                                        ]
                                    },
                                    {
                                        id: "c-1307",
                                        administrativeCode: "130700",
                                        name: "张家口市",
                                        shortName: "张家口",
                                        locationId: "",
                                        districts: [
                                            "桥东区",
                                            "桥西区",
                                            "宣化区",
                                            "下花园区",
                                            "万全区",
                                            "崇礼区",
                                            "张北县",
                                            "康保县",
                                            "沽源县",
                                            "尚义县",
                                            "蔚县",
                                            "阳原县",
                                            "怀安县",
                                            "怀来县",
                                            "涿鹿县",
                                            "赤城县"
                                        ]
                                    },
                                    {
                                        id: "c-1308",
                                        administrativeCode: "130800",
                                        name: "承德市",
                                        shortName: "承德",
                                        locationId: "",
                                        districts: [
                                            "双桥区",
                                            "双滦区",
                                            "鹰手营子矿区",
                                            "承德县",
                                            "兴隆县",
                                            "滦平县",
                                            "隆化县",
                                            "丰宁满族自治县",
                                            "宽城满族自治县",
                                            "围场满族蒙古族自治县",
                                            "平泉市"
                                        ]
                                    },
                                    {
                                        id: "c-1309",
                                        administrativeCode: "130900",
                                        name: "沧州市",
                                        shortName: "沧州",
                                        locationId: "",
                                        districts: [
                                            "新华区",
                                            "运河区",
                                            "沧县",
                                            "青县",
                                            "东光县",
                                            "海兴县",
                                            "盐山县",
                                            "肃宁县",
                                            "南皮县",
                                            "吴桥县",
                                            "献县",
                                            "孟村回族自治县",
                                            "泊头市",
                                            "任丘市",
                                            "黄骅市",
                                            "河间市"
                                        ]
                                    },
                                    {
                                        id: "c-1310",
                                        administrativeCode: "131000",
                                        name: "廊坊市",
                                        shortName: "廊坊",
                                        locationId: "",
                                        districts: [
                                            "安次区",
                                            "广阳区",
                                            "固安县",
                                            "永清县",
                                            "香河县",
                                            "大城县",
                                            "文安县",
                                            "大厂回族自治县",
                                            "霸州市",
                                            "三河市"
                                        ]
                                    },
                                    {
                                        id: "c-1311",
                                        administrativeCode: "131100",
                                        name: "衡水市",
                                        shortName: "衡水",
                                        locationId: "",
                                        districts: [
                                            "桃城区",
                                            "冀州区",
                                            "枣强县",
                                            "武邑县",
                                            "武强县",
                                            "饶阳县",
                                            "安平县",
                                            "故城县",
                                            "景县",
                                            "阜城县",
                                            "深州市"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-14",
                                administrativeCode: "140000",
                                name: "山西省",
                                cities: [
                                    {
                                        id: "c-1401",
                                        administrativeCode: "140100",
                                        name: "太原市",
                                        shortName: "太原",
                                        locationId: "",
                                        districts: [
                                            "小店区",
                                            "迎泽区",
                                            "杏花岭区",
                                            "尖草坪区",
                                            "万柏林区",
                                            "晋源区",
                                            "清徐县",
                                            "阳曲县",
                                            "娄烦县",
                                            "古交市"
                                        ]
                                    },
                                    {
                                        id: "c-1402",
                                        administrativeCode: "140200",
                                        name: "大同市",
                                        shortName: "大同",
                                        locationId: "",
                                        districts: [
                                            "新荣区",
                                            "平城区",
                                            "云冈区",
                                            "云州区",
                                            "阳高县",
                                            "天镇县",
                                            "广灵县",
                                            "灵丘县",
                                            "浑源县",
                                            "左云县"
                                        ]
                                    },
                                    {
                                        id: "c-1403",
                                        administrativeCode: "140300",
                                        name: "阳泉市",
                                        shortName: "阳泉",
                                        locationId: "",
                                        districts: [
                                            "城区",
                                            "矿区",
                                            "郊区",
                                            "平定县",
                                            "盂县"
                                        ]
                                    },
                                    {
                                        id: "c-1404",
                                        administrativeCode: "140400",
                                        name: "长治市",
                                        shortName: "长治",
                                        locationId: "",
                                        districts: [
                                            "潞州区",
                                            "上党区",
                                            "屯留区",
                                            "潞城区",
                                            "襄垣县",
                                            "平顺县",
                                            "黎城县",
                                            "壶关县",
                                            "长子县",
                                            "武乡县",
                                            "沁县",
                                            "沁源县"
                                        ]
                                    },
                                    {
                                        id: "c-1405",
                                        administrativeCode: "140500",
                                        name: "晋城市",
                                        shortName: "晋城",
                                        locationId: "",
                                        districts: [
                                            "城区",
                                            "沁水县",
                                            "阳城县",
                                            "陵川县",
                                            "泽州县",
                                            "高平市"
                                        ]
                                    },
                                    {
                                        id: "c-1406",
                                        administrativeCode: "140600",
                                        name: "朔州市",
                                        shortName: "朔州",
                                        locationId: "",
                                        districts: [
                                            "朔城区",
                                            "平鲁区",
                                            "山阴县",
                                            "应县",
                                            "右玉县",
                                            "怀仁市"
                                        ]
                                    },
                                    {
                                        id: "c-1407",
                                        administrativeCode: "140700",
                                        name: "晋中市",
                                        shortName: "晋中",
                                        locationId: "",
                                        districts: [
                                            "榆次区",
                                            "太谷区",
                                            "榆社县",
                                            "左权县",
                                            "和顺县",
                                            "昔阳县",
                                            "寿阳县",
                                            "祁县",
                                            "平遥县",
                                            "灵石县",
                                            "介休市"
                                        ]
                                    },
                                    {
                                        id: "c-1408",
                                        administrativeCode: "140800",
                                        name: "运城市",
                                        shortName: "运城",
                                        locationId: "",
                                        districts: [
                                            "盐湖区",
                                            "临猗县",
                                            "万荣县",
                                            "闻喜县",
                                            "稷山县",
                                            "新绛县",
                                            "绛县",
                                            "垣曲县",
                                            "夏县",
                                            "平陆县",
                                            "芮城县",
                                            "永济市",
                                            "河津市"
                                        ]
                                    },
                                    {
                                        id: "c-1409",
                                        administrativeCode: "140900",
                                        name: "忻州市",
                                        shortName: "忻州",
                                        locationId: "",
                                        districts: [
                                            "忻府区",
                                            "定襄县",
                                            "五台县",
                                            "代县",
                                            "繁峙县",
                                            "宁武县",
                                            "静乐县",
                                            "神池县",
                                            "五寨县",
                                            "岢岚县",
                                            "河曲县",
                                            "保德县",
                                            "偏关县",
                                            "原平市"
                                        ]
                                    },
                                    {
                                        id: "c-1410",
                                        administrativeCode: "141000",
                                        name: "临汾市",
                                        shortName: "临汾",
                                        locationId: "",
                                        districts: [
                                            "尧都区",
                                            "曲沃县",
                                            "翼城县",
                                            "襄汾县",
                                            "洪洞县",
                                            "古县",
                                            "安泽县",
                                            "浮山县",
                                            "吉县",
                                            "乡宁县",
                                            "大宁县",
                                            "隰县",
                                            "永和县",
                                            "蒲县",
                                            "汾西县",
                                            "侯马市",
                                            "霍州市"
                                        ]
                                    },
                                    {
                                        id: "c-1411",
                                        administrativeCode: "141100",
                                        name: "吕梁市",
                                        shortName: "吕梁",
                                        locationId: "",
                                        districts: [
                                            "离石区",
                                            "文水县",
                                            "交城县",
                                            "兴县",
                                            "临县",
                                            "柳林县",
                                            "石楼县",
                                            "岚县",
                                            "方山县",
                                            "中阳县",
                                            "交口县",
                                            "孝义市",
                                            "汾阳市"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-15",
                                administrativeCode: "150000",
                                name: "内蒙古自治区",
                                cities: [
                                    {
                                        id: "c-1501",
                                        administrativeCode: "150100",
                                        name: "呼和浩特市",
                                        shortName: "呼和浩特",
                                        locationId: "",
                                        districts: [
                                            "新城区",
                                            "回民区",
                                            "玉泉区",
                                            "赛罕区",
                                            "土默特左旗",
                                            "托克托县",
                                            "和林格尔县",
                                            "清水河县",
                                            "武川县"
                                        ]
                                    },
                                    {
                                        id: "c-1502",
                                        administrativeCode: "150200",
                                        name: "包头市",
                                        shortName: "包头",
                                        locationId: "",
                                        districts: [
                                            "东河区",
                                            "昆都仑区",
                                            "青山区",
                                            "石拐区",
                                            "白云鄂博矿区",
                                            "九原区",
                                            "土默特右旗",
                                            "固阳县",
                                            "达尔罕茂明安联合旗"
                                        ]
                                    },
                                    {
                                        id: "c-1503",
                                        administrativeCode: "150300",
                                        name: "乌海市",
                                        shortName: "乌海",
                                        locationId: "",
                                        districts: [
                                            "海勃湾区",
                                            "海南区",
                                            "乌达区"
                                        ]
                                    },
                                    {
                                        id: "c-1504",
                                        administrativeCode: "150400",
                                        name: "赤峰市",
                                        shortName: "赤峰",
                                        locationId: "",
                                        districts: [
                                            "红山区",
                                            "元宝山区",
                                            "松山区",
                                            "阿鲁科尔沁旗",
                                            "巴林左旗",
                                            "巴林右旗",
                                            "林西县",
                                            "克什克腾旗",
                                            "翁牛特旗",
                                            "喀喇沁旗",
                                            "宁城县",
                                            "敖汉旗"
                                        ]
                                    },
                                    {
                                        id: "c-1505",
                                        administrativeCode: "150500",
                                        name: "通辽市",
                                        shortName: "通辽",
                                        locationId: "",
                                        districts: [
                                            "科尔沁区",
                                            "科尔沁左翼中旗",
                                            "科尔沁左翼后旗",
                                            "开鲁县",
                                            "库伦旗",
                                            "奈曼旗",
                                            "扎鲁特旗",
                                            "霍林郭勒市"
                                        ]
                                    },
                                    {
                                        id: "c-1506",
                                        administrativeCode: "150600",
                                        name: "鄂尔多斯市",
                                        shortName: "鄂尔多斯",
                                        locationId: "",
                                        districts: [
                                            "东胜区",
                                            "康巴什区",
                                            "达拉特旗",
                                            "准格尔旗",
                                            "鄂托克前旗",
                                            "鄂托克旗",
                                            "杭锦旗",
                                            "乌审旗",
                                            "伊金霍洛旗"
                                        ]
                                    },
                                    {
                                        id: "c-1507",
                                        administrativeCode: "150700",
                                        name: "呼伦贝尔市",
                                        shortName: "呼伦贝尔",
                                        locationId: "",
                                        districts: [
                                            "海拉尔区",
                                            "扎赉诺尔区",
                                            "阿荣旗",
                                            "莫力达瓦达斡尔族自治旗",
                                            "鄂伦春自治旗",
                                            "鄂温克族自治旗",
                                            "陈巴尔虎旗",
                                            "新巴尔虎左旗",
                                            "新巴尔虎右旗",
                                            "满洲里市",
                                            "牙克石市",
                                            "扎兰屯市",
                                            "额尔古纳市",
                                            "根河市"
                                        ]
                                    },
                                    {
                                        id: "c-1508",
                                        administrativeCode: "150800",
                                        name: "巴彦淖尔市",
                                        shortName: "巴彦淖尔",
                                        locationId: "",
                                        districts: [
                                            "临河区",
                                            "五原县",
                                            "磴口县",
                                            "乌拉特前旗",
                                            "乌拉特中旗",
                                            "乌拉特后旗",
                                            "杭锦后旗"
                                        ]
                                    },
                                    {
                                        id: "c-1509",
                                        administrativeCode: "150900",
                                        name: "乌兰察布市",
                                        shortName: "乌兰察布",
                                        locationId: "",
                                        districts: [
                                            "集宁区",
                                            "卓资县",
                                            "化德县",
                                            "商都县",
                                            "兴和县",
                                            "凉城县",
                                            "察哈尔右翼前旗",
                                            "察哈尔右翼中旗",
                                            "察哈尔右翼后旗",
                                            "四子王旗",
                                            "丰镇市"
                                        ]
                                    },
                                    {
                                        id: "c-1522",
                                        administrativeCode: "152200",
                                        name: "兴安盟",
                                        shortName: "兴安",
                                        locationId: "",
                                        districts: [
                                            "乌兰浩特市",
                                            "阿尔山市",
                                            "科尔沁右翼前旗",
                                            "科尔沁右翼中旗",
                                            "扎赉特旗",
                                            "突泉县"
                                        ]
                                    },
                                    {
                                        id: "c-1525",
                                        administrativeCode: "152500",
                                        name: "锡林郭勒盟",
                                        shortName: "锡林郭勒",
                                        locationId: "",
                                        districts: [
                                            "二连浩特市",
                                            "锡林浩特市",
                                            "阿巴嘎旗",
                                            "苏尼特左旗",
                                            "苏尼特右旗",
                                            "东乌珠穆沁旗",
                                            "西乌珠穆沁旗",
                                            "太仆寺旗",
                                            "镶黄旗",
                                            "正镶白旗",
                                            "正蓝旗",
                                            "多伦县"
                                        ]
                                    },
                                    {
                                        id: "c-1529",
                                        administrativeCode: "152900",
                                        name: "阿拉善盟",
                                        shortName: "阿拉善",
                                        locationId: "",
                                        districts: [
                                            "阿拉善左旗",
                                            "阿拉善右旗",
                                            "额济纳旗"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-21",
                                administrativeCode: "210000",
                                name: "辽宁省",
                                cities: [
                                    {
                                        id: "c-2101",
                                        administrativeCode: "210100",
                                        name: "沈阳市",
                                        shortName: "沈阳",
                                        locationId: "",
                                        districts: [
                                            "和平区",
                                            "沈河区",
                                            "大东区",
                                            "皇姑区",
                                            "铁西区",
                                            "苏家屯区",
                                            "浑南区",
                                            "沈北新区",
                                            "于洪区",
                                            "辽中区",
                                            "康平县",
                                            "法库县",
                                            "新民市"
                                        ]
                                    },
                                    {
                                        id: "c-2102",
                                        administrativeCode: "210200",
                                        name: "大连市",
                                        shortName: "大连",
                                        locationId: "",
                                        districts: [
                                            "中山区",
                                            "西岗区",
                                            "沙河口区",
                                            "甘井子区",
                                            "旅顺口区",
                                            "金州区",
                                            "普兰店区",
                                            "长海县",
                                            "瓦房店市",
                                            "庄河市"
                                        ]
                                    },
                                    {
                                        id: "c-2103",
                                        administrativeCode: "210300",
                                        name: "鞍山市",
                                        shortName: "鞍山",
                                        locationId: "",
                                        districts: [
                                            "铁东区",
                                            "铁西区",
                                            "立山区",
                                            "千山区",
                                            "台安县",
                                            "岫岩满族自治县",
                                            "海城市"
                                        ]
                                    },
                                    {
                                        id: "c-2104",
                                        administrativeCode: "210400",
                                        name: "抚顺市",
                                        shortName: "抚顺",
                                        locationId: "",
                                        districts: [
                                            "新抚区",
                                            "东洲区",
                                            "望花区",
                                            "顺城区",
                                            "抚顺县",
                                            "新宾满族自治县",
                                            "清原满族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-2105",
                                        administrativeCode: "210500",
                                        name: "本溪市",
                                        shortName: "本溪",
                                        locationId: "",
                                        districts: [
                                            "平山区",
                                            "溪湖区",
                                            "明山区",
                                            "南芬区",
                                            "本溪满族自治县",
                                            "桓仁满族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-2106",
                                        administrativeCode: "210600",
                                        name: "丹东市",
                                        shortName: "丹东",
                                        locationId: "",
                                        districts: [
                                            "元宝区",
                                            "振兴区",
                                            "振安区",
                                            "宽甸满族自治县",
                                            "东港市",
                                            "凤城市"
                                        ]
                                    },
                                    {
                                        id: "c-2107",
                                        administrativeCode: "210700",
                                        name: "锦州市",
                                        shortName: "锦州",
                                        locationId: "",
                                        districts: [
                                            "古塔区",
                                            "凌河区",
                                            "太和区",
                                            "黑山县",
                                            "义县",
                                            "凌海市",
                                            "北镇市"
                                        ]
                                    },
                                    {
                                        id: "c-2108",
                                        administrativeCode: "210800",
                                        name: "营口市",
                                        shortName: "营口",
                                        locationId: "",
                                        districts: [
                                            "站前区",
                                            "西市区",
                                            "鲅鱼圈区",
                                            "老边区",
                                            "盖州市",
                                            "大石桥市"
                                        ]
                                    },
                                    {
                                        id: "c-2109",
                                        administrativeCode: "210900",
                                        name: "阜新市",
                                        shortName: "阜新",
                                        locationId: "",
                                        districts: [
                                            "海州区",
                                            "新邱区",
                                            "太平区",
                                            "清河门区",
                                            "细河区",
                                            "阜新蒙古族自治县",
                                            "彰武县"
                                        ]
                                    },
                                    {
                                        id: "c-2110",
                                        administrativeCode: "211000",
                                        name: "辽阳市",
                                        shortName: "辽阳",
                                        locationId: "",
                                        districts: [
                                            "白塔区",
                                            "文圣区",
                                            "宏伟区",
                                            "弓长岭区",
                                            "太子河区",
                                            "辽阳县",
                                            "灯塔市"
                                        ]
                                    },
                                    {
                                        id: "c-2111",
                                        administrativeCode: "211100",
                                        name: "盘锦市",
                                        shortName: "盘锦",
                                        locationId: "",
                                        districts: [
                                            "双台子区",
                                            "兴隆台区",
                                            "大洼区",
                                            "盘山县"
                                        ]
                                    },
                                    {
                                        id: "c-2112",
                                        administrativeCode: "211200",
                                        name: "铁岭市",
                                        shortName: "铁岭",
                                        locationId: "",
                                        districts: [
                                            "银州区",
                                            "清河区",
                                            "铁岭县",
                                            "西丰县",
                                            "昌图县",
                                            "调兵山市",
                                            "开原市"
                                        ]
                                    },
                                    {
                                        id: "c-2113",
                                        administrativeCode: "211300",
                                        name: "朝阳市",
                                        shortName: "朝阳",
                                        locationId: "",
                                        districts: [
                                            "双塔区",
                                            "龙城区",
                                            "朝阳县",
                                            "建平县",
                                            "喀喇沁左翼蒙古族自治县",
                                            "北票市",
                                            "凌源市"
                                        ]
                                    },
                                    {
                                        id: "c-2114",
                                        administrativeCode: "211400",
                                        name: "葫芦岛市",
                                        shortName: "葫芦岛",
                                        locationId: "",
                                        districts: [
                                            "连山区",
                                            "龙港区",
                                            "南票区",
                                            "绥中县",
                                            "建昌县",
                                            "兴城市"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-22",
                                administrativeCode: "220000",
                                name: "吉林省",
                                cities: [
                                    {
                                        id: "c-2201",
                                        administrativeCode: "220100",
                                        name: "长春市",
                                        shortName: "长春",
                                        locationId: "",
                                        districts: [
                                            "南关区",
                                            "宽城区",
                                            "朝阳区",
                                            "二道区",
                                            "绿园区",
                                            "双阳区",
                                            "九台区",
                                            "农安县",
                                            "榆树市",
                                            "德惠市",
                                            "公主岭市"
                                        ]
                                    },
                                    {
                                        id: "c-2202",
                                        administrativeCode: "220200",
                                        name: "吉林市",
                                        shortName: "吉林市",
                                        locationId: "",
                                        districts: [
                                            "昌邑区",
                                            "龙潭区",
                                            "船营区",
                                            "丰满区",
                                            "永吉县",
                                            "蛟河市",
                                            "桦甸市",
                                            "舒兰市",
                                            "磐石市"
                                        ]
                                    },
                                    {
                                        id: "c-2203",
                                        administrativeCode: "220300",
                                        name: "四平市",
                                        shortName: "四平",
                                        locationId: "",
                                        districts: [
                                            "铁西区",
                                            "铁东区",
                                            "梨树县",
                                            "伊通满族自治县",
                                            "双辽市"
                                        ]
                                    },
                                    {
                                        id: "c-2204",
                                        administrativeCode: "220400",
                                        name: "辽源市",
                                        shortName: "辽源",
                                        locationId: "",
                                        districts: [
                                            "龙山区",
                                            "西安区",
                                            "东丰县",
                                            "东辽县"
                                        ]
                                    },
                                    {
                                        id: "c-2205",
                                        administrativeCode: "220500",
                                        name: "通化市",
                                        shortName: "通化",
                                        locationId: "",
                                        districts: [
                                            "东昌区",
                                            "二道江区",
                                            "通化县",
                                            "辉南县",
                                            "柳河县",
                                            "梅河口市",
                                            "集安市"
                                        ]
                                    },
                                    {
                                        id: "c-2206",
                                        administrativeCode: "220600",
                                        name: "白山市",
                                        shortName: "白山",
                                        locationId: "",
                                        districts: [
                                            "浑江区",
                                            "江源区",
                                            "抚松县",
                                            "靖宇县",
                                            "长白朝鲜族自治县",
                                            "临江市"
                                        ]
                                    },
                                    {
                                        id: "c-2207",
                                        administrativeCode: "220700",
                                        name: "松原市",
                                        shortName: "松原",
                                        locationId: "",
                                        districts: [
                                            "宁江区",
                                            "前郭尔罗斯蒙古族自治县",
                                            "长岭县",
                                            "乾安县",
                                            "扶余市"
                                        ]
                                    },
                                    {
                                        id: "c-2208",
                                        administrativeCode: "220800",
                                        name: "白城市",
                                        shortName: "白城",
                                        locationId: "",
                                        districts: [
                                            "洮北区",
                                            "镇赉县",
                                            "通榆县",
                                            "洮南市",
                                            "大安市"
                                        ]
                                    },
                                    {
                                        id: "c-2224",
                                        administrativeCode: "222400",
                                        name: "延边朝鲜族自治州",
                                        shortName: "延边",
                                        locationId: "",
                                        districts: [
                                            "延吉市",
                                            "图们市",
                                            "敦化市",
                                            "珲春市",
                                            "龙井市",
                                            "和龙市",
                                            "汪清县",
                                            "安图县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-23",
                                administrativeCode: "230000",
                                name: "黑龙江省",
                                cities: [
                                    {
                                        id: "c-2301",
                                        administrativeCode: "230100",
                                        name: "哈尔滨市",
                                        shortName: "哈尔滨",
                                        locationId: "",
                                        districts: [
                                            "道里区",
                                            "南岗区",
                                            "道外区",
                                            "平房区",
                                            "松北区",
                                            "香坊区",
                                            "呼兰区",
                                            "阿城区",
                                            "双城区",
                                            "依兰县",
                                            "方正县",
                                            "宾县",
                                            "巴彦县",
                                            "木兰县",
                                            "通河县",
                                            "延寿县",
                                            "尚志市",
                                            "五常市"
                                        ]
                                    },
                                    {
                                        id: "c-2302",
                                        administrativeCode: "230200",
                                        name: "齐齐哈尔市",
                                        shortName: "齐齐哈尔",
                                        locationId: "",
                                        districts: [
                                            "龙沙区",
                                            "建华区",
                                            "铁锋区",
                                            "昂昂溪区",
                                            "富拉尔基区",
                                            "碾子山区",
                                            "梅里斯达斡尔族区",
                                            "龙江县",
                                            "依安县",
                                            "泰来县",
                                            "甘南县",
                                            "富裕县",
                                            "克山县",
                                            "克东县",
                                            "拜泉县",
                                            "讷河市"
                                        ]
                                    },
                                    {
                                        id: "c-2303",
                                        administrativeCode: "230300",
                                        name: "鸡西市",
                                        shortName: "鸡西",
                                        locationId: "",
                                        districts: [
                                            "鸡冠区",
                                            "恒山区",
                                            "滴道区",
                                            "梨树区",
                                            "城子河区",
                                            "麻山区",
                                            "鸡东县",
                                            "虎林市",
                                            "密山市"
                                        ]
                                    },
                                    {
                                        id: "c-2304",
                                        administrativeCode: "230400",
                                        name: "鹤岗市",
                                        shortName: "鹤岗",
                                        locationId: "",
                                        districts: [
                                            "向阳区",
                                            "工农区",
                                            "南山区",
                                            "兴安区",
                                            "东山区",
                                            "兴山区",
                                            "萝北县",
                                            "绥滨县"
                                        ]
                                    },
                                    {
                                        id: "c-2305",
                                        administrativeCode: "230500",
                                        name: "双鸭山市",
                                        shortName: "双鸭山",
                                        locationId: "",
                                        districts: [
                                            "尖山区",
                                            "岭东区",
                                            "四方台区",
                                            "宝山区",
                                            "集贤县",
                                            "友谊县",
                                            "宝清县",
                                            "饶河县"
                                        ]
                                    },
                                    {
                                        id: "c-2306",
                                        administrativeCode: "230600",
                                        name: "大庆市",
                                        shortName: "大庆",
                                        locationId: "",
                                        districts: [
                                            "萨尔图区",
                                            "龙凤区",
                                            "让胡路区",
                                            "红岗区",
                                            "大同区",
                                            "肇州县",
                                            "肇源县",
                                            "林甸县",
                                            "杜尔伯特蒙古族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-2307",
                                        administrativeCode: "230700",
                                        name: "伊春市",
                                        shortName: "伊春",
                                        locationId: "",
                                        districts: [
                                            "伊美区",
                                            "乌翠区",
                                            "友好区",
                                            "嘉荫县",
                                            "汤旺县",
                                            "丰林县",
                                            "大箐山县",
                                            "南岔县",
                                            "金林区",
                                            "铁力市"
                                        ]
                                    },
                                    {
                                        id: "c-2308",
                                        administrativeCode: "230800",
                                        name: "佳木斯市",
                                        shortName: "佳木斯",
                                        locationId: "",
                                        districts: [
                                            "向阳区",
                                            "前进区",
                                            "东风区",
                                            "郊区",
                                            "桦南县",
                                            "桦川县",
                                            "汤原县",
                                            "同江市",
                                            "富锦市",
                                            "抚远市"
                                        ]
                                    },
                                    {
                                        id: "c-2309",
                                        administrativeCode: "230900",
                                        name: "七台河市",
                                        shortName: "七台河",
                                        locationId: "",
                                        districts: [
                                            "新兴区",
                                            "桃山区",
                                            "茄子河区",
                                            "勃利县"
                                        ]
                                    },
                                    {
                                        id: "c-2310",
                                        administrativeCode: "231000",
                                        name: "牡丹江市",
                                        shortName: "牡丹江",
                                        locationId: "",
                                        districts: [
                                            "东安区",
                                            "阳明区",
                                            "爱民区",
                                            "西安区",
                                            "林口县",
                                            "绥芬河市",
                                            "海林市",
                                            "宁安市",
                                            "穆棱市",
                                            "东宁市"
                                        ]
                                    },
                                    {
                                        id: "c-2311",
                                        administrativeCode: "231100",
                                        name: "黑河市",
                                        shortName: "黑河",
                                        locationId: "",
                                        districts: [
                                            "爱辉区",
                                            "逊克县",
                                            "孙吴县",
                                            "北安市",
                                            "五大连池市",
                                            "嫩江市"
                                        ]
                                    },
                                    {
                                        id: "c-2312",
                                        administrativeCode: "231200",
                                        name: "绥化市",
                                        shortName: "绥化",
                                        locationId: "",
                                        districts: [
                                            "北林区",
                                            "望奎县",
                                            "兰西县",
                                            "青冈县",
                                            "庆安县",
                                            "明水县",
                                            "绥棱县",
                                            "安达市",
                                            "肇东市",
                                            "海伦市"
                                        ]
                                    },
                                    {
                                        id: "c-2327",
                                        administrativeCode: "232700",
                                        name: "大兴安岭地区",
                                        shortName: "大兴安岭",
                                        locationId: "",
                                        districts: [
                                            "漠河市",
                                            "呼玛县",
                                            "塔河县",
                                            "加格达奇区"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "shanghai",
                                administrativeCode: "310000",
                                name: "上海市",
                                cities: [
                                    {
                                        id: "shanghai",
                                        administrativeCode: "310100",
                                        name: "上海市",
                                        shortName: "上海",
                                        locationId: "101020100",
                                        districts: [
                                            "黄浦区",
                                            "徐汇区",
                                            "长宁区",
                                            "静安区",
                                            "普陀区",
                                            "虹口区",
                                            "杨浦区",
                                            "闵行区",
                                            "宝山区",
                                            "嘉定区",
                                            "浦东新区",
                                            "金山区",
                                            "松江区",
                                            "青浦区",
                                            "奉贤区",
                                            "崇明区"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "jiangsu",
                                administrativeCode: "320000",
                                name: "江苏省",
                                cities: [
                                    {
                                        id: "nanjing",
                                        administrativeCode: "320100",
                                        name: "南京市",
                                        shortName: "南京",
                                        locationId: "101190101",
                                        districts: [
                                            "玄武区",
                                            "秦淮区",
                                            "建邺区",
                                            "鼓楼区",
                                            "浦口区",
                                            "栖霞区",
                                            "雨花台区",
                                            "江宁区",
                                            "六合区",
                                            "溧水区",
                                            "高淳区"
                                        ]
                                    },
                                    {
                                        id: "wuxi",
                                        administrativeCode: "320200",
                                        name: "无锡市",
                                        shortName: "无锡",
                                        locationId: "101190201",
                                        districts: [
                                            "锡山区",
                                            "惠山区",
                                            "滨湖区",
                                            "梁溪区",
                                            "新吴区",
                                            "江阴市",
                                            "宜兴市"
                                        ]
                                    },
                                    {
                                        id: "c-3203",
                                        administrativeCode: "320300",
                                        name: "徐州市",
                                        shortName: "徐州",
                                        locationId: "",
                                        districts: [
                                            "鼓楼区",
                                            "云龙区",
                                            "贾汪区",
                                            "泉山区",
                                            "铜山区",
                                            "丰县",
                                            "沛县",
                                            "睢宁县",
                                            "新沂市",
                                            "邳州市"
                                        ]
                                    },
                                    {
                                        id: "c-3204",
                                        administrativeCode: "320400",
                                        name: "常州市",
                                        shortName: "常州",
                                        locationId: "",
                                        districts: [
                                            "天宁区",
                                            "钟楼区",
                                            "新北区",
                                            "武进区",
                                            "金坛区",
                                            "溧阳市"
                                        ]
                                    },
                                    {
                                        id: "suzhou",
                                        administrativeCode: "320500",
                                        name: "苏州市",
                                        shortName: "苏州",
                                        locationId: "101190401",
                                        districts: [
                                            "虎丘区",
                                            "吴中区",
                                            "相城区",
                                            "姑苏区",
                                            "吴江区",
                                            "常熟市",
                                            "张家港市",
                                            "昆山市",
                                            "太仓市"
                                        ]
                                    },
                                    {
                                        id: "c-3206",
                                        administrativeCode: "320600",
                                        name: "南通市",
                                        shortName: "南通",
                                        locationId: "",
                                        districts: [
                                            "通州区",
                                            "崇川区",
                                            "海门区",
                                            "如东县",
                                            "启东市",
                                            "如皋市",
                                            "海安市"
                                        ]
                                    },
                                    {
                                        id: "c-3207",
                                        administrativeCode: "320700",
                                        name: "连云港市",
                                        shortName: "连云港",
                                        locationId: "",
                                        districts: [
                                            "连云区",
                                            "海州区",
                                            "赣榆区",
                                            "东海县",
                                            "灌云县",
                                            "灌南县"
                                        ]
                                    },
                                    {
                                        id: "c-3208",
                                        administrativeCode: "320800",
                                        name: "淮安市",
                                        shortName: "淮安",
                                        locationId: "",
                                        districts: [
                                            "淮安区",
                                            "淮阴区",
                                            "清江浦区",
                                            "洪泽区",
                                            "涟水县",
                                            "盱眙县",
                                            "金湖县"
                                        ]
                                    },
                                    {
                                        id: "c-3209",
                                        administrativeCode: "320900",
                                        name: "盐城市",
                                        shortName: "盐城",
                                        locationId: "",
                                        districts: [
                                            "亭湖区",
                                            "盐都区",
                                            "大丰区",
                                            "响水县",
                                            "滨海县",
                                            "阜宁县",
                                            "射阳县",
                                            "建湖县",
                                            "东台市"
                                        ]
                                    },
                                    {
                                        id: "c-3210",
                                        administrativeCode: "321000",
                                        name: "扬州市",
                                        shortName: "扬州",
                                        locationId: "",
                                        districts: [
                                            "广陵区",
                                            "邗江区",
                                            "江都区",
                                            "宝应县",
                                            "仪征市",
                                            "高邮市"
                                        ]
                                    },
                                    {
                                        id: "c-3211",
                                        administrativeCode: "321100",
                                        name: "镇江市",
                                        shortName: "镇江",
                                        locationId: "",
                                        districts: [
                                            "京口区",
                                            "润州区",
                                            "丹徒区",
                                            "丹阳市",
                                            "扬中市",
                                            "句容市"
                                        ]
                                    },
                                    {
                                        id: "c-3212",
                                        administrativeCode: "321200",
                                        name: "泰州市",
                                        shortName: "泰州",
                                        locationId: "",
                                        districts: [
                                            "海陵区",
                                            "高港区",
                                            "姜堰区",
                                            "兴化市",
                                            "靖江市",
                                            "泰兴市"
                                        ]
                                    },
                                    {
                                        id: "c-3213",
                                        administrativeCode: "321300",
                                        name: "宿迁市",
                                        shortName: "宿迁",
                                        locationId: "",
                                        districts: [
                                            "宿城区",
                                            "宿豫区",
                                            "沭阳县",
                                            "泗阳县",
                                            "泗洪县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "zhejiang",
                                administrativeCode: "330000",
                                name: "浙江省",
                                cities: [
                                    {
                                        id: "hangzhou",
                                        administrativeCode: "330100",
                                        name: "杭州市",
                                        shortName: "杭州",
                                        locationId: "101210101",
                                        districts: [
                                            "上城区",
                                            "拱墅区",
                                            "西湖区",
                                            "滨江区",
                                            "萧山区",
                                            "余杭区",
                                            "富阳区",
                                            "临安区",
                                            "临平区",
                                            "钱塘区",
                                            "桐庐县",
                                            "淳安县",
                                            "建德市"
                                        ]
                                    },
                                    {
                                        id: "ningbo",
                                        administrativeCode: "330200",
                                        name: "宁波市",
                                        shortName: "宁波",
                                        locationId: "101210401",
                                        districts: [
                                            "海曙区",
                                            "江北区",
                                            "北仑区",
                                            "镇海区",
                                            "鄞州区",
                                            "奉化区",
                                            "象山县",
                                            "宁海县",
                                            "余姚市",
                                            "慈溪市"
                                        ]
                                    },
                                    {
                                        id: "c-3303",
                                        administrativeCode: "330300",
                                        name: "温州市",
                                        shortName: "温州",
                                        locationId: "",
                                        districts: [
                                            "鹿城区",
                                            "龙湾区",
                                            "瓯海区",
                                            "洞头区",
                                            "永嘉县",
                                            "平阳县",
                                            "苍南县",
                                            "文成县",
                                            "泰顺县",
                                            "瑞安市",
                                            "乐清市",
                                            "龙港市"
                                        ]
                                    },
                                    {
                                        id: "c-3304",
                                        administrativeCode: "330400",
                                        name: "嘉兴市",
                                        shortName: "嘉兴",
                                        locationId: "",
                                        districts: [
                                            "南湖区",
                                            "秀洲区",
                                            "嘉善县",
                                            "海盐县",
                                            "海宁市",
                                            "平湖市",
                                            "桐乡市"
                                        ]
                                    },
                                    {
                                        id: "c-3305",
                                        administrativeCode: "330500",
                                        name: "湖州市",
                                        shortName: "湖州",
                                        locationId: "",
                                        districts: [
                                            "吴兴区",
                                            "南浔区",
                                            "德清县",
                                            "长兴县",
                                            "安吉县"
                                        ]
                                    },
                                    {
                                        id: "c-3306",
                                        administrativeCode: "330600",
                                        name: "绍兴市",
                                        shortName: "绍兴",
                                        locationId: "",
                                        districts: [
                                            "越城区",
                                            "柯桥区",
                                            "上虞区",
                                            "新昌县",
                                            "诸暨市",
                                            "嵊州市"
                                        ]
                                    },
                                    {
                                        id: "c-3307",
                                        administrativeCode: "330700",
                                        name: "金华市",
                                        shortName: "金华",
                                        locationId: "",
                                        districts: [
                                            "婺城区",
                                            "金东区",
                                            "武义县",
                                            "浦江县",
                                            "磐安县",
                                            "兰溪市",
                                            "义乌市",
                                            "东阳市",
                                            "永康市"
                                        ]
                                    },
                                    {
                                        id: "c-3308",
                                        administrativeCode: "330800",
                                        name: "衢州市",
                                        shortName: "衢州",
                                        locationId: "",
                                        districts: [
                                            "柯城区",
                                            "衢江区",
                                            "常山县",
                                            "开化县",
                                            "龙游县",
                                            "江山市"
                                        ]
                                    },
                                    {
                                        id: "c-3309",
                                        administrativeCode: "330900",
                                        name: "舟山市",
                                        shortName: "舟山",
                                        locationId: "",
                                        districts: [
                                            "定海区",
                                            "普陀区",
                                            "岱山县",
                                            "嵊泗县"
                                        ]
                                    },
                                    {
                                        id: "c-3310",
                                        administrativeCode: "331000",
                                        name: "台州市",
                                        shortName: "台州",
                                        locationId: "",
                                        districts: [
                                            "椒江区",
                                            "黄岩区",
                                            "路桥区",
                                            "三门县",
                                            "天台县",
                                            "仙居县",
                                            "温岭市",
                                            "临海市",
                                            "玉环市"
                                        ]
                                    },
                                    {
                                        id: "c-3311",
                                        administrativeCode: "331100",
                                        name: "丽水市",
                                        shortName: "丽水",
                                        locationId: "",
                                        districts: [
                                            "莲都区",
                                            "青田县",
                                            "缙云县",
                                            "遂昌县",
                                            "松阳县",
                                            "云和县",
                                            "庆元县",
                                            "景宁畲族自治县",
                                            "龙泉市"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-34",
                                administrativeCode: "340000",
                                name: "安徽省",
                                cities: [
                                    {
                                        id: "c-3401",
                                        administrativeCode: "340100",
                                        name: "合肥市",
                                        shortName: "合肥",
                                        locationId: "",
                                        districts: [
                                            "瑶海区",
                                            "庐阳区",
                                            "蜀山区",
                                            "包河区",
                                            "长丰县",
                                            "肥东县",
                                            "肥西县",
                                            "庐江县",
                                            "巢湖市"
                                        ]
                                    },
                                    {
                                        id: "c-3402",
                                        administrativeCode: "340200",
                                        name: "芜湖市",
                                        shortName: "芜湖",
                                        locationId: "",
                                        districts: [
                                            "镜湖区",
                                            "鸠江区",
                                            "弋江区",
                                            "湾沚区",
                                            "繁昌区",
                                            "南陵县",
                                            "无为市"
                                        ]
                                    },
                                    {
                                        id: "c-3403",
                                        administrativeCode: "340300",
                                        name: "蚌埠市",
                                        shortName: "蚌埠",
                                        locationId: "",
                                        districts: [
                                            "龙子湖区",
                                            "蚌山区",
                                            "禹会区",
                                            "淮上区",
                                            "怀远县",
                                            "五河县",
                                            "固镇县"
                                        ]
                                    },
                                    {
                                        id: "c-3404",
                                        administrativeCode: "340400",
                                        name: "淮南市",
                                        shortName: "淮南",
                                        locationId: "",
                                        districts: [
                                            "大通区",
                                            "田家庵区",
                                            "谢家集区",
                                            "八公山区",
                                            "潘集区",
                                            "凤台县",
                                            "寿县"
                                        ]
                                    },
                                    {
                                        id: "c-3405",
                                        administrativeCode: "340500",
                                        name: "马鞍山市",
                                        shortName: "马鞍山",
                                        locationId: "",
                                        districts: [
                                            "花山区",
                                            "雨山区",
                                            "博望区",
                                            "当涂县",
                                            "含山县",
                                            "和县"
                                        ]
                                    },
                                    {
                                        id: "c-3406",
                                        administrativeCode: "340600",
                                        name: "淮北市",
                                        shortName: "淮北",
                                        locationId: "",
                                        districts: [
                                            "杜集区",
                                            "相山区",
                                            "烈山区",
                                            "濉溪县"
                                        ]
                                    },
                                    {
                                        id: "c-3407",
                                        administrativeCode: "340700",
                                        name: "铜陵市",
                                        shortName: "铜陵",
                                        locationId: "",
                                        districts: [
                                            "铜官区",
                                            "义安区",
                                            "郊区",
                                            "枞阳县"
                                        ]
                                    },
                                    {
                                        id: "c-3408",
                                        administrativeCode: "340800",
                                        name: "安庆市",
                                        shortName: "安庆",
                                        locationId: "",
                                        districts: [
                                            "迎江区",
                                            "大观区",
                                            "宜秀区",
                                            "怀宁县",
                                            "太湖县",
                                            "宿松县",
                                            "望江县",
                                            "岳西县",
                                            "桐城市",
                                            "潜山市"
                                        ]
                                    },
                                    {
                                        id: "c-3410",
                                        administrativeCode: "341000",
                                        name: "黄山市",
                                        shortName: "黄山",
                                        locationId: "",
                                        districts: [
                                            "屯溪区",
                                            "黄山区",
                                            "徽州区",
                                            "歙县",
                                            "休宁县",
                                            "黟县",
                                            "祁门县"
                                        ]
                                    },
                                    {
                                        id: "c-3411",
                                        administrativeCode: "341100",
                                        name: "滁州市",
                                        shortName: "滁州",
                                        locationId: "",
                                        districts: [
                                            "琅琊区",
                                            "南谯区",
                                            "来安县",
                                            "全椒县",
                                            "定远县",
                                            "凤阳县",
                                            "天长市",
                                            "明光市"
                                        ]
                                    },
                                    {
                                        id: "c-3412",
                                        administrativeCode: "341200",
                                        name: "阜阳市",
                                        shortName: "阜阳",
                                        locationId: "",
                                        districts: [
                                            "颍州区",
                                            "颍东区",
                                            "颍泉区",
                                            "临泉县",
                                            "太和县",
                                            "阜南县",
                                            "颍上县",
                                            "界首市"
                                        ]
                                    },
                                    {
                                        id: "c-3413",
                                        administrativeCode: "341300",
                                        name: "宿州市",
                                        shortName: "宿州",
                                        locationId: "",
                                        districts: [
                                            "埇桥区",
                                            "砀山县",
                                            "萧县",
                                            "灵璧县",
                                            "泗县"
                                        ]
                                    },
                                    {
                                        id: "c-3415",
                                        administrativeCode: "341500",
                                        name: "六安市",
                                        shortName: "六安",
                                        locationId: "",
                                        districts: [
                                            "金安区",
                                            "裕安区",
                                            "叶集区",
                                            "霍邱县",
                                            "舒城县",
                                            "金寨县",
                                            "霍山县"
                                        ]
                                    },
                                    {
                                        id: "c-3416",
                                        administrativeCode: "341600",
                                        name: "亳州市",
                                        shortName: "亳州",
                                        locationId: "",
                                        districts: [
                                            "谯城区",
                                            "涡阳县",
                                            "蒙城县",
                                            "利辛县"
                                        ]
                                    },
                                    {
                                        id: "c-3417",
                                        administrativeCode: "341700",
                                        name: "池州市",
                                        shortName: "池州",
                                        locationId: "",
                                        districts: [
                                            "贵池区",
                                            "东至县",
                                            "石台县",
                                            "青阳县"
                                        ]
                                    },
                                    {
                                        id: "c-3418",
                                        administrativeCode: "341800",
                                        name: "宣城市",
                                        shortName: "宣城",
                                        locationId: "",
                                        districts: [
                                            "宣州区",
                                            "郎溪县",
                                            "泾县",
                                            "绩溪县",
                                            "旌德县",
                                            "宁国市",
                                            "广德市"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "fujian",
                                administrativeCode: "350000",
                                name: "福建省",
                                cities: [
                                    {
                                        id: "fuzhou",
                                        administrativeCode: "350100",
                                        name: "福州市",
                                        shortName: "福州",
                                        locationId: "101230101",
                                        districts: [
                                            "鼓楼区",
                                            "台江区",
                                            "仓山区",
                                            "马尾区",
                                            "晋安区",
                                            "长乐区",
                                            "闽侯县",
                                            "连江县",
                                            "罗源县",
                                            "闽清县",
                                            "永泰县",
                                            "平潭县",
                                            "福清市"
                                        ]
                                    },
                                    {
                                        id: "xiamen",
                                        administrativeCode: "350200",
                                        name: "厦门市",
                                        shortName: "厦门",
                                        locationId: "101230201",
                                        districts: [
                                            "思明区",
                                            "海沧区",
                                            "湖里区",
                                            "集美区",
                                            "同安区",
                                            "翔安区"
                                        ]
                                    },
                                    {
                                        id: "c-3503",
                                        administrativeCode: "350300",
                                        name: "莆田市",
                                        shortName: "莆田",
                                        locationId: "",
                                        districts: [
                                            "城厢区",
                                            "涵江区",
                                            "荔城区",
                                            "秀屿区",
                                            "仙游县"
                                        ]
                                    },
                                    {
                                        id: "c-3504",
                                        administrativeCode: "350400",
                                        name: "三明市",
                                        shortName: "三明",
                                        locationId: "",
                                        districts: [
                                            "三元区",
                                            "沙县区",
                                            "明溪县",
                                            "清流县",
                                            "宁化县",
                                            "大田县",
                                            "尤溪县",
                                            "将乐县",
                                            "泰宁县",
                                            "建宁县",
                                            "永安市"
                                        ]
                                    },
                                    {
                                        id: "c-3505",
                                        administrativeCode: "350500",
                                        name: "泉州市",
                                        shortName: "泉州",
                                        locationId: "",
                                        districts: [
                                            "鲤城区",
                                            "丰泽区",
                                            "洛江区",
                                            "泉港区",
                                            "惠安县",
                                            "安溪县",
                                            "永春县",
                                            "德化县",
                                            "金门县",
                                            "石狮市",
                                            "晋江市",
                                            "南安市"
                                        ]
                                    },
                                    {
                                        id: "c-3506",
                                        administrativeCode: "350600",
                                        name: "漳州市",
                                        shortName: "漳州",
                                        locationId: "",
                                        districts: [
                                            "芗城区",
                                            "龙文区",
                                            "龙海区",
                                            "长泰区",
                                            "云霄县",
                                            "漳浦县",
                                            "诏安县",
                                            "东山县",
                                            "南靖县",
                                            "平和县",
                                            "华安县"
                                        ]
                                    },
                                    {
                                        id: "c-3507",
                                        administrativeCode: "350700",
                                        name: "南平市",
                                        shortName: "南平",
                                        locationId: "",
                                        districts: [
                                            "延平区",
                                            "建阳区",
                                            "顺昌县",
                                            "浦城县",
                                            "光泽县",
                                            "松溪县",
                                            "政和县",
                                            "邵武市",
                                            "武夷山市",
                                            "建瓯市"
                                        ]
                                    },
                                    {
                                        id: "c-3508",
                                        administrativeCode: "350800",
                                        name: "龙岩市",
                                        shortName: "龙岩",
                                        locationId: "",
                                        districts: [
                                            "新罗区",
                                            "永定区",
                                            "长汀县",
                                            "上杭县",
                                            "武平县",
                                            "连城县",
                                            "漳平市"
                                        ]
                                    },
                                    {
                                        id: "c-3509",
                                        administrativeCode: "350900",
                                        name: "宁德市",
                                        shortName: "宁德",
                                        locationId: "",
                                        districts: [
                                            "蕉城区",
                                            "霞浦县",
                                            "古田县",
                                            "屏南县",
                                            "寿宁县",
                                            "周宁县",
                                            "柘荣县",
                                            "福安市",
                                            "福鼎市"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-36",
                                administrativeCode: "360000",
                                name: "江西省",
                                cities: [
                                    {
                                        id: "c-3601",
                                        administrativeCode: "360100",
                                        name: "南昌市",
                                        shortName: "南昌",
                                        locationId: "",
                                        districts: [
                                            "东湖区",
                                            "西湖区",
                                            "青云谱区",
                                            "青山湖区",
                                            "新建区",
                                            "红谷滩区",
                                            "南昌县",
                                            "安义县",
                                            "进贤县"
                                        ]
                                    },
                                    {
                                        id: "c-3602",
                                        administrativeCode: "360200",
                                        name: "景德镇市",
                                        shortName: "景德镇",
                                        locationId: "",
                                        districts: [
                                            "昌江区",
                                            "珠山区",
                                            "浮梁县",
                                            "乐平市"
                                        ]
                                    },
                                    {
                                        id: "c-3603",
                                        administrativeCode: "360300",
                                        name: "萍乡市",
                                        shortName: "萍乡",
                                        locationId: "",
                                        districts: [
                                            "安源区",
                                            "湘东区",
                                            "莲花县",
                                            "上栗县",
                                            "芦溪县"
                                        ]
                                    },
                                    {
                                        id: "c-3604",
                                        administrativeCode: "360400",
                                        name: "九江市",
                                        shortName: "九江",
                                        locationId: "",
                                        districts: [
                                            "濂溪区",
                                            "浔阳区",
                                            "柴桑区",
                                            "武宁县",
                                            "修水县",
                                            "永修县",
                                            "德安县",
                                            "都昌县",
                                            "湖口县",
                                            "彭泽县",
                                            "瑞昌市",
                                            "共青城市",
                                            "庐山市"
                                        ]
                                    },
                                    {
                                        id: "c-3605",
                                        administrativeCode: "360500",
                                        name: "新余市",
                                        shortName: "新余",
                                        locationId: "",
                                        districts: [
                                            "渝水区",
                                            "分宜县"
                                        ]
                                    },
                                    {
                                        id: "c-3606",
                                        administrativeCode: "360600",
                                        name: "鹰潭市",
                                        shortName: "鹰潭",
                                        locationId: "",
                                        districts: [
                                            "月湖区",
                                            "余江区",
                                            "贵溪市"
                                        ]
                                    },
                                    {
                                        id: "c-3607",
                                        administrativeCode: "360700",
                                        name: "赣州市",
                                        shortName: "赣州",
                                        locationId: "",
                                        districts: [
                                            "章贡区",
                                            "南康区",
                                            "赣县区",
                                            "信丰县",
                                            "大余县",
                                            "上犹县",
                                            "崇义县",
                                            "安远县",
                                            "定南县",
                                            "全南县",
                                            "宁都县",
                                            "于都县",
                                            "兴国县",
                                            "会昌县",
                                            "寻乌县",
                                            "石城县",
                                            "瑞金市",
                                            "龙南市"
                                        ]
                                    },
                                    {
                                        id: "c-3608",
                                        administrativeCode: "360800",
                                        name: "吉安市",
                                        shortName: "吉安",
                                        locationId: "",
                                        districts: [
                                            "吉州区",
                                            "青原区",
                                            "吉安县",
                                            "吉水县",
                                            "峡江县",
                                            "新干县",
                                            "永丰县",
                                            "泰和县",
                                            "遂川县",
                                            "万安县",
                                            "安福县",
                                            "永新县",
                                            "井冈山市"
                                        ]
                                    },
                                    {
                                        id: "c-3609",
                                        administrativeCode: "360900",
                                        name: "宜春市",
                                        shortName: "宜春",
                                        locationId: "",
                                        districts: [
                                            "袁州区",
                                            "奉新县",
                                            "万载县",
                                            "上高县",
                                            "宜丰县",
                                            "靖安县",
                                            "铜鼓县",
                                            "丰城市",
                                            "樟树市",
                                            "高安市"
                                        ]
                                    },
                                    {
                                        id: "c-3610",
                                        administrativeCode: "361000",
                                        name: "抚州市",
                                        shortName: "抚州",
                                        locationId: "",
                                        districts: [
                                            "临川区",
                                            "东乡区",
                                            "南城县",
                                            "黎川县",
                                            "南丰县",
                                            "崇仁县",
                                            "乐安县",
                                            "宜黄县",
                                            "金溪县",
                                            "资溪县",
                                            "广昌县"
                                        ]
                                    },
                                    {
                                        id: "c-3611",
                                        administrativeCode: "361100",
                                        name: "上饶市",
                                        shortName: "上饶",
                                        locationId: "",
                                        districts: [
                                            "信州区",
                                            "广丰区",
                                            "广信区",
                                            "玉山县",
                                            "铅山县",
                                            "横峰县",
                                            "弋阳县",
                                            "余干县",
                                            "鄱阳县",
                                            "万年县",
                                            "婺源县",
                                            "德兴市"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "shandong",
                                administrativeCode: "370000",
                                name: "山东省",
                                cities: [
                                    {
                                        id: "jinan",
                                        administrativeCode: "370100",
                                        name: "济南市",
                                        shortName: "济南",
                                        locationId: "101120101",
                                        districts: [
                                            "历下区",
                                            "市中区",
                                            "槐荫区",
                                            "天桥区",
                                            "历城区",
                                            "长清区",
                                            "章丘区",
                                            "济阳区",
                                            "莱芜区",
                                            "钢城区",
                                            "平阴县",
                                            "商河县"
                                        ]
                                    },
                                    {
                                        id: "qingdao",
                                        administrativeCode: "370200",
                                        name: "青岛市",
                                        shortName: "青岛",
                                        locationId: "101120201",
                                        districts: [
                                            "市南区",
                                            "市北区",
                                            "黄岛区",
                                            "崂山区",
                                            "李沧区",
                                            "城阳区",
                                            "即墨区",
                                            "胶州市",
                                            "平度市",
                                            "莱西市"
                                        ]
                                    },
                                    {
                                        id: "c-3703",
                                        administrativeCode: "370300",
                                        name: "淄博市",
                                        shortName: "淄博",
                                        locationId: "",
                                        districts: [
                                            "淄川区",
                                            "张店区",
                                            "博山区",
                                            "临淄区",
                                            "周村区",
                                            "桓台县",
                                            "高青县",
                                            "沂源县"
                                        ]
                                    },
                                    {
                                        id: "c-3704",
                                        administrativeCode: "370400",
                                        name: "枣庄市",
                                        shortName: "枣庄",
                                        locationId: "",
                                        districts: [
                                            "市中区",
                                            "薛城区",
                                            "峄城区",
                                            "台儿庄区",
                                            "山亭区",
                                            "滕州市"
                                        ]
                                    },
                                    {
                                        id: "c-3705",
                                        administrativeCode: "370500",
                                        name: "东营市",
                                        shortName: "东营",
                                        locationId: "",
                                        districts: [
                                            "东营区",
                                            "河口区",
                                            "垦利区",
                                            "利津县",
                                            "广饶县"
                                        ]
                                    },
                                    {
                                        id: "c-3706",
                                        administrativeCode: "370600",
                                        name: "烟台市",
                                        shortName: "烟台",
                                        locationId: "",
                                        districts: [
                                            "芝罘区",
                                            "福山区",
                                            "牟平区",
                                            "莱山区",
                                            "蓬莱区",
                                            "龙口市",
                                            "莱阳市",
                                            "莱州市",
                                            "招远市",
                                            "栖霞市",
                                            "海阳市"
                                        ]
                                    },
                                    {
                                        id: "c-3707",
                                        administrativeCode: "370700",
                                        name: "潍坊市",
                                        shortName: "潍坊",
                                        locationId: "",
                                        districts: [
                                            "潍城区",
                                            "寒亭区",
                                            "坊子区",
                                            "奎文区",
                                            "临朐县",
                                            "昌乐县",
                                            "青州市",
                                            "诸城市",
                                            "寿光市",
                                            "安丘市",
                                            "高密市",
                                            "昌邑市"
                                        ]
                                    },
                                    {
                                        id: "c-3708",
                                        administrativeCode: "370800",
                                        name: "济宁市",
                                        shortName: "济宁",
                                        locationId: "",
                                        districts: [
                                            "任城区",
                                            "兖州区",
                                            "微山县",
                                            "鱼台县",
                                            "金乡县",
                                            "嘉祥县",
                                            "汶上县",
                                            "泗水县",
                                            "梁山县",
                                            "曲阜市",
                                            "邹城市"
                                        ]
                                    },
                                    {
                                        id: "c-3709",
                                        administrativeCode: "370900",
                                        name: "泰安市",
                                        shortName: "泰安",
                                        locationId: "",
                                        districts: [
                                            "泰山区",
                                            "岱岳区",
                                            "宁阳县",
                                            "东平县",
                                            "新泰市",
                                            "肥城市"
                                        ]
                                    },
                                    {
                                        id: "c-3710",
                                        administrativeCode: "371000",
                                        name: "威海市",
                                        shortName: "威海",
                                        locationId: "",
                                        districts: [
                                            "环翠区",
                                            "文登区",
                                            "荣成市",
                                            "乳山市"
                                        ]
                                    },
                                    {
                                        id: "c-3711",
                                        administrativeCode: "371100",
                                        name: "日照市",
                                        shortName: "日照",
                                        locationId: "",
                                        districts: [
                                            "东港区",
                                            "岚山区",
                                            "五莲县",
                                            "莒县"
                                        ]
                                    },
                                    {
                                        id: "c-3713",
                                        administrativeCode: "371300",
                                        name: "临沂市",
                                        shortName: "临沂",
                                        locationId: "",
                                        districts: [
                                            "兰山区",
                                            "罗庄区",
                                            "河东区",
                                            "沂南县",
                                            "郯城县",
                                            "沂水县",
                                            "兰陵县",
                                            "费县",
                                            "平邑县",
                                            "莒南县",
                                            "蒙阴县",
                                            "临沭县"
                                        ]
                                    },
                                    {
                                        id: "c-3714",
                                        administrativeCode: "371400",
                                        name: "德州市",
                                        shortName: "德州",
                                        locationId: "",
                                        districts: [
                                            "德城区",
                                            "陵城区",
                                            "宁津县",
                                            "庆云县",
                                            "临邑县",
                                            "齐河县",
                                            "平原县",
                                            "夏津县",
                                            "武城县",
                                            "乐陵市",
                                            "禹城市"
                                        ]
                                    },
                                    {
                                        id: "c-3715",
                                        administrativeCode: "371500",
                                        name: "聊城市",
                                        shortName: "聊城",
                                        locationId: "",
                                        districts: [
                                            "东昌府区",
                                            "茌平区",
                                            "阳谷县",
                                            "莘县",
                                            "东阿县",
                                            "冠县",
                                            "高唐县",
                                            "临清市"
                                        ]
                                    },
                                    {
                                        id: "c-3716",
                                        administrativeCode: "371600",
                                        name: "滨州市",
                                        shortName: "滨州",
                                        locationId: "",
                                        districts: [
                                            "滨城区",
                                            "沾化区",
                                            "惠民县",
                                            "阳信县",
                                            "无棣县",
                                            "博兴县",
                                            "邹平市"
                                        ]
                                    },
                                    {
                                        id: "c-3717",
                                        administrativeCode: "371700",
                                        name: "菏泽市",
                                        shortName: "菏泽",
                                        locationId: "",
                                        districts: [
                                            "牡丹区",
                                            "定陶区",
                                            "曹县",
                                            "单县",
                                            "成武县",
                                            "巨野县",
                                            "郓城县",
                                            "鄄城县",
                                            "东明县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "henan",
                                administrativeCode: "410000",
                                name: "河南省",
                                cities: [
                                    {
                                        id: "zhengzhou",
                                        administrativeCode: "410100",
                                        name: "郑州市",
                                        shortName: "郑州",
                                        locationId: "101180101",
                                        districts: [
                                            "中原区",
                                            "二七区",
                                            "管城回族区",
                                            "金水区",
                                            "上街区",
                                            "惠济区",
                                            "中牟县",
                                            "巩义市",
                                            "荥阳市",
                                            "新密市",
                                            "新郑市",
                                            "登封市"
                                        ]
                                    },
                                    {
                                        id: "c-4102",
                                        administrativeCode: "410200",
                                        name: "开封市",
                                        shortName: "开封",
                                        locationId: "",
                                        districts: [
                                            "龙亭区",
                                            "顺河回族区",
                                            "鼓楼区",
                                            "禹王台区",
                                            "祥符区",
                                            "杞县",
                                            "通许县",
                                            "尉氏县",
                                            "兰考县"
                                        ]
                                    },
                                    {
                                        id: "c-4103",
                                        administrativeCode: "410300",
                                        name: "洛阳市",
                                        shortName: "洛阳",
                                        locationId: "",
                                        districts: [
                                            "老城区",
                                            "西工区",
                                            "瀍河回族区",
                                            "涧西区",
                                            "偃师区",
                                            "孟津区",
                                            "洛龙区",
                                            "新安县",
                                            "栾川县",
                                            "嵩县",
                                            "汝阳县",
                                            "宜阳县",
                                            "洛宁县",
                                            "伊川县"
                                        ]
                                    },
                                    {
                                        id: "c-4104",
                                        administrativeCode: "410400",
                                        name: "平顶山市",
                                        shortName: "平顶山",
                                        locationId: "",
                                        districts: [
                                            "新华区",
                                            "卫东区",
                                            "石龙区",
                                            "湛河区",
                                            "宝丰县",
                                            "叶县",
                                            "鲁山县",
                                            "郏县",
                                            "舞钢市",
                                            "汝州市"
                                        ]
                                    },
                                    {
                                        id: "c-4105",
                                        administrativeCode: "410500",
                                        name: "安阳市",
                                        shortName: "安阳",
                                        locationId: "",
                                        districts: [
                                            "文峰区",
                                            "北关区",
                                            "殷都区",
                                            "龙安区",
                                            "安阳县",
                                            "汤阴县",
                                            "滑县",
                                            "内黄县",
                                            "林州市"
                                        ]
                                    },
                                    {
                                        id: "c-4106",
                                        administrativeCode: "410600",
                                        name: "鹤壁市",
                                        shortName: "鹤壁",
                                        locationId: "",
                                        districts: [
                                            "鹤山区",
                                            "山城区",
                                            "淇滨区",
                                            "浚县",
                                            "淇县"
                                        ]
                                    },
                                    {
                                        id: "c-4107",
                                        administrativeCode: "410700",
                                        name: "新乡市",
                                        shortName: "新乡",
                                        locationId: "",
                                        districts: [
                                            "红旗区",
                                            "卫滨区",
                                            "凤泉区",
                                            "牧野区",
                                            "新乡县",
                                            "获嘉县",
                                            "原阳县",
                                            "延津县",
                                            "封丘县",
                                            "卫辉市",
                                            "辉县市",
                                            "长垣市"
                                        ]
                                    },
                                    {
                                        id: "c-4108",
                                        administrativeCode: "410800",
                                        name: "焦作市",
                                        shortName: "焦作",
                                        locationId: "",
                                        districts: [
                                            "解放区",
                                            "中站区",
                                            "马村区",
                                            "山阳区",
                                            "修武县",
                                            "博爱县",
                                            "武陟县",
                                            "温县",
                                            "沁阳市",
                                            "孟州市"
                                        ]
                                    },
                                    {
                                        id: "c-4109",
                                        administrativeCode: "410900",
                                        name: "濮阳市",
                                        shortName: "濮阳",
                                        locationId: "",
                                        districts: [
                                            "华龙区",
                                            "清丰县",
                                            "南乐县",
                                            "范县",
                                            "台前县",
                                            "濮阳县"
                                        ]
                                    },
                                    {
                                        id: "c-4110",
                                        administrativeCode: "411000",
                                        name: "许昌市",
                                        shortName: "许昌",
                                        locationId: "",
                                        districts: [
                                            "魏都区",
                                            "建安区",
                                            "鄢陵县",
                                            "襄城县",
                                            "禹州市",
                                            "长葛市"
                                        ]
                                    },
                                    {
                                        id: "c-4111",
                                        administrativeCode: "411100",
                                        name: "漯河市",
                                        shortName: "漯河",
                                        locationId: "",
                                        districts: [
                                            "源汇区",
                                            "郾城区",
                                            "召陵区",
                                            "舞阳县",
                                            "临颍县"
                                        ]
                                    },
                                    {
                                        id: "c-4112",
                                        administrativeCode: "411200",
                                        name: "三门峡市",
                                        shortName: "三门峡",
                                        locationId: "",
                                        districts: [
                                            "湖滨区",
                                            "陕州区",
                                            "渑池县",
                                            "卢氏县",
                                            "义马市",
                                            "灵宝市"
                                        ]
                                    },
                                    {
                                        id: "c-4113",
                                        administrativeCode: "411300",
                                        name: "南阳市",
                                        shortName: "南阳",
                                        locationId: "",
                                        districts: [
                                            "宛城区",
                                            "卧龙区",
                                            "南召县",
                                            "方城县",
                                            "西峡县",
                                            "镇平县",
                                            "内乡县",
                                            "淅川县",
                                            "社旗县",
                                            "唐河县",
                                            "新野县",
                                            "桐柏县",
                                            "邓州市"
                                        ]
                                    },
                                    {
                                        id: "c-4114",
                                        administrativeCode: "411400",
                                        name: "商丘市",
                                        shortName: "商丘",
                                        locationId: "",
                                        districts: [
                                            "梁园区",
                                            "睢阳区",
                                            "民权县",
                                            "睢县",
                                            "宁陵县",
                                            "柘城县",
                                            "虞城县",
                                            "夏邑县",
                                            "永城市"
                                        ]
                                    },
                                    {
                                        id: "c-4115",
                                        administrativeCode: "411500",
                                        name: "信阳市",
                                        shortName: "信阳",
                                        locationId: "",
                                        districts: [
                                            "浉河区",
                                            "平桥区",
                                            "罗山县",
                                            "光山县",
                                            "新县",
                                            "商城县",
                                            "固始县",
                                            "潢川县",
                                            "淮滨县",
                                            "息县"
                                        ]
                                    },
                                    {
                                        id: "c-4116",
                                        administrativeCode: "411600",
                                        name: "周口市",
                                        shortName: "周口",
                                        locationId: "",
                                        districts: [
                                            "川汇区",
                                            "淮阳区",
                                            "扶沟县",
                                            "西华县",
                                            "商水县",
                                            "沈丘县",
                                            "郸城县",
                                            "太康县",
                                            "鹿邑县",
                                            "项城市"
                                        ]
                                    },
                                    {
                                        id: "c-4117",
                                        administrativeCode: "411700",
                                        name: "驻马店市",
                                        shortName: "驻马店",
                                        locationId: "",
                                        districts: [
                                            "驿城区",
                                            "西平县",
                                            "上蔡县",
                                            "平舆县",
                                            "正阳县",
                                            "确山县",
                                            "泌阳县",
                                            "汝南县",
                                            "遂平县",
                                            "新蔡县"
                                        ]
                                    },
                                    {
                                        id: "c-419001",
                                        administrativeCode: "419001",
                                        name: "济源市",
                                        shortName: "济源",
                                        locationId: "",
                                        districts: [
                                            "济源市"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "hubei",
                                administrativeCode: "420000",
                                name: "湖北省",
                                cities: [
                                    {
                                        id: "wuhan",
                                        administrativeCode: "420100",
                                        name: "武汉市",
                                        shortName: "武汉",
                                        locationId: "101200101",
                                        districts: [
                                            "江岸区",
                                            "江汉区",
                                            "硚口区",
                                            "汉阳区",
                                            "武昌区",
                                            "青山区",
                                            "洪山区",
                                            "东西湖区",
                                            "汉南区",
                                            "蔡甸区",
                                            "江夏区",
                                            "黄陂区",
                                            "新洲区"
                                        ]
                                    },
                                    {
                                        id: "c-4202",
                                        administrativeCode: "420200",
                                        name: "黄石市",
                                        shortName: "黄石",
                                        locationId: "",
                                        districts: [
                                            "黄石港区",
                                            "西塞山区",
                                            "下陆区",
                                            "铁山区",
                                            "阳新县",
                                            "大冶市"
                                        ]
                                    },
                                    {
                                        id: "c-4203",
                                        administrativeCode: "420300",
                                        name: "十堰市",
                                        shortName: "十堰",
                                        locationId: "",
                                        districts: [
                                            "茅箭区",
                                            "张湾区",
                                            "郧阳区",
                                            "郧西县",
                                            "竹山县",
                                            "竹溪县",
                                            "房县",
                                            "丹江口市"
                                        ]
                                    },
                                    {
                                        id: "yichang",
                                        administrativeCode: "420500",
                                        name: "宜昌市",
                                        shortName: "宜昌",
                                        locationId: "101200901",
                                        districts: [
                                            "西陵区",
                                            "伍家岗区",
                                            "点军区",
                                            "猇亭区",
                                            "夷陵区",
                                            "远安县",
                                            "兴山县",
                                            "秭归县",
                                            "长阳土家族自治县",
                                            "五峰土家族自治县",
                                            "宜都市",
                                            "当阳市",
                                            "枝江市"
                                        ]
                                    },
                                    {
                                        id: "c-4206",
                                        administrativeCode: "420600",
                                        name: "襄阳市",
                                        shortName: "襄阳",
                                        locationId: "",
                                        districts: [
                                            "襄城区",
                                            "樊城区",
                                            "襄州区",
                                            "南漳县",
                                            "谷城县",
                                            "保康县",
                                            "老河口市",
                                            "枣阳市",
                                            "宜城市"
                                        ]
                                    },
                                    {
                                        id: "c-4207",
                                        administrativeCode: "420700",
                                        name: "鄂州市",
                                        shortName: "鄂州",
                                        locationId: "",
                                        districts: [
                                            "梁子湖区",
                                            "华容区",
                                            "鄂城区"
                                        ]
                                    },
                                    {
                                        id: "c-4208",
                                        administrativeCode: "420800",
                                        name: "荆门市",
                                        shortName: "荆门",
                                        locationId: "",
                                        districts: [
                                            "东宝区",
                                            "掇刀区",
                                            "沙洋县",
                                            "钟祥市",
                                            "京山市"
                                        ]
                                    },
                                    {
                                        id: "c-4209",
                                        administrativeCode: "420900",
                                        name: "孝感市",
                                        shortName: "孝感",
                                        locationId: "",
                                        districts: [
                                            "孝南区",
                                            "孝昌县",
                                            "大悟县",
                                            "云梦县",
                                            "应城市",
                                            "安陆市",
                                            "汉川市"
                                        ]
                                    },
                                    {
                                        id: "c-4210",
                                        administrativeCode: "421000",
                                        name: "荆州市",
                                        shortName: "荆州",
                                        locationId: "",
                                        districts: [
                                            "沙市区",
                                            "荆州区",
                                            "公安县",
                                            "江陵县",
                                            "石首市",
                                            "洪湖市",
                                            "松滋市",
                                            "监利市"
                                        ]
                                    },
                                    {
                                        id: "c-4211",
                                        administrativeCode: "421100",
                                        name: "黄冈市",
                                        shortName: "黄冈",
                                        locationId: "",
                                        districts: [
                                            "黄州区",
                                            "团风县",
                                            "红安县",
                                            "罗田县",
                                            "英山县",
                                            "浠水县",
                                            "蕲春县",
                                            "黄梅县",
                                            "麻城市",
                                            "武穴市"
                                        ]
                                    },
                                    {
                                        id: "c-4212",
                                        administrativeCode: "421200",
                                        name: "咸宁市",
                                        shortName: "咸宁",
                                        locationId: "",
                                        districts: [
                                            "咸安区",
                                            "嘉鱼县",
                                            "通城县",
                                            "崇阳县",
                                            "通山县",
                                            "赤壁市"
                                        ]
                                    },
                                    {
                                        id: "c-4213",
                                        administrativeCode: "421300",
                                        name: "随州市",
                                        shortName: "随州",
                                        locationId: "",
                                        districts: [
                                            "曾都区",
                                            "随县",
                                            "广水市"
                                        ]
                                    },
                                    {
                                        id: "c-4228",
                                        administrativeCode: "422800",
                                        name: "恩施土家族苗族自治州",
                                        shortName: "恩施",
                                        locationId: "",
                                        districts: [
                                            "恩施市",
                                            "利川市",
                                            "建始县",
                                            "巴东县",
                                            "宣恩县",
                                            "咸丰县",
                                            "来凤县",
                                            "鹤峰县"
                                        ]
                                    },
                                    {
                                        id: "c-429004",
                                        administrativeCode: "429004",
                                        name: "仙桃市",
                                        shortName: "仙桃",
                                        locationId: "",
                                        districts: [
                                            "仙桃市"
                                        ]
                                    },
                                    {
                                        id: "c-429005",
                                        administrativeCode: "429005",
                                        name: "潜江市",
                                        shortName: "潜江",
                                        locationId: "",
                                        districts: [
                                            "潜江市"
                                        ]
                                    },
                                    {
                                        id: "c-429006",
                                        administrativeCode: "429006",
                                        name: "天门市",
                                        shortName: "天门",
                                        locationId: "",
                                        districts: [
                                            "天门市"
                                        ]
                                    },
                                    {
                                        id: "c-429021",
                                        administrativeCode: "429021",
                                        name: "神农架林区",
                                        shortName: "神农架",
                                        locationId: "",
                                        districts: [
                                            "神农架林区"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "hunan",
                                administrativeCode: "430000",
                                name: "湖南省",
                                cities: [
                                    {
                                        id: "changsha",
                                        administrativeCode: "430100",
                                        name: "长沙市",
                                        shortName: "长沙",
                                        locationId: "101250101",
                                        districts: [
                                            "芙蓉区",
                                            "天心区",
                                            "岳麓区",
                                            "开福区",
                                            "雨花区",
                                            "望城区",
                                            "长沙县",
                                            "浏阳市",
                                            "宁乡市"
                                        ]
                                    },
                                    {
                                        id: "c-4302",
                                        administrativeCode: "430200",
                                        name: "株洲市",
                                        shortName: "株洲",
                                        locationId: "",
                                        districts: [
                                            "荷塘区",
                                            "芦淞区",
                                            "石峰区",
                                            "天元区",
                                            "渌口区",
                                            "攸县",
                                            "茶陵县",
                                            "炎陵县",
                                            "醴陵市"
                                        ]
                                    },
                                    {
                                        id: "c-4303",
                                        administrativeCode: "430300",
                                        name: "湘潭市",
                                        shortName: "湘潭",
                                        locationId: "",
                                        districts: [
                                            "雨湖区",
                                            "岳塘区",
                                            "湘潭县",
                                            "湘乡市",
                                            "韶山市"
                                        ]
                                    },
                                    {
                                        id: "c-4304",
                                        administrativeCode: "430400",
                                        name: "衡阳市",
                                        shortName: "衡阳",
                                        locationId: "",
                                        districts: [
                                            "珠晖区",
                                            "雁峰区",
                                            "石鼓区",
                                            "蒸湘区",
                                            "南岳区",
                                            "衡阳县",
                                            "衡南县",
                                            "衡山县",
                                            "衡东县",
                                            "祁东县",
                                            "耒阳市",
                                            "常宁市"
                                        ]
                                    },
                                    {
                                        id: "c-4305",
                                        administrativeCode: "430500",
                                        name: "邵阳市",
                                        shortName: "邵阳",
                                        locationId: "",
                                        districts: [
                                            "双清区",
                                            "大祥区",
                                            "北塔区",
                                            "新邵县",
                                            "邵阳县",
                                            "隆回县",
                                            "洞口县",
                                            "绥宁县",
                                            "新宁县",
                                            "城步苗族自治县",
                                            "武冈市",
                                            "邵东市"
                                        ]
                                    },
                                    {
                                        id: "c-4306",
                                        administrativeCode: "430600",
                                        name: "岳阳市",
                                        shortName: "岳阳",
                                        locationId: "",
                                        districts: [
                                            "岳阳楼区",
                                            "云溪区",
                                            "君山区",
                                            "岳阳县",
                                            "华容县",
                                            "湘阴县",
                                            "平江县",
                                            "汨罗市",
                                            "临湘市"
                                        ]
                                    },
                                    {
                                        id: "c-4307",
                                        administrativeCode: "430700",
                                        name: "常德市",
                                        shortName: "常德",
                                        locationId: "",
                                        districts: [
                                            "武陵区",
                                            "鼎城区",
                                            "安乡县",
                                            "汉寿县",
                                            "澧县",
                                            "临澧县",
                                            "桃源县",
                                            "石门县",
                                            "津市市"
                                        ]
                                    },
                                    {
                                        id: "c-4308",
                                        administrativeCode: "430800",
                                        name: "张家界市",
                                        shortName: "张家界",
                                        locationId: "",
                                        districts: [
                                            "永定区",
                                            "武陵源区",
                                            "慈利县",
                                            "桑植县"
                                        ]
                                    },
                                    {
                                        id: "c-4309",
                                        administrativeCode: "430900",
                                        name: "益阳市",
                                        shortName: "益阳",
                                        locationId: "",
                                        districts: [
                                            "资阳区",
                                            "赫山区",
                                            "南县",
                                            "桃江县",
                                            "安化县",
                                            "沅江市"
                                        ]
                                    },
                                    {
                                        id: "c-4310",
                                        administrativeCode: "431000",
                                        name: "郴州市",
                                        shortName: "郴州",
                                        locationId: "",
                                        districts: [
                                            "北湖区",
                                            "苏仙区",
                                            "桂阳县",
                                            "宜章县",
                                            "永兴县",
                                            "嘉禾县",
                                            "临武县",
                                            "汝城县",
                                            "桂东县",
                                            "安仁县",
                                            "资兴市"
                                        ]
                                    },
                                    {
                                        id: "c-4311",
                                        administrativeCode: "431100",
                                        name: "永州市",
                                        shortName: "永州",
                                        locationId: "",
                                        districts: [
                                            "零陵区",
                                            "冷水滩区",
                                            "东安县",
                                            "双牌县",
                                            "道县",
                                            "江永县",
                                            "宁远县",
                                            "蓝山县",
                                            "新田县",
                                            "江华瑶族自治县",
                                            "祁阳市"
                                        ]
                                    },
                                    {
                                        id: "c-4312",
                                        administrativeCode: "431200",
                                        name: "怀化市",
                                        shortName: "怀化",
                                        locationId: "",
                                        districts: [
                                            "鹤城区",
                                            "中方县",
                                            "沅陵县",
                                            "辰溪县",
                                            "溆浦县",
                                            "会同县",
                                            "麻阳苗族自治县",
                                            "新晃侗族自治县",
                                            "芷江侗族自治县",
                                            "靖州苗族侗族自治县",
                                            "通道侗族自治县",
                                            "洪江市"
                                        ]
                                    },
                                    {
                                        id: "c-4313",
                                        administrativeCode: "431300",
                                        name: "娄底市",
                                        shortName: "娄底",
                                        locationId: "",
                                        districts: [
                                            "娄星区",
                                            "双峰县",
                                            "新化县",
                                            "冷水江市",
                                            "涟源市"
                                        ]
                                    },
                                    {
                                        id: "c-4331",
                                        administrativeCode: "433100",
                                        name: "湘西土家族苗族自治州",
                                        shortName: "湘西",
                                        locationId: "",
                                        districts: [
                                            "吉首市",
                                            "泸溪县",
                                            "凤凰县",
                                            "花垣县",
                                            "保靖县",
                                            "古丈县",
                                            "永顺县",
                                            "龙山县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "guangdong",
                                administrativeCode: "440000",
                                name: "广东省",
                                cities: [
                                    {
                                        id: "guangzhou",
                                        administrativeCode: "440100",
                                        name: "广州市",
                                        shortName: "广州",
                                        locationId: "101280101",
                                        districts: [
                                            "荔湾区",
                                            "越秀区",
                                            "海珠区",
                                            "天河区",
                                            "白云区",
                                            "黄埔区",
                                            "番禺区",
                                            "花都区",
                                            "南沙区",
                                            "从化区",
                                            "增城区"
                                        ]
                                    },
                                    {
                                        id: "c-4402",
                                        administrativeCode: "440200",
                                        name: "韶关市",
                                        shortName: "韶关",
                                        locationId: "",
                                        districts: [
                                            "武江区",
                                            "浈江区",
                                            "曲江区",
                                            "始兴县",
                                            "仁化县",
                                            "翁源县",
                                            "乳源瑶族自治县",
                                            "新丰县",
                                            "乐昌市",
                                            "南雄市"
                                        ]
                                    },
                                    {
                                        id: "shenzhen",
                                        administrativeCode: "440300",
                                        name: "深圳市",
                                        shortName: "深圳",
                                        locationId: "101280601",
                                        districts: [
                                            "罗湖区",
                                            "福田区",
                                            "南山区",
                                            "宝安区",
                                            "龙岗区",
                                            "盐田区",
                                            "龙华区",
                                            "坪山区",
                                            "光明区"
                                        ]
                                    },
                                    {
                                        id: "c-4404",
                                        administrativeCode: "440400",
                                        name: "珠海市",
                                        shortName: "珠海",
                                        locationId: "",
                                        districts: [
                                            "香洲区",
                                            "斗门区",
                                            "金湾区"
                                        ]
                                    },
                                    {
                                        id: "c-4405",
                                        administrativeCode: "440500",
                                        name: "汕头市",
                                        shortName: "汕头",
                                        locationId: "",
                                        districts: [
                                            "龙湖区",
                                            "金平区",
                                            "濠江区",
                                            "潮阳区",
                                            "潮南区",
                                            "澄海区",
                                            "南澳县"
                                        ]
                                    },
                                    {
                                        id: "foshan",
                                        administrativeCode: "440600",
                                        name: "佛山市",
                                        shortName: "佛山",
                                        locationId: "101280800",
                                        districts: [
                                            "禅城区",
                                            "南海区",
                                            "顺德区",
                                            "三水区",
                                            "高明区"
                                        ]
                                    },
                                    {
                                        id: "c-4407",
                                        administrativeCode: "440700",
                                        name: "江门市",
                                        shortName: "江门",
                                        locationId: "",
                                        districts: [
                                            "蓬江区",
                                            "江海区",
                                            "新会区",
                                            "台山市",
                                            "开平市",
                                            "鹤山市",
                                            "恩平市"
                                        ]
                                    },
                                    {
                                        id: "c-4408",
                                        administrativeCode: "440800",
                                        name: "湛江市",
                                        shortName: "湛江",
                                        locationId: "",
                                        districts: [
                                            "赤坎区",
                                            "霞山区",
                                            "坡头区",
                                            "麻章区",
                                            "遂溪县",
                                            "徐闻县",
                                            "廉江市",
                                            "雷州市",
                                            "吴川市"
                                        ]
                                    },
                                    {
                                        id: "c-4409",
                                        administrativeCode: "440900",
                                        name: "茂名市",
                                        shortName: "茂名",
                                        locationId: "",
                                        districts: [
                                            "茂南区",
                                            "电白区",
                                            "高州市",
                                            "化州市",
                                            "信宜市"
                                        ]
                                    },
                                    {
                                        id: "c-4412",
                                        administrativeCode: "441200",
                                        name: "肇庆市",
                                        shortName: "肇庆",
                                        locationId: "",
                                        districts: [
                                            "端州区",
                                            "鼎湖区",
                                            "高要区",
                                            "广宁县",
                                            "怀集县",
                                            "封开县",
                                            "德庆县",
                                            "四会市"
                                        ]
                                    },
                                    {
                                        id: "c-4413",
                                        administrativeCode: "441300",
                                        name: "惠州市",
                                        shortName: "惠州",
                                        locationId: "",
                                        districts: [
                                            "惠城区",
                                            "惠阳区",
                                            "博罗县",
                                            "惠东县",
                                            "龙门县"
                                        ]
                                    },
                                    {
                                        id: "c-4414",
                                        administrativeCode: "441400",
                                        name: "梅州市",
                                        shortName: "梅州",
                                        locationId: "",
                                        districts: [
                                            "梅江区",
                                            "梅县区",
                                            "大埔县",
                                            "丰顺县",
                                            "五华县",
                                            "平远县",
                                            "蕉岭县",
                                            "兴宁市"
                                        ]
                                    },
                                    {
                                        id: "c-4415",
                                        administrativeCode: "441500",
                                        name: "汕尾市",
                                        shortName: "汕尾",
                                        locationId: "",
                                        districts: [
                                            "城区",
                                            "海丰县",
                                            "陆河县",
                                            "陆丰市"
                                        ]
                                    },
                                    {
                                        id: "c-4416",
                                        administrativeCode: "441600",
                                        name: "河源市",
                                        shortName: "河源",
                                        locationId: "",
                                        districts: [
                                            "源城区",
                                            "紫金县",
                                            "龙川县",
                                            "连平县",
                                            "和平县",
                                            "东源县"
                                        ]
                                    },
                                    {
                                        id: "c-4417",
                                        administrativeCode: "441700",
                                        name: "阳江市",
                                        shortName: "阳江",
                                        locationId: "",
                                        districts: [
                                            "江城区",
                                            "阳东区",
                                            "阳西县",
                                            "阳春市"
                                        ]
                                    },
                                    {
                                        id: "c-4418",
                                        administrativeCode: "441800",
                                        name: "清远市",
                                        shortName: "清远",
                                        locationId: "",
                                        districts: [
                                            "清城区",
                                            "清新区",
                                            "佛冈县",
                                            "阳山县",
                                            "连山壮族瑶族自治县",
                                            "连南瑶族自治县",
                                            "英德市",
                                            "连州市"
                                        ]
                                    },
                                    {
                                        id: "c-4419",
                                        administrativeCode: "441900",
                                        name: "东莞市",
                                        shortName: "东莞",
                                        locationId: "",
                                        districts: [
                                            "东莞市"
                                        ]
                                    },
                                    {
                                        id: "c-4420",
                                        administrativeCode: "442000",
                                        name: "中山市",
                                        shortName: "中山",
                                        locationId: "",
                                        districts: [
                                            "中山市"
                                        ]
                                    },
                                    {
                                        id: "c-4451",
                                        administrativeCode: "445100",
                                        name: "潮州市",
                                        shortName: "潮州",
                                        locationId: "",
                                        districts: [
                                            "湘桥区",
                                            "潮安区",
                                            "饶平县"
                                        ]
                                    },
                                    {
                                        id: "c-4452",
                                        administrativeCode: "445200",
                                        name: "揭阳市",
                                        shortName: "揭阳",
                                        locationId: "",
                                        districts: [
                                            "榕城区",
                                            "揭东区",
                                            "揭西县",
                                            "惠来县",
                                            "普宁市"
                                        ]
                                    },
                                    {
                                        id: "c-4453",
                                        administrativeCode: "445300",
                                        name: "云浮市",
                                        shortName: "云浮",
                                        locationId: "",
                                        districts: [
                                            "云城区",
                                            "云安区",
                                            "新兴县",
                                            "郁南县",
                                            "罗定市"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-45",
                                administrativeCode: "450000",
                                name: "广西壮族自治区",
                                cities: [
                                    {
                                        id: "c-4501",
                                        administrativeCode: "450100",
                                        name: "南宁市",
                                        shortName: "南宁",
                                        locationId: "",
                                        districts: [
                                            "兴宁区",
                                            "青秀区",
                                            "江南区",
                                            "西乡塘区",
                                            "良庆区",
                                            "邕宁区",
                                            "武鸣区",
                                            "隆安县",
                                            "马山县",
                                            "上林县",
                                            "宾阳县",
                                            "横州市"
                                        ]
                                    },
                                    {
                                        id: "c-4502",
                                        administrativeCode: "450200",
                                        name: "柳州市",
                                        shortName: "柳州",
                                        locationId: "",
                                        districts: [
                                            "城中区",
                                            "鱼峰区",
                                            "柳南区",
                                            "柳北区",
                                            "柳江区",
                                            "柳城县",
                                            "鹿寨县",
                                            "融安县",
                                            "融水苗族自治县",
                                            "三江侗族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-4503",
                                        administrativeCode: "450300",
                                        name: "桂林市",
                                        shortName: "桂林",
                                        locationId: "",
                                        districts: [
                                            "秀峰区",
                                            "叠彩区",
                                            "象山区",
                                            "七星区",
                                            "雁山区",
                                            "临桂区",
                                            "阳朔县",
                                            "灵川县",
                                            "全州县",
                                            "兴安县",
                                            "永福县",
                                            "灌阳县",
                                            "龙胜各族自治县",
                                            "资源县",
                                            "平乐县",
                                            "恭城瑶族自治县",
                                            "荔浦市"
                                        ]
                                    },
                                    {
                                        id: "c-4504",
                                        administrativeCode: "450400",
                                        name: "梧州市",
                                        shortName: "梧州",
                                        locationId: "",
                                        districts: [
                                            "万秀区",
                                            "长洲区",
                                            "龙圩区",
                                            "苍梧县",
                                            "藤县",
                                            "蒙山县",
                                            "岑溪市"
                                        ]
                                    },
                                    {
                                        id: "c-4505",
                                        administrativeCode: "450500",
                                        name: "北海市",
                                        shortName: "北海",
                                        locationId: "",
                                        districts: [
                                            "海城区",
                                            "银海区",
                                            "铁山港区",
                                            "合浦县"
                                        ]
                                    },
                                    {
                                        id: "c-4506",
                                        administrativeCode: "450600",
                                        name: "防城港市",
                                        shortName: "防城港",
                                        locationId: "",
                                        districts: [
                                            "港口区",
                                            "防城区",
                                            "上思县",
                                            "东兴市"
                                        ]
                                    },
                                    {
                                        id: "c-4507",
                                        administrativeCode: "450700",
                                        name: "钦州市",
                                        shortName: "钦州",
                                        locationId: "",
                                        districts: [
                                            "钦南区",
                                            "钦北区",
                                            "灵山县",
                                            "浦北县"
                                        ]
                                    },
                                    {
                                        id: "c-4508",
                                        administrativeCode: "450800",
                                        name: "贵港市",
                                        shortName: "贵港",
                                        locationId: "",
                                        districts: [
                                            "港北区",
                                            "港南区",
                                            "覃塘区",
                                            "平南县",
                                            "桂平市"
                                        ]
                                    },
                                    {
                                        id: "c-4509",
                                        administrativeCode: "450900",
                                        name: "玉林市",
                                        shortName: "玉林",
                                        locationId: "",
                                        districts: [
                                            "玉州区",
                                            "福绵区",
                                            "容县",
                                            "陆川县",
                                            "博白县",
                                            "兴业县",
                                            "北流市"
                                        ]
                                    },
                                    {
                                        id: "c-4510",
                                        administrativeCode: "451000",
                                        name: "百色市",
                                        shortName: "百色",
                                        locationId: "",
                                        districts: [
                                            "右江区",
                                            "田阳区",
                                            "田东县",
                                            "德保县",
                                            "那坡县",
                                            "凌云县",
                                            "乐业县",
                                            "田林县",
                                            "西林县",
                                            "隆林各族自治县",
                                            "靖西市",
                                            "平果市"
                                        ]
                                    },
                                    {
                                        id: "c-4511",
                                        administrativeCode: "451100",
                                        name: "贺州市",
                                        shortName: "贺州",
                                        locationId: "",
                                        districts: [
                                            "八步区",
                                            "平桂区",
                                            "昭平县",
                                            "钟山县",
                                            "富川瑶族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-4512",
                                        administrativeCode: "451200",
                                        name: "河池市",
                                        shortName: "河池",
                                        locationId: "",
                                        districts: [
                                            "金城江区",
                                            "宜州区",
                                            "南丹县",
                                            "天峨县",
                                            "凤山县",
                                            "东兰县",
                                            "罗城仫佬族自治县",
                                            "环江毛南族自治县",
                                            "巴马瑶族自治县",
                                            "都安瑶族自治县",
                                            "大化瑶族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-4513",
                                        administrativeCode: "451300",
                                        name: "来宾市",
                                        shortName: "来宾",
                                        locationId: "",
                                        districts: [
                                            "兴宾区",
                                            "忻城县",
                                            "象州县",
                                            "武宣县",
                                            "金秀瑶族自治县",
                                            "合山市"
                                        ]
                                    },
                                    {
                                        id: "c-4514",
                                        administrativeCode: "451400",
                                        name: "崇左市",
                                        shortName: "崇左",
                                        locationId: "",
                                        districts: [
                                            "江州区",
                                            "扶绥县",
                                            "宁明县",
                                            "龙州县",
                                            "大新县",
                                            "天等县",
                                            "凭祥市"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-46",
                                administrativeCode: "460000",
                                name: "海南省",
                                cities: [
                                    {
                                        id: "c-4601",
                                        administrativeCode: "460100",
                                        name: "海口市",
                                        shortName: "海口",
                                        locationId: "",
                                        districts: [
                                            "秀英区",
                                            "龙华区",
                                            "琼山区",
                                            "美兰区"
                                        ]
                                    },
                                    {
                                        id: "c-4602",
                                        administrativeCode: "460200",
                                        name: "三亚市",
                                        shortName: "三亚",
                                        locationId: "",
                                        districts: [
                                            "海棠区",
                                            "吉阳区",
                                            "天涯区",
                                            "崖州区"
                                        ]
                                    },
                                    {
                                        id: "c-4603",
                                        administrativeCode: "460300",
                                        name: "三沙市",
                                        shortName: "三沙",
                                        locationId: "",
                                        districts: [
                                            "西沙区",
                                            "南沙区"
                                        ]
                                    },
                                    {
                                        id: "c-4604",
                                        administrativeCode: "460400",
                                        name: "儋州市",
                                        shortName: "儋州",
                                        locationId: "",
                                        districts: [
                                            "儋州市"
                                        ]
                                    },
                                    {
                                        id: "c-469001",
                                        administrativeCode: "469001",
                                        name: "五指山市",
                                        shortName: "五指山",
                                        locationId: "",
                                        districts: [
                                            "五指山市"
                                        ]
                                    },
                                    {
                                        id: "c-469002",
                                        administrativeCode: "469002",
                                        name: "琼海市",
                                        shortName: "琼海",
                                        locationId: "",
                                        districts: [
                                            "琼海市"
                                        ]
                                    },
                                    {
                                        id: "c-469005",
                                        administrativeCode: "469005",
                                        name: "文昌市",
                                        shortName: "文昌",
                                        locationId: "",
                                        districts: [
                                            "文昌市"
                                        ]
                                    },
                                    {
                                        id: "c-469006",
                                        administrativeCode: "469006",
                                        name: "万宁市",
                                        shortName: "万宁",
                                        locationId: "",
                                        districts: [
                                            "万宁市"
                                        ]
                                    },
                                    {
                                        id: "c-469007",
                                        administrativeCode: "469007",
                                        name: "东方市",
                                        shortName: "东方",
                                        locationId: "",
                                        districts: [
                                            "东方市"
                                        ]
                                    },
                                    {
                                        id: "c-469021",
                                        administrativeCode: "469021",
                                        name: "定安县",
                                        shortName: "定安",
                                        locationId: "",
                                        districts: [
                                            "定安县"
                                        ]
                                    },
                                    {
                                        id: "c-469022",
                                        administrativeCode: "469022",
                                        name: "屯昌县",
                                        shortName: "屯昌",
                                        locationId: "",
                                        districts: [
                                            "屯昌县"
                                        ]
                                    },
                                    {
                                        id: "c-469023",
                                        administrativeCode: "469023",
                                        name: "澄迈县",
                                        shortName: "澄迈",
                                        locationId: "",
                                        districts: [
                                            "澄迈县"
                                        ]
                                    },
                                    {
                                        id: "c-469024",
                                        administrativeCode: "469024",
                                        name: "临高县",
                                        shortName: "临高",
                                        locationId: "",
                                        districts: [
                                            "临高县"
                                        ]
                                    },
                                    {
                                        id: "c-469025",
                                        administrativeCode: "469025",
                                        name: "白沙黎族自治县",
                                        shortName: "白沙",
                                        locationId: "",
                                        districts: [
                                            "白沙黎族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-469026",
                                        administrativeCode: "469026",
                                        name: "昌江黎族自治县",
                                        shortName: "昌江",
                                        locationId: "",
                                        districts: [
                                            "昌江黎族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-469027",
                                        administrativeCode: "469027",
                                        name: "乐东黎族自治县",
                                        shortName: "乐东",
                                        locationId: "",
                                        districts: [
                                            "乐东黎族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-469028",
                                        administrativeCode: "469028",
                                        name: "陵水黎族自治县",
                                        shortName: "陵水",
                                        locationId: "",
                                        districts: [
                                            "陵水黎族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-469029",
                                        administrativeCode: "469029",
                                        name: "保亭黎族苗族自治县",
                                        shortName: "保亭",
                                        locationId: "",
                                        districts: [
                                            "保亭黎族苗族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-469030",
                                        administrativeCode: "469030",
                                        name: "琼中黎族苗族自治县",
                                        shortName: "琼中",
                                        locationId: "",
                                        districts: [
                                            "琼中黎族苗族自治县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-50",
                                administrativeCode: "500000",
                                name: "重庆市",
                                cities: [
                                    {
                                        id: "c-5001",
                                        administrativeCode: "500100",
                                        name: "重庆城区",
                                        shortName: "重庆城区",
                                        locationId: "",
                                        districts: [
                                            "万州区",
                                            "涪陵区",
                                            "渝中区",
                                            "大渡口区",
                                            "沙坪坝区",
                                            "九龙坡区",
                                            "南岸区",
                                            "北碚区",
                                            "綦江区",
                                            "大足区",
                                            "巴南区",
                                            "黔江区",
                                            "长寿区",
                                            "江津区",
                                            "合川区",
                                            "永川区",
                                            "南川区",
                                            "璧山区",
                                            "铜梁区",
                                            "潼南区",
                                            "荣昌区",
                                            "开州区",
                                            "梁平区",
                                            "武隆区",
                                            "两江新区"
                                        ]
                                    },
                                    {
                                        id: "c-5002",
                                        administrativeCode: "500200",
                                        name: "重庆郊县",
                                        shortName: "重庆郊县",
                                        locationId: "",
                                        districts: [
                                            "城口县",
                                            "丰都县",
                                            "垫江县",
                                            "忠县",
                                            "云阳县",
                                            "奉节县",
                                            "巫山县",
                                            "巫溪县",
                                            "石柱土家族自治县",
                                            "秀山土家族苗族自治县",
                                            "酉阳土家族苗族自治县",
                                            "彭水苗族土家族自治县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "sichuan",
                                administrativeCode: "510000",
                                name: "四川省",
                                cities: [
                                    {
                                        id: "chengdu",
                                        administrativeCode: "510100",
                                        name: "成都市",
                                        shortName: "成都",
                                        locationId: "101270101",
                                        districts: [
                                            "锦江区",
                                            "青羊区",
                                            "金牛区",
                                            "武侯区",
                                            "成华区",
                                            "龙泉驿区",
                                            "青白江区",
                                            "新都区",
                                            "温江区",
                                            "双流区",
                                            "郫都区",
                                            "新津区",
                                            "金堂县",
                                            "大邑县",
                                            "蒲江县",
                                            "都江堰市",
                                            "彭州市",
                                            "邛崃市",
                                            "崇州市",
                                            "简阳市"
                                        ]
                                    },
                                    {
                                        id: "c-5103",
                                        administrativeCode: "510300",
                                        name: "自贡市",
                                        shortName: "自贡",
                                        locationId: "",
                                        districts: [
                                            "自流井区",
                                            "贡井区",
                                            "大安区",
                                            "沿滩区",
                                            "荣县",
                                            "富顺县"
                                        ]
                                    },
                                    {
                                        id: "c-5104",
                                        administrativeCode: "510400",
                                        name: "攀枝花市",
                                        shortName: "攀枝花",
                                        locationId: "",
                                        districts: [
                                            "东区",
                                            "西区",
                                            "仁和区",
                                            "米易县",
                                            "盐边县"
                                        ]
                                    },
                                    {
                                        id: "c-5105",
                                        administrativeCode: "510500",
                                        name: "泸州市",
                                        shortName: "泸州",
                                        locationId: "",
                                        districts: [
                                            "江阳区",
                                            "纳溪区",
                                            "龙马潭区",
                                            "泸县",
                                            "合江县",
                                            "叙永县",
                                            "古蔺县"
                                        ]
                                    },
                                    {
                                        id: "c-5106",
                                        administrativeCode: "510600",
                                        name: "德阳市",
                                        shortName: "德阳",
                                        locationId: "",
                                        districts: [
                                            "旌阳区",
                                            "罗江区",
                                            "中江县",
                                            "广汉市",
                                            "什邡市",
                                            "绵竹市"
                                        ]
                                    },
                                    {
                                        id: "mianyang",
                                        administrativeCode: "510700",
                                        name: "绵阳市",
                                        shortName: "绵阳",
                                        locationId: "101270401",
                                        districts: [
                                            "涪城区",
                                            "游仙区",
                                            "安州区",
                                            "三台县",
                                            "盐亭县",
                                            "梓潼县",
                                            "北川羌族自治县",
                                            "平武县",
                                            "江油市"
                                        ]
                                    },
                                    {
                                        id: "c-5108",
                                        administrativeCode: "510800",
                                        name: "广元市",
                                        shortName: "广元",
                                        locationId: "",
                                        districts: [
                                            "利州区",
                                            "昭化区",
                                            "朝天区",
                                            "旺苍县",
                                            "青川县",
                                            "剑阁县",
                                            "苍溪县"
                                        ]
                                    },
                                    {
                                        id: "c-5109",
                                        administrativeCode: "510900",
                                        name: "遂宁市",
                                        shortName: "遂宁",
                                        locationId: "",
                                        districts: [
                                            "船山区",
                                            "安居区",
                                            "蓬溪县",
                                            "大英县",
                                            "射洪市"
                                        ]
                                    },
                                    {
                                        id: "c-5110",
                                        administrativeCode: "511000",
                                        name: "内江市",
                                        shortName: "内江",
                                        locationId: "",
                                        districts: [
                                            "市中区",
                                            "东兴区",
                                            "威远县",
                                            "资中县",
                                            "隆昌市"
                                        ]
                                    },
                                    {
                                        id: "c-5111",
                                        administrativeCode: "511100",
                                        name: "乐山市",
                                        shortName: "乐山",
                                        locationId: "",
                                        districts: [
                                            "市中区",
                                            "沙湾区",
                                            "五通桥区",
                                            "金口河区",
                                            "犍为县",
                                            "井研县",
                                            "夹江县",
                                            "沐川县",
                                            "峨边彝族自治县",
                                            "马边彝族自治县",
                                            "峨眉山市"
                                        ]
                                    },
                                    {
                                        id: "c-5113",
                                        administrativeCode: "511300",
                                        name: "南充市",
                                        shortName: "南充",
                                        locationId: "",
                                        districts: [
                                            "顺庆区",
                                            "高坪区",
                                            "嘉陵区",
                                            "南部县",
                                            "营山县",
                                            "蓬安县",
                                            "仪陇县",
                                            "西充县",
                                            "阆中市"
                                        ]
                                    },
                                    {
                                        id: "c-5114",
                                        administrativeCode: "511400",
                                        name: "眉山市",
                                        shortName: "眉山",
                                        locationId: "",
                                        districts: [
                                            "东坡区",
                                            "彭山区",
                                            "仁寿县",
                                            "洪雅县",
                                            "丹棱县",
                                            "青神县"
                                        ]
                                    },
                                    {
                                        id: "c-5115",
                                        administrativeCode: "511500",
                                        name: "宜宾市",
                                        shortName: "宜宾",
                                        locationId: "",
                                        districts: [
                                            "翠屏区",
                                            "南溪区",
                                            "叙州区",
                                            "江安县",
                                            "长宁县",
                                            "高县",
                                            "珙县",
                                            "筠连县",
                                            "兴文县",
                                            "屏山县"
                                        ]
                                    },
                                    {
                                        id: "c-5116",
                                        administrativeCode: "511600",
                                        name: "广安市",
                                        shortName: "广安",
                                        locationId: "",
                                        districts: [
                                            "广安区",
                                            "前锋区",
                                            "岳池县",
                                            "武胜县",
                                            "邻水县",
                                            "华蓥市"
                                        ]
                                    },
                                    {
                                        id: "c-5117",
                                        administrativeCode: "511700",
                                        name: "达州市",
                                        shortName: "达州",
                                        locationId: "",
                                        districts: [
                                            "通川区",
                                            "达川区",
                                            "宣汉县",
                                            "开江县",
                                            "大竹县",
                                            "渠县",
                                            "万源市"
                                        ]
                                    },
                                    {
                                        id: "c-5118",
                                        administrativeCode: "511800",
                                        name: "雅安市",
                                        shortName: "雅安",
                                        locationId: "",
                                        districts: [
                                            "雨城区",
                                            "名山区",
                                            "荥经县",
                                            "汉源县",
                                            "石棉县",
                                            "天全县",
                                            "芦山县",
                                            "宝兴县"
                                        ]
                                    },
                                    {
                                        id: "c-5119",
                                        administrativeCode: "511900",
                                        name: "巴中市",
                                        shortName: "巴中",
                                        locationId: "",
                                        districts: [
                                            "巴州区",
                                            "恩阳区",
                                            "通江县",
                                            "南江县",
                                            "平昌县"
                                        ]
                                    },
                                    {
                                        id: "c-5120",
                                        administrativeCode: "512000",
                                        name: "资阳市",
                                        shortName: "资阳",
                                        locationId: "",
                                        districts: [
                                            "雁江区",
                                            "安岳县",
                                            "乐至县"
                                        ]
                                    },
                                    {
                                        id: "c-5132",
                                        administrativeCode: "513200",
                                        name: "阿坝藏族羌族自治州",
                                        shortName: "阿坝",
                                        locationId: "",
                                        districts: [
                                            "马尔康市",
                                            "汶川县",
                                            "理县",
                                            "茂县",
                                            "松潘县",
                                            "九寨沟县",
                                            "金川县",
                                            "小金县",
                                            "黑水县",
                                            "壤塘县",
                                            "阿坝县",
                                            "若尔盖县",
                                            "红原县"
                                        ]
                                    },
                                    {
                                        id: "c-5133",
                                        administrativeCode: "513300",
                                        name: "甘孜藏族自治州",
                                        shortName: "甘孜",
                                        locationId: "",
                                        districts: [
                                            "康定市",
                                            "泸定县",
                                            "丹巴县",
                                            "九龙县",
                                            "雅江县",
                                            "道孚县",
                                            "炉霍县",
                                            "甘孜县",
                                            "新龙县",
                                            "德格县",
                                            "白玉县",
                                            "石渠县",
                                            "色达县",
                                            "理塘县",
                                            "巴塘县",
                                            "乡城县",
                                            "稻城县",
                                            "得荣县"
                                        ]
                                    },
                                    {
                                        id: "c-5134",
                                        administrativeCode: "513400",
                                        name: "凉山彝族自治州",
                                        shortName: "凉山",
                                        locationId: "",
                                        districts: [
                                            "西昌市",
                                            "会理市",
                                            "木里藏族自治县",
                                            "盐源县",
                                            "德昌县",
                                            "会东县",
                                            "宁南县",
                                            "普格县",
                                            "布拖县",
                                            "金阳县",
                                            "昭觉县",
                                            "喜德县",
                                            "冕宁县",
                                            "越西县",
                                            "甘洛县",
                                            "美姑县",
                                            "雷波县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-52",
                                administrativeCode: "520000",
                                name: "贵州省",
                                cities: [
                                    {
                                        id: "c-5201",
                                        administrativeCode: "520100",
                                        name: "贵阳市",
                                        shortName: "贵阳",
                                        locationId: "",
                                        districts: [
                                            "南明区",
                                            "云岩区",
                                            "花溪区",
                                            "乌当区",
                                            "白云区",
                                            "观山湖区",
                                            "开阳县",
                                            "息烽县",
                                            "修文县",
                                            "清镇市"
                                        ]
                                    },
                                    {
                                        id: "c-5202",
                                        administrativeCode: "520200",
                                        name: "六盘水市",
                                        shortName: "六盘水",
                                        locationId: "",
                                        districts: [
                                            "钟山区",
                                            "六枝特区",
                                            "水城区",
                                            "盘州市"
                                        ]
                                    },
                                    {
                                        id: "c-5203",
                                        administrativeCode: "520300",
                                        name: "遵义市",
                                        shortName: "遵义",
                                        locationId: "",
                                        districts: [
                                            "红花岗区",
                                            "汇川区",
                                            "播州区",
                                            "桐梓县",
                                            "绥阳县",
                                            "正安县",
                                            "道真仡佬族苗族自治县",
                                            "务川仡佬族苗族自治县",
                                            "凤冈县",
                                            "湄潭县",
                                            "余庆县",
                                            "习水县",
                                            "赤水市",
                                            "仁怀市"
                                        ]
                                    },
                                    {
                                        id: "c-5204",
                                        administrativeCode: "520400",
                                        name: "安顺市",
                                        shortName: "安顺",
                                        locationId: "",
                                        districts: [
                                            "西秀区",
                                            "平坝区",
                                            "普定县",
                                            "镇宁布依族苗族自治县",
                                            "关岭布依族苗族自治县",
                                            "紫云苗族布依族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-5205",
                                        administrativeCode: "520500",
                                        name: "毕节市",
                                        shortName: "毕节",
                                        locationId: "",
                                        districts: [
                                            "七星关区",
                                            "大方县",
                                            "金沙县",
                                            "织金县",
                                            "纳雍县",
                                            "威宁彝族回族苗族自治县",
                                            "赫章县",
                                            "黔西市"
                                        ]
                                    },
                                    {
                                        id: "c-5206",
                                        administrativeCode: "520600",
                                        name: "铜仁市",
                                        shortName: "铜仁",
                                        locationId: "",
                                        districts: [
                                            "碧江区",
                                            "万山区",
                                            "江口县",
                                            "玉屏侗族自治县",
                                            "石阡县",
                                            "思南县",
                                            "印江土家族苗族自治县",
                                            "德江县",
                                            "沿河土家族自治县",
                                            "松桃苗族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-5223",
                                        administrativeCode: "522300",
                                        name: "黔西南布依族苗族自治州",
                                        shortName: "黔西南",
                                        locationId: "",
                                        districts: [
                                            "兴义市",
                                            "兴仁市",
                                            "普安县",
                                            "晴隆县",
                                            "贞丰县",
                                            "望谟县",
                                            "册亨县",
                                            "安龙县"
                                        ]
                                    },
                                    {
                                        id: "c-5226",
                                        administrativeCode: "522600",
                                        name: "黔东南苗族侗族自治州",
                                        shortName: "黔东南",
                                        locationId: "",
                                        districts: [
                                            "凯里市",
                                            "黄平县",
                                            "施秉县",
                                            "三穗县",
                                            "镇远县",
                                            "岑巩县",
                                            "天柱县",
                                            "锦屏县",
                                            "剑河县",
                                            "台江县",
                                            "黎平县",
                                            "榕江县",
                                            "从江县",
                                            "雷山县",
                                            "麻江县",
                                            "丹寨县"
                                        ]
                                    },
                                    {
                                        id: "c-5227",
                                        administrativeCode: "522700",
                                        name: "黔南布依族苗族自治州",
                                        shortName: "黔南",
                                        locationId: "",
                                        districts: [
                                            "都匀市",
                                            "福泉市",
                                            "荔波县",
                                            "贵定县",
                                            "瓮安县",
                                            "独山县",
                                            "平塘县",
                                            "罗甸县",
                                            "长顺县",
                                            "龙里县",
                                            "惠水县",
                                            "三都水族自治县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-53",
                                administrativeCode: "530000",
                                name: "云南省",
                                cities: [
                                    {
                                        id: "c-5301",
                                        administrativeCode: "530100",
                                        name: "昆明市",
                                        shortName: "昆明",
                                        locationId: "",
                                        districts: [
                                            "五华区",
                                            "盘龙区",
                                            "官渡区",
                                            "西山区",
                                            "东川区",
                                            "呈贡区",
                                            "晋宁区",
                                            "富民县",
                                            "宜良县",
                                            "石林彝族自治县",
                                            "嵩明县",
                                            "禄劝彝族苗族自治县",
                                            "寻甸回族彝族自治县",
                                            "安宁市"
                                        ]
                                    },
                                    {
                                        id: "c-5303",
                                        administrativeCode: "530300",
                                        name: "曲靖市",
                                        shortName: "曲靖",
                                        locationId: "",
                                        districts: [
                                            "麒麟区",
                                            "沾益区",
                                            "马龙区",
                                            "陆良县",
                                            "师宗县",
                                            "罗平县",
                                            "富源县",
                                            "会泽县",
                                            "宣威市"
                                        ]
                                    },
                                    {
                                        id: "c-5304",
                                        administrativeCode: "530400",
                                        name: "玉溪市",
                                        shortName: "玉溪",
                                        locationId: "",
                                        districts: [
                                            "红塔区",
                                            "江川区",
                                            "通海县",
                                            "华宁县",
                                            "易门县",
                                            "峨山彝族自治县",
                                            "新平彝族傣族自治县",
                                            "元江哈尼族彝族傣族自治县",
                                            "澄江市"
                                        ]
                                    },
                                    {
                                        id: "c-5305",
                                        administrativeCode: "530500",
                                        name: "保山市",
                                        shortName: "保山",
                                        locationId: "",
                                        districts: [
                                            "隆阳区",
                                            "施甸县",
                                            "龙陵县",
                                            "昌宁县",
                                            "腾冲市"
                                        ]
                                    },
                                    {
                                        id: "c-5306",
                                        administrativeCode: "530600",
                                        name: "昭通市",
                                        shortName: "昭通",
                                        locationId: "",
                                        districts: [
                                            "昭阳区",
                                            "鲁甸县",
                                            "巧家县",
                                            "盐津县",
                                            "大关县",
                                            "永善县",
                                            "绥江县",
                                            "镇雄县",
                                            "彝良县",
                                            "威信县",
                                            "水富市"
                                        ]
                                    },
                                    {
                                        id: "c-5307",
                                        administrativeCode: "530700",
                                        name: "丽江市",
                                        shortName: "丽江",
                                        locationId: "",
                                        districts: [
                                            "古城区",
                                            "玉龙纳西族自治县",
                                            "永胜县",
                                            "华坪县",
                                            "宁蒗彝族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-5308",
                                        administrativeCode: "530800",
                                        name: "普洱市",
                                        shortName: "普洱",
                                        locationId: "",
                                        districts: [
                                            "思茅区",
                                            "宁洱哈尼族彝族自治县",
                                            "墨江哈尼族自治县",
                                            "景东彝族自治县",
                                            "景谷傣族彝族自治县",
                                            "镇沅彝族哈尼族拉祜族自治县",
                                            "江城哈尼族彝族自治县",
                                            "孟连傣族拉祜族佤族自治县",
                                            "澜沧拉祜族自治县",
                                            "西盟佤族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-5309",
                                        administrativeCode: "530900",
                                        name: "临沧市",
                                        shortName: "临沧",
                                        locationId: "",
                                        districts: [
                                            "临翔区",
                                            "凤庆县",
                                            "云县",
                                            "永德县",
                                            "镇康县",
                                            "双江拉祜族佤族布朗族傣族自治县",
                                            "耿马傣族佤族自治县",
                                            "沧源佤族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-5323",
                                        administrativeCode: "532300",
                                        name: "楚雄彝族自治州",
                                        shortName: "楚雄",
                                        locationId: "",
                                        districts: [
                                            "楚雄市",
                                            "禄丰市",
                                            "双柏县",
                                            "牟定县",
                                            "南华县",
                                            "姚安县",
                                            "大姚县",
                                            "永仁县",
                                            "元谋县",
                                            "武定县"
                                        ]
                                    },
                                    {
                                        id: "c-5325",
                                        administrativeCode: "532500",
                                        name: "红河哈尼族彝族自治州",
                                        shortName: "红河",
                                        locationId: "",
                                        districts: [
                                            "个旧市",
                                            "开远市",
                                            "蒙自市",
                                            "弥勒市",
                                            "屏边苗族自治县",
                                            "建水县",
                                            "石屏县",
                                            "泸西县",
                                            "元阳县",
                                            "红河县",
                                            "金平苗族瑶族傣族自治县",
                                            "绿春县",
                                            "河口瑶族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-5326",
                                        administrativeCode: "532600",
                                        name: "文山壮族苗族自治州",
                                        shortName: "文山",
                                        locationId: "",
                                        districts: [
                                            "文山市",
                                            "砚山县",
                                            "西畴县",
                                            "麻栗坡县",
                                            "马关县",
                                            "丘北县",
                                            "广南县",
                                            "富宁县"
                                        ]
                                    },
                                    {
                                        id: "c-5328",
                                        administrativeCode: "532800",
                                        name: "西双版纳傣族自治州",
                                        shortName: "西双版纳",
                                        locationId: "",
                                        districts: [
                                            "景洪市",
                                            "勐海县",
                                            "勐腊县"
                                        ]
                                    },
                                    {
                                        id: "c-5329",
                                        administrativeCode: "532900",
                                        name: "大理白族自治州",
                                        shortName: "大理",
                                        locationId: "",
                                        districts: [
                                            "大理市",
                                            "漾濞彝族自治县",
                                            "祥云县",
                                            "宾川县",
                                            "弥渡县",
                                            "南涧彝族自治县",
                                            "巍山彝族回族自治县",
                                            "永平县",
                                            "云龙县",
                                            "洱源县",
                                            "剑川县",
                                            "鹤庆县"
                                        ]
                                    },
                                    {
                                        id: "c-5331",
                                        administrativeCode: "533100",
                                        name: "德宏傣族景颇族自治州",
                                        shortName: "德宏",
                                        locationId: "",
                                        districts: [
                                            "瑞丽市",
                                            "芒市",
                                            "梁河县",
                                            "盈江县",
                                            "陇川县"
                                        ]
                                    },
                                    {
                                        id: "c-5333",
                                        administrativeCode: "533300",
                                        name: "怒江傈僳族自治州",
                                        shortName: "怒江",
                                        locationId: "",
                                        districts: [
                                            "泸水市",
                                            "福贡县",
                                            "贡山独龙族怒族自治县",
                                            "兰坪白族普米族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-5334",
                                        administrativeCode: "533400",
                                        name: "迪庆藏族自治州",
                                        shortName: "迪庆",
                                        locationId: "",
                                        districts: [
                                            "香格里拉市",
                                            "德钦县",
                                            "维西傈僳族自治县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-54",
                                administrativeCode: "540000",
                                name: "西藏自治区",
                                cities: [
                                    {
                                        id: "c-5401",
                                        administrativeCode: "540100",
                                        name: "拉萨市",
                                        shortName: "拉萨",
                                        locationId: "",
                                        districts: [
                                            "城关区",
                                            "堆龙德庆区",
                                            "达孜区",
                                            "林周县",
                                            "当雄县",
                                            "尼木县",
                                            "曲水县",
                                            "墨竹工卡县"
                                        ]
                                    },
                                    {
                                        id: "c-5402",
                                        administrativeCode: "540200",
                                        name: "日喀则市",
                                        shortName: "日喀则",
                                        locationId: "",
                                        districts: [
                                            "桑珠孜区",
                                            "南木林县",
                                            "江孜县",
                                            "定日县",
                                            "萨迦县",
                                            "拉孜县",
                                            "昂仁县",
                                            "谢通门县",
                                            "白朗县",
                                            "仁布县",
                                            "康马县",
                                            "定结县",
                                            "仲巴县",
                                            "亚东县",
                                            "吉隆县",
                                            "聂拉木县",
                                            "萨嘎县",
                                            "岗巴县"
                                        ]
                                    },
                                    {
                                        id: "c-5403",
                                        administrativeCode: "540300",
                                        name: "昌都市",
                                        shortName: "昌都",
                                        locationId: "",
                                        districts: [
                                            "卡若区",
                                            "江达县",
                                            "贡觉县",
                                            "类乌齐县",
                                            "丁青县",
                                            "察雅县",
                                            "八宿县",
                                            "左贡县",
                                            "芒康县",
                                            "洛隆县",
                                            "边坝县"
                                        ]
                                    },
                                    {
                                        id: "c-5404",
                                        administrativeCode: "540400",
                                        name: "林芝市",
                                        shortName: "林芝",
                                        locationId: "",
                                        districts: [
                                            "巴宜区",
                                            "工布江达县",
                                            "墨脱县",
                                            "波密县",
                                            "察隅县",
                                            "朗县",
                                            "米林市"
                                        ]
                                    },
                                    {
                                        id: "c-5405",
                                        administrativeCode: "540500",
                                        name: "山南市",
                                        shortName: "山南",
                                        locationId: "",
                                        districts: [
                                            "乃东区",
                                            "扎囊县",
                                            "贡嘎县",
                                            "桑日县",
                                            "琼结县",
                                            "曲松县",
                                            "措美县",
                                            "洛扎县",
                                            "加查县",
                                            "隆子县",
                                            "浪卡子县",
                                            "错那市"
                                        ]
                                    },
                                    {
                                        id: "c-5406",
                                        administrativeCode: "540600",
                                        name: "那曲市",
                                        shortName: "那曲",
                                        locationId: "",
                                        districts: [
                                            "色尼区",
                                            "嘉黎县",
                                            "比如县",
                                            "聂荣县",
                                            "安多县",
                                            "申扎县",
                                            "索县",
                                            "班戈县",
                                            "巴青县",
                                            "尼玛县",
                                            "双湖县"
                                        ]
                                    },
                                    {
                                        id: "c-5425",
                                        administrativeCode: "542500",
                                        name: "阿里地区",
                                        shortName: "阿里",
                                        locationId: "",
                                        districts: [
                                            "普兰县",
                                            "札达县",
                                            "噶尔县",
                                            "日土县",
                                            "革吉县",
                                            "改则县",
                                            "措勤县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-61",
                                administrativeCode: "610000",
                                name: "陕西省",
                                cities: [
                                    {
                                        id: "c-6101",
                                        administrativeCode: "610100",
                                        name: "西安市",
                                        shortName: "西安",
                                        locationId: "",
                                        districts: [
                                            "新城区",
                                            "碑林区",
                                            "莲湖区",
                                            "灞桥区",
                                            "未央区",
                                            "雁塔区",
                                            "阎良区",
                                            "临潼区",
                                            "长安区",
                                            "高陵区",
                                            "鄠邑区",
                                            "蓝田县",
                                            "周至县"
                                        ]
                                    },
                                    {
                                        id: "c-6102",
                                        administrativeCode: "610200",
                                        name: "铜川市",
                                        shortName: "铜川",
                                        locationId: "",
                                        districts: [
                                            "王益区",
                                            "印台区",
                                            "耀州区",
                                            "宜君县"
                                        ]
                                    },
                                    {
                                        id: "c-6103",
                                        administrativeCode: "610300",
                                        name: "宝鸡市",
                                        shortName: "宝鸡",
                                        locationId: "",
                                        districts: [
                                            "渭滨区",
                                            "金台区",
                                            "陈仓区",
                                            "凤翔区",
                                            "岐山县",
                                            "扶风县",
                                            "眉县",
                                            "陇县",
                                            "千阳县",
                                            "麟游县",
                                            "凤县",
                                            "太白县"
                                        ]
                                    },
                                    {
                                        id: "c-6104",
                                        administrativeCode: "610400",
                                        name: "咸阳市",
                                        shortName: "咸阳",
                                        locationId: "",
                                        districts: [
                                            "秦都区",
                                            "杨陵区",
                                            "渭城区",
                                            "三原县",
                                            "泾阳县",
                                            "乾县",
                                            "礼泉县",
                                            "永寿县",
                                            "长武县",
                                            "旬邑县",
                                            "淳化县",
                                            "武功县",
                                            "兴平市",
                                            "彬州市"
                                        ]
                                    },
                                    {
                                        id: "c-6105",
                                        administrativeCode: "610500",
                                        name: "渭南市",
                                        shortName: "渭南",
                                        locationId: "",
                                        districts: [
                                            "临渭区",
                                            "华州区",
                                            "潼关县",
                                            "大荔县",
                                            "合阳县",
                                            "澄城县",
                                            "蒲城县",
                                            "白水县",
                                            "富平县",
                                            "韩城市",
                                            "华阴市"
                                        ]
                                    },
                                    {
                                        id: "c-6106",
                                        administrativeCode: "610600",
                                        name: "延安市",
                                        shortName: "延安",
                                        locationId: "",
                                        districts: [
                                            "宝塔区",
                                            "安塞区",
                                            "延长县",
                                            "延川县",
                                            "志丹县",
                                            "吴起县",
                                            "甘泉县",
                                            "富县",
                                            "洛川县",
                                            "宜川县",
                                            "黄龙县",
                                            "黄陵县",
                                            "子长市"
                                        ]
                                    },
                                    {
                                        id: "c-6107",
                                        administrativeCode: "610700",
                                        name: "汉中市",
                                        shortName: "汉中",
                                        locationId: "",
                                        districts: [
                                            "汉台区",
                                            "南郑区",
                                            "城固县",
                                            "洋县",
                                            "西乡县",
                                            "勉县",
                                            "宁强县",
                                            "略阳县",
                                            "镇巴县",
                                            "留坝县",
                                            "佛坪县"
                                        ]
                                    },
                                    {
                                        id: "c-6108",
                                        administrativeCode: "610800",
                                        name: "榆林市",
                                        shortName: "榆林",
                                        locationId: "",
                                        districts: [
                                            "榆阳区",
                                            "横山区",
                                            "府谷县",
                                            "靖边县",
                                            "定边县",
                                            "绥德县",
                                            "米脂县",
                                            "佳县",
                                            "吴堡县",
                                            "清涧县",
                                            "子洲县",
                                            "神木市"
                                        ]
                                    },
                                    {
                                        id: "c-6109",
                                        administrativeCode: "610900",
                                        name: "安康市",
                                        shortName: "安康",
                                        locationId: "",
                                        districts: [
                                            "汉滨区",
                                            "汉阴县",
                                            "石泉县",
                                            "宁陕县",
                                            "紫阳县",
                                            "岚皋县",
                                            "平利县",
                                            "镇坪县",
                                            "白河县",
                                            "旬阳市"
                                        ]
                                    },
                                    {
                                        id: "c-6110",
                                        administrativeCode: "611000",
                                        name: "商洛市",
                                        shortName: "商洛",
                                        locationId: "",
                                        districts: [
                                            "商州区",
                                            "洛南县",
                                            "丹凤县",
                                            "商南县",
                                            "山阳县",
                                            "镇安县",
                                            "柞水县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-62",
                                administrativeCode: "620000",
                                name: "甘肃省",
                                cities: [
                                    {
                                        id: "c-6201",
                                        administrativeCode: "620100",
                                        name: "兰州市",
                                        shortName: "兰州",
                                        locationId: "",
                                        districts: [
                                            "城关区",
                                            "七里河区",
                                            "西固区",
                                            "安宁区",
                                            "红古区",
                                            "永登县",
                                            "皋兰县",
                                            "榆中县"
                                        ]
                                    },
                                    {
                                        id: "c-6202",
                                        administrativeCode: "620200",
                                        name: "嘉峪关市",
                                        shortName: "嘉峪关",
                                        locationId: "",
                                        districts: [
                                            "嘉峪关市"
                                        ]
                                    },
                                    {
                                        id: "c-6203",
                                        administrativeCode: "620300",
                                        name: "金昌市",
                                        shortName: "金昌",
                                        locationId: "",
                                        districts: [
                                            "金川区",
                                            "永昌县"
                                        ]
                                    },
                                    {
                                        id: "c-6204",
                                        administrativeCode: "620400",
                                        name: "白银市",
                                        shortName: "白银",
                                        locationId: "",
                                        districts: [
                                            "白银区",
                                            "平川区",
                                            "靖远县",
                                            "会宁县",
                                            "景泰县"
                                        ]
                                    },
                                    {
                                        id: "c-6205",
                                        administrativeCode: "620500",
                                        name: "天水市",
                                        shortName: "天水",
                                        locationId: "",
                                        districts: [
                                            "秦州区",
                                            "麦积区",
                                            "清水县",
                                            "秦安县",
                                            "甘谷县",
                                            "武山县",
                                            "张家川回族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-6206",
                                        administrativeCode: "620600",
                                        name: "武威市",
                                        shortName: "武威",
                                        locationId: "",
                                        districts: [
                                            "凉州区",
                                            "民勤县",
                                            "古浪县",
                                            "天祝藏族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-6207",
                                        administrativeCode: "620700",
                                        name: "张掖市",
                                        shortName: "张掖",
                                        locationId: "",
                                        districts: [
                                            "甘州区",
                                            "肃南裕固族自治县",
                                            "民乐县",
                                            "临泽县",
                                            "高台县",
                                            "山丹县"
                                        ]
                                    },
                                    {
                                        id: "c-6208",
                                        administrativeCode: "620800",
                                        name: "平凉市",
                                        shortName: "平凉",
                                        locationId: "",
                                        districts: [
                                            "崆峒区",
                                            "泾川县",
                                            "灵台县",
                                            "崇信县",
                                            "庄浪县",
                                            "静宁县",
                                            "华亭市"
                                        ]
                                    },
                                    {
                                        id: "c-6209",
                                        administrativeCode: "620900",
                                        name: "酒泉市",
                                        shortName: "酒泉",
                                        locationId: "",
                                        districts: [
                                            "肃州区",
                                            "金塔县",
                                            "瓜州县",
                                            "肃北蒙古族自治县",
                                            "阿克塞哈萨克族自治县",
                                            "玉门市",
                                            "敦煌市"
                                        ]
                                    },
                                    {
                                        id: "c-6210",
                                        administrativeCode: "621000",
                                        name: "庆阳市",
                                        shortName: "庆阳",
                                        locationId: "",
                                        districts: [
                                            "西峰区",
                                            "庆城县",
                                            "环县",
                                            "华池县",
                                            "合水县",
                                            "正宁县",
                                            "宁县",
                                            "镇原县"
                                        ]
                                    },
                                    {
                                        id: "c-6211",
                                        administrativeCode: "621100",
                                        name: "定西市",
                                        shortName: "定西",
                                        locationId: "",
                                        districts: [
                                            "安定区",
                                            "通渭县",
                                            "陇西县",
                                            "渭源县",
                                            "临洮县",
                                            "漳县",
                                            "岷县"
                                        ]
                                    },
                                    {
                                        id: "c-6212",
                                        administrativeCode: "621200",
                                        name: "陇南市",
                                        shortName: "陇南",
                                        locationId: "",
                                        districts: [
                                            "武都区",
                                            "成县",
                                            "文县",
                                            "宕昌县",
                                            "康县",
                                            "西和县",
                                            "礼县",
                                            "徽县",
                                            "两当县"
                                        ]
                                    },
                                    {
                                        id: "c-6229",
                                        administrativeCode: "622900",
                                        name: "临夏回族自治州",
                                        shortName: "临夏",
                                        locationId: "",
                                        districts: [
                                            "临夏市",
                                            "临夏县",
                                            "康乐县",
                                            "永靖县",
                                            "广河县",
                                            "和政县",
                                            "东乡族自治县",
                                            "积石山保安族东乡族撒拉族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-6230",
                                        administrativeCode: "623000",
                                        name: "甘南藏族自治州",
                                        shortName: "甘南",
                                        locationId: "",
                                        districts: [
                                            "合作市",
                                            "临潭县",
                                            "卓尼县",
                                            "舟曲县",
                                            "迭部县",
                                            "玛曲县",
                                            "碌曲县",
                                            "夏河县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-63",
                                administrativeCode: "630000",
                                name: "青海省",
                                cities: [
                                    {
                                        id: "c-6301",
                                        administrativeCode: "630100",
                                        name: "西宁市",
                                        shortName: "西宁",
                                        locationId: "",
                                        districts: [
                                            "城东区",
                                            "城中区",
                                            "城西区",
                                            "城北区",
                                            "湟中区",
                                            "大通回族土族自治县",
                                            "湟源县"
                                        ]
                                    },
                                    {
                                        id: "c-6302",
                                        administrativeCode: "630200",
                                        name: "海东市",
                                        shortName: "海东",
                                        locationId: "",
                                        districts: [
                                            "乐都区",
                                            "平安区",
                                            "民和回族土族自治县",
                                            "互助土族自治县",
                                            "化隆回族自治县",
                                            "循化撒拉族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-6322",
                                        administrativeCode: "632200",
                                        name: "海北藏族自治州",
                                        shortName: "海北",
                                        locationId: "",
                                        districts: [
                                            "门源回族自治县",
                                            "祁连县",
                                            "海晏县",
                                            "刚察县"
                                        ]
                                    },
                                    {
                                        id: "c-6323",
                                        administrativeCode: "632300",
                                        name: "黄南藏族自治州",
                                        shortName: "黄南",
                                        locationId: "",
                                        districts: [
                                            "同仁市",
                                            "尖扎县",
                                            "泽库县",
                                            "河南蒙古族自治县"
                                        ]
                                    },
                                    {
                                        id: "c-6325",
                                        administrativeCode: "632500",
                                        name: "海南藏族自治州",
                                        shortName: "海南",
                                        locationId: "",
                                        districts: [
                                            "共和县",
                                            "同德县",
                                            "贵德县",
                                            "兴海县",
                                            "贵南县"
                                        ]
                                    },
                                    {
                                        id: "c-6326",
                                        administrativeCode: "632600",
                                        name: "果洛藏族自治州",
                                        shortName: "果洛",
                                        locationId: "",
                                        districts: [
                                            "玛沁县",
                                            "班玛县",
                                            "甘德县",
                                            "达日县",
                                            "久治县",
                                            "玛多县"
                                        ]
                                    },
                                    {
                                        id: "c-6327",
                                        administrativeCode: "632700",
                                        name: "玉树藏族自治州",
                                        shortName: "玉树",
                                        locationId: "",
                                        districts: [
                                            "玉树市",
                                            "杂多县",
                                            "称多县",
                                            "治多县",
                                            "囊谦县",
                                            "曲麻莱县"
                                        ]
                                    },
                                    {
                                        id: "c-6328",
                                        administrativeCode: "632800",
                                        name: "海西蒙古族藏族自治州",
                                        shortName: "海西",
                                        locationId: "",
                                        districts: [
                                            "格尔木市",
                                            "德令哈市",
                                            "茫崖市",
                                            "乌兰县",
                                            "都兰县",
                                            "天峻县",
                                            "大柴旦行政委员会"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-64",
                                administrativeCode: "640000",
                                name: "宁夏回族自治区",
                                cities: [
                                    {
                                        id: "c-6401",
                                        administrativeCode: "640100",
                                        name: "银川市",
                                        shortName: "银川",
                                        locationId: "",
                                        districts: [
                                            "兴庆区",
                                            "西夏区",
                                            "金凤区",
                                            "永宁县",
                                            "贺兰县",
                                            "灵武市"
                                        ]
                                    },
                                    {
                                        id: "c-6402",
                                        administrativeCode: "640200",
                                        name: "石嘴山市",
                                        shortName: "石嘴山",
                                        locationId: "",
                                        districts: [
                                            "大武口区",
                                            "惠农区",
                                            "平罗县"
                                        ]
                                    },
                                    {
                                        id: "c-6403",
                                        administrativeCode: "640300",
                                        name: "吴忠市",
                                        shortName: "吴忠",
                                        locationId: "",
                                        districts: [
                                            "利通区",
                                            "红寺堡区",
                                            "盐池县",
                                            "同心县",
                                            "青铜峡市"
                                        ]
                                    },
                                    {
                                        id: "c-6404",
                                        administrativeCode: "640400",
                                        name: "固原市",
                                        shortName: "固原",
                                        locationId: "",
                                        districts: [
                                            "原州区",
                                            "西吉县",
                                            "隆德县",
                                            "泾源县",
                                            "彭阳县"
                                        ]
                                    },
                                    {
                                        id: "c-6405",
                                        administrativeCode: "640500",
                                        name: "中卫市",
                                        shortName: "中卫",
                                        locationId: "",
                                        districts: [
                                            "沙坡头区",
                                            "中宁县",
                                            "海原县"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-65",
                                administrativeCode: "650000",
                                name: "新疆维吾尔自治区",
                                cities: [
                                    {
                                        id: "c-6501",
                                        administrativeCode: "650100",
                                        name: "乌鲁木齐市",
                                        shortName: "乌鲁木齐",
                                        locationId: "",
                                        districts: [
                                            "天山区",
                                            "沙依巴克区",
                                            "新市区",
                                            "水磨沟区",
                                            "头屯河区",
                                            "达坂城区",
                                            "米东区",
                                            "乌鲁木齐县"
                                        ]
                                    },
                                    {
                                        id: "c-6502",
                                        administrativeCode: "650200",
                                        name: "克拉玛依市",
                                        shortName: "克拉玛依",
                                        locationId: "",
                                        districts: [
                                            "独山子区",
                                            "克拉玛依区",
                                            "白碱滩区",
                                            "乌尔禾区"
                                        ]
                                    },
                                    {
                                        id: "c-6504",
                                        administrativeCode: "650400",
                                        name: "吐鲁番市",
                                        shortName: "吐鲁番",
                                        locationId: "",
                                        districts: [
                                            "高昌区",
                                            "鄯善县",
                                            "托克逊县"
                                        ]
                                    },
                                    {
                                        id: "c-6505",
                                        administrativeCode: "650500",
                                        name: "哈密市",
                                        shortName: "哈密",
                                        locationId: "",
                                        districts: [
                                            "伊州区",
                                            "巴里坤哈萨克自治县",
                                            "伊吾县"
                                        ]
                                    },
                                    {
                                        id: "c-6523",
                                        administrativeCode: "652300",
                                        name: "昌吉回族自治州",
                                        shortName: "昌吉",
                                        locationId: "",
                                        districts: [
                                            "昌吉市",
                                            "阜康市",
                                            "呼图壁县",
                                            "玛纳斯县",
                                            "奇台县",
                                            "吉木萨尔县",
                                            "木垒哈萨克自治县"
                                        ]
                                    },
                                    {
                                        id: "c-6527",
                                        administrativeCode: "652700",
                                        name: "博尔塔拉蒙古自治州",
                                        shortName: "博尔塔拉",
                                        locationId: "",
                                        districts: [
                                            "博乐市",
                                            "阿拉山口市",
                                            "精河县",
                                            "温泉县"
                                        ]
                                    },
                                    {
                                        id: "c-6528",
                                        administrativeCode: "652800",
                                        name: "巴音郭楞蒙古自治州",
                                        shortName: "巴音郭楞",
                                        locationId: "",
                                        districts: [
                                            "库尔勒市",
                                            "轮台县",
                                            "尉犁县",
                                            "若羌县",
                                            "且末县",
                                            "焉耆回族自治县",
                                            "和静县",
                                            "和硕县",
                                            "博湖县"
                                        ]
                                    },
                                    {
                                        id: "c-6529",
                                        administrativeCode: "652900",
                                        name: "阿克苏地区",
                                        shortName: "阿克苏",
                                        locationId: "",
                                        districts: [
                                            "阿克苏市",
                                            "库车市",
                                            "温宿县",
                                            "沙雅县",
                                            "新和县",
                                            "拜城县",
                                            "乌什县",
                                            "阿瓦提县",
                                            "柯坪县"
                                        ]
                                    },
                                    {
                                        id: "c-6530",
                                        administrativeCode: "653000",
                                        name: "克孜勒苏柯尔克孜自治州",
                                        shortName: "克孜勒苏",
                                        locationId: "",
                                        districts: [
                                            "阿图什市",
                                            "阿克陶县",
                                            "阿合奇县",
                                            "乌恰县"
                                        ]
                                    },
                                    {
                                        id: "c-6531",
                                        administrativeCode: "653100",
                                        name: "喀什地区",
                                        shortName: "喀什",
                                        locationId: "",
                                        districts: [
                                            "喀什市",
                                            "疏附县",
                                            "疏勒县",
                                            "英吉沙县",
                                            "泽普县",
                                            "莎车县",
                                            "叶城县",
                                            "麦盖提县",
                                            "岳普湖县",
                                            "伽师县",
                                            "巴楚县",
                                            "塔什库尔干塔吉克自治县"
                                        ]
                                    },
                                    {
                                        id: "c-6532",
                                        administrativeCode: "653200",
                                        name: "和田地区",
                                        shortName: "和田",
                                        locationId: "",
                                        districts: [
                                            "和田市",
                                            "和田县",
                                            "墨玉县",
                                            "皮山县",
                                            "洛浦县",
                                            "策勒县",
                                            "于田县",
                                            "民丰县"
                                        ]
                                    },
                                    {
                                        id: "c-6540",
                                        administrativeCode: "654000",
                                        name: "伊犁哈萨克自治州",
                                        shortName: "伊犁",
                                        locationId: "",
                                        districts: [
                                            "伊宁市",
                                            "奎屯市",
                                            "霍尔果斯市",
                                            "伊宁县",
                                            "察布查尔锡伯自治县",
                                            "霍城县",
                                            "巩留县",
                                            "新源县",
                                            "昭苏县",
                                            "特克斯县",
                                            "尼勒克县"
                                        ]
                                    },
                                    {
                                        id: "c-6542",
                                        administrativeCode: "654200",
                                        name: "塔城地区",
                                        shortName: "塔城",
                                        locationId: "",
                                        districts: [
                                            "塔城市",
                                            "乌苏市",
                                            "沙湾市",
                                            "额敏县",
                                            "托里县",
                                            "裕民县",
                                            "和布克赛尔蒙古自治县"
                                        ]
                                    },
                                    {
                                        id: "c-6543",
                                        administrativeCode: "654300",
                                        name: "阿勒泰地区",
                                        shortName: "阿勒泰",
                                        locationId: "",
                                        districts: [
                                            "阿勒泰市",
                                            "布尔津县",
                                            "富蕴县",
                                            "福海县",
                                            "哈巴河县",
                                            "青河县",
                                            "吉木乃县"
                                        ]
                                    },
                                    {
                                        id: "c-659001",
                                        administrativeCode: "659001",
                                        name: "石河子市",
                                        shortName: "石河子",
                                        locationId: "",
                                        districts: [
                                            "石河子市"
                                        ]
                                    },
                                    {
                                        id: "c-659002",
                                        administrativeCode: "659002",
                                        name: "阿拉尔市",
                                        shortName: "阿拉尔",
                                        locationId: "",
                                        districts: [
                                            "阿拉尔市"
                                        ]
                                    },
                                    {
                                        id: "c-659003",
                                        administrativeCode: "659003",
                                        name: "图木舒克市",
                                        shortName: "图木舒克",
                                        locationId: "",
                                        districts: [
                                            "图木舒克市"
                                        ]
                                    },
                                    {
                                        id: "c-659004",
                                        administrativeCode: "659004",
                                        name: "五家渠市",
                                        shortName: "五家渠",
                                        locationId: "",
                                        districts: [
                                            "五家渠市"
                                        ]
                                    },
                                    {
                                        id: "c-659005",
                                        administrativeCode: "659005",
                                        name: "北屯市",
                                        shortName: "北屯",
                                        locationId: "",
                                        districts: [
                                            "北屯市"
                                        ]
                                    },
                                    {
                                        id: "c-659006",
                                        administrativeCode: "659006",
                                        name: "铁门关市",
                                        shortName: "铁门关",
                                        locationId: "",
                                        districts: [
                                            "铁门关市"
                                        ]
                                    },
                                    {
                                        id: "c-659007",
                                        administrativeCode: "659007",
                                        name: "双河市",
                                        shortName: "双河",
                                        locationId: "",
                                        districts: [
                                            "双河市"
                                        ]
                                    },
                                    {
                                        id: "c-659008",
                                        administrativeCode: "659008",
                                        name: "可克达拉市",
                                        shortName: "可克达拉",
                                        locationId: "",
                                        districts: [
                                            "可克达拉市"
                                        ]
                                    },
                                    {
                                        id: "c-659009",
                                        administrativeCode: "659009",
                                        name: "昆玉市",
                                        shortName: "昆玉",
                                        locationId: "",
                                        districts: [
                                            "昆玉市"
                                        ]
                                    },
                                    {
                                        id: "c-659010",
                                        administrativeCode: "659010",
                                        name: "胡杨河市",
                                        shortName: "胡杨河",
                                        locationId: "",
                                        districts: [
                                            "胡杨河市"
                                        ]
                                    },
                                    {
                                        id: "c-659011",
                                        administrativeCode: "659011",
                                        name: "新星市",
                                        shortName: "新星",
                                        locationId: "",
                                        districts: [
                                            "新星市"
                                        ]
                                    },
                                    {
                                        id: "c-659012",
                                        administrativeCode: "659012",
                                        name: "白杨市",
                                        shortName: "白杨",
                                        locationId: "",
                                        districts: [
                                            "白杨市"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-71",
                                administrativeCode: "710000",
                                name: "台湾省",
                                cities: [
                                    {
                                        id: "c-7101",
                                        administrativeCode: "710100",
                                        name: "台北市",
                                        shortName: "台北",
                                        locationId: "",
                                        districts: [
                                            "中正区",
                                            "大同区",
                                            "中山区",
                                            "松山区",
                                            "大安区",
                                            "万华区",
                                            "信义区",
                                            "士林区",
                                            "北投区",
                                            "内湖区",
                                            "南港区",
                                            "文山区"
                                        ]
                                    },
                                    {
                                        id: "c-7102",
                                        administrativeCode: "710200",
                                        name: "高雄市",
                                        shortName: "高雄",
                                        locationId: "",
                                        districts: [
                                            "新兴区",
                                            "前金区",
                                            "苓雅区",
                                            "盐埕区",
                                            "鼓山区",
                                            "旗津区",
                                            "前镇区",
                                            "三民区",
                                            "左营区",
                                            "楠梓区",
                                            "小港区",
                                            "仁武区",
                                            "大社区",
                                            "冈山区",
                                            "路竹区",
                                            "阿莲区",
                                            "田寮区",
                                            "燕巢区",
                                            "桥头区",
                                            "梓官区",
                                            "弥陀区",
                                            "永安区",
                                            "湖内区",
                                            "凤山区",
                                            "大寮区",
                                            "林园区",
                                            "鸟松区",
                                            "大树区",
                                            "旗山区",
                                            "美浓区",
                                            "六龟区",
                                            "内门区",
                                            "杉林区",
                                            "甲仙区",
                                            "桃源区",
                                            "那玛夏区",
                                            "茂林区",
                                            "茄萣区"
                                        ]
                                    },
                                    {
                                        id: "c-7103",
                                        administrativeCode: "710300",
                                        name: "台南市",
                                        shortName: "台南",
                                        locationId: "",
                                        districts: [
                                            "中西区",
                                            "东区",
                                            "南区",
                                            "北区",
                                            "安平区",
                                            "安南区",
                                            "永康区",
                                            "归仁区",
                                            "新化区",
                                            "左镇区",
                                            "玉井区",
                                            "楠西区",
                                            "南化区",
                                            "仁德区",
                                            "关庙区",
                                            "龙崎区",
                                            "官田区",
                                            "麻豆区",
                                            "佳里区",
                                            "西港区",
                                            "七股区",
                                            "将军区",
                                            "学甲区",
                                            "北门区",
                                            "新营区",
                                            "后壁区",
                                            "白河区",
                                            "东山区",
                                            "六甲区",
                                            "下营区",
                                            "柳营区",
                                            "盐水区",
                                            "善化区",
                                            "大内区",
                                            "山上区",
                                            "新市区",
                                            "安定区"
                                        ]
                                    },
                                    {
                                        id: "c-7104",
                                        administrativeCode: "710400",
                                        name: "台中市",
                                        shortName: "台中",
                                        locationId: "",
                                        districts: [
                                            "中区",
                                            "东区",
                                            "南区",
                                            "西区",
                                            "北区",
                                            "北屯区",
                                            "西屯区",
                                            "南屯区",
                                            "太平区",
                                            "大里区",
                                            "雾峰区",
                                            "乌日区",
                                            "丰原区",
                                            "后里区",
                                            "石冈区",
                                            "东势区",
                                            "和平区",
                                            "新社区",
                                            "潭子区",
                                            "大雅区",
                                            "神冈区",
                                            "大肚区",
                                            "沙鹿区",
                                            "龙井区",
                                            "梧栖区",
                                            "清水区",
                                            "大甲区",
                                            "外埔区",
                                            "大安区"
                                        ]
                                    },
                                    {
                                        id: "c-7106",
                                        administrativeCode: "710600",
                                        name: "南投县",
                                        shortName: "南投",
                                        locationId: "",
                                        districts: [
                                            "南投市",
                                            "中寮乡",
                                            "草屯镇",
                                            "国姓乡",
                                            "埔里镇",
                                            "仁爱乡",
                                            "名间乡",
                                            "集集镇",
                                            "水里乡",
                                            "鱼池乡",
                                            "信义乡",
                                            "竹山镇",
                                            "鹿谷乡"
                                        ]
                                    },
                                    {
                                        id: "c-7107",
                                        administrativeCode: "710700",
                                        name: "基隆市",
                                        shortName: "基隆",
                                        locationId: "",
                                        districts: [
                                            "仁爱区",
                                            "信义区",
                                            "中正区",
                                            "中山区",
                                            "安乐区",
                                            "暖暖区",
                                            "七堵区"
                                        ]
                                    },
                                    {
                                        id: "c-7108",
                                        administrativeCode: "710800",
                                        name: "新竹市",
                                        shortName: "新竹市",
                                        locationId: "",
                                        districts: [
                                            "东区",
                                            "北区",
                                            "香山区"
                                        ]
                                    },
                                    {
                                        id: "c-7109",
                                        administrativeCode: "710900",
                                        name: "嘉义市",
                                        shortName: "嘉义市",
                                        locationId: "",
                                        districts: [
                                            "东区",
                                            "西区"
                                        ]
                                    },
                                    {
                                        id: "c-7111",
                                        administrativeCode: "711100",
                                        name: "新北市",
                                        shortName: "新北",
                                        locationId: "",
                                        districts: [
                                            "万里区",
                                            "金山区",
                                            "板桥区",
                                            "汐止区",
                                            "深坑区",
                                            "石碇区",
                                            "瑞芳区",
                                            "平溪区",
                                            "双溪区",
                                            "贡寮区",
                                            "新店区",
                                            "坪林区",
                                            "乌来区",
                                            "永和区",
                                            "中和区",
                                            "土城区",
                                            "三峡区",
                                            "树林区",
                                            "莺歌区",
                                            "三重区",
                                            "新庄区",
                                            "泰山区",
                                            "林口区",
                                            "芦洲区",
                                            "五股区",
                                            "八里区",
                                            "淡水区",
                                            "三芝区",
                                            "石门区"
                                        ]
                                    },
                                    {
                                        id: "c-7112",
                                        administrativeCode: "711200",
                                        name: "宜兰县",
                                        shortName: "宜兰",
                                        locationId: "",
                                        districts: [
                                            "宜兰市",
                                            "头城镇",
                                            "礁溪乡",
                                            "壮围乡",
                                            "员山乡",
                                            "罗东镇",
                                            "三星乡",
                                            "大同乡",
                                            "五结乡",
                                            "冬山乡",
                                            "苏澳镇",
                                            "南澳乡"
                                        ]
                                    },
                                    {
                                        id: "c-7113",
                                        administrativeCode: "711300",
                                        name: "新竹县",
                                        shortName: "新竹县",
                                        locationId: "",
                                        districts: [
                                            "竹北市",
                                            "湖口乡",
                                            "新丰乡",
                                            "新埔镇",
                                            "关西镇",
                                            "芎林乡",
                                            "宝山乡",
                                            "竹东镇",
                                            "五峰乡",
                                            "横山乡",
                                            "尖石乡",
                                            "北埔乡",
                                            "峨眉乡"
                                        ]
                                    },
                                    {
                                        id: "c-7114",
                                        administrativeCode: "711400",
                                        name: "桃园市",
                                        shortName: "桃园",
                                        locationId: "",
                                        districts: [
                                            "中坜区",
                                            "平镇区",
                                            "龙潭区",
                                            "杨梅区",
                                            "新屋区",
                                            "观音区",
                                            "桃园区",
                                            "龟山区",
                                            "八德区",
                                            "大溪区",
                                            "复兴区",
                                            "大园区",
                                            "芦竹区"
                                        ]
                                    },
                                    {
                                        id: "c-7115",
                                        administrativeCode: "711500",
                                        name: "苗栗县",
                                        shortName: "苗栗",
                                        locationId: "",
                                        districts: [
                                            "竹南镇",
                                            "头份市",
                                            "三湾乡",
                                            "南庄乡",
                                            "狮潭乡",
                                            "后龙镇",
                                            "通霄镇",
                                            "苑里镇",
                                            "苗栗市",
                                            "造桥乡",
                                            "头屋乡",
                                            "公馆乡",
                                            "大湖乡",
                                            "泰安乡",
                                            "铜锣乡",
                                            "三义乡",
                                            "西湖乡",
                                            "卓兰镇"
                                        ]
                                    },
                                    {
                                        id: "c-7117",
                                        administrativeCode: "711700",
                                        name: "彰化县",
                                        shortName: "彰化",
                                        locationId: "",
                                        districts: [
                                            "彰化市",
                                            "芬园乡",
                                            "花坛乡",
                                            "秀水乡",
                                            "鹿港镇",
                                            "福兴乡",
                                            "线西乡",
                                            "和美镇",
                                            "伸港乡",
                                            "员林市",
                                            "社头乡",
                                            "永靖乡",
                                            "埔心乡",
                                            "溪湖镇",
                                            "大村乡",
                                            "埔盐乡",
                                            "田中镇",
                                            "北斗镇",
                                            "田尾乡",
                                            "埤头乡",
                                            "溪州乡",
                                            "竹塘乡",
                                            "二林镇",
                                            "大城乡",
                                            "芳苑乡",
                                            "二水乡"
                                        ]
                                    },
                                    {
                                        id: "c-7119",
                                        administrativeCode: "711900",
                                        name: "嘉义县",
                                        shortName: "嘉义县",
                                        locationId: "",
                                        districts: [
                                            "番路乡",
                                            "梅山乡",
                                            "竹崎乡",
                                            "阿里山乡",
                                            "中埔乡",
                                            "大埔乡",
                                            "水上乡",
                                            "鹿草乡",
                                            "太保市",
                                            "朴子市",
                                            "东石乡",
                                            "六脚乡",
                                            "新港乡",
                                            "民雄乡",
                                            "大林镇",
                                            "溪口乡",
                                            "义竹乡",
                                            "布袋镇"
                                        ]
                                    },
                                    {
                                        id: "c-7121",
                                        administrativeCode: "712100",
                                        name: "云林县",
                                        shortName: "云林",
                                        locationId: "",
                                        districts: [
                                            "斗南镇",
                                            "大埤乡",
                                            "虎尾镇",
                                            "土库镇",
                                            "褒忠乡",
                                            "东势乡",
                                            "台西乡",
                                            "仑背乡",
                                            "麦寮乡",
                                            "斗六市",
                                            "林内乡",
                                            "古坑乡",
                                            "莿桐乡",
                                            "西螺镇",
                                            "二仑乡",
                                            "北港镇",
                                            "水林乡",
                                            "口湖乡",
                                            "四湖乡",
                                            "元长乡"
                                        ]
                                    },
                                    {
                                        id: "c-7124",
                                        administrativeCode: "712400",
                                        name: "屏东县",
                                        shortName: "屏东",
                                        locationId: "",
                                        districts: [
                                            "屏东市",
                                            "三地门乡",
                                            "雾台乡",
                                            "玛家乡",
                                            "九如乡",
                                            "里港乡",
                                            "高树乡",
                                            "盐埔乡",
                                            "长治乡",
                                            "麟洛乡",
                                            "竹田乡",
                                            "内埔乡",
                                            "万丹乡",
                                            "潮州镇",
                                            "泰武乡",
                                            "来义乡",
                                            "万峦乡",
                                            "崁顶乡",
                                            "新埤乡",
                                            "南州乡",
                                            "林边乡",
                                            "东港镇",
                                            "琉球乡",
                                            "佳冬乡",
                                            "新园乡",
                                            "枋寮乡",
                                            "枋山乡",
                                            "春日乡",
                                            "狮子乡",
                                            "车城乡",
                                            "牡丹乡",
                                            "恒春镇",
                                            "满州乡"
                                        ]
                                    },
                                    {
                                        id: "c-7125",
                                        administrativeCode: "712500",
                                        name: "台东县",
                                        shortName: "台东",
                                        locationId: "",
                                        districts: [
                                            "台东市",
                                            "绿岛乡",
                                            "兰屿乡",
                                            "延平乡",
                                            "卑南乡",
                                            "鹿野乡",
                                            "关山镇",
                                            "海端乡",
                                            "池上乡",
                                            "东河乡",
                                            "成功镇",
                                            "长滨乡",
                                            "金峰乡",
                                            "大武乡",
                                            "达仁乡",
                                            "太麻里乡"
                                        ]
                                    },
                                    {
                                        id: "c-7126",
                                        administrativeCode: "712600",
                                        name: "花莲县",
                                        shortName: "花莲",
                                        locationId: "",
                                        districts: [
                                            "花莲市",
                                            "新城乡",
                                            "秀林乡",
                                            "吉安乡",
                                            "寿丰乡",
                                            "凤林镇",
                                            "光复乡",
                                            "丰滨乡",
                                            "瑞穗乡",
                                            "万荣乡",
                                            "玉里镇",
                                            "卓溪乡",
                                            "富里乡"
                                        ]
                                    },
                                    {
                                        id: "c-7127",
                                        administrativeCode: "712700",
                                        name: "澎湖县",
                                        shortName: "澎湖",
                                        locationId: "",
                                        districts: [
                                            "马公市",
                                            "西屿乡",
                                            "望安乡",
                                            "七美乡",
                                            "白沙乡",
                                            "湖西乡"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-81",
                                administrativeCode: "810000",
                                name: "香港特别行政区",
                                cities: [
                                    {
                                        id: "c-8100",
                                        administrativeCode: "810000",
                                        name: "香港特别行政区",
                                        shortName: "香港",
                                        locationId: "",
                                        districts: [
                                            "香港特别行政区"
                                        ]
                                    }
                                ]
                            },
                            {
                                id: "p-82",
                                administrativeCode: "820000",
                                name: "澳门特别行政区",
                                cities: [
                                    {
                                        id: "c-8200",
                                        administrativeCode: "820000",
                                        name: "澳门特别行政区",
                                        shortName: "澳门",
                                        locationId: "",
                                        districts: [
                                            "澳门特别行政区"
                                        ]
                                    }
                                ]
                            }
                        ];
                        function copyOption(id, name, selected) {
                            return {
                                id: id,
                                name: name,
                                selected: selected,
                                itemClass: selected ? "picker-item picker-item-selected" : "picker-item"
                            };
                        }
                        function getProvinceOptions(selectedId) {
                            const options = [];
                            for(let index = 0; index < PROVINCES.length; index += 1){
                                const province = PROVINCES[index];
                                options.push(copyOption(province.id, province.name, province.id === selectedId));
                            }
                            return options;
                        }
                        function getProvince(provinceId) {
                            for(let index = 0; index < PROVINCES.length; index += 1)if (PROVINCES[index].id === provinceId) return PROVINCES[index];
                            return PROVINCES[0];
                        }
                        function getCityOptions(provinceId, selectedId) {
                            const province = getProvince(provinceId);
                            const options = [];
                            for(let index = 0; index < province.cities.length; index += 1){
                                const city = province.cities[index];
                                options.push(copyOption(city.id, city.name, city.id === selectedId));
                            }
                            return options;
                        }
                        function getCityOption(provinceId, cityId) {
                            const province = getProvince(provinceId);
                            for(let index = 0; index < province.cities.length; index += 1)if (province.cities[index].id === cityId) return province.cities[index];
                            return province.cities[0];
                        }
                        function getDistrictOptions(provinceId, cityId, selectedName) {
                            const city = getCityOption(provinceId, cityId);
                            const options = [
                                copyOption("", "不选择", "" === selectedName)
                            ];
                            for(let index = 0; index < city.districts.length; index += 1){
                                const district = city.districts[index];
                                options.push(copyOption(district, district, district === selectedName));
                            }
                            return options;
                        }
                        function buildCustomCity(provinceId, cityId, districtName) {
                            const province = getProvince(provinceId);
                            const city = getCityOption(provinceId, cityId);
                            const hasDistrict = !!districtName;
                            const cityAndProvince = city.name === province.name ? province.name : city.name + " · " + province.name;
                            return {
                                id: "custom-" + city.id + (hasDistrict ? "-" + districtName : ""),
                                locationId: hasDistrict ? "" : city.locationId,
                                administrativeCode: city.administrativeCode,
                                weatherKey: city.id,
                                weatherLocationName: hasDistrict ? districtName : city.shortName,
                                weatherAdmName: hasDistrict ? city.shortName : province.name,
                                weatherProvinceName: province.name,
                                weatherCityName: city.shortName,
                                name: hasDistrict ? districtName : city.shortName,
                                detailName: hasDistrict ? districtName : city.name,
                                administrativeArea: hasDistrict ? cityAndProvince : province.name,
                                country: "中国",
                                isBase: false
                            };
                        }
                        function normalizeAdministrativeName(value) {
                            return String(value || "").replace(/s+/g, "").replace(/特别行政区$/, "").replace(/自治州$/, "").replace(/自治县$/, "").replace(/自治区$/, "").replace(/地区$/, "").replace(/林区$/, "").replace(/[省市区县盟旗镇乡]$/, "");
                        }
                        function administrativeNameMatches(left, right) {
                            const normalizedLeft = normalizeAdministrativeName(left);
                            const normalizedRight = normalizeAdministrativeName(right);
                            return !!normalizedLeft && !!normalizedRight && (normalizedLeft === normalizedRight || 0 === normalizedLeft.indexOf(normalizedRight) || 0 === normalizedRight.indexOf(normalizedLeft));
                        }
                        function resolveAdministrativeLocation(locationName, adm2, adm1) {
                            let matchedProvince = null;
                            let matchedCity = null;
                            let matchedDistrict = "";
                            for(let provinceIndex = 0; provinceIndex < PROVINCES.length; provinceIndex += 1){
                                const province = PROVINCES[provinceIndex];
                                if (administrativeNameMatches(province.name, adm1)) {
                                    matchedProvince = province;
                                    break;
                                }
                            }
                            const provinces = matchedProvince ? [
                                matchedProvince
                            ] : PROVINCES;
                            for(let provinceIndex = 0; provinceIndex < provinces.length; provinceIndex += 1){
                                const province = provinces[provinceIndex];
                                for(let cityIndex = 0; cityIndex < province.cities.length; cityIndex += 1){
                                    const city = province.cities[cityIndex];
                                    if (administrativeNameMatches(city.name, adm2) || administrativeNameMatches(city.shortName, adm2)) {
                                        matchedProvince = province;
                                        matchedCity = city;
                                        break;
                                    }
                                }
                                if (matchedCity) break;
                            }
                            const districtProvinces = matchedProvince ? [
                                matchedProvince
                            ] : PROVINCES;
                            for(let provinceIndex = 0; provinceIndex < districtProvinces.length; provinceIndex += 1){
                                const province = districtProvinces[provinceIndex];
                                const cities = matchedCity ? [
                                    matchedCity
                                ] : province.cities;
                                for(let cityIndex = 0; cityIndex < cities.length; cityIndex += 1){
                                    const city = cities[cityIndex];
                                    for(let districtIndex = 0; districtIndex < city.districts.length; districtIndex += 1){
                                        const district = city.districts[districtIndex];
                                        if (administrativeNameMatches(district, locationName)) {
                                            matchedProvince = province;
                                            matchedCity = city;
                                            matchedDistrict = district;
                                            break;
                                        }
                                    }
                                    if (matchedDistrict) break;
                                }
                                if (matchedDistrict) break;
                            }
                            return {
                                name: matchedDistrict || locationName || (matchedCity ? matchedCity.name : adm2),
                                cityName: matchedCity ? matchedCity.name : adm2 || locationName,
                                provinceName: matchedProvince ? matchedProvince.name : adm1
                            };
                        }
                    },
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
                                top: 0,
                                backgroundColor: "rgba(255, 255, 255, 0.12)"
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
                                width: "210px",
                                height: "54px",
                                position: "absolute",
                                left: "27px",
                                top: "22px",
                                fontSize: "29px",
                                fontWeight: "bold",
                                textAlign: "left"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "page-time"
                                ]
                            ],
                            {
                                width: "96px",
                                height: "38px",
                                position: "absolute",
                                left: "268px",
                                top: "29px",
                                fontSize: "19px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "add-page"
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
                                    "picker-label"
                                ]
                            ],
                            {
                                width: "122px",
                                height: "32px",
                                position: "absolute",
                                top: "68px",
                                fontSize: "19px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "picker-label-province"
                                ]
                            ],
                            {
                                left: "22px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "picker-label-city"
                                ]
                            ],
                            {
                                left: "155px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "picker-label-district"
                                ]
                            ],
                            {
                                left: "288px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "picker-list"
                                ]
                            ],
                            {
                                width: "122px",
                                height: "225px",
                                position: "absolute",
                                top: "101px",
                                borderTopWidth: "1px",
                                borderRightWidth: "1px",
                                borderBottomWidth: "1px",
                                borderLeftWidth: "1px",
                                borderTopColor: "rgba(220,224,232,0.95)",
                                borderRightColor: "rgba(220,224,232,0.95)",
                                borderBottomColor: "rgba(220,224,232,0.95)",
                                borderLeftColor: "rgba(220,224,232,0.95)",
                                borderRadius: "15px",
                                backgroundColor: "rgba(255, 255, 255, 0.95)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "picker-province"
                                ]
                            ],
                            {
                                left: "22px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "picker-city"
                                ]
                            ],
                            {
                                left: "155px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "picker-district"
                                ]
                            ],
                            {
                                left: "288px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "picker-item"
                                ]
                            ],
                            {
                                width: "120px",
                                height: "52px",
                                borderBottomWidth: "1px",
                                borderTopColor: "#e5e9f0",
                                borderRightColor: "#e5e9f0",
                                borderBottomColor: "#e5e9f0",
                                borderLeftColor: "#e5e9f0",
                                backgroundColor: "rgba(255, 255, 255, 0.96)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "picker-item-selected"
                                ]
                            ],
                            {
                                backgroundColor: "#dcecff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "picker-text"
                                ]
                            ],
                            {
                                width: "120px",
                                height: "52px",
                                color: "#536078",
                                fontSize: "17px",
                                lineHeight: "50px",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "picker-text-selected"
                                ]
                            ],
                            {
                                color: "#176fd1",
                                fontWeight: "bold"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "add-summary"
                                ]
                            ],
                            {
                                width: "380px",
                                height: "32px",
                                position: "absolute",
                                left: "26px",
                                top: "337px",
                                fontSize: "17px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "add-city-button"
                                ]
                            ],
                            {
                                width: "244px",
                                height: "52px",
                                position: "absolute",
                                left: "94px",
                                top: "374px",
                                borderRadius: "17px",
                                backgroundColor: "#2385ee"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "add-city-button-text"
                                ]
                            ],
                            {
                                width: "244px",
                                height: "52px",
                                color: "#ffffff",
                                fontSize: "21px",
                                fontWeight: "bold",
                                lineHeight: "50px",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "add-status"
                                ]
                            ],
                            {
                                width: "350px",
                                height: "30px",
                                position: "absolute",
                                left: "41px",
                                top: "433px",
                                fontSize: "16px",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "city-card"
                                ]
                            ],
                            {
                                width: "376px",
                                height: "70px",
                                position: "absolute",
                                left: "28px",
                                borderTopWidth: "1px",
                                borderRightWidth: "1px",
                                borderBottomWidth: "1px",
                                borderLeftWidth: "1px",
                                borderTopColor: "rgba(226,226,226,0.8)",
                                borderRightColor: "rgba(226,226,226,0.8)",
                                borderBottomColor: "rgba(226,226,226,0.8)",
                                borderLeftColor: "rgba(226,226,226,0.8)",
                                borderRadius: "17px",
                                backgroundColor: "rgba(255, 255, 255, 0.94)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "card-1"
                                ]
                            ],
                            {
                                top: "84px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "card-2"
                                ]
                            ],
                            {
                                top: "168px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "card-3"
                                ]
                            ],
                            {
                                top: "251px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "card-4"
                                ]
                            ],
                            {
                                top: "334px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "card-selected"
                                ]
                            ],
                            {
                                backgroundColor: "rgba(255, 255, 255, 0.98)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "city-name"
                                ]
                            ],
                            {
                                width: "215px",
                                height: "42px",
                                position: "absolute",
                                left: "27px",
                                top: "13px",
                                color: "#102758",
                                fontSize: "27px",
                                fontWeight: "bold",
                                textAlign: "left"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "current-location-badge"
                                ]
                            ],
                            {
                                width: "70px",
                                height: "28px",
                                position: "absolute",
                                right: "67px",
                                top: "20px",
                                color: "#287bd5",
                                fontSize: "14px",
                                fontWeight: "bold",
                                lineHeight: "26px",
                                textAlign: "center",
                                borderTopWidth: "1px",
                                borderRightWidth: "1px",
                                borderBottomWidth: "1px",
                                borderLeftWidth: "1px",
                                borderTopColor: "#9bc8f5",
                                borderRightColor: "#9bc8f5",
                                borderBottomColor: "#9bc8f5",
                                borderLeftColor: "#9bc8f5",
                                borderRadius: "10px",
                                backgroundColor: "#eaf5ff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "delete-button"
                                ]
                            ],
                            {
                                width: "58px",
                                height: "36px",
                                position: "absolute",
                                right: "66px",
                                top: "16px",
                                borderTopWidth: "1px",
                                borderRightWidth: "1px",
                                borderBottomWidth: "1px",
                                borderLeftWidth: "1px",
                                borderTopColor: "#f0b7b7",
                                borderRightColor: "#f0b7b7",
                                borderBottomColor: "#f0b7b7",
                                borderLeftColor: "#f0b7b7",
                                borderRadius: "12px",
                                backgroundColor: "#fff2f2"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "delete-button-text"
                                ]
                            ],
                            {
                                width: "58px",
                                height: "36px",
                                color: "#d74a4a",
                                fontSize: "17px",
                                fontWeight: "bold",
                                lineHeight: "34px",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "radio"
                                ]
                            ],
                            {
                                width: "28px",
                                height: "28px",
                                position: "absolute",
                                right: "27px",
                                top: "20px",
                                borderTopWidth: "2px",
                                borderRightWidth: "2px",
                                borderBottomWidth: "2px",
                                borderLeftWidth: "2px",
                                borderTopColor: "#a7a7a7",
                                borderRightColor: "#a7a7a7",
                                borderBottomColor: "#a7a7a7",
                                borderLeftColor: "#a7a7a7",
                                borderRadius: "50%",
                                backgroundColor: "#ffffff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "radio-selected"
                                ]
                            ],
                            {
                                borderTopWidth: "3px",
                                borderRightWidth: "3px",
                                borderBottomWidth: "3px",
                                borderLeftWidth: "3px",
                                borderTopColor: "#1684ff",
                                borderRightColor: "#1684ff",
                                borderBottomColor: "#1684ff",
                                borderLeftColor: "#1684ff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "radio-core"
                                ]
                            ],
                            {
                                width: "14px",
                                height: "14px",
                                position: "absolute",
                                left: "4px",
                                top: "4px",
                                borderRadius: "50%",
                                backgroundColor: "#1684ff"
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
                                width: "108px",
                                height: "111px",
                                position: "absolute",
                                left: "292px",
                                top: "368px"
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
                                    "dialog-mask"
                                ]
                            ],
                            {
                                width: "100%",
                                height: "100%",
                                position: "absolute",
                                left: 0,
                                top: 0,
                                zIndex: 80,
                                backgroundColor: "rgba(0, 0, 0, 0.52)"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "delete-dialog"
                                ]
                            ],
                            {
                                width: "334px",
                                height: "184px",
                                position: "absolute",
                                left: "49px",
                                top: "142px",
                                borderRadius: "22px",
                                backgroundColor: "#ffffff"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "dialog-title"
                                ]
                            ],
                            {
                                width: "280px",
                                height: "40px",
                                position: "absolute",
                                left: "27px",
                                top: "17px",
                                color: "#152957",
                                fontSize: "25px",
                                fontWeight: "bold",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "dialog-message"
                                ]
                            ],
                            {
                                width: "290px",
                                height: "35px",
                                position: "absolute",
                                left: "22px",
                                top: "62px",
                                color: "#48536b",
                                fontSize: "19px",
                                textAlign: "center"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "dialog-button"
                                ]
                            ],
                            {
                                width: "132px",
                                height: "48px",
                                position: "absolute",
                                top: "118px",
                                borderRadius: "14px"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "dialog-cancel"
                                ]
                            ],
                            {
                                left: "26px",
                                backgroundColor: "#edf0f5"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "dialog-confirm"
                                ]
                            ],
                            {
                                right: "26px",
                                backgroundColor: "#e85b5b"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "dialog-cancel-text"
                                ]
                            ],
                            {
                                width: "132px",
                                height: "48px",
                                fontSize: "19px",
                                fontWeight: "bold",
                                lineHeight: "46px",
                                textAlign: "center",
                                color: "#37445e"
                            }
                        ],
                        [
                            [
                                [
                                    0,
                                    "dialog-confirm-text"
                                ]
                            ],
                            {
                                width: "132px",
                                height: "48px",
                                fontSize: "19px",
                                fontWeight: "bold",
                                lineHeight: "46px",
                                textAlign: "center",
                                color: "#ffffff"
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
                        var _cityPickerData = __webpack_require__("./src/common/city-picker-data.js");
                        function _interopRequireDefault(e) {
                            return e && e.__esModule ? e : {
                                default: e
                            };
                        }
                        const APP_RESOURCE_ROOT = "/data/app/com.application.watch.redesign";
                        const STANDBY_DELAY_MS = 60000;
                        const PAGE_SIZE = 4;
                        const SWIPE_DISTANCE = 60;
                        var _default = exports.default = {
                            private: {
                                cities: [],
                                baseCities: [],
                                customCities: [],
                                visibleCities: [],
                                selectedCityId: "beijing",
                                pageIndex: 0,
                                pageCount: 2,
                                timeText: "12:44",
                                pageMode: "list",
                                titleText: "城市选择",
                                backgroundImage: "/common/backgrounds/dark-purple.png",
                                pageStyle: {
                                    backgroundColor: "#0f0d2c"
                                },
                                titleClass: "title text-light",
                                timeClass: "page-time text-light",
                                pickerLabelClass: "picker-label text-light",
                                addSummaryClass: "add-summary text-light",
                                addStatusClass: "add-status text-light",
                                provinceOptions: [],
                                cityOptions: [],
                                districtOptions: [],
                                selectedProvinceId: "hubei",
                                selectedPickerCityId: "wuhan",
                                selectedDistrictName: "",
                                addSummary: "湖北省 · 武汉市",
                                addStatus: "",
                                catFrames: [],
                                catDuration: "100ms",
                                activeActionId: "",
                                catAnimatorReady: false,
                                catStartTimerId: null,
                                catHealthTimerId: null,
                                standbyTimerId: null,
                                clockTimerId: null,
                                screenStandby: false,
                                ignoreTouchEnd: false,
                                touchX: 0,
                                touchY: 0,
                                returnTimerId: null,
                                leaving: false,
                                customCitiesCallbackRegistered: false,
                                deleteDialogVisible: false,
                                deleteTargetId: "",
                                deleteTargetName: "",
                                locationListener: null,
                                hasShown: false
                            },
                            onInit () {
                                this.loadPage();
                                this.observeCustomCities();
                                this.updateTime();
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
                                this.deleteDialogVisible = false;
                                if (this.hasShown) this.loadPage();
                                else this.hasShown = true;
                                this.observeCustomCities();
                                this.observeCurrentLocation();
                                this.startClock();
                                if (this.catAnimatorReady) this.queueCatAnimationStart();
                                this.startCatHealthCheck();
                                this.startStandbyTimer();
                            },
                            onHide () {
                                this.stopObservingCurrentLocation();
                                this.stopClock();
                                this.stopStandbyTimer();
                                this.cancelReturn();
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.stopCatHealthCheck();
                            },
                            onDestroy () {
                                this.stopObservingCurrentLocation();
                                this.stopClock();
                                this.stopStandbyTimer();
                                this.cancelReturn();
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
                                this.stopCatHealthCheck();
                                this.catAnimatorReady = false;
                            },
                            loadPage () {
                                const customization = this.$app.$def.getCustomization();
                                const background = (0, _customization.getBackground)(customization.backgroundId);
                                const action = (0, _customization.getAction)(customization.actionId);
                                const baseCities = (0, _weatherCities.getCities)();
                                const customCities = this.$app.$def.getCustomCities();
                                const currentLocation = this.$app.$def.getCurrentLocation();
                                const locationCities = [
                                    currentLocation
                                ].concat(customCities);
                                this.cities = baseCities.concat(locationCities);
                                this.baseCities = baseCities;
                                this.customCities = locationCities;
                                this.backgroundImage = background.src;
                                this.pageStyle = {
                                    backgroundColor: background.edgeColor || "#0f0d2c"
                                };
                                this.titleClass = "title text-" + background.foreground;
                                this.timeClass = "page-time text-" + background.foreground;
                                this.pickerLabelClass = "picker-label text-" + background.foreground;
                                this.addSummaryClass = "add-summary text-" + background.foreground;
                                this.addStatusClass = "add-status text-" + background.foreground;
                                this.selectedCityId = customization.cityId;
                                this.pageCount = 2 + Math.ceil(locationCities.length / PAGE_SIZE);
                                if (this.pageIndex >= this.pageCount) this.pageIndex = this.pageCount - 1;
                                if (this.pageIndex < 0) this.pageIndex = 0;
                                this.rebuildVisibleCities();
                                this.rebuildPickerOptions();
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
                            observeCustomCities () {
                                if (this.customCitiesCallbackRegistered) return;
                                this.customCitiesCallbackRegistered = true;
                                this.$app.$def.whenCustomCitiesReady(()=>{
                                    this.customCitiesCallbackRegistered = false;
                                    if (!this.leaving) this.loadPage();
                                });
                            },
                            observeCurrentLocation () {
                                if (this.locationListener) return;
                                this.locationListener = ()=>{
                                    if (!this.leaving) this.loadPage();
                                };
                                this.$app.$def.addCurrentLocationListener(this.locationListener);
                            },
                            stopObservingCurrentLocation () {
                                if (!this.locationListener) return;
                                this.$app.$def.removeCurrentLocationListener(this.locationListener);
                                this.locationListener = null;
                            },
                            rebuildVisibleCities () {
                                let sourceCities = [];
                                let start = 0;
                                if (0 === this.pageIndex) {
                                    this.pageMode = "list";
                                    this.titleText = "城市选择";
                                    sourceCities = this.baseCities;
                                } else if (1 === this.pageIndex) {
                                    this.pageMode = "add";
                                    this.titleText = "添加城市";
                                } else {
                                    this.pageMode = "list";
                                    this.titleText = "城市选择";
                                    sourceCities = this.customCities;
                                    start = (this.pageIndex - 2) * PAGE_SIZE;
                                }
                                const end = Math.min(start + PAGE_SIZE, sourceCities.length);
                                const visible = [];
                                for(let index = start; index < end; index += 1){
                                    const source = sourceCities[index];
                                    const slot = index - start + 1;
                                    const selected = source.id === this.selectedCityId;
                                    visible.push({
                                        id: source.id,
                                        name: source.name,
                                        isBase: source.isBase,
                                        isCurrentLocation: !!source.isCurrentLocation,
                                        locationReady: false !== source.locationReady,
                                        deletable: false !== source.deletable && !source.isBase && !source.isCurrentLocation,
                                        selected: selected,
                                        cardClass: "city-card card-" + slot + (selected ? " card-selected" : "")
                                    });
                                }
                                this.visibleCities = visible;
                            },
                            pad (value) {
                                return value < 10 ? "0" + value : "" + value;
                            },
                            updateTime () {
                                const now = new Date();
                                this.timeText = this.pad(now.getHours()) + ":" + this.pad(now.getMinutes());
                            },
                            startClock () {
                                if (this.clockTimerId) return;
                                this.updateTime();
                                this.scheduleNextClockTick();
                            },
                            scheduleNextClockTick () {
                                const now = new Date();
                                const delay = Math.max(1000, (60 - now.getSeconds()) * 1000);
                                this.clockTimerId = setTimeout(()=>{
                                    this.clockTimerId = null;
                                    this.updateTime();
                                    this.scheduleNextClockTick();
                                }, delay);
                            },
                            stopClock () {
                                if (!this.clockTimerId) return;
                                clearTimeout(this.clockTimerId);
                                this.clockTimerId = null;
                            },
                            rebuildPickerOptions () {
                                const province = (0, _cityPickerData.getProvince)(this.selectedProvinceId);
                                this.selectedProvinceId = province.id;
                                const city = (0, _cityPickerData.getCityOption)(province.id, this.selectedPickerCityId);
                                this.selectedPickerCityId = city.id;
                                let districtExists = "" === this.selectedDistrictName;
                                for(let index = 0; index < city.districts.length; index += 1)if (city.districts[index] === this.selectedDistrictName) districtExists = true;
                                if (!districtExists) this.selectedDistrictName = "";
                                this.provinceOptions = (0, _cityPickerData.getProvinceOptions)(this.selectedProvinceId);
                                this.cityOptions = (0, _cityPickerData.getCityOptions)(this.selectedProvinceId, this.selectedPickerCityId);
                                this.districtOptions = (0, _cityPickerData.getDistrictOptions)(this.selectedProvinceId, this.selectedPickerCityId, this.selectedDistrictName);
                                this.addSummary = province.name + " · " + city.name + (this.selectedDistrictName ? " · " + this.selectedDistrictName : "");
                            },
                            selectProvince (provinceId, event) {
                                if (event && event.stop) event.stop();
                                if ("add" !== this.pageMode || this.deleteDialogVisible) return;
                                const province = (0, _cityPickerData.getProvince)(provinceId);
                                this.selectedProvinceId = province.id;
                                this.selectedPickerCityId = province.cities[0].id;
                                this.selectedDistrictName = "";
                                this.addStatus = "";
                                this.rebuildPickerOptions();
                                this.registerActivity();
                            },
                            selectPickerCity (cityId, event) {
                                if (event && event.stop) event.stop();
                                if ("add" !== this.pageMode || this.deleteDialogVisible) return;
                                const city = (0, _cityPickerData.getCityOption)(this.selectedProvinceId, cityId);
                                this.selectedPickerCityId = city.id;
                                this.selectedDistrictName = "";
                                this.addStatus = "";
                                this.rebuildPickerOptions();
                                this.registerActivity();
                            },
                            selectDistrict (districtName, event) {
                                if (event && event.stop) event.stop();
                                if ("add" !== this.pageMode || this.deleteDialogVisible) return;
                                this.selectedDistrictName = districtName || "";
                                this.addStatus = "";
                                this.rebuildPickerOptions();
                                this.registerActivity();
                            },
                            addSelectedCity (event) {
                                if (event && event.stop) event.stop();
                                if ("add" !== this.pageMode || this.deleteDialogVisible) return;
                                const city = (0, _cityPickerData.buildCustomCity)(this.selectedProvinceId, this.selectedPickerCityId, this.selectedDistrictName);
                                const result = this.$app.$def.addCustomCity(city);
                                if (!result || "invalid" === result.status) {
                                    this.addStatus = "添加失败，请重试";
                                    return;
                                }
                                if ("exists" === result.status) {
                                    this.addStatus = "该城市已在列表中";
                                    this.registerActivity();
                                    return;
                                }
                                this.addStatus = "";
                                const customCities = this.$app.$def.getCustomCities();
                                let addedIndex = customCities.length - 1;
                                for(let index = 0; index < customCities.length; index += 1)if (customCities[index].id === city.id) addedIndex = index;
                                this.pageIndex = 2 + Math.floor((Math.max(0, addedIndex) + 1) / PAGE_SIZE);
                                this.loadPage();
                                this.queueCatAnimationStart();
                                this.registerActivity();
                            },
                            setPage (index) {
                                if (this.leaving || this.screenStandby || this.deleteDialogVisible) return;
                                const nextIndex = Math.max(0, Math.min(index, this.pageCount - 1));
                                if (nextIndex === this.pageIndex) return;
                                this.pageIndex = nextIndex;
                                this.rebuildVisibleCities();
                                if ("add" === this.pageMode) {
                                    this.cancelCatAnimationStart();
                                    this.pauseCatAnimation();
                                } else this.queueCatAnimationStart();
                                this.registerActivity();
                            },
                            selectCity (cityId) {
                                if (this.leaving || this.screenStandby || this.deleteDialogVisible) return;
                                this.registerActivity();
                                let city = null;
                                for(let index = 0; index < this.cities.length; index += 1)if (this.cities[index].id === cityId) city = this.cities[index];
                                if (!city) return;
                                if (city.isCurrentLocation && !city.locationReady) return void this.$app.$def.refreshCurrentLocation(true);
                                this.selectedCityId = city.id;
                                this.rebuildVisibleCities();
                                this.$app.$def.setCity(city.id);
                                this.openWeatherDetail();
                            },
                            requestDelete (cityId, cityName, event) {
                                if (event && event.stop) event.stop();
                                if (this.leaving || this.screenStandby) return;
                                let target = null;
                                for(let index = 0; index < this.cities.length; index += 1)if (this.cities[index].id === cityId) target = this.cities[index];
                                if (!target || target.isBase || target.isCurrentLocation || false === target.deletable) return;
                                this.deleteTargetId = target.id;
                                this.deleteTargetName = cityName || target.name;
                                this.deleteDialogVisible = true;
                                this.registerActivity();
                            },
                            holdDialog (event) {
                                if (event && event.stop) event.stop();
                            },
                            cancelDelete (event) {
                                if (event && event.stop) event.stop();
                                this.deleteDialogVisible = false;
                                this.deleteTargetId = "";
                                this.deleteTargetName = "";
                                this.registerActivity();
                            },
                            confirmDelete (event) {
                                if (event && event.stop) event.stop();
                                const targetId = this.deleteTargetId;
                                this.deleteDialogVisible = false;
                                this.deleteTargetId = "";
                                this.deleteTargetName = "";
                                if (targetId) this.$app.$def.removeCustomCity(targetId);
                                this.loadPage();
                                if ("add" === this.pageMode) {
                                    this.cancelCatAnimationStart();
                                    this.pauseCatAnimation();
                                } else this.queueCatAnimationStart();
                                this.registerActivity();
                            },
                            openWeatherDetail () {
                                if (this.leaving || this.screenStandby) return;
                                this.leaving = true;
                                this.stopObservingCurrentLocation();
                                this.stopClock();
                                this.stopStandbyTimer();
                                this.cancelCatAnimationStart();
                                this.stopCatHealthCheck();
                                this.stopCatAnimation();
                                this.cancelReturn();
                                this.returnTimerId = setTimeout(()=>{
                                    this.returnTimerId = null;
                                    try {
                                        _system.default.replace({
                                            uri: "/pages/weatherdetail"
                                        });
                                    } catch (error) {
                                        this.leaving = false;
                                        this.observeCurrentLocation();
                                        this.startClock();
                                        this.queueCatAnimationStart();
                                        this.startCatHealthCheck();
                                        this.startStandbyTimer();
                                        console.log("open weather detail failed", error);
                                    }
                                }, 160);
                            },
                            startCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("cityCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.start();
                                } catch (error) {
                                    console.log("city cat animator start failed", error);
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
                                const animator = this.$element("cityCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.pause();
                                } catch (error) {
                                    console.log("city cat animator pause failed", error);
                                }
                            },
                            stopCatAnimation () {
                                if (!this.catAnimatorReady) return;
                                const animator = this.$element("cityCatAnimator");
                                if (!animator) return;
                                try {
                                    animator.stop();
                                } catch (error) {
                                    console.log("city cat animator stop failed", error);
                                }
                            },
                            startCatHealthCheck () {
                                if (this.catHealthTimerId || this.screenStandby) return;
                                this.catHealthTimerId = setInterval(()=>{
                                    const animator = this.$element("cityCatAnimator");
                                    if (!this.catAnimatorReady || !animator) return;
                                    try {
                                        const state = animator.getState();
                                        if ("paused" === state) animator.resume();
                                        if ("stopped" === state) animator.start();
                                    } catch (error) {
                                        console.log("city cat animator health check failed", error);
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
                                this.stopClock();
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
                                this.startClock();
                                this.cancelCatAnimationStart();
                                this.stopCatAnimation();
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
                            lockViewport (event) {
                                if (event && event.stop) event.stop();
                                return true;
                            },
                            onTouchStart (event) {
                                if (this.deleteDialogVisible) return void this.lockViewport(event);
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
                                if (this.deleteDialogVisible) return;
                                if (this.ignoreTouchEnd) {
                                    this.ignoreTouchEnd = false;
                                    return;
                                }
                                this.registerActivity();
                                const point = this.touchPoint(event, true);
                                const deltaX = point.x - this.touchX;
                                const deltaY = point.y - this.touchY;
                                const horizontal = Math.abs(deltaX) > 1.2 * Math.abs(deltaY);
                                const vertical = Math.abs(deltaY) > 1.2 * Math.abs(deltaX);
                                if (vertical && deltaY < -SWIPE_DISTANCE) return void this.returnToWeather();
                                if (horizontal && deltaX < -SWIPE_DISTANCE) this.setPage(this.pageIndex + 1);
                                if (horizontal && deltaX > SWIPE_DISTANCE) this.setPage(this.pageIndex - 1);
                            },
                            handleSwipe (event) {
                                if (this.screenStandby) return void this.wakeScreen();
                                if (this.deleteDialogVisible) return;
                                this.registerActivity();
                                const direction = event && (event.direction || event.detail && event.detail.direction);
                                if ("up" === direction) this.returnToWeather();
                                if ("left" === direction) this.setPage(this.pageIndex + 1);
                                if ("right" === direction) this.setPage(this.pageIndex - 1);
                            },
                            queueReturn () {
                                this.cancelReturn();
                                this.stopObservingCurrentLocation();
                                this.stopClock();
                                this.stopStandbyTimer();
                                this.cancelCatAnimationStart();
                                this.stopCatHealthCheck();
                                this.stopCatAnimation();
                                this.returnTimerId = setTimeout(()=>{
                                    this.returnTimerId = null;
                                    try {
                                        _system.default.replace({
                                            uri: "/pages/weather"
                                        });
                                    } catch (error) {
                                        this.leaving = false;
                                        this.observeCurrentLocation();
                                        this.startClock();
                                        this.queueCatAnimationStart();
                                        this.startCatHealthCheck();
                                        this.startStandbyTimer();
                                        console.log("close city selector failed", error);
                                    }
                                }, 160);
                            },
                            cancelReturn () {
                                if (!this.returnTimerId) return;
                                clearTimeout(this.returnTimerId);
                                this.returnTimerId = null;
                            },
                            returnToWeather () {
                                if (this.leaving || this.deleteDialogVisible) return;
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
                                                classList: [
                                                    "veil"
                                                ]
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
                                                value: function() {
                                                    return _vm_.titleText;
                                                }
                                            }
                                        }, []),
                                        aiot.__ce__("text", {
                                            __vm__: _vm_,
                                            __opts__: {
                                                classList: function() {
                                                    const $classValue$ = _vm_.timeClass;
                                                    if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                    return $classValue$;
                                                },
                                                value: function() {
                                                    return _vm_.timeText;
                                                }
                                            }
                                        }, []),
                                        aiot.__ci__({
                                            __vm__: _vm_,
                                            __opts__: {
                                                shown: function() {
                                                    return "list" === _vm_.pageMode;
                                                }
                                            }
                                        }, function() {
                                            return [
                                                aiot.__cf__({
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        exp: function() {
                                                            return {
                                                                __list__: _vm_.visibleCities,
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
                                                                    const $classValue$ = $item.cardClass;
                                                                    if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                    return $classValue$;
                                                                },
                                                                events: {
                                                                    click: function(evt) {
                                                                        return _vm_.selectCity($item.id, evt);
                                                                    }
                                                                }
                                                            }
                                                        }, [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "city-name"
                                                                    ],
                                                                    value: function() {
                                                                        return $item.name;
                                                                    }
                                                                }
                                                            }, []),
                                                            aiot.__ci__({
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    shown: function() {
                                                                        return $item.isCurrentLocation;
                                                                    }
                                                                }
                                                            }, function() {
                                                                return [
                                                                    aiot.__ce__("text", {
                                                                        __vm__: _vm_,
                                                                        __opts__: {
                                                                            classList: [
                                                                                "current-location-badge"
                                                                            ],
                                                                            value: "当前位置"
                                                                        }
                                                                    }, [])
                                                                ];
                                                            }),
                                                            aiot.__ci__({
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    shown: function() {
                                                                        return $item.deletable;
                                                                    }
                                                                }
                                                            }, function() {
                                                                return [
                                                                    aiot.__ce__("div", {
                                                                        __vm__: _vm_,
                                                                        __opts__: {
                                                                            classList: [
                                                                                "delete-button"
                                                                            ],
                                                                            events: {
                                                                                click: function(evt) {
                                                                                    return _vm_.requestDelete($item.id, $item.name, evt);
                                                                                }
                                                                            }
                                                                        }
                                                                    }, [
                                                                        aiot.__ce__("text", {
                                                                            __vm__: _vm_,
                                                                            __opts__: {
                                                                                classList: [
                                                                                    "delete-button-text"
                                                                                ],
                                                                                value: "删除"
                                                                            }
                                                                        }, [])
                                                                    ])
                                                                ];
                                                            }),
                                                            aiot.__ce__("div", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: function() {
                                                                        const $classValue$ = $item.selected ? "radio radio-selected" : "radio";
                                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                        return $classValue$;
                                                                    }
                                                                }
                                                            }, [
                                                                aiot.__ci__({
                                                                    __vm__: _vm_,
                                                                    __opts__: {
                                                                        shown: function() {
                                                                            return $item.selected;
                                                                        }
                                                                    }
                                                                }, function() {
                                                                    return [
                                                                        aiot.__ce__("div", {
                                                                            __vm__: _vm_,
                                                                            __opts__: {
                                                                                classList: [
                                                                                    "radio-core"
                                                                                ]
                                                                            }
                                                                        }, [])
                                                                    ];
                                                                })
                                                            ])
                                                        ])
                                                    ];
                                                })
                                            ];
                                        }),
                                        aiot.__ci__({
                                            __vm__: _vm_,
                                            __opts__: {
                                                shown: function() {
                                                    return "add" === _vm_.pageMode;
                                                }
                                            }
                                        }, function() {
                                            return [
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "add-page"
                                                        ]
                                                    }
                                                }, [
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: function() {
                                                                const $classValue$ = _vm_.pickerLabelClass + " picker-label-province";
                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                return $classValue$;
                                                            },
                                                            value: "省",
                                                            static: true
                                                        }
                                                    }, []),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: function() {
                                                                const $classValue$ = _vm_.pickerLabelClass + " picker-label-city";
                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                return $classValue$;
                                                            },
                                                            value: "市",
                                                            static: true
                                                        }
                                                    }, []),
                                                    aiot.__ce__("text", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: function() {
                                                                const $classValue$ = _vm_.pickerLabelClass + " picker-label-district";
                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                return $classValue$;
                                                            },
                                                            value: "区",
                                                            static: true
                                                        }
                                                    }, []),
                                                    aiot.__ce__("list", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "picker-list",
                                                                "picker-province"
                                                            ],
                                                            bounces: "false"
                                                        }
                                                    }, [
                                                        aiot.__cf__({
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                exp: function() {
                                                                    return {
                                                                        __list__: _vm_.provinceOptions,
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
                                                                        type: "province",
                                                                        classList: function() {
                                                                            const $classValue$ = $item.itemClass;
                                                                            if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                            return $classValue$;
                                                                        },
                                                                        events: {
                                                                            click: function(evt) {
                                                                                return _vm_.selectProvince($item.id, evt);
                                                                            }
                                                                        }
                                                                    }
                                                                }, [
                                                                    aiot.__ce__("text", {
                                                                        __vm__: _vm_,
                                                                        __opts__: {
                                                                            classList: function() {
                                                                                const $classValue$ = $item.selected ? "picker-text picker-text-selected" : "picker-text";
                                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                                return $classValue$;
                                                                            },
                                                                            value: function() {
                                                                                return $item.name;
                                                                            }
                                                                        }
                                                                    }, [])
                                                                ])
                                                            ];
                                                        })
                                                    ]),
                                                    aiot.__ce__("list", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "picker-list",
                                                                "picker-city"
                                                            ],
                                                            bounces: "false"
                                                        }
                                                    }, [
                                                        aiot.__cf__({
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                exp: function() {
                                                                    return {
                                                                        __list__: _vm_.cityOptions,
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
                                                                        type: "city",
                                                                        classList: function() {
                                                                            const $classValue$ = $item.itemClass;
                                                                            if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                            return $classValue$;
                                                                        },
                                                                        events: {
                                                                            click: function(evt) {
                                                                                return _vm_.selectPickerCity($item.id, evt);
                                                                            }
                                                                        }
                                                                    }
                                                                }, [
                                                                    aiot.__ce__("text", {
                                                                        __vm__: _vm_,
                                                                        __opts__: {
                                                                            classList: function() {
                                                                                const $classValue$ = $item.selected ? "picker-text picker-text-selected" : "picker-text";
                                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                                return $classValue$;
                                                                            },
                                                                            value: function() {
                                                                                return $item.name;
                                                                            }
                                                                        }
                                                                    }, [])
                                                                ])
                                                            ];
                                                        })
                                                    ]),
                                                    aiot.__ce__("list", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "picker-list",
                                                                "picker-district"
                                                            ],
                                                            bounces: "false"
                                                        }
                                                    }, [
                                                        aiot.__cf__({
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                exp: function() {
                                                                    return {
                                                                        __list__: _vm_.districtOptions,
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
                                                                        type: "district",
                                                                        classList: function() {
                                                                            const $classValue$ = $item.itemClass;
                                                                            if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                            return $classValue$;
                                                                        },
                                                                        events: {
                                                                            click: function(evt) {
                                                                                return _vm_.selectDistrict($item.id, evt);
                                                                            }
                                                                        }
                                                                    }
                                                                }, [
                                                                    aiot.__ce__("text", {
                                                                        __vm__: _vm_,
                                                                        __opts__: {
                                                                            classList: function() {
                                                                                const $classValue$ = $item.selected ? "picker-text picker-text-selected" : "picker-text";
                                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                                return $classValue$;
                                                                            },
                                                                            value: function() {
                                                                                return $item.name;
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
                                                            classList: function() {
                                                                const $classValue$ = _vm_.addSummaryClass;
                                                                if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                return $classValue$;
                                                            },
                                                            value: function() {
                                                                return _vm_.addSummary;
                                                            }
                                                        }
                                                    }, []),
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "add-city-button"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.addSelectedCity(evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "add-city-button-text"
                                                                ],
                                                                value: "添加城市"
                                                            }
                                                        }, [])
                                                    ]),
                                                    aiot.__ci__({
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            shown: function() {
                                                                return _vm_.addStatus;
                                                            }
                                                        }
                                                    }, function() {
                                                        return [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: function() {
                                                                        const $classValue$ = _vm_.addStatusClass;
                                                                        if ('string' == typeof $classValue$) return $classValue$.split(' ').map((item)=>item.trim()).filter(Boolean);
                                                                        return $classValue$;
                                                                    },
                                                                    value: function() {
                                                                        return _vm_.addStatus;
                                                                    }
                                                                }
                                                            }, [])
                                                        ];
                                                    })
                                                ])
                                            ];
                                        }),
                                        aiot.__ci__({
                                            __vm__: _vm_,
                                            __opts__: {
                                                shown: function() {
                                                    return "list" === _vm_.pageMode;
                                                }
                                            }
                                        }, function() {
                                            return [
                                                aiot.__ce__("image-animator", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "corner-cat"
                                                        ],
                                                        id: "cityCatAnimator",
                                                        images: function() {
                                                            return _vm_.catFrames;
                                                        },
                                                        duration: function() {
                                                            return _vm_.catDuration;
                                                        },
                                                        iteration: "infinite",
                                                        fixedsize: "true"
                                                    }
                                                }, [])
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
                                        }),
                                        aiot.__ci__({
                                            __vm__: _vm_,
                                            __opts__: {
                                                shown: function() {
                                                    return _vm_.deleteDialogVisible;
                                                }
                                            }
                                        }, function() {
                                            return [
                                                aiot.__ce__("div", {
                                                    __vm__: _vm_,
                                                    __opts__: {
                                                        classList: [
                                                            "dialog-mask"
                                                        ],
                                                        events: {
                                                            touchstart: function(evt) {
                                                                return _vm_.lockViewport(evt);
                                                            },
                                                            touchmove: function(evt) {
                                                                return _vm_.lockViewport(evt);
                                                            },
                                                            click: function(evt) {
                                                                return _vm_.cancelDelete(evt);
                                                            }
                                                        }
                                                    }
                                                }, [
                                                    aiot.__ce__("div", {
                                                        __vm__: _vm_,
                                                        __opts__: {
                                                            classList: [
                                                                "delete-dialog"
                                                            ],
                                                            events: {
                                                                click: function(evt) {
                                                                    return _vm_.holdDialog(evt);
                                                                }
                                                            }
                                                        }
                                                    }, [
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "dialog-title"
                                                                ],
                                                                value: "删除城市"
                                                            }
                                                        }, []),
                                                        aiot.__ce__("text", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "dialog-message"
                                                                ],
                                                                value: function() {
                                                                    return "确定删除“" + _vm_.deleteTargetName + "”吗？";
                                                                }
                                                            }
                                                        }, []),
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "dialog-button",
                                                                    "dialog-cancel"
                                                                ],
                                                                events: {
                                                                    click: function(evt) {
                                                                        return _vm_.cancelDelete(evt);
                                                                    }
                                                                }
                                                            }
                                                        }, [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "dialog-cancel-text"
                                                                    ],
                                                                    value: "取消"
                                                                }
                                                            }, [])
                                                        ]),
                                                        aiot.__ce__("div", {
                                                            __vm__: _vm_,
                                                            __opts__: {
                                                                classList: [
                                                                    "dialog-button",
                                                                    "dialog-confirm"
                                                                ],
                                                                events: {
                                                                    click: function(evt) {
                                                                        return _vm_.confirmDelete(evt);
                                                                    }
                                                                }
                                                            }
                                                        }, [
                                                            aiot.__ce__("text", {
                                                                __vm__: _vm_,
                                                                __opts__: {
                                                                    classList: [
                                                                        "dialog-confirm-text"
                                                                    ],
                                                                    value: "删除"
                                                                }
                                                            }, [])
                                                        ])
                                                    ])
                                                ])
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
