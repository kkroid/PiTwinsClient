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

class Client {
public:
    evpp::TCPClient *client = nullptr;
    evpp::EventLoop *loop = nullptr;

    Client() = default;

    ~Client() {
        spdlog::info("~Client");
        delete loop;
        delete client;
    }

    static Client &getInstance() {
        static Client instance;
        return instance;
    }

    // 拒绝拷贝构造
    Client(const Client &rhs) = delete;

    // 拒绝拷贝赋值
    Client &operator=(const Client &rhs) = delete;

    void init(const std::string& addr, const evpp::ConnectionCallback& ccb, const evpp::MessageCallback& mcb);

    void connect();

    void disconnect();
};


#endif //PITWINSCLIENT_CLIENT_H
