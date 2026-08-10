#ifndef _SHELL_COMMON_LOG_H_
#define _SHELL_COMMON_LOG_H_

#include <syslog.h>

#define SHELL_LOG(level, fmt, ...) \
    syslog(level, "[QSHELL][%s:%d]" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)

#define SHELL_LOG_LEVEL_DEBUG 0
#define SHELL_LOG_LEVEL_INFO 1
#define SHELL_LOG_LEVEL_WARN 2
#define SHELL_LOG_LEVEL_ERROR 3
#define SHELL_LOG_LEVEL_ALERT 4
#define SHELL_LOG_LEVEL_OFF 5

#ifndef SHELL_LOG_LEVEL
#define SHELL_LOG_LEVEL SHELL_LOG_LEVEL_INFO
#endif

#if SHELL_LOG_LEVEL_DEBUG >= SHELL_LOG_LEVEL
#define SHELL_LOG_DEBUG(fmt, ...) SHELL_LOG_(LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define SHELL_LOG_DEBUG(fmt, ...)
#endif

#if SHELL_LOG_LEVEL_INFO >= SHELL_LOG_LEVEL
#define SHELL_LOG_INFO(fmt, ...) SHELL_LOG(LOG_INFO, fmt, ##__VA_ARGS__)
#else
#define SHELL_LOG_INFO(fmt, ...)
#endif

#if SHELL_LOG_LEVEL_WARN >= SHELL_LOG_LEVEL
#define SHELL_LOG_WARN(fmt, ...) SHELL_LOG(LOG_WARNING, fmt, ##__VA_ARGS__)
#else
#define SHELL_LOG_WARN(fmt, ...)
#endif

#if SHELL_LOG_LEVEL_ERROR >= SHELL_LOG_LEVEL
#define SHELL_LOG_ERROR(fmt, ...) SHELL_LOG(LOG_ERR, fmt, ##__VA_ARGS__)
#else
#define SHELL_LOG_ERROR(fmt, ...)
#endif

#if SHELL_LOG_LEVEL_ALERT >= SHELL_LOG_LEVEL
#define SHELL_LOG_ALERT(fmt, ...) SHELL_LOG(LOG_ALERT, fmt, ##__VA_ARGS__)
#else
#define SHELL_LOG_ALERT(fmt, ...)
#endif

#endif
