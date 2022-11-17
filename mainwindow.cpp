
#include <QDebug>
#include <iostream>
#include <QHostAddress>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <QSignalMapper>
QT_BEGIN_NAMESPACE
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    uiInit();
}

MainWindow::~MainWindow() //析构函数，删除ui界面
{
    delete ui;
}

void MainWindow::uiInit()
{
    relay_tcpClient = new HMyTcpClient(666);
    relay_tcpClient->cui = ui;
    if (relay_tcpClient && !relay_tcpClient->isConnected())
    {
        relay_tcpClient->connetServer("192.168.2.232", 60000);
    }

    sensor_tcpClient = new HMyTcpClient(777);
    sensor_tcpClient->cui = ui;
    if (sensor_tcpClient && !sensor_tcpClient->isConnected())
    {
        sensor_tcpClient->connetServer("192.168.2.80", 10123);
    }

    Maininterface_map = new QSignalMapper();

    Maininterface_btn<<ui->Relay_status<<ui->production_monitoring<<ui->outdoor_parameters;
    for (int i = 0; i < Maininterface_btn.size(); ++i)
    {
        Maininterface_map->setMapping(Maininterface_btn[i], i);
        connect(Maininterface_btn[i], SIGNAL(clicked(bool)), Maininterface_map, SLOT(map()));
    }
    connect(Maininterface_map, SIGNAL(mapped(int)), this, SLOT(Maininterface_ctl(int)));
}

void MainWindow::Maininterface_ctl(int flag)
{
    ui->stackedWidget->setCurrentIndex(flag);
    for (int i = 0; i < Maininterface_btn.size(); i++)
    {
        if (i == flag)
            Maininterface_btn[i]->setStyleSheet("QPushButton{background: #4169E1;}");
        else
            Maininterface_btn[i]->setStyleSheet("QPushButton{background: #08D9D6;}");
    }
}
