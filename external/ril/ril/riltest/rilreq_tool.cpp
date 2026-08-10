#include "ril_parcel_ctor_verifier.h"
#include <arpa/inet.h>
#include <chrono>
#include <memory>
#include <parcel.h>
#include <poll.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <syslog.h>
#include <unistd.h>
#include <vector>

#define SOCKET_NAME_RIL "/dev/socket/rild"

static int error_check(int argc, char** argv)
{
    if (argc < 3)
        return -1;

    return 0;
}

struct ril_req {
    void* data;
    size_t data_len;
};

struct ril_req_hdr {
    uint32_t len;
    uint32_t reqnum;
    uint32_t token;
};

static bool construct_ril_request(int id, int req, Parcel& p, struct ril_req& ril_req)
{
    struct ril_req_hdr header;
    ril_req.data_len = p.dataSize() + sizeof(header);
    ril_req.data = malloc(ril_req.data_len);
    if (!ril_req.data) {
        syslog(LOG_ERR, "Failed to allocate memory for ril request");
        return false;
    }

    header.len = htonl(ril_req.data_len - sizeof(header.len));
    header.reqnum = req;
    header.token = id;

    memcpy(ril_req.data, &header, sizeof(header));
    if (p.dataSize()) {
        memcpy((char*)ril_req.data + sizeof(header), p.data(), p.dataSize());
    }

    return true;
}

static int create_socket_and_connect(const char* name)
{
    struct sockaddr_un addr;
    size_t namelen;
    int fd;

    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    namelen = strlen(name);

    if (namelen > sizeof(addr) - offsetof(struct sockaddr_un, sun_path) - 1) {
        return -1;
    }

    strncpy(addr.sun_path, name, namelen);
    addr.sun_family = AF_LOCAL;
    socklen_t len = namelen + offsetof(struct sockaddr_un, sun_path) + 1;
    if (connect(fd, (struct sockaddr*)&addr, len) < 0) {
        perror("connect");
        return -1;
    }

    return fd;
}

extern "C" int main(int argc, char** argv)
{
    int sim_id = 0;
    char socket_name[20];

    if (error_check(argc, argv) < 0) {
        print_usage(argv[0]);
        return -1;
    }

    sim_id = atoi(argv[1]);
    memset(socket_name, 0, sizeof(socket_name));
    if (sim_id == 0) {
        strncpy(socket_name, SOCKET_NAME_RIL, sizeof(socket_name) - 1);
    } else {
        syslog(LOG_ERR, "MultiSim Not Supported");
        return -1;
    }

    int fd = create_socket_and_connect(socket_name);
    if (fd < 0) {
        syslog(LOG_ERR, "Failed opening ril socket: %s", socket_name);
        return -1;
    }

    sleep(1);
    {
        char junk[2048];
        recv(fd, junk, sizeof(junk), 0);
        syslog(LOG_INFO, "Receive handshake data from rild");
    }

    int test_case_id = atoi(argv[2]);
    if (test_case_id < 0 || test_case_id > case_num()) {
        syslog(LOG_ERR, "Invalid test case id: %d", test_case_id);
        close(fd);
        return -1;
    }

    struct ril_test_case* test_case = get_test_case(test_case_id);
    syslog(LOG_INFO, "Test case %d start: %s", test_case->id, test_case->case_name);

    Parcel p;
    if (!test_case->ctor(p)) {
        syslog(LOG_ERR, "Failed to construct parcel data.");
        close(fd);
        return -1;
    }

    struct ril_req ril_req;
    if (!construct_ril_request(test_case->id, test_case->reqnum, p, ril_req)) {
        syslog(LOG_ERR, "Failed to construct ril request");
        close(fd);
        return -1;
    }

    int ret = send(fd, ril_req.data, ril_req.data_len, 0);
    if (ret < 0 || static_cast<size_t>(ret) != ril_req.data_len) {
        perror("Socket write error when sending ril_request");
        free(ril_req.data);
        close(fd);
        return -1;
    }

    free(ril_req.data);

    struct pollfd pfd;
    pfd.events = POLLIN;
    pfd.fd = fd;
    ret = poll(&pfd, 1, 5000);
    if (ret < 0) {
        perror("poll");
        close(fd);
        return -1;
    } else if (ret == 0) {
        syslog(LOG_ERR, "Timeout occurred, did you implement the RIL response function?");
        close(fd);
        return -1;
    }

    std::vector<uint8_t> buffer;
    uint8_t tmp[2048];

    const int timeout_ms = 5000;
    auto start_time = std::chrono::steady_clock::now();

    while (true) {
        struct pollfd pfd_recv;
        pfd_recv.fd = fd;
        pfd_recv.events = POLLIN;
        int poll_ret = poll(&pfd_recv, 1, timeout_ms);

        if (poll_ret < 0) {
            perror("poll in recv loop");
            break;
        } else if (poll_ret == 0) {
            syslog(LOG_INFO, "No data received within timeout, stopping recv loop");
            break;
        }

        ret = recv(fd, tmp, sizeof(tmp), 0);
        if (ret > 0) {
            buffer.insert(buffer.end(), tmp, tmp + ret);
        } else if (ret == 0) {
            syslog(LOG_INFO, "Socket closed by peer");
            break;
        } else {
            perror("recv");
            break;
        }

        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
        if (elapsed_ms > timeout_ms) {
            syslog(LOG_INFO, "Recv loop timeout after %lld ms", elapsed_ms);
            break;
        }
    }

    size_t offset = 0;
    std::vector<std::unique_ptr<Parcel>> parcels;
    while (offset < buffer.size()) {
        if (buffer.size() - offset < 4) {
            syslog(LOG_ERR, "Incomplete Parcel length at offset %zu", offset);
            break;
        }
        uint32_t len = ntohl(*(uint32_t*)(buffer.data() + offset));
        offset += 4;
        if (buffer.size() - offset < len) {
            syslog(LOG_ERR, "Incomplete Parcel data at offset %zu", offset);
            break;
        }
        auto parcel = std::make_unique<Parcel>();
        parcel->setData(buffer.data() + offset, len);
        parcels.push_back(std::move(parcel));
        offset += len;
    }

    for (auto& parcel : parcels) {
        int32_t type;
        if (parcel->readInt32(&type) != 0) {
            syslog(LOG_ERR, "Failed to read Parcel type");
            continue;
        }
        parcel->setDataPosition(0);

        if (type == RESPONSE_SOLICITED) {
            syslog(LOG_INFO, "Verifying test case %s", test_case->case_name);
            if (!test_case->verif(test_case->id, *parcel)) {
                syslog(LOG_ERR, "Test case %d verification failed: %s",
                    test_case->id, test_case->case_name);
                return -1;
            }
        } else if (type == RESPONSE_UNSOLICITED) {
            if (!verify_unsolicited(*parcel)) {
                syslog(LOG_ERR, "Unsolicited response verification failed");
                return -1;
            }
        } else {
            syslog(LOG_ERR, "Unknown Parcel type: %ld", type);
            return -1;
        }
    }

    syslog(LOG_INFO, "Test case %d success: %s", test_case_id, test_case->case_name);
    close(fd);

    return 0;
}