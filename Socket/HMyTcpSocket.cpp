#include "HMyTcpSocket.h"

HMyTcpSocket::HMyTcpSocket(quintptr socketDescriptor, QObject *parent) : QTcpSocket(parent),
                                                                         socketID(socketDescriptor)
{
    this->setSocketDescriptor(socketDescriptor);
}

HMyTcpSocket::~HMyTcpSocket()
{
    this->disconnectFromHost();
    this->deleteLater();
}

void HMyTcpSocket::sendData(QString data, const int id)
{
    //如果是服务器判断好，直接调用write会出现跨线程的现象，所以服务器用广播，每个连接判断是否是自己要发送的信息。
    if (id == socketID) //判断是否是此socket的信息
    {
        QByteArray byteData = data.toUtf8();
        QString strfData = QString("%1").arg(byteData.size(), 8, 10, QChar('0')) + data;
        QByteArray byte = strfData.toUtf8();
        write(byte, byte.size());
        if (!waitForBytesWritten())
        {
            qDebug() << "write error";
        }
    }
}

void HMyTcpSocket::disConnectTcp(int id)
{
    if (id == socketID)
    {
        this->disconnectFromHost();
    }
    else if (id == -1)
    {
        this->disconnectFromHost();
        this->deleteLater();
    }
}
