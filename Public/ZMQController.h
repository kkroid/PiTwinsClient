//
// Created by Will Zhang on 2021/1/18.
//

#ifndef PITWINSCLIENT_ZMQCONTROLLER_H
#define PITWINSCLIENT_ZMQCONTROLLER_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include "zmq.hpp"
#include "ctpl_stl.h"
#include "Snowflak.h"
#include "utils.h"
#include "spdlog/spdlog.h"


using namespace std;
using namespace zmq;
using namespace cv;


class ZMQController {
private:
    socket_t *videoSubscriber{};
    socket_t *cmdPusher{};
    ctpl::thread_pool *threadPool;
    // VideoCapture *videoCapture{};
    vector<int> params;
public:
    Snowflake snowflake;
    const string BASE_CMD = R"({"tid":%lu,"type":1,"payload":{"pin":%d,"value":%d,"key":%d}})";

    ZMQController() {
        cout << "ZMQController created" << endl;
        threadPool = new ctpl::thread_pool(2);
        // videoCapture = new VideoCapture("http://127.0.0.1:15555/?action=stream");
        // videoCapture->set(CAP_PROP_CONVERT_RGB, 0);
        params.push_back(IMWRITE_JPEG_QUALITY);
        params.push_back(60);
    }

    virtual ~ZMQController() {
        cout << "ZMQController released" << endl;
        if (cmdPusher) {
            cmdPusher->close();
        }
        if (videoSubscriber) {
            videoSubscriber->close();
        }
        // if (videoCapture) {
        //     videoCapture->release();
        // }
        delete cmdPusher;
        delete threadPool;
        // delete videoCapture;
    }

    UInt64 nextId() {
        return snowflake.nextId();
    }

    void connect(const string &subAddr, const string &pushAddr) {
        spdlog::info("ZMQController connecting");
        context_t ctx;
        videoSubscriber = new socket_t(ctx, zmq::socket_type::sub);
        videoSubscriber->connect(subAddr);
        videoSubscriber->set(sockopt::subscribe, "");
        cmdPusher = new socket_t(ctx, socket_type::push);
        cmdPusher->connect(pushAddr);
        spdlog::info("ZMQController connected");
        for (;;) {
            Mat matFrame;
            message_t msg;
            auto ret = videoSubscriber->recv(msg);
            // auto ret = videoCapture->read(matFrame);
            if (!ret) {
                spdlog::error("Failed to recv data");
                break;
            }
            auto *ucharData = static_cast<uchar *>(msg.data());
            vector<uchar> data;
            for (int i = 0, n = msg.size(); i < n; i++) {
                data.push_back(ucharData[i]);
            }
            matFrame = imdecode(data, IMREAD_COLOR);
            int key = waitKey(1);
            if (key >= 0) {
                threadPool->push([this, key](int id) {
                    string cmd = format(BASE_CMD, nextId(), 300, 250, key);
                    spdlog::info("ZMQController: send cmd: {}", cmd);
                    push(cmd);
                });
            }
            imshow("PiTwinsClient", matFrame);
            matFrame.release();
        }
    }

    void push(const string &cmd) {
        cmdPusher->send(const_buffer(cmd.c_str(), cmd.size()), send_flags::none);
    }
};

#endif //PITWINSCLIENT_ZMQCONTROLLER_H
