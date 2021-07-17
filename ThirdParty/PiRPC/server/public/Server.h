//
// Created by Will Zhang on 2021/7/10.
//

#ifndef PITWINS_SERVER_H
#define PITWINS_SERVER_H

#include <spdlog/spdlog.h>
#include <tcp_server.h>
#include <buffer.h>
#include <tcp_conn.h>
#include <event_loop.h>
#include "slice.h"
#include "buffer.h"

#define ADDR_CMD_SERVER "0.0.0.0:5556"
#define ADDR_VIDEO_SERVER "0.0.0.0:5555"
#define NAME_CMD_SERVER "CMDServer"
#define NAME_VIDEO_SERVER "VideoServer"
#define THREAD_NUM_VIDEO_SERVER 4
#define THREAD_NUM_CMD_SERVER 4


class Server {
private:
    int threadNum = THREAD_NUM_CMD_SERVER;
    std::string addr = ADDR_CMD_SERVER;
    std::string name = NAME_CMD_SERVER;
    evpp::TCPServer *server = nullptr;
    evpp::EventLoop *loop = nullptr;
    int clientCount = 0;
    std::shared_ptr<evpp::TCPConn> tcpConnPtr = nullptr;
public:

    Server() = default;

    ~Server() {
        spdlog::info("~Server");
        if (tcpConnPtr) {
            tcpConnPtr->Close();
        }
        delete loop;
        delete server;
    }

    static Server &getVideoInstance() {
        static Server videoInstance;
        videoInstance.addr = ADDR_VIDEO_SERVER;
        videoInstance.name = NAME_VIDEO_SERVER;
        videoInstance.threadNum = THREAD_NUM_VIDEO_SERVER;
        return videoInstance;
    }

    static Server &getCMDInstance() {
        static Server cmdInstance;
        cmdInstance.addr = ADDR_CMD_SERVER;
        cmdInstance.name = NAME_CMD_SERVER;
        cmdInstance.threadNum = THREAD_NUM_CMD_SERVER;
        return cmdInstance;
    }

    void init(const evpp::ConnectionCallback &ccb = nullptr, const evpp::MessageCallback &mcb = nullptr) {
        loop = new evpp::EventLoop();
        server = new evpp::TCPServer(loop, addr, name, threadNum);
        setConnectionCallback(ccb);
        setMessageCallback(mcb);
    }

    void setConnectionCallback(const evpp::ConnectionCallback &ccb);

    void setMessageCallback(const evpp::MessageCallback &mcb);

    void run();

    bool isRunning() {
        return server != nullptr && server->IsRunning();
    }

    void send(const char *s) {
        send(s, strlen(s));
    }

    void send(const void *d, size_t dlen) {
        if (tcpConnPtr) {
            tcpConnPtr->Send(d, dlen);
            spdlog::info("send frame to {}", tcpConnPtr->remote_addr());
        }
    }

    void send(const std::string &d) {
        if (tcpConnPtr) {
            tcpConnPtr->Send(d);
        }
    }

    void send(const evpp::Slice &message) {
        if (tcpConnPtr) {
            tcpConnPtr->Send(message);
        }
    }

    void send(evpp::Buffer *buf) {
        if (tcpConnPtr) {
            tcpConnPtr->Send(buf);
        }
    }

    int getClientCount() {
        return clientCount;
    }

    void release();

    // 拒绝拷贝构造
    Server(const Server &rhs) = delete;

    // 拒绝拷贝赋值
    Server &operator=(const Server &rhs) = delete;
};

#endif //PITWINS_SERVER_H
