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
#include "PiRPCCallbacks.h"


namespace PiRPC {
    class Client {
    private:
        std::string addr = "";
        std::string name = "Client";
        evpp::TCPClient *client = nullptr;
        evpp::EventLoop *loop = nullptr;
        PiRPC::OnNewMsgReceived _onNewMsgReceived = nullptr;
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

        void init(std::string address, std::string clientName, const evpp::ConnectionCallback &ccb = nullptr);

        void setConnectionCallback(const evpp::ConnectionCallback &ccb = nullptr);

        void setOnNewMsgReceivedCallback(PiRPC::OnNewMsgReceived onNewMsgReceived) {
            _onNewMsgReceived = std::move(onNewMsgReceived);
        }

        void connect();

        void disconnect();

        void Send(const char *s) {
            Send(s, strlen(s));
        }

        void Send(const void *d, size_t dlen) {
            if (client && client->conn()->IsConnected()) {
                client->conn()->Send(d, dlen);
            }
        }

        void Send(const std::string &msg) {
            // spdlog::info("send msg:{}", msg);
            Send(msg.data(), msg.length());
        }
    };
}


#endif //PITWINSCLIENT_CLIENT_H
