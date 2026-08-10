#ifndef _SHELL_PERMISSION_MANAGER_H_
#define _SHELL_PERMISSION_MANAGER_H_

#include <functional>
#include <memory>
#include <string>

#include "feature_main_exports.h"
#include "feature_permission.h"

namespace shell {

class PermissionManager {
public:
    enum {
        UNKNOWN, // unknwon the result
        GRANTED,
        DENIED, // denied
        TIMEOUT,
        ERROR,
    };

    class Observer {
    public:
        virtual ~Observer() { }

        virtual void onPermissionRequire(
            const std::string& package_name,
            FeaturePermissionId permid,
            const std::string& permission,
            const std::string& api_name,
            bool need_block) { }

        virtual void onPermssionResult(
            const std::string& package_name,
            FeaturePermissionId permid,
            const std::string& permission,
            const std::string& api_name,
            int result,
            bool need_block);
    };

    class Client {
    public:
        virtual ~Client() { }

        virtual void requirePermission(
            const std::string& package_name,
            FeaturePermissionId permid,
            const std::string& permission,
            const std::string& api_name,
            std::function<void(int)> result_cb)
            = 0;

        virtual int requirePermissionBlock(
            const std::string& package_name,
            FeaturePermissionId permid,
            const std::string& permission,
            const std::string& api_name)
        {
            return DENIED;
        }
    };

    PermissionManager() { }

    virtual ~PermissionManager() { }

    virtual bool init(const std::string& package_name, FeatureManagerHandle hfm) = 0;

    virtual void uninit() = 0;

    virtual void verifyAuth(FeaturePermissionId permid, const char* permission, const char* api_name, FeaturePermissionsHandle handle, bool need_block) = 0;

    void setClient(std::unique_ptr<Client>&& client)
    {
        client_ = std::move(client);
    }

    void addObserver(std::unique_ptr<Observer>&& observer)
    {
        observers_.push_back(std::move(observer));
    }

    static std::unique_ptr<PermissionManager> Create();

    static bool validatePermissionName(const char* permission_name, char* permission_buf, size_t max);

protected:
    bool requirePermission(
        const std::string& package_name,
        FeaturePermissionId permid,
        const std::string& permission,
        const std::string& api_name,
        bool need_block,
        std::function<void(int)> result_cb)
    {
        onPermissionRequire(package_name, permid, permission, api_name);
        if (!client_) {
            onPermssionResult(package_name, permid, permission, api_name, ERROR);
            return false;
        }
        client_->requirePermission(package_name, permid, permission, api_name,
            [this, package_name, permid, permission, api_name, result_cb, need_block](int result) {
                result_cb(result);
                onPermssionResult(package_name, permid, permission, api_name, result, need_block);
            });
        return true;
    }

    int requirePermissionBlock(
        const std::string& package,
        FeaturePermissionId permid,
        const std::string& permission,
        const std::string& api_name)
    {
        onPermissionRequire(package, permid, permission, api_name, true);
        int result = DENIED;
        if (client_) {
            result = client_->requirePermissionBlock(package, permid, permission, api_name);
        }
        onPermssionResult(package, permid, permission, api_name, result, true);
        return result;
    }

    void onPermissionRequire(const std::string& package,
        FeaturePermissionId permid,
        const std::string& permission,
        const std::string& api_name,
        bool need_block = false)
    {
        for (auto& ob : observers_) {
            ob->onPermissionRequire(package, permid, permission, api_name, need_block);
        }
    }

    void onPermssionResult(const std::string& package,
        FeaturePermissionId permid,
        const std::string& permission,
        const std::string& api_name,
        int result,
        bool need_block = false)
    {
        for (auto& ob : observers_) {
            ob->onPermssionResult(package, permid, permission, api_name, result, need_block);
        }
    }

private:
    std::unique_ptr<Client> client_;
    std::vector<std::unique_ptr<Observer>> observers_;
};

}

#endif
