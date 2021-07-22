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
    std::string name = "Client";
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

    static Client &getMsgClient() {
        static Client msgClient;
        return msgClient;
    }

    // 拒绝拷贝构造
    Client(const Client &rhs) = delete;

    // 拒绝拷贝赋值
    Client &operator=(const Client &rhs) = delete;

    void init(std::string address, std::string clientName, const evpp::ConnectionCallback &ccb = nullptr,
              const evpp::MessageCallback &mcb = nullptr);

    void setConnectionCallback(const evpp::ConnectionCallback &ccb = nullptr);

    void setMessageCallback(const evpp::MessageCallback &mcb = nullptr);

    void connect();

    void disconnect();

    void Send(const char* s) {
        Send(s, strlen(s));
    }
    void Send(const void* d, size_t dlen) {
        if (client && client->conn()->IsConnected()) {
            client->conn()->Send(d, dlen);
        }
    }

    void Send(const std::string& msg) {
        Send(msg.data(), msg.length());
    }
};


#endif //PITWINSCLIENT_CLIENT_H
