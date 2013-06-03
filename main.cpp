#include <QtGui/QApplication>
#include <QtGui/QSplashScreen>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtCore/QTimer>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtDebug>

#include "mainwidget.h"
#include "database.h"
#include "operationhis.h"
#include "myqsplashscreen.h"
#include "myapplication.h"

//#include "PinyinFrame.h"
//#include "PinyinImpl.h"


int main(int argc, char *argv[])
{
    //    QApplication a(argc, argv);

    MyApplication a(argc, argv);
//    a.installEventFilter(&a);

    /* for pinyin-input ,the next 3 lines*/
//    const QApplication::Type type = a.type();
//    if (QApplication::GuiServer == type)
//    {
//        QPinyinImpl::installInputMethod();
//    }


    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));

#if 1
    QFont font  = a.font();
    font.setFamily("unifont");
    font.setPixelSize(13);//baseline(=1)+Asent(=13) = 14
    a.setFont(font);
#endif
    Database database;

    myQSplashScreen *splash = new myQSplashScreen;
    QTimer *databaseTimer = new QTimer(splash);
    QTimer *updateTimer1s = new QTimer(splash);
    updateTimer1s->setInterval(1000);
    QObject::connect(databaseTimer,SIGNAL(timeout()), splash,SLOT(clearMessage()));
    QObject::connect(updateTimer1s,SIGNAL(timeout()), splash,SLOT(updateSplashMessage()));
    updateTimer1s->start();
    splash->setFont(QFont("Times", 18, QFont::Bold));
    splash->setPixmap(QPixmap(":/image/start.png"));
    databaseTimer->setInterval(3000);
    databaseTimer->setSingleShot(true);

    splash->show();
    databaseTimer->start();
    //splash->showMessage(QObject::tr("数据库初始化..."),Qt::AlignCenter, Qt::white);//We are initializing the database...
    //splash->showMessage(QObject::tr("数据库初始化中...\n\n\n\n\n\n\n\n\n\n\n\n\n\n%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")),Qt::AlignCenter, Qt::white);


    if (!database.createConnection(QString("QSQLITE"), QString("database.db")))
    {
        return 1;
    }
    else
    {
        database.initUnitSet();
        database.initBusSet();
        database.initLinkSet();
        database.initFireHis();
        database.initFaultHis();
        database.initFeedbackHis();
        database.initIobrInfo();
        database.initPasswordInfo();
        database.initHostInfo();
        database.initOperationHis();
        database.initShieldHis();
    }

    OperationHis::Update("系统启动");
    MainWidget mainwin;    
    QTimer *mainwinTimer = new QTimer(splash);
    mainwinTimer->setInterval(3000);
    mainwinTimer->setSingleShot(true);    
    QObject::connect(mainwinTimer,SIGNAL(timeout()), &mainwin,SLOT(show()));
    QObject::connect(mainwinTimer,SIGNAL(timeout()), splash,SLOT(deleteLater()));
    QObject::connect(mainwinTimer,SIGNAL(timeout()), updateTimer1s,SLOT(deleteLater()));
    QObject::connect(&a,SIGNAL(restartMainTimerSignal()),&mainwin,SLOT(restartChangeWidgettimer30sSlot()));
    mainwinTimer->start();

    splash->showMessage(QObject::tr("系统初始化中...\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")),Qt::AlignCenter, Qt::black);

    //QObject::connect(&mainwin,SIGNAL(resetsysSignal()),qApp,SLOT(quit()));

    /* for pinyin-input the next 4 lines*/
//    int nret = a.exec();
//    if (QApplication::GuiServer == type)
//        QPinyinImpl::releaseInputMethod();
//    return nret;

    a.exec();

}
