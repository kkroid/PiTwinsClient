//
// Created by Will Zhang on 2021/7/12.
//
#include "Client.h"

#include <utility>

namespace PiRPC {
    void Client::init(std::string address, std::string clientName) {
        addr = std::move(address);
        name = std::move(clientName);
        loop = new evpp::EventLoop();
        client = new evpp::TCPClient(loop, addr, name);
        client->SetConnectionCallback([this](const evpp::TCPConnPtr &connPtr) {
            spdlog::info("{} connection status:{}", client->name(), connPtr->status());
            if (_onConnectionChanged) {
                _onConnectionChanged(connPtr->status());
            }
        });
        client->SetMessageCallback([this](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
            // 拆包分包处理
            int32_t realDataSize = buffer->PeekInt32();
            int32_t headerSize = sizeof(realDataSize);
            int32_t currentDataSize = buffer->size() - headerSize;
            int32_t delta = realDataSize - currentDataSize;
            if (delta > 0) {
                spdlog::info("数据不够,realDataSize:{}, currentDataSize:{}, peek:{}, delta:{}", realDataSize,
                             currentDataSize, buffer->PeekInt32(), realDataSize - currentDataSize);
                return;
            } else if (delta == 0) {
                spdlog::info("数据充足,realDataSize:{}, currentDataSize:{}, headerSize:{}", realDataSize, currentDataSize,
                             headerSize);
                buffer->Skip(headerSize);
                if (_onNewMsgReceived) {
                    _onNewMsgReceived(buffer->data(), buffer->size());
                }
                // TODO 调查为什么需要reset
                buffer->Reset();
            } else {
                int count = 0;
                while (true) {
                    if (realDataSize - currentDataSize < 0) {
                        spdlog::info("数据多了{}，拆包，发送处理, realDataSize:{}, currentDataSize:{}, headerSize:{}", count++,
                                     realDataSize, currentDataSize, headerSize);
                        evpp::Buffer data;
                        buffer->Skip(headerSize);
                        data.Append(buffer->data(), realDataSize);
                        buffer->Skip(realDataSize);
                        if (_onNewMsgReceived) {
                            _onNewMsgReceived(data.data(), data.size());
                        }
                        realDataSize = buffer->PeekInt32();
                        currentDataSize = buffer->size() - headerSize;
                    } else {
                        break;
                    }
                }
            }
        });
    }

    void Client::connect() {
        client->Connect();
        client->set_auto_reconnect(true);
        loop->Run();
    }

    void Client::disconnect() {
        if (client) {
            client->conn()->Close();
            client->Disconnect();
            loop->Stop();
        }
    }
}
