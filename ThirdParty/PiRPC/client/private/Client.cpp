//
// Created by Will Zhang on 2021/7/12.
//
#include "Client.h"

void Client::init(const std::string &addr, const evpp::ConnectionCallback &ccb, const evpp::MessageCallback& mcb) {
    loop = new evpp::EventLoop();
    client = new evpp::TCPClient(loop, addr, "PiClient");
    client->SetConnectionCallback(ccb);
    client->SetMessageCallback(mcb);
}

void Client::connect() {
    client->Connect();
    client->set_auto_reconnect(true);
    loop->Run();
}

void Client::disconnect() {
    client->Disconnect();
}
