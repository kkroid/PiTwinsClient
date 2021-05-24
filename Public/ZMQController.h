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
    int type = 1;
#ifdef USE_MJPEG
    VideoCapture *videoCapture{};
#endif
    vector<int> params;
public:
    Snowflake snowflake;
    const string BASE_CMD = R"({"tid":%lu,"type":%d,"payload":{"key":%d,"value":%d}})";

    ZMQController() {
        cout << "ZMQController created" << endl;
        threadPool = new ctpl::thread_pool(2);
#ifdef USE_MJPEG
        videoCapture = new VideoCapture();
        videoCapture->set(CAP_PROP_CONVERT_RGB, 0);
        // videoCapture->set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
        videoCapture->open("http://127.0.0.1:15555/?action=stream", CAP_OPENCV_MJPEG);
#endif
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
#ifdef USE_MJPEG
        if (videoCapture) {
            videoCapture->release();
        }
        delete videoCapture;
#endif
        delete cmdPusher;
        delete threadPool;
    }

    UInt64 nextId() {
        return snowflake.nextId();
    }

    void connect(const string &subAddr, const string &pushAddr) {
        spdlog::info("ZMQController connecting");
        context_t ctx;
#ifndef USE_MJPEG
        videoSubscriber = new socket_t(ctx, zmq::socket_type::sub);
        videoSubscriber->connect(subAddr);
        videoSubscriber->set(sockopt::subscribe, "");
#endif
        cmdPusher = new socket_t(ctx, socket_type::pub);
        cmdPusher->connect(pushAddr);
        spdlog::info("ZMQController connected");
        while (true) {
#ifdef USE_MJPEG
            // if (!videoCapture->isOpened()) {
            //     spdlog::warn("videoCapture not opened, wait...");
            //     sleep(1);
            //     continue;
            // }
            Mat matFrame;
            int ret = videoCapture->read(matFrame);
            if (!ret) {
                spdlog::error("Failed to recv data:{}", ret);
                break;
            }
#else
            message_t msg;
            auto ret = videoSubscriber->recv(msg);
            if (!ret) {
                spdlog::error("Failed to recv data");
                break;
            }
            auto *ucharData = static_cast<uchar *>(msg.data());
            vector<uchar> data;
            for (int i = 0, n = msg.size(); i < n; i++) {
                data.push_back(ucharData[i]);
            }
            Mat matFrame = imdecode(data, IMREAD_COLOR);
#endif
            int key = waitKey(1);
            // esc:27, 1: 49, 2: 50
            if (key >= 0) {
                if (key == 27) {
                    destroyAllWindows();
                    matFrame.release();
                    exit(0);
                } else {
                    if (key == 49) {
                        type = 1;
                    } else if (key == 50) {
                        type = 2;
                    } else {
                        threadPool->push([this, key](int id) {
                            int sendKey = 1;
                            if (key == 113) {
                                sendKey = 8;
                            } else {
                                sendKey = key + 1;
                            }
                            string cmd = format(BASE_CMD, nextId(), type, sendKey, 255);
                            spdlog::info("ZMQController: send cmd: {}", cmd);
                            push(cmd);
                        });
                    }
                }
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
