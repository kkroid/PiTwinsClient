//
// Created by Will Zhang on 2021/7/12.
//

#ifndef PITWINSCLIENT_CLIENT_H
#define PITWINSCLIENT_CLIENT_H

#include <spdlog/spdlog.h>
#include <tcp_client.h>
#include <buffer.h>
#include <event_loop.h>
#include <tcp_conn.h>

#define ADDR_CMD_SERVER "127.0.0.1:15556"
#define ADDR_VIDEO_SERVER "127.0.0.1:15555"

class Client {
private:
    std::string addr = ADDR_CMD_SERVER;
    evpp::TCPClient *client = nullptr;
    evpp::EventLoop *loop = nullptr;
public:

    Client() = default;

    ~Client() {
        spdlog::info("~Client");
        delete loop;
        delete client;
    }

    static Client &getVideoClient(const char *_addr = nullptr) {
        static Client videoClient;
        videoClient.addr = _addr == nullptr ? ADDR_VIDEO_SERVER : _addr;
        return videoClient;
    }

    static Client &getCMDClient(const char *_addr = nullptr) {
        static Client cmdClient;
        cmdClient.addr = _addr == nullptr ? ADDR_CMD_SERVER : _addr;
        return cmdClient;
    }

    // 拒绝拷贝构造
    Client(const Client &rhs) = delete;

    // 拒绝拷贝赋值
    Client &operator=(const Client &rhs) = delete;

    void init(const evpp::ConnectionCallback &ccb = nullptr,
              const evpp::MessageCallback &mcb = nullptr);

    void setConnectionCallback(const evpp::ConnectionCallback &ccb = nullptr);

    void setMessageCallback(const evpp::MessageCallback &mcb = nullptr);

    void connect();

    void disconnect();
};


#endif //PITWINSCLIENT_CLIENT_H
