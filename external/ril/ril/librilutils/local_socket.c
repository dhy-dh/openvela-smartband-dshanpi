/*
 * Copyright (C) 2023 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "LOCAL_SOCKET"
#define NDEBUG 1

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <local_socket.h>
#include <telephony/ril.h>

#ifdef CONFIG_RPMSG_RIL

#include <netpacket/rpmsg.h>

static int create_rpmsg_socket(void)
{
    const struct sockaddr_rpmsg addr = {
        .rp_family = AF_RPMSG,
        .rp_name = SOCKET_NAME_RIL,
        .rp_cpu = "",
    };

    const socklen_t addrlen = sizeof(struct sockaddr_rpmsg);
    int fd = socket(AF_RPMSG, SOCK_STREAM, 0);
    if (fd < 0) {
        RLOGE("Failed to open RPMSG socket '%s': %s\n", "rpmsg_ril", strerror(errno));
        return -1;
    }

    int ret = bind(fd, (const struct sockaddr*)&addr, addrlen);
    if (ret) {
        RLOGE("Failed to bind RPMSG socket '%s': %s\n", "rpmsg_ril", strerror(errno));
        close(fd);
        return -1;
    }

    RLOGD("Successfully created and bound RPMSG socket '%s'", "rpmsg_ril");
    return fd;
}

#else

static int create_unix_socket(void)
{
    struct sockaddr_un addr;
    int fd, ret;

    fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        RLOGE("Failed to open socket '%s': %s\n", SOCKET_NAME_RIL, strerror(errno));
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), LOCAL_SOCKET_DIR "/%s", SOCKET_NAME_RIL);

    ret = unlink(addr.sun_path);
    if (ret != 0 && errno != ENOENT) {
        RLOGE("Failed to unlink old socket '%s': %s\n", SOCKET_NAME_RIL, strerror(errno));
        close(fd);
        return -1;
    }

    ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret) {
        RLOGE("Failed to bind socket '%s': %s\n", SOCKET_NAME_RIL, strerror(errno));
        unlink(addr.sun_path);
        close(fd);
        return -1;
    }

    RLOGD("Successfully created and bound UNIX socket '%s'", SOCKET_NAME_RIL);
    return fd;
}

#endif

int local_get_control_socket(const char* name)
{
    char key[64] = LOCAL_SOCKET_ENV_PREFIX;
    const char* val = NULL;
    int fd = 0;

    strlcpy(key + sizeof(LOCAL_SOCKET_ENV_PREFIX) - 1, name,
        sizeof(key) - sizeof(LOCAL_SOCKET_ENV_PREFIX));

    RLOGD("get env info");
    val = getenv(key);
    RLOGD("get env info val is %s", val);
    if (!val) {
        return -1;
    }

    errno = 0;
    fd = strtol(val, NULL, 10);
    if (errno) {
        return -1;
    }

    return fd;
}

int ril_socket_create(void)
{
#if defined(CONFIG_RPMSG_RIL)
    return create_rpmsg_socket();

#else
    return create_unix_socket();

#endif
}