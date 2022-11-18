#include "Application.h"
#include <QDebug>

#if defined(Q_OS_LINUX)
#include <unistd.h>
#include <time.h>
#endif

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv, QApplication::GuiServer), _screen(NULL),
      _isScreenSaver(false)
{
    //打开背光
    Backlight(true);

    //设置定时器
    _timer = new QTimer();
    _timer->setInterval(3 * 1000);
    _timer->start();
    QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(OperationTimeOut()));
}

void Application::Backlight(bool status)
{
    if (status)
        system("echo \"4882\" > /sys/class/backlight/intel_backlight/brightness ");
    else
        system("echo \"0\" > /sys/class/backlight/intel_backlight/brightness ");
}

void Application::setInterval(int sec)
{
    _timer->stop();
    _timer->setInterval(sec * 1000);
    _timer->start();
}

void Application::OperationTimeOut()
{
    EnterScreenSaver();
}

//进入屏保
void Application::EnterScreenSaver()
{
    if (_mutex.tryLock())
    {
        if (!_isScreenSaver)
        {
            Backlight(false);

            if (_screen == NULL)
            { //实例化一个QWidget，可做屏保 和 接收唤醒时触摸事件
                _screen = new QWidget;
                _screen->showFullScreen();
            }
            _isScreenSaver = true;
            qDebug() << __FUNCTION__;
        }
        _mutex.unlock();
    }
}

//退出屏保
void Application::ExitScreenSaver()
{
    _mutex.lock();
    if (_isScreenSaver)
    {

        if (_screen != NULL)
        {
#if defined(Q_OS_LINUX)
            usleep(1000 * 300);
#endif
            _screen->close();
            _screen = NULL;
        }
        _isScreenSaver = false;
        Backlight(true);
        qDebug() << __FUNCTION__;
    }
    _mutex.unlock();
}

bool Application::notify(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::MouseMove)
    {
        _timer->stop();
        ExitScreenSaver();
    }
    else
    {
        if (!_timer->isActive())
            _timer->start();
    }
    return QApplication::notify(obj, e);
}
