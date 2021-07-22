//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_MESSAGERECEIVER_H
#define PITWINS_MESSAGERECEIVER_H


#include <buffer.h>
#include <tcp_conn.h>
#include "spdlog/spdlog.h"
#include "json.hpp"
#include "CameraMsgProcessor.h"
#include "ServoMsgProcessor.h"

using namespace evpp;

#define MSG_TYPE_CAMERA 0
#define MSG_TYPE_SERVO 1
#define MSG_TYPE_DC_MOTOR 2

class MessageReceiver {
private:
    std::map<int, MessageProcessor*> processorMapping;
public:

    MessageReceiver() = default;

    ~MessageReceiver() {
        processorMapping.empty();
    }

    static MessageReceiver &getInstance() {
        static MessageReceiver instance;
        return instance;
    }

    // 拒绝拷贝构造
    MessageReceiver(const MessageReceiver &rhs) = delete;

    // 拒绝拷贝赋值
    MessageReceiver &operator=(const MessageReceiver &rhs) = delete;

    void onNewMsgReceived(const TCPConnPtr &tcpConnPtr, Buffer *buffer) {
        std::string msg = buffer->ToString();
        try {
            nlohmann::json obj = nlohmann::json::parse(msg);
            MessageProcessor* processor = getOrCreateProcessor(obj["type"]);
            processor->process(obj);
        } catch (nlohmann::detail::exception& e) {
            MessageProcessor::processUnknownMessage(buffer, e.what());
        }
    }

    MessageProcessor* getOrCreateProcessor(int type) {
        MessageProcessor* processor = processorMapping[type];
        if (nullptr == processor) {
            switch (type) {
                case MSG_TYPE_CAMERA:
                    processor = new CameraMsgProcessor();
                    break;
                case MSG_TYPE_SERVO:
                    processor = new ServoMsgProcessor();
                    break;
                case MSG_TYPE_DC_MOTOR:
                    spdlog::info("No impl yet:{}", type);
                    break;
                default:
                    spdlog::warn("No such msg processor:{}", type);
                    break;
            }
        }
        return processor;
    }
};

#endif //PITWINS_MESSAGERECEIVER_H
