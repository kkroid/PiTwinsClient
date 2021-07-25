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

#include <utility>
#include "slice.h"
#include "buffer.h"
#include "MessageReceiver.h"

#define ADDR_MSG_SERVER "0.0.0.0:5556"
#define ADDR_VIDEO_SERVER "0.0.0.0:5555"
#define NAME_MSG_SERVER "MsgServer"
#define NAME_VIDEO_SERVER "VideoServer"
#define THREAD_NUM 2


namespace PiRPC {
    class Server {
    private:
        int threadNum = THREAD_NUM;
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
            return videoInstance;
        }

        static Server &getMsgInstance() {
            static Server msgInstance;
            return msgInstance;
        }

        void init(std::string address, std::string serverName, const evpp::ConnectionCallback &ccb = nullptr,
                  const evpp::MessageCallback &mcb = nullptr) {
            addr = std::move(address);
            name = std::move(serverName);
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

        void send(const void *data, size_t len) {
            if (tcpConnPtr) {
                Buffer buffer;
                buffer.AppendInt32(len);
                buffer.Append(data, len);
                tcpConnPtr->Send(&buffer);
                // spdlog::info("{} send a msg to {}", name, tcpConnPtr->remote_addr());
            }
        }

        void send(const std::string &d) {
            send(d.c_str(), d.length());
        }

        void release();

        // 拒绝拷贝构造
        Server(const Server &rhs) = delete;

        // 拒绝拷贝赋值
        Server &operator=(const Server &rhs) = delete;
    };
}

#endif //PITWINS_SERVER_H
