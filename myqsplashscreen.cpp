#include "myqsplashscreen.h"
#include <QDateTime>
#include <QDebug>
//myQSplashScreen::myQSplashScreen(QObject *parent) :
//    QSplashScreen(parent)
//{
//}
void myQSplashScreen::updateSplashMessage()
{
    //qDebug()<<"update slash----------------------------------";
    this->showMessage(QObject::tr("系统初始化中......\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")),Qt::AlignCenter,Qt::black);
}
