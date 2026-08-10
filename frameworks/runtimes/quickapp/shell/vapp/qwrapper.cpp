#include "qwrapper.h"
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

static fs::path get_app_path()
{
#ifdef __NuttX__
    const char* path = CONFIG_HAP_APP_PATH;
#else
    const char* path = currentPath()
#endif
    fs::path app_path(path);

    app_path /= "app";

#ifndef __NuttX__
    free(path);
#endif

    return app_path;
}

extern "C" const char* QWrapperGetAppPath(char* buf, size_t max)
{
    auto app_path = get_app_path();
    auto path_str = app_path.string();
    max = std::min(max, path_str.length());

    strncpy(buf, path_str.c_str(), max);
    buf[max] = 0;

    return buf;
}

const char* QWrapperGetBaseDataDir(const char* package_name, char* buf, size_t max)
{
    // TODO same as app_path?
    auto app_path = get_app_path() / package_name;
    auto path_str = app_path.string();
    max = std::min(max, path_str.length());

    strncpy(buf, path_str.c_str(), max);
    buf[max] = 0;
    return buf;
}
