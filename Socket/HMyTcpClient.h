#ifndef HMYTCPCLIENT_H
#define HMYTCPCLIENT_H

#include <QObject>
#include <QThread>
#include "ui_mainwindow.h"
#define HTCPCLIENT_HEART_TIMEOUT_COUNT 3

class HMyTcpClient : public QObject
{
     Q_OBJECT
public:
     explicit HMyTcpClient(int Handle, QObject *parent = 0);

     //连接服务器，参数包括是否断线重连
     bool connetServer(QString hostName, int port, int timeout = 3000, bool bAutoReconnect = true);
     //断开与服务器连接
     void disConnectServer();
     //连接是否成功
     bool isConnected();
     Ui::MainWindow *cui;
     int m_nHandle;

private:
     bool m_bConnectedSuccess;
     QThread *m_thread;
     QTimer *m_heartTimer;
     int m_nHearTimeoutCount;

     QString m_strHostName;
     int m_port;
     int m_timeout;

     bool m_bAutoReConnectServer; //是否开启断线重连
signals:
     void sigSendData(QString strData, int handle);                  //向服务端发送数据
     void sigDisConnectTcp();                                        //断开与服务端的连接
     void sigConnectServer(QString hostName, int port, int timeout); //连接服务器

private slots:
     void sltHeartTimer();
     void sltServerDisconnect(); //与服务端断开连接
     void sltRevDataFromServer(QString data);

public slots:
     void sltServerResult(int index);
};

#endif // HMYTCPCLIENT_H
