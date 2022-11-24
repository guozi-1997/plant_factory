#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QTimer>
#include <QWidget>
#include <QMutex>

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);
    bool notify(QObject *, QEvent *);

    void Backlight(bool status);
    void setInterval(int sec);

public slots:
    //操作超时
    void OperationTimeOut();

    //进入屏保
    void EnterScreenSaver();
    //退出屏保
    void ExitScreenSaver();

private:
    QTimer *_timer;
    bool _isScreenSaver;
    QWidget *_screen;
    QMutex _mutex;
};

#endif // APPLICATION_H