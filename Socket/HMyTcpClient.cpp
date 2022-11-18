#include "HMyTcpClient.h"
#include "HMyTcpWorker.h"

HMyTcpClient::HMyTcpClient(int Handle, QObject *parent) : QObject(parent),
                                                          m_nHandle(Handle),
                                                          m_bConnectedSuccess(false),
                                                          m_thread(NULL),
                                                          m_nHearTimeoutCount(0),
                                                          m_bAutoReConnectServer(true)
{
    m_heartTimer = new QTimer;
    connect(m_heartTimer, SIGNAL(timeout()), this, SLOT(sltHeartTimer()));
}

bool HMyTcpClient::isConnected()
{
    return m_bConnectedSuccess;
}

bool HMyTcpClient::connetServer(QString hostName, int port, int timeout, bool bAutoReconnect)
{
    if (hostName.isEmpty() || port <= 0)
    {
        return false;
    }

    m_strHostName = hostName;
    m_port = port;
    m_timeout = timeout;

    m_bAutoReConnectServer = bAutoReconnect;
    HMyTcpWorker *worker = new HMyTcpWorker(m_nHandle, cui);

    m_thread = new QThread(worker);
    worker->moveToThread(m_thread);

    connect(worker, SIGNAL(sigDisconnect()), this, SLOT(sltServerDisconnect()));
    connect(worker, SIGNAL(sigRevDataFromServer(QString)), this, SLOT(sltRevDataFromServer(QString)));
    connect(worker, SIGNAL(sigResult(int)), this, SLOT(sltServerResult(int)), Qt::QueuedConnection);

    connect(this, SIGNAL(sigConnectServer(QString, int, int)), worker, SLOT(sltConnectServer(QString, int, int)), Qt::QueuedConnection);
    connect(this, SIGNAL(sigSendData(QString, int)), worker, SLOT(sltSendData(QString, int)), Qt::QueuedConnection);
    connect(this, SIGNAL(sigDisConnectTcp()), worker, SLOT(sltDisConnect()), Qt::QueuedConnection);

    m_thread->start();

    emit sigConnectServer(hostName, port, timeout);

    return true;
}

void HMyTcpClient::disConnectServer()
{
    if (m_bConnectedSuccess)
    {
        emit sigDisConnectTcp();
        m_bAutoReConnectServer = false;
    }
}

void HMyTcpClient::sltServerResult(int index)
{
    switch (index)
    {
    case -1:
        m_bConnectedSuccess = false;
        qDebug() << QString::fromLocal8Bit("连接服务器失败") << m_nHandle;
        break;
    case 0:
    {
        m_bConnectedSuccess = true;
        m_nHearTimeoutCount = 0;
        qDebug() << QString::fromLocal8Bit("连接服务器成功") << m_nHandle;
        m_heartTimer->start(1000);
    }
    break;
    default:
        break;
    }
}

void HMyTcpClient::sltServerDisconnect()
{
    if (m_bConnectedSuccess)
    {
        m_bConnectedSuccess = false;
        if (!m_bAutoReConnectServer) //如果自动重连机制没有开启，就把线程掐掉
        {
            if (m_heartTimer->isActive())
            {
                m_heartTimer->stop();
            }

            if (m_thread->isRunning())
            {
                m_thread->quit();
                m_thread->wait();
                m_thread = NULL;
            }
        }
    }
}

void HMyTcpClient::sltHeartTimer()
{
    m_nHearTimeoutCount++;
    if (m_nHearTimeoutCount > HTCPCLIENT_HEART_TIMEOUT_COUNT)
    {
        if (!m_bConnectedSuccess && m_bAutoReConnectServer)
        {

            emit sigConnectServer(m_strHostName, m_port, m_timeout);
        }

        m_nHearTimeoutCount = 0;
    }

    if (m_bConnectedSuccess)
    {
        // QString strHeart;
        // strHeart = QString("<HEAD><MCODE>%1</MCODE><DATA>%2</DATA></HEAD>").arg("0X01");
        // qDebug() << QString::fromLocal8Bit("发送心跳包") << strHeart << m_nHandle;
        // emit sigSendData(strHeart, m_nHandle);
        /***************显示时间***********************/
        /*         QFont font("Microsoft YaHei", 20, 50);
                QDateTime time = QDateTime::currentDateTime();
                QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
                cui->time->setFont(font);
                cui->time->setText(str); */
    }
}

void HMyTcpClient::sltRevDataFromServer(QString data)
{
    m_nHearTimeoutCount = 0;
}
