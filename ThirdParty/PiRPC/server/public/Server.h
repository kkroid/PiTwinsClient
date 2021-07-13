//
// Created by Will Zhang on 2021/7/10.
//

#ifndef PITWINS_SERVER_H
#define PITWINS_SERVER_H

#include <spdlog/spdlog.h>
#include <tcp_server.h>
#include <buffer.h>
#include <event_loop.h>
#include <tcp_conn.h>


class Server {
public:
    std::string DEFAULT_ADDR = "0.0.0.0:8888";
    int DEFAULT_THREAD_NUM = 4;
    evpp::TCPServer *server = nullptr;
    evpp::EventLoop *loop = nullptr;

    Server() = default;

    ~Server() {
        spdlog::info("~Server");
        delete loop;
        delete server;
    }

    static Server &getInstance() {
        static Server instance;
        return instance;
    }

    void init(const std::string& addr, const evpp::ConnectionCallback &ccb, const evpp::MessageCallback& mcb);

    void run();

    // 拒绝拷贝构造
    Server(const Server &rhs) = delete;

    // 拒绝拷贝赋值
    Server &operator=(const Server &rhs) = delete;
};

#endif //PITWINS_SERVER_H
