#ifndef _SHELL_UTILS_H_
#define _SHELL_UTILS_H_

#include "common/shell_log.h"

#define SHELL_UNLIKELY(expr) __builtin_expect(!!(expr), 0)

#define SHELL_STRINGIFY_(x) #x
#define SHELL_STRINGIFY(x) SHELL_STRINGIFY_(x)

/**
 * @brief 断言,处理错误中止运行
 */
#define SHELL_ERROR_AND_ABORT(expr)                                                                                   \
    do {                                                                                                              \
        SHELL_LOG_ERROR("%s:%s Assertion '%s' failed.", __FILE__ ":" SHELL_STRINGIFY(__LINE__), __FUNCTION__, #expr); \
        assert(0);                                                                                                    \
    } while (0)

// 断言检查
#define SHELL_CHECK(expr)                \
    do {                                 \
        if (SHELL_UNLIKELY(!(expr))) {   \
            SHELL_ERROR_AND_ABORT(expr); \
        }                                \
    } while (0)

#endif // _SHELL_UTILS_H_
