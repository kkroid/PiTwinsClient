//
// Created by Will Zhang on 2021/7/21.
//

#ifndef PITWINS_MESSAGEGENERATOR_H
#define PITWINS_MESSAGEGENERATOR_H

#include <utility>

#include "Snowflak.h"

#define TYPE_OPEN_CAMERA 0
#define TYPE_CLOSE_CAMERA 1
#define TYPE_CAMERA_OPENED 2

#define BASE_MSG_FORMAT R"({"tid":%llu,"type":%d,"payload":%s})"


class MessageGenerator {

public:
    template<typename ... Args>
    static std::string format(const std::string &format, Args ... args) {
        int size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
        if (size <= 0) { throw std::runtime_error("Error during formatting."); }
        std::unique_ptr<char[]> buf(new char[size]);
        snprintf(buf.get(), size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }

    static std::string gen(int type, const std::string& payload) {
        static Snowflake snowflake;
        return format(BASE_MSG_FORMAT, snowflake.nextId(), type, payload.c_str());
    }
};

#endif //PITWINS_MESSAGEGENERATOR_H
