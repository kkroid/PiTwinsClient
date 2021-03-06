#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Client.h"
#include "utils.h"
#include <opencv2/opencv.hpp>
#include "commom/json.hpp"

using namespace std;
using namespace PiRPC;

#define UP 16777235
#define DOWN 16777237
#define LEFT 16777234
#define RIGHT 16777236
#define W 87
#define S 83
#define A 65
#define D 68

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->cmdAddr->setText("127.0.0.1:15556");
    ui->videoAddr->setText("127.0.0.1:15555");
    threadPool = new ctpl::thread_pool(2);
    spdlog::info("MainWindow created");
}

MainWindow::~MainWindow() {
    delete ui;
    threadPool->clear_queue();
    delete threadPool;
}

void MainWindow::on_connectBtn_clicked() {
    auto msgServer = ui->cmdAddr->text().toStdString();
    ui->cmdAddr->clearFocus();
    ui->videoAddr->clearFocus();
    ui->Frame->setFocus();
    spdlog::info("Connect button clicked, msgServer = {}", msgServer);

    threadPool->push([this, msgServer](int id) {
        spdlog::info("start to connect msg server");
        Client::getMsgClient().init(msgServer, "msg server");
        Client::getMsgClient().setOnConnectionChangedCallback([this](int status) {
            onMsgServerConnectionChanged(status);
        });
        Client::getMsgClient().setOnNewMsgReceivedCallback([this](const char *data, size_t size) {
            onMsgReceived(std::string(data, size));
        });
        Client::getMsgClient().connect();
    });
}

void MainWindow::keyPressEvent(QKeyEvent *keyEvent) {
    switch (keyEvent->key()) {
        case UP:
            Client::getMsgClient().Send(MsgGen::servo(2, -1));
            break;
        case DOWN:
            Client::getMsgClient().Send(MsgGen::servo(2, 1));
            break;
        case LEFT:
            Client::getMsgClient().Send(MsgGen::servo(1, 1));
            break;
        case RIGHT:
            Client::getMsgClient().Send(MsgGen::servo(1, -1));
            break;
        case W:
            break;
        case S:
            break;
        case A:
            break;
        case D:
            break;
        default:
            break;
    }
    spdlog::info("{} = [{}] key pressed", keyEvent->text().toStdString(), keyEvent->key());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    spdlog::info("window onClose called");
    Client::getMsgClient().disconnect();
    Client::getVideoClient().disconnect();
}

void MainWindow::onMsgServerConnectionChanged(int status) {
    if (status == 2) {
        // request open camera
        std::string msg = MsgGen::camera(0, 0);
        Client::getMsgClient().Send(msg);
        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::green);
        ui->cmdLabel->setPalette(pe);
    } else if (status == 3) {
        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::black);
        ui->cmdLabel->setPalette(pe);
    }
}

void MainWindow::onVideoServerConnectionChanged(int status) {
    if (status == 2) {
        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::green);
        ui->videoLabel->setPalette(pe);
    } else if (status == 3) {
        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::black);
        ui->videoLabel->setPalette(pe);
    }
}

void MainWindow::onMsgReceived(const std::string &cmd) {
    spdlog::info("Client Received A Message:{}", cmd);
    nlohmann::json obj = nlohmann::json::parse(cmd);
    int type = obj["type"];
    if (type == TYPE_CAMERA_CTRL) {
        auto videoServer = ui->videoAddr->text().toStdString();
        threadPool->push([this, videoServer](int id) {
            spdlog::info("start to connect video server");
            Client::getVideoClient().init(videoServer, "video server");
            Client::getVideoClient().setOnConnectionChangedCallback([this](const int status) {
                onVideoServerConnectionChanged(status);
            });
            Client::getVideoClient().setOnNewMsgReceivedCallback([this](const char *data, size_t size) {
                vector<unsigned char> frameData(data, data + size);
                onVideoFrameReceived(frameData);
            });
            Client::getVideoClient().connect(false);
        });
    }
}

void MainWindow::onVideoFrameReceived(const std::vector<unsigned char> &frameData) {
    cv::Mat matFrame = cv::imdecode(frameData, cv::IMREAD_UNCHANGED);
    if (matFrame.empty() || matFrame.rows == 0) {
        spdlog::warn("Invalid frame:{}, {}, {}, ignored", frameData.size(), matFrame.rows, matFrame.cols);
        matFrame.release();
        return;
    }
    spdlog::trace("Valid frame:{}, {}, {}", frameData.size(), matFrame.rows, matFrame.cols);

    QImage img = QImage((const uchar *) (matFrame.data),
                        matFrame.cols, matFrame.rows,
                        matFrame.cols * matFrame.channels(),
                        QImage::Format_BGR888);
    ui->Frame->setPixmap(QPixmap::fromImage(img));
    ui->Frame->resize(ui->Frame->pixmap()->size());
    ui->Frame->show();
    matFrame.release();
}
