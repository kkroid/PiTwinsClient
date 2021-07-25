//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_MESSAGEPROCESSOR_H
#define PITWINS_MESSAGEPROCESSOR_H

#include "json.hpp"
#include "spdlog/spdlog.h"
#include <buffer.h>
#include "MessageGenerator.h"

class MessageProcessor {
public:
    virtual void process(nlohmann::json msg) = 0;
    virtual void process(evpp::Buffer *buffer) = 0;

    static void processUnknownMessage(evpp::Buffer *buffer, const std::string &error) {
        spdlog::warn("processUnknownMessage error:{}, msg:{}", error, buffer->ToString());
    }
};

#endif //PITWINS_MESSAGEPROCESSOR_H
