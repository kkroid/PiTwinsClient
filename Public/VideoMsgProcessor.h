//
// Created by Will Zhang on 2021/7/23.
//

#ifndef PITWINSCLIENT_VIDEOMSGPROCESSOR_H
#define PITWINSCLIENT_VIDEOMSGPROCESSOR_H

#include "MessageProcessor.h"

class VideoMsgProcessor : MessageProcessor {
public:
    void process(nlohmann::json msg) override;
};

#endif //PITWINSCLIENT_VIDEOMSGPROCESSOR_H
