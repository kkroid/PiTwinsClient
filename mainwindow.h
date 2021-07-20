#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ctpl_stl.h"
#include <QKeyEvent>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ctpl::thread_pool *threadPool;
private slots:
    void on_connect_clicked();
    void keyPressEvent(QKeyEvent *keyEvent);
    void closeEvent(QCloseEvent *event);

};
#endif // MAINWINDOW_H
