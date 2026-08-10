#include "permission_manager.h"

#include <cstring>

#include "common/shell_log.h"

namespace shell {

#define QAPP_JS_PERM_NAME_PREFIX "hapjs.permission."
#define QAPP_SYS_PERM_NAME_PREFIX "vela.permission."

#ifdef CONFIG_QUICKAPP_PERMISSION_NAME_LEN
#define QAPP_PERM_NAME_LEN CONFIG_QUICKAPP_PERMISSION_NAME_LEN
#else
#define QAPP_PERM_NAME_LEN 36
#endif

class PermissionManagerImpl : public PermissionManager {

public:
    PermissionManagerImpl() { }
    virtual ~PermissionManagerImpl() { }

    bool init(const std::string& package_name, FeatureManagerHandle hfm) override
    {
        package_name_ = package_name;
        feature_manager_ = hfm;
        FeatureSetPermissionsCallback(feature_manager_, _feature_permission_query, this);
        return true;
    }

    void uninit() override
    {
    }

    void verifyAuth(FeaturePermissionId permid, const char* permission, const char* api_name, FeaturePermissionsHandle handle, bool need_block) override;

private:
    void doPermissionResult(int result, FeaturePermissionsHandle handle);

    static void _feature_permission_query(FeaturePermissionsHandle handle, const FeaturePermissionsInfo* info, void* data);

    FeatureManagerHandle feature_manager_;
    std::string package_name_;
};

std::unique_ptr<PermissionManager> PermissionManager::Create()
{
    return std::unique_ptr<PermissionManager>(new PermissionManagerImpl());
}

bool PermissionManager::validatePermissionName(const char* permission_name, char* permission_buf, size_t max)
{
    if (!permission_name || max == 0) {
        return false;
    }

    std::string_view name(permission_name);
    constexpr std::string_view JS_PREFIX = (QAPP_JS_PERM_NAME_PREFIX);
    constexpr std::string_view SYS_PREFIX = (QAPP_SYS_PERM_NAME_PREFIX);

    std::string result;

    if (name.compare(0, JS_PREFIX.size(), JS_PREFIX) == 0) {
        result = std::string(SYS_PREFIX) + std::string(name.substr(JS_PREFIX.length()));
    } else if (name.compare(0, SYS_PREFIX.size(), SYS_PREFIX) == 0) {
        result = std::string(name);
    } else {
        SHELL_LOG_WARN("[%s] permission: %s is invalid!", __func__, permission_name);
        return false;
    }

    if (result.size() > max) {
        SHELL_LOG_WARN("[%s] Permission name '%s' is too long for buffer", __func__, permission_name);
        return false;
    }

    std::copy(result.begin(), result.end(), permission_buf);
    permission_buf[result.length()] = '\0';

    return true;
}

void PermissionManagerImpl::verifyAuth(FeaturePermissionId permid, const char* permission, const char* api_name, FeaturePermissionsHandle handle, bool need_block)
{
    char permission_buf[QAPP_PERM_NAME_LEN];
    SHELL_LOG_INFO("permission(%s) api(%s) handle(%p) block(%d)", permission, api_name, handle, need_block);
    bool ret = validatePermissionName(permission, permission_buf, sizeof(permission_buf) - 1);
    if (!ret) {
        FeatureRejectPermissions(feature_manager_, handle, FEATURE_PERMS_DENIED);
        return;
    }

    requirePermission(package_name_, permid, permission_buf, api_name, need_block,
        [this, handle](int result) {
            SHELL_LOG_INFO("permission: back to feature res(%d) handle(%p)", result, handle);
            doPermissionResult(result, handle);
        });
}

void PermissionManagerImpl::doPermissionResult(int result, FeaturePermissionsHandle handle)
{
    if (result == PermissionManager::GRANTED) {
        FeatureGrantPermissions(feature_manager_, handle);
    } else {
        FeatureRejectPermissions(feature_manager_, handle, FEATURE_PERMS_DENIED);
    }
}

void PermissionManagerImpl::_feature_permission_query(FeaturePermissionsHandle handle, const FeaturePermissionsInfo* info, void* data)
{

    PermissionManagerImpl* pimpl = (PermissionManagerImpl*)data;

    for (size_t i = 0; i < FEATURE_PERMISSION_MAX; i++) {
        if (!HAS_PERMISSION(*(info->permissions), i)) {
            continue;
        }

        FeaturePermissionId permid = static_cast<FeaturePermissionId>(i);
        pimpl->verifyAuth(permid, FeatureGetPermissionName(permid), info->api_name, handle, !info->has_async_cbs);
        break;
    }
}

} // namespace shell
