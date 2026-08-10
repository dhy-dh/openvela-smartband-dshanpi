#快应用框架权限管理模块

详细设计请参考权限管理设计文档。

## 权限管理模块接口
权限信息存在数据库中，权限模块提供如下接口：

| API | 场景 | 描述 | 返回值 |备注 |
|----|---|----|----|-----|
|int QueryPermission(char *PackageName, PermissionInfo **permissionInfo, int *permissionsNum)|Setting进行应用权限查询时调用|入参是包名，出参是所有拥有权限名和数量|总是成功,成功查询的权限个数会存储在出参中|-|
|int UpdatePermission(char *PackageName, const PermissionInfo *permissionInfo)|Setting进行应用权限更改时调用|更改指定包指定权限|成功设置0,失败设置-EINVAL,错误包名或权限名|-|
|int CheckPermission(char *PackageName, PermissionInfo *permissionInfo, int uid)|快应用框架查询权限时调用|查询指定包的指定权限|该接口总是返回成功,但是通过callback传递查询结果|-|
|Int SavePermissions(const char *packageName, const PermissionInfo **permissionInfo, int permissionNum)|快应用框架安装应用时调用|读取描述文件中定义需要的所有权限，保存在数据库中|总是成功|-|
|Int DeletePermissions(char *PackageName)|快应用框架卸载应用时调用|删除指定应用的权限记录|总是成功|-|
|int showPermissionConfirmDialog(char *packageName, PermissionInfo info, int contextId, void(*confirmCb)(char *packageName, PermissionInfo permissionInfo, int contextId))|需要用户确认授权时调用|弹出权限确认框，由外部实现|总是成功|用户操作结果由回调返回|


## 场景拆解
对于快应用框架来说，权限处理大致有如下几个场景需要考虑：
* 应用的manifest.json中新增permission字段，要求用户手动写入permission。对于旧应用和未提供此字段的应用，需要框架在运行时从features中自动生成该信息
* 应用安装时，使用SavePermissions接口将应用所需权限写入权限数据库中，权限值为**未授权**，这是为了能够让settings读取到应用权限信息。
* 应用卸载时，调用DeletePermissions删除所有的权限信息，避免权限残留。
* 应用更新时，需要将权限读出来，做diff合并，尽可能保留原有权限不变，将新权限合入进去，这块要重点测一下。
* 应用请求接口功能时，调用CheckPermission，根据返回值，决定是否调用showPermissionConfirmDialog弹出确认窗口并接续后续流程。

#整体设计
实现一个Native Feature导入到JS环境中，由js framework根据需要在接口中调用，完成整体流程。
如果用户拒绝，则不再进行后续的鉴权，待应用onReady后由框架直接调terminate退出。

## 权限的确定 & 权限和Feature的映射关系
Feature中需要维护一个映射表：
1. Feature到权限名的映射表，用于在manifest.json中没有permission字段时自动生成权限列表
2. ~~权限名到权限ID的映射表，用于将权限名转为权限ID。由于manifest.json内permission字段的存在，必须维护权限名到权限ID的映射关系。~~

## 权限和接口的对应关系
当前的简化设计中，这个关系由JS逻辑代码维护，在特定Feature接口中硬编码入对特定权限的检查。
**TODO: 补充feature接口和权限的对应关系**

## Native Feature接口设计
鉴权接口只需要一个verifyAuth接口即可，其原型如下：
````C++
// 需要传入permissionName和apiName
// permissionName是要鉴权的权限名
// apiName即要鉴权的api接口名， 比如 fetch或者getDeviceInfo
void verifyAuth(const char* permissionName, const char* apiName, AuthCfg cfg);
enum class PermissionErrorCode {
    success = 0,    // 已授权
    not_allow = 1,  // 权限未声明
    deined = 2,     // 用户拒绝
};
struct AuthCfg {
    void (*success)(const char* permissionName);
    void (*fail)(const char* permissionName, PermissionErrorCode errorCode);
};
````

#前置依赖
1. 权限名和Feature的映射关系是怎样的？
2. ~~支持的权限名都是哪些，对应的权限ID是什么？~~ 不需要权限ID，仅使用权限名
3. ~~PermissionInfo的具体定义是什么？~~
4. apiName都有哪些？由谁定义？
    > 先确定有哪些权限，再确定要鉴权的API，最后确定apiName，我们可以先给一个版本出来，让可穿戴评估


关于3：
````C
typedef struct permission_info
{
    enum perm_stat state;
    char name[PERM_NAME_LEN];
    char name[PERM_DESC_LEN];
}permission_info;
````

#模块设计

PermissionManager

````C++
// 权限读取接口
class PermissionReader {
public:
    /**
      从manifest.json中读permissions
    */
    static std::vector<std::string> read(jse_context_ref ctx, jse_value_t manifest)
    {
        std::vector<std::string> result;
        if (!jse_object_has_prop(ctx, manifest, "permissions"))
        {
    // 没有permissions字段，从feature转换
    jse_value_t features = jse_object_get_property(ctx, manifest, "features");
    // convert
    result = fromFeatures(ctx, features);
    jse_free_value(ctx, features);
        } else {
    // 从permissions读取
        }
        return result;
}
}
;

enum class PermissionErrorCode : uint8_t {
    success = 0, // 已授权
    not_allow = 1, // 权限未声明
    deined = 2, // 用户拒绝
};

/**
    权限管理器，保存在FrameworkExt上
**/
class PermissionManager {
public:
    // 删除应用权限，删应用时调用
    void delPermissions(const char* packageName);
    // 保存单个权限
    void setPermission(const char* packageName, const char* permissionName, PermissionState state);
    // 初始化应用权限，应用安装时使用，会删除然后重新初始化
    void initAppPermissions(const char* packageName, const std::vector<std::string>& permissions);
    // 更新权限，应用更新时调用，需要做diff
    void updateAppPermissions(const char* packageName, const std::vector<std::string>& permissions);
    // 检查权限，查询权限数据库
    PermissionErrorCode checkPermission(const char* packageName, const char* permissionName);
};
````

    InstalledPackageInfo和PackageManager中新增权限相关内容

````C++ typedef struct AppInfo {
    char package[128];
    char icon[64];
    char versionName[64];
    int versionCode;
    unsigned int minAPILevel;
    int minPlatformVersion;
    char fingerprint[20];
    int fingerprint_len;
    bool standalone; // APP是否依赖手机 默认false
    bool needNetwork; // APP是否依赖网络 默认false
    bool background; // APP是否支持后台运行
    ////////////// 新增 ///////////////////
    int64_t installedTime; // 安装时间
    //////////////////////////////////////
    int name_len;
    struct i18NEntry {
        char lang[16]; // zh-CN, en ... etc
        char value[64]; // 具体值
    } * names;
} AppInfo;

````