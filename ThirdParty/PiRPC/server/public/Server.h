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
#include "MessageReceiver.h"

#define ADDR_MSG_SERVER "0.0.0.0:5556"
#define ADDR_VIDEO_SERVER "0.0.0.0:5555"
#define NAME_MSG_SERVER "MsgServer"
#define NAME_VIDEO_SERVER "VideoServer"
#define THREAD_NUM_VIDEO_SERVER 4
#define THREAD_NUM_MSG_SERVER 4


class Server {
private:
    int threadNum = THREAD_NUM_MSG_SERVER;
    std::string addr = ADDR_MSG_SERVER;
    std::string name = NAME_MSG_SERVER;
    evpp::TCPServer *server = nullptr;
    evpp::EventLoop *loop = nullptr;
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

    static Server &getMsgInstance() {
        static Server msgInstance;
        msgInstance.addr = ADDR_MSG_SERVER;
        msgInstance.name = NAME_MSG_SERVER;
        msgInstance.threadNum = THREAD_NUM_MSG_SERVER;
        return msgInstance;
    }

    void init() {
        loop = new evpp::EventLoop();
        server = new evpp::TCPServer(loop, addr, name, threadNum);
        setConnectionCallback(nullptr);
        setMessageCallback(nullptr);
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
            // spdlog::info("{} send a msg to {}", name, tcpConnPtr->remote_addr());
        }
    }

    void send(const std::string &d) {
        send(d.c_str());
    }

    void release();

    // 拒绝拷贝构造
    Server(const Server &rhs) = delete;

    // 拒绝拷贝赋值
    Server &operator=(const Server &rhs) = delete;
};

#endif //PITWINS_SERVER_H
