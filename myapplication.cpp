#include "myapplication.h"
#include <QDebug>
#include <QtGui/QApplication>

MyApplication::MyApplication(int argc, char *argv[])
    :QApplication(argc, argv)
{

}


bool MyApplication::eventFilter(QObject *target, QEvent *event)
{
//    if( target == w || target == v)
//    {
        if(event->type() == QEvent::MouseButtonPress
                || event->type() == QEvent::MouseMove
                || event->type() == QEvent::KeyPress )
        {
            emit restartMainTimerSignal();
        }
//        qDebug() <<"有事件发生";
//    }
    return QApplication::eventFilter(target,event);
}
