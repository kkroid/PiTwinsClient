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

#define DEFAULT_ADDR_CMD_SERVER "127.0.0.1:15556"
#define DEFAULT_ADDR_VIDEO_SERVER "127.0.0.1:15555"

class Client {
private:
    std::string addr = DEFAULT_ADDR_CMD_SERVER;
    evpp::TCPClient *client = nullptr;
    evpp::EventLoop *loop = nullptr;
public:

    Client() = default;

    ~Client() {
        spdlog::info("~Client");
        delete loop;
        delete client;
    }

    static Client &getVideoClient() {
        static Client videoClient;
        return videoClient;
    }

    static Client &getCMDClient() {
        static Client cmdClient;
        return cmdClient;
    }

    // 拒绝拷贝构造
    Client(const Client &rhs) = delete;

    // 拒绝拷贝赋值
    Client &operator=(const Client &rhs) = delete;

    void init(const char *address, char *name, const evpp::ConnectionCallback &ccb = nullptr,
              const evpp::MessageCallback &mcb = nullptr);

    void setConnectionCallback(const evpp::ConnectionCallback &ccb = nullptr);

    void setMessageCallback(const evpp::MessageCallback &mcb = nullptr);

    void connect();

    void disconnect();
};


#endif //PITWINSCLIENT_CLIENT_H
