//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_MESSAGERECEIVER_H
#define PITWINS_MESSAGERECEIVER_H


#include <buffer.h>
#include <tcp_conn.h>
#include "spdlog/spdlog.h"
#include "json.hpp"

#if PiRPC_TYPE == 0

#include "CameraMsgProcessor.h"
#include "ServoMsgProcessor.h"

#elif PiRPC_TYPE == 1

#include "MessageProcessor.h"

#endif

using namespace evpp;


namespace PiRPC {
    class MessageReceiver {
    private:
        std::map<int, MessageProcessor *> processorMapping;
    public:
        typedef std::function<void(const char *, size_t)> MessageCallback;

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
            buffer->Reset();
            try {
                nlohmann::json obj = nlohmann::json::parse(msg);
                MessageProcessor *processor = getOrCreateProcessor(obj["type"]);
                processor->process(obj);
            } catch (nlohmann::detail::exception &e) {
                MessageProcessor::processUnknownMessage(buffer, e.what());
            }
        }

        MessageProcessor *getOrCreateProcessor(int type) {
            MessageProcessor *processor = processorMapping[type];
            if (nullptr == processor) {
                switch (type) {
#if PiRPC_TYPE == 0
                    case TYPE_CAMERA_CTRL:
                        processor = new CameraMsgProcessor();
                        processorMapping[type] = processor;
                        break;
                    case TYPE_SERVO_CTRL:
                        processor = new ServoMsgProcessor();
                        processorMapping[type] = processor;
                        break;
                    case TYPE_MOTOR_CTRL:
                        spdlog::info("No impl yet:{}", type);
                        // processor = new ServoMsgProcessor();
                        break;
#elif PiRPC_TYPE == 1

#endif
                    default:
                        spdlog::warn("No such msg processor:{}", type);
                        break;
                }
            }
            return processor;
        }
    };
}

#endif //PITWINS_MESSAGERECEIVER_H
