#include <QtGui/QStatusBar>
#include <QtGui/QLabel>
#include <QtCore/QDateTime>
#include <QtGui/QKeyEvent>
#include <QtCore/QObject>
#include <QtGui/QAction>
#include <QtGui/QTableWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QTableWidgetItem>
#include <QtGui/QLineEdit>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtCore/QVariant>
#include <QDebug>
/* for reset the system */
#include <QtGui/QApplication>
#include <QtGui/QSplashScreen>
#include <QTimer>
#include <QWSServer>
#include <QTextCodec>//add by qingwu
#include <QTabWidget>//add by linwotian
#include <QPainter>
#include <QTime>


#include "boardregdialog.h"
#include "pollingthread.h"
#include "boardregthread.h"
#include "menuwindow.h"
#include "viewmenuwindow.h"
#include "mainwidget.h"
#include "type.h"

#include "ch452.h"
#include "keydetect.h"
#include "pic16f676.h"
#include "passworddialog.h"
#include "serial.h"
#include "eventdialog.h"
#include "operationhis.h"
#include "handwidget.h"
#include "shieldwidget.h"
#include "myqsplashscreen.h"


#define HIS_TABLE_ROWS 10000
#define LISTWIDGET_COLUMN 1
#define  LISTWIDGET_ROW  999
#define  LISTWIDGET_ROW_OF_FIRE  7
#define LABEL_WIDTH  50
#define LABEL_HEIGHT  22
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

//#define QWS
bool operator==( const nodeID &a, const nodeID &b )
{
    return (   ( a.gethostnum() == b.gethostnum() )
            && ( a.getiobrnum() == b.getiobrnum() )
            && ( a.getloopnum() == b.getloopnum() )
            && ( a.getnodenum() == b.getnodenum() ) );
}

uint qHash( const nodeID &key )
{
    return (  qHash( key.gethostnum() << 24 )
            | qHash( key.getiobrnum() << 16 )
            | qHash( key.getloopnum() << 8 )
            | qHash( key.getnodenum()      )  );
}

MainWidget::MainWidget(QWidget *parent) :
    QDialog(parent)
{
    //qDebug()<<"MainWidget_______currentThreadId_____:"<<QThread::currentThreadId();
    w = new MenuWindow(this);
    v = new ViewMenuWindow(this);
    eventdialog = new eventDialog(this);

    oneSecondTimer = new QTimer(this);
    oneSecondTimer->setInterval(1000);//1s

    ifire = 0;
    ilink = 0;
    timer3s = new QTimer(this);
    timer3s->setInterval(3000);

    timer4s = new QTimer(this);
    timer4s->setInterval(4000);
    timer4s->setSingleShot(true);

    timer10s = new QTimer(this);
    timer10s->setInterval(10000);
    timer10s->setSingleShot(true);

    //use changeWidgettimer30s to test if there is no operation occur ,then close the widget except the mainwidge or eventdialog;
    changeWidgettimer30s = new QTimer(this);
    changeWidgettimer30s->setInterval(30000);
   // changeWidgettimer30s->setInterval(15000);
    changeWidgettimer30s->setSingleShot(true);


    menutype = 0;
    createThread();
    initMaxId();
    createItems();
    createActions();

    //by linwotian,2013.5.27
    connect(eventdialog,SIGNAL(hideWindowSignal()),this,SLOT(hideWindow()));
    connect(eventdialog,SIGNAL(secondhideWindowSignal()),this,SLOT(secondhideWindow()));


    //createStatusBar();//by qingwu:
    //getHashBusFromBusset();//hashBusOut and hashBusIn initialize ,get data from busset table in the database
    keyThread = new KeyDetect(this);    
    keyThread->soundLedResetSlot();
    keyThread->start();
    //    keyThread->checkPowerSlot();
    //    qDebug()<<"check power slot.at start.";

    initSystem();
    timerEvent(0);
    startTimer(1000);
    setWindowTitle (tr("JB-TGZ2L-HM9000 火灾报警控制器（联动型）"));

    setFixedSize(800,600);
    setWindowFlags(windowFlags() &~ (Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint));
    setWindowFlags(Qt::FramelessWindowHint);//hide the window title-bar

    /*add by qingwu:2013.1.14 */
    connect(timer10s,SIGNAL(timeout()),this,SLOT(updateFireWidgetSlot()));
    connect(timer10s,SIGNAL(timeout()),this,SLOT(timer3sStart()));

    changeWidgettimer30s->start();
    connect(changeWidgettimer30s,SIGNAL(timeout()),this,SLOT(ChangeWidgettimer30sTimeoutSlot()));




    connect(timer3s,SIGNAL(timeout()),this,SLOT(updateFireWidgetSlot()));
    connect(timer3s,SIGNAL(timeout()),this,SLOT(updateLinkWidgetSlot()));
    connect(fireTableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(currentFireChangedSlot()));

    connect(oneSecondTimer,SIGNAL(timeout()),this,SLOT(dealDelayHash()));
    connect(this,SIGNAL(updateOneEventSignal(QString)),eventdialog,SLOT(updateOneEventSlot(QString)));
    connect(eventdialog,SIGNAL(rejected()),this,SLOT(eventDialogRejectedSlot()));
    connect(w,SIGNAL(rejected()),this,SLOT(eventDialogRejectedSlot()));
    connect(v,SIGNAL(rejected()),this,SLOT(eventDialogRejectedSlot()));

    //for oneEventShowDialog
    connect(timer4s,SIGNAL(timeout()),eventdialog,SLOT(close()));
    connect(this,SIGNAL(oneEventShowSignal(QString,QList<QString>&,char)),eventdialog,SLOT(oneEventShowSLot(QString,QList<QString>&,char)));
    //connect(this,SIGNAL(clearEventDialogSignal()),eventdialog,SLOT(clearEventDialogSlot()));

    /*按下复位键后，将菜单窗口的子窗口close掉，mainwidget和menuwindow窗口不会close*/
    /*按下复位键后，menuwindow窗口hide，mainwidget窗口show*/
    /*按下复位键后，将菜单窗口的子窗口close掉，mainwidget和menuwindow窗口不会close*/
    /*按下复位键后，menuwindow窗口hide，mainwidget窗口show*/
    /*按下复位键后，将联动过滤列表清掉*/
    connect(this, SIGNAL(sysResetKeyPressed()), w, SIGNAL(closedialog()));
    connect(this, SIGNAL(sysResetKeyPressed()), w, SLOT(hide()));
    connect(this, SIGNAL(sysResetKeyPressed()), this, SLOT(show()));
    connect(this, SIGNAL(sysResetKeyPressed()), v, SIGNAL(closedialog()));
    connect(this, SIGNAL(sysResetKeyPressed()), v, SLOT(hide()));
    connect(this, SIGNAL(sysResetKeyPressed()), w, SLOT(nowLinkClear()));

    /*按下复位键后，重新开始巡检和登记*/
    connect(boardDialog,SIGNAL(okPushButtonClicked()),this,SLOT(okPushButtonClickedSlot()));//almost like reset the system
    connect(boardDialog,SIGNAL(cancelPushButtonClicked()),this,SLOT(cancelPushButtonClickedSlot()));
    connect(boardDialog,SIGNAL(regAllOverSignal()),this,SLOT(regAllOverSlot()));//almost like reset the system
    //connect(w,SIGNAL(regButtonClicked()),boardDialog,SLOT(exec()));

    connect(boardDialog,SIGNAL(singleBoardNumSignal(int)),this,SLOT(singleBoardNumSlot(int )));
    connect(w,SIGNAL(regButtonClicked()),boardDialog,SLOT(refreshDialog()));

    connect(boardThread,SIGNAL(regStart()),boardDialog,SLOT(regStartMessage()));
    connect(boardThread,SIGNAL(regFault()),boardDialog,SLOT(regFaultMessage()));
    connect(boardThread,SIGNAL(currentRegBoardSgl(int)),boardDialog,SLOT(currentRegBoardSlot(int)));
    connect(boardThread,SIGNAL(regAllOver(int)),boardDialog,SLOT(regOverSlot(int )));
    connect(boardThread,SIGNAL(singleRegOver(int)),boardDialog,SLOT(singleRegOverSlot(int )));

    /*火警...等来了之后，将菜单窗口子窗口关闭，并且隐藏菜单窗口*/
    connect(this,SIGNAL(closeWVSignal()),w,SIGNAL(closedialog()));
    connect(this,SIGNAL(closeWVSignal()),w,SLOT(hide()));
    /*v*/
    connect(this,SIGNAL(closeWVSignal()),v,SIGNAL(closedialog()));
    connect(this,SIGNAL(closeWVSignal()),v,SLOT(hide()));

    /*add by qingwu 2013.4.8 for 声光警报器，主、备电故障,巡检开始之后再处理*/
    connect(keyThread,SIGNAL(localFaultSignal(int,int,int,int,unsigned char,unsigned char,QDateTime)),this,SLOT(eventComeSlot(int,int, int, int, unsigned char, unsigned char, QDateTime)));
    connect(keyThread,SIGNAL(sgFlagChangeSignal(char)),this,SLOT(sgFlagChangeSlot(char)));


    connect(polling,SIGNAL(eventComes(int,int,int,int,unsigned char,unsigned char,QDateTime)),this,SLOT(eventComeSlot(int,int, int, int, unsigned char, unsigned char, QDateTime)));
    connect(polling,SIGNAL(KeyInfComes(int,int,int,unsigned char,QDateTime)),this,SLOT(KeyInfSlot(int, int, int, unsigned char, QDateTime)));
    //connect(this,SIGNAL(busCommandSignal(nodeID)),polling,SLOT(sendBusLedSlot(nodeID)));
    //connect(this,SIGNAL(sendLinkSignal(nodeID)),polling,SLOT(linkEnqueueSlot(nodeID)));
    connect(this,SIGNAL(nodeEnqueueSignal(char,nodeID)),polling,SLOT(nodeEnqueueSlot(char,nodeID)));
    connect(handWidget,SIGNAL(handNode(char ,int)),this,SLOT(dealHandNodeSlot(char,int)));

    connect(w,SIGNAL(linkNodeSignal(int )),this,SLOT(dealLinkNodeSlot(int )));
    connect(this,SIGNAL(eventTrueComes(int,int,int,unsigned char,unsigned char)),w,SIGNAL(transferEvent(int, int, int, unsigned char, unsigned char)));

    //    connect(this,SIGNAL(soundLedResetSignal()),keyThread,SLOT(soundLedResetSlot()));//
    //    connect(this,SIGNAL(LedSetSignal(int)),keyThread,SLOT(ledSetSlot(int)));
    //    connect(this,SIGNAL(LedPutSignal(int)),keyThread,SLOT(ledPutSlot(int)));

    connect(this,SIGNAL(linkSoundOffSignal()),keyThread,SLOT(linkSoundOffSlot()));
    connect(this,SIGNAL(ledStateChangeSignal(char,char)),keyThread,SLOT(ledStateChangeSlot(char,char)));

    connect(keyThread,SIGNAL(keyPressedSignal(int)),this,SLOT(keyPressedSlot(int)));
    connect(this,SIGNAL(soundErasureKeyPressed()),keyThread,SLOT(soundErasureSlot()));


    //add by linwotian
    connect(keyThread,SIGNAL(setLockkeyFlagTrue()),this,SLOT(setLockkeyFlagTrueSlot()));
    connect(keyThread,SIGNAL(setLockkeyFlagFalse()),this,SLOT(setLockkeyFlagFalseSlot()));
    connect(this,SIGNAL(updateLockkeyTime(int)),keyThread,SLOT(resetLockKeyTimer(int)));


    connect(eventTabWidget,SIGNAL(currentChanged(int)),this,SLOT(updateTabWidget(int)));

    //sleep(3);//3s delay for sys init...I dont know..
    QTime t;
    t.start();
    while(t.elapsed()<3000)//3s
        QCoreApplication::processEvents();

    boardDialog->statrTimer15s();
    boardDialog->exec();

    this->initSystemLight();
}

MainWidget::~MainWidget()
{
    delete(w);
    ::close(fd_printer);
    deleteLater();
}

void MainWidget::initSystem()
{    
    faultAllNumber = 0;
    linkAllNumber = 0;
    fireAllNumber = 0;

    linkFeedBackFlag = 0;
    oneEventShowFlag = 0;
    lockkeyFlag = false;
    selfcheckFlag = false;
    sound_alarm_fault_Flag = false;
    //sound_alarm_shield_Flag = true;//this flag's init is in initSystemLight().get value from database.
    sound_alarm_startOrStop_Flag = false;
    enterPasswordFlag = false;


    changeWindowFlag = false;
    keyThread->initFlags();

    int can_fd = ::open("/dev/can", O_RDWR);
    if(can_fd < 0)
    {
        qDebug()<<"in initSystem: can't open can_dev, can_fd = "<< can_fd <<endl;
        return;
    }
    else
        qDebug() <<"in initSystem: open  can_dev successfully! can_fd = "<< can_fd <<endl;
    //发送三次复位 三次自检
    for(int i = 0; i < 3; i++)
        sendResetFrame( can_fd);

    qDebug()<<"25s delay between reset and selfcheck...";
    //sleep(25);//25s delay
    QTime t;
    t.start();
    while(t.elapsed()<25000)//25s
        QCoreApplication::processEvents();


    for(int i = 0; i < 3; i++)
        sendSelfCheckFrame(can_fd);

    ::close(can_fd);

    printerInit();
    initMaxId();

}

void MainWidget::printerInit()
{
    //add by qingwu:2012.11.05
    char dev_name[] = "/dev/s3c_serial1";
    char *dev = dev_name;
    fd_printer = OpenDev(dev);
    if (fd_printer > 0)
        qDebug()<<"s3c_serial1 open for printer,fd_printer="<<fd_printer;//printf(" s3c_serial1 open for printer \n");
    else
    {
        qDebug()<<"Can't Open Serial Port for printer,fd_printer="<<fd_printer;//printf("Can't Open Serial Port for printer !\n");
        return;
    }
    set_Opt( fd_printer , 9600 , 8 , 1 , 'n' );

    if( printerFlagOfWidget )
    {
        qDebug(" printerFlagOfWidget():%x",printerFlagOfWidget);
        QString str1,str2,str3;//
        if( sysResetFlag )
        {
            str1 = QString("");
            str2 = QString("%1 %2\n").arg(tr("复位时间:")).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            str3 = QString("******系统复位******\n");
            doPrint(str1,str2,str3);
        }
        else
        {
            str1 = QString("******Welcome******\n");
            str2 = QString("%1 %2\n").arg(tr("开机时间:")).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            str3 = QString("汉字标签\n事件类型 单元类型 栋/层/区\n月-日 时:分 主机-接口-回路-单元\n事件打印格式\n");
            doPrint(str1,str2,str3);
        }
        qDebug()<<"print system start or restart over.";
    }

}

void MainWidget::createThread()
{
    polling = new pollingThread(this);
    boardThread = new boardRegThread(this);
    boardDialog = new BoardRegDialog(w);

}

void MainWidget::keyPressedSlot(int key_value)
{
    this->restartChangeWidgettimer30sSlot();
    if(lockkeyFlag)
    {
        switch(key_value)
        {

        case KEY_SOUND_ERASURE:
            OperationHis::Update("系统消音");
            emit soundErasureKeyPressed();
            break;
#ifdef QWS
        case KEY_0:
            QWSServer::processKeyEvent(0x30,Qt::Key_0,0,TRUE,FALSE);
            break;
        case KEY_1:
            QWSServer::processKeyEvent(0x31,Qt::Key_1,0,TRUE,FALSE);
            break;
        case KEY_2:
            QWSServer::processKeyEvent(0x32,Qt::Key_2,0,TRUE,FALSE);
            break;
        case KEY_3:
            QWSServer::processKeyEvent(0x33,Qt::Key_3,0,TRUE,FALSE);
            break;
        case KEY_4:
            QWSServer::processKeyEvent(0x34,Qt::Key_4,0,TRUE,FALSE);
            break;
        case KEY_5:
            QWSServer::processKeyEvent(0x35,Qt::Key_5,0,TRUE,FALSE);
            break;
        case KEY_6:
            QWSServer::processKeyEvent(0x36,Qt::Key_6,0,TRUE,FALSE);
            break;
        case KEY_7:
            QWSServer::processKeyEvent(0x37,Qt::Key_7,0,TRUE,FALSE);
            break;
        case KEY_8:
            QWSServer::processKeyEvent(0x38,Qt::Key_8,0,TRUE,FALSE);
            break;
        case KEY_9:
            QWSServer::processKeyEvent(0x39,Qt::Key_9,0,TRUE,FALSE);
            break;
        case KEY_ENTER:
            QWSServer::processKeyEvent(0,Qt::Key_Enter,0,TRUE,FALSE);
            break;
        case KEY_ESCAPE:
            QWSServer::processKeyEvent(0,Qt::Key_Escape,0,TRUE,FALSE);
            break;
#endif
        default:
            if(!enterPasswordFlag)
            {
                if(menutype == 0)
                {
                    enterPasswordFlag = true;
                    PasswordDialog password(5, w);
                    password.exec();
                    if (password.matchFlag)
                    {
                        OperationHis::Update("按键解锁");
                        //restart the timer
                        keyThread->dealwithUnlockkeyEvent();
                        keyThread->lockKeyTimer->stop();
                        keyThread->lockKeyTimer->start();
                    }
                    enterPasswordFlag = false;
                }
                else
                {
                    enterPasswordFlag = true;
                    PasswordDialog password(5, v);
                    password.exec();
                    if (password.matchFlag)
                    {
                        OperationHis::Update("按键解锁");
                        //restart the timer
                        keyThread->dealwithUnlockkeyEvent();
                        keyThread->lockKeyTimer->stop();
                        keyThread->lockKeyTimer->start();
                    }
                    enterPasswordFlag = false;
                }
            }
        }
    }
    else
    {
        //if there is an key press and the system is unlockkey,restart the timer.
        keyThread->lockKeyTimer->stop();
        keyThread->lockKeyTimer->start();

        switch(key_value)
        {
        case KEY_RESET:
        {
            if(!enterPasswordFlag)
            {
                if(menutype == 0)
                {
                    enterPasswordFlag = true;
                    PasswordDialog password(1, w);
                    password.exec();
                    if (password.matchFlag)
                    {
                        OperationHis::Update("系统复位");
                        emit sysResetKeyPressed();
                        resetSystem();
                    }
                    enterPasswordFlag = false;
                }
                else
                {
                    enterPasswordFlag = true;
                    PasswordDialog password(1, v);
                    password.exec();
                    if (password.matchFlag)
                    {
                        OperationHis::Update("系统复位");
                        emit sysResetKeyPressed();
                        resetSystem();
                    }
                    enterPasswordFlag = false;
                }
            }
        }
            break;
        case KEY_SOUND_ERASURE:
            OperationHis::Update("系统消音");
            emit soundErasureKeyPressed();
            break;

        case KEY_SELFCHECK:
            if(!selfcheckFlag)
            {
                OperationHis::Update("系统自检");
                this->systemSelfcheck();
            }
            break;

        case KEY_CAAVALARM:
            if((!sound_alarm_shield_Flag) && (!sound_alarm_fault_Flag))
            {
                if(!sound_alarm_startOrStop_Flag)
                {
                    OperationHis::Update("手动启动声光报警器");
                    emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_START,LED_PUT_ON);
                    this->eventComeSlot(TI_LinkageStart, 0, 0, 0, 0x14, 0x0b, QDateTime::currentDateTime());
                    sound_alarm_startOrStop_Flag = true;
                }
                else
                {
                    OperationHis::Update("手动关闭声光报警器");
                    emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_START,LED_PUT_OFF);
                    this->eventComeSlot(TI_LinkageStop, 0, 0, 0, 0x14, 0x0b, QDateTime::currentDateTime());
                    sound_alarm_startOrStop_Flag = false;
                }
            }
            break;

#ifdef QWS


            //the key under has not define the function
            //#define KEY_SELFCHECK 0x60
            //#define KEY_HANDAUTO 0x43
            //#define KEY_HOST 0x4b
            //#define KEY_LOOP 0x53
            //#define KEY_CAAVALARM 0x5b //combined aural and visual alarm 澹板厜鎶ヨ鍣//#define KEY_STOP 0x63
            //#define KEY_IOBR 0x6b
            //#define KEY_UNIT 0x73
            //#define KEY_START 0x7b
            //#define KEY_CHANGE 0x54

        case KEY_PROGRAMME:
            QWSServer::processKeyEvent(0,Qt::Key_F2,0,TRUE,FALSE);
            break;

        case KEY_SHIELD:
            QWSServer::processKeyEvent(0,Qt::Key_F4,0,TRUE,FALSE);
            break;

            //    case KEY_SHIFT:
            //       QWSServer::processKeyEvent(0,Qt::Key_Shift,0,TRUE,FALSE);
            //        break;
            //    case KEY_CONTROL:
            //       QWSServer::processKeyEvent(0,Qt::Key_Control,0,TRUE,FALSE);
            //        break;
        case KEY_CHANGE:
            QWSServer::processKeyEvent(0,Qt::Key_CapsLock,0,TRUE,FALSE);
            break;
            //    case KEY_BACKTAB:
            //       QWSServer::processKeyEvent(0,Qt::Key_Backtab,0,TRUE,FALSE);
            //        break;
            //    case KEY_RETURN:
            //       QWSServer::processKeyEvent(0,Qt::Key_Return,0,TRUE,FALSE);
            //        break;
            //    case KEY_DELETE:
            //       QWSServer::processKeyEvent(0,Qt::Key_Delete,0,TRUE,FALSE);
            //        break;
        case KEY_REFER:
            QWSServer::processKeyEvent(0,Qt::Key_F3,0,TRUE,FALSE);
            break;


        case KEY_PAGEDOWN:
            QWSServer::processKeyEvent(0,Qt::Key_PageDown,0,TRUE,FALSE);
            break;
        case KEY_PAGEUP:
            QWSServer::processKeyEvent(0,Qt::Key_PageUp,0,TRUE,FALSE);
            break;

        case KEY_LEFT:
            QWSServer::processKeyEvent(0,Qt::Key_Left,0,TRUE,FALSE);
            break;
        case KEY_UP:
            QWSServer::processKeyEvent(0,Qt::Key_Up,0,TRUE,FALSE);
            break;
        case KEY_RIGHT:
            QWSServer::processKeyEvent(0,Qt::Key_Right,0,TRUE,FALSE);
            break;
        case KEY_DOWN:
            QWSServer::processKeyEvent(0,Qt::Key_Down,0,TRUE,FALSE);
            break;
        case KEY_ENTER:
            QWSServer::processKeyEvent(0,Qt::Key_Enter,0,TRUE,FALSE);
            break;
        case KEY_ESCAPE:
            QWSServer::processKeyEvent(0,Qt::Key_Escape,0,TRUE,FALSE);
            break;
            //    case KEY_NUMLOCK:
            //       QWSServer::processKeyEvent(0,Qt::Key_NumLock,0,TRUE,FALSE);
            //        break;

        case KEY_PLUS:
            QWSServer::processKeyEvent(0x2b,Qt::Key_Plus,0,TRUE,FALSE);
            break;
        case KEY_SLASH:
            QWSServer::processKeyEvent(0x2f,Qt::Key_Slash,0,TRUE,FALSE);
            break;
        case KEY_ASTERISK:
            QWSServer::processKeyEvent(0x2a,Qt::Key_Asterisk,0,TRUE,FALSE);
            break;

        case KEY_0:
            QWSServer::processKeyEvent(0x30,Qt::Key_0,0,TRUE,FALSE);
            break;
        case KEY_1:
            QWSServer::processKeyEvent(0x31,Qt::Key_1,0,TRUE,FALSE);
            break;
        case KEY_2:
            QWSServer::processKeyEvent(0x32,Qt::Key_2,0,TRUE,FALSE);
            break;
        case KEY_3:
            QWSServer::processKeyEvent(0x33,Qt::Key_3,0,TRUE,FALSE);
            break;
        case KEY_4:
            QWSServer::processKeyEvent(0x34,Qt::Key_4,0,TRUE,FALSE);
            break;
        case KEY_5:
            QWSServer::processKeyEvent(0x35,Qt::Key_5,0,TRUE,FALSE);
            break;
        case KEY_6:
            QWSServer::processKeyEvent(0x36,Qt::Key_6,0,TRUE,FALSE);
            break;
        case KEY_7:
            QWSServer::processKeyEvent(0x37,Qt::Key_7,0,TRUE,FALSE);
            break;
        case KEY_8:
            QWSServer::processKeyEvent(0x38,Qt::Key_8,0,TRUE,FALSE);
            break;
        case KEY_9:
            QWSServer::processKeyEvent(0x39,Qt::Key_9,0,TRUE,FALSE);
            break;

        case KEY_A:
            QWSServer::processKeyEvent(0x41,Qt::Key_A,0,TRUE,FALSE);
            break;
        case KEY_B:
            QWSServer::processKeyEvent(0x42,Qt::Key_B,0,TRUE,FALSE);
            break;
        case KEY_C:
            QWSServer::processKeyEvent(0x43,Qt::Key_C,0,TRUE,FALSE);
            break;
        case KEY_D:
            QWSServer::processKeyEvent(0x44,Qt::Key_D,0,TRUE,FALSE);
            break;
        case KEY_E:
            QWSServer::processKeyEvent(0x45,Qt::Key_E,0,TRUE,FALSE);
            break;
        case KEY_F:
            QWSServer::processKeyEvent(0x46,Qt::Key_F,0,TRUE,FALSE);
            break;
        case KEY_G:
            QWSServer::processKeyEvent(0x47,Qt::Key_G,0,TRUE,FALSE);
            break;
        case KEY_H:
            QWSServer::processKeyEvent(0x48,Qt::Key_H,0,TRUE,FALSE);
            break;
        case KEY_I:
            QWSServer::processKeyEvent(0x49,Qt::Key_I,0,TRUE,FALSE);
            break;
        case KEY_J:
            QWSServer::processKeyEvent(0x4a,Qt::Key_J,0,TRUE,FALSE);
            break;
        case KEY_K:
            QWSServer::processKeyEvent(0x4b,Qt::Key_K,0,TRUE,FALSE);
            break;
        case KEY_L:
            QWSServer::processKeyEvent(0x4c,Qt::Key_L,0,TRUE,FALSE);
            break;
        case KEY_M:
            QWSServer::processKeyEvent(0x4d,Qt::Key_M,0,TRUE,FALSE);
            break;
        case KEY_N:
            QWSServer::processKeyEvent(0x4e,Qt::Key_N,0,TRUE,FALSE);
            break;
        case KEY_O:
            QWSServer::processKeyEvent(0x4f,Qt::Key_O,0,TRUE,FALSE);
            break;
        case KEY_P:
            QWSServer::processKeyEvent(0x50,Qt::Key_P,0,TRUE,FALSE);
            break;
        case KEY_Q:
            QWSServer::processKeyEvent(0x51,Qt::Key_Q,0,TRUE,FALSE);
            break;
        case KEY_R:
            QWSServer::processKeyEvent(0x52,Qt::Key_R,0,TRUE,FALSE);
            break;
        case KEY_S:
            QWSServer::processKeyEvent(0x53,Qt::Key_S,0,TRUE,FALSE);
            break;
        case KEY_T:
            QWSServer::processKeyEvent(0x54,Qt::Key_T,0,TRUE,FALSE);
            break;
        case KEY_U:
            QWSServer::processKeyEvent(0x55,Qt::Key_U,0,TRUE,FALSE);
            break;
        case KEY_V:
            QWSServer::processKeyEvent(0x56,Qt::Key_V,0,TRUE,FALSE);
            break;
        case KEY_W:
            QWSServer::processKeyEvent(0x57,Qt::Key_W,0,TRUE,FALSE);
            break;
        case KEY_X:
            QWSServer::processKeyEvent(0x58,Qt::Key_X,0,TRUE,FALSE);
            break;
        case KEY_Y:
            QWSServer::processKeyEvent(0x59,Qt::Key_Y,0,TRUE,FALSE);
            break;
        case KEY_Z:
            QWSServer::processKeyEvent(0x5a,Qt::Key_Z,0,TRUE,FALSE);
            break;


#endif
        }
    }
}

void MainWidget::resetSystem()
{
    qDebug()<<"perpare for restarting the system...";

    sysResetFlag = 1;//it is the restart,not start.
    ::close(fd_printer);

    eventdialog->hide();
    this->show();//show main widget

    /*灭灯 消音*/
    //emit soundLedResetSignal();
    keyThread->soundLedResetSlot();
    /*清掉四个QTableWidget*/

    fireList.clear();
    linkList.clear();
    faultList.clear();
    superviseList.clear();
    delayList.clear();

    eventNumberChangedSlot();
    qDebug()<<"IN RESET SYSTEM: set NumLabel 000";

    firstFireTableWidget->clear();
    fireTableWidget->clear();
    linkTableWidget->clear();

    superviseTableWidget->clear();
    faultTableWidget->clear();
    delayTableWidget->clear();
    //shieldTableWidget->clear();
    //handTableWidget->clear();


    eventdialog->clearEventDialogSlot();//emit clearEventDialogSignal();
    eventTabWidget->setCurrentIndex(0);

    qDebug()<<"IN RESET SYSTEM: clear TableWidget of mainWidget";

    hashFire.clear();
    hashFault.clear();
    hashLink.clear();
    //hashDelay.clear();
    //hashBusOut.clear();
    //hashBusIn.clear();
    hashFireDelay.clear();

    hashLinkStartDelay.clear();
    hashLinkPulseDelay.clear();
    hashRestart60s.clear();//for 重报节点
    hashRestart29min.clear();
    hashLinkageAction.clear();
    //getHashBusFromBusset();

    qDebug()<<"IN RESET SYSTEM: clear 4 hash table and 4 eventList";

    backgroundLabelDispaly();
    this->initSystemLight();

    //restartSplash();//界面初始化
    QPixmap pixmap(":/image/start.png");
    myQSplashScreen splash;// = new myQSplashScreen;myQSplashScreen splash(pixmap);//QSplashScreen splash(pixmap);
    splash.setPixmap(pixmap);
    splash.setFont(QFont("Times", 18, QFont::Bold));
    splash.show();
    QTimer *updateTimer1s = new QTimer(&splash);
    updateTimer1s->setInterval(1000);
    QObject::connect(updateTimer1s,SIGNAL(timeout()), &splash,SLOT(updateSplashMessage()));
    QObject::connect(polling,SIGNAL(started()), updateTimer1s,SLOT(deleteLater()));
    QObject::connect(polling,SIGNAL(started()), &splash,SLOT(clearMessage()));
    QObject::connect(polling,SIGNAL(started()), &splash,SLOT(deleteLater()));

    splash.showMessage(QObject::tr("系统初始化中...\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")),Qt::AlignCenter, Qt::white);//splash.showMessage(QObject::tr("系统重新初始化中..."),Qt::AlignCenter, Qt::yellow);//We are restarting the application1...
    updateTimer1s->start();
    qApp->processEvents();
    qDebug()<<"image init started !";

    /*线程退出*/
    if(boardThread->isRunning())
    {
        qDebug()<<"IN RESET SYSTEM: boardThread TO-BE-STOPPED!";
        boardThread->setSysResetFlag();
        boardThread->stopBoardRegThread();
        boardThread->quit();
        qDebug()<<"IN RESET SYSTEM: boardThread quit";
    }
    if(polling->isRunning())
    {
        qDebug()<<"IN RESET SYSTEM: polling TO-BE-STOPPED!";
        polling->stopPolling();//timer in polling thread stopped!
        polling->quit();
        qDebug()<<"IN RESET SYSTEM: polling Thread quit";
    }
    boardThread->wait();
    polling->wait();
    //    delete(boardThread);
    //    qDebug()<<"IN RESET SYSTEM: boardThread deleted";
    delete(polling);
    qDebug()<<"IN RESET SYSTEM: pollingThread deleted";
    //    delete(boardDialog);
    //    qDebug()<<"IN RESET SYSTEM: boardDialog deleted";

    //    splash.showMessage(QObject::tr("系统重新初始化中..."),Qt::AlignCenter, Qt::red);
    //    qApp->processEvents();
    //    qDebug()<<"image init started 222222 !";

    /*重新创建线程*/
    polling = new pollingThread(this);
    //boardThread = new boardRegThread(this);
    qDebug()<<"IN RESET SYSTEM: creat new thread.";

    /*重新初始化系统*/
    initSystem();
    qDebug()<<"IN RESET SYSTEM: send reset and selfcheck frame";
    qDebug()<<"IN RESET SYSTEM: re-connect signals and slots";//update on 9.29


    connect(polling,SIGNAL(KeyInfComes(int,int,int,unsigned char,QDateTime)),this,SLOT(KeyInfSlot(int, int, int, unsigned char, QDateTime)));
    connect(polling,SIGNAL(eventComes(int,int,int,int,unsigned char,unsigned char,QDateTime)),this,SLOT(eventComeSlot(int,int, int, int, unsigned char, unsigned char, QDateTime)));
    //    connect(this,SIGNAL(busCommandSignal(nodeID)),polling,SLOT(sendBusLedSlot(nodeID)));
    //    connect(this,SIGNAL(sendLinkSignal(nodeID)),polling,SLOT(linkEnqueueSlot(nodeID)));
    connect(this,SIGNAL(nodeEnqueueSignal(char,nodeID)),polling,SLOT(nodeEnqueueSlot(char,nodeID)));

    //sleep(1);//1s delay for sys init...I dont know..
    //boardDialog->exec();//复位的时候，不显示重新登记

    polling->start();
    changeWindowFlag = true;
    oneSecondTimer->start();
    timer3s->start();
    this->show();
    keyThread->pollingStartSlot();
    qDebug()<<"in menuwidget::resetSys(): start polling....";

}
void MainWidget::sendResetFrame(int can_fd)
{
    /******  send Reset frame ************/
    int nwrite = 0;
    struct can_frame sendFrame;
    memset (&sendFrame, 0x0, sizeof(struct can_frame));
    sendFrame.priority = Priority2;
    sendFrame.ti = TI_Rst;
    sendFrame.src_addr = CONTROLLADDR;
    sendFrame.dest_addr = 0x00;
    sendFrame.can_dlc = 0x00;
    qDebug() <<"send Reset frame ...";
    nwrite = write(can_fd, &sendFrame, sizeof(struct can_frame));
}

void MainWidget::sendSelfCheckFrame(int can_fd)
{
    /******  send SelfCheck frame ************/
    struct can_frame sendFrame;
    int nwrite = 0;
    memset (&sendFrame, 0x0, sizeof(struct can_frame));
    sendFrame.priority = Priority2;
    sendFrame.ti = TI_SelfCheck;
    sendFrame.src_addr = CONTROLLADDR;
    sendFrame.dest_addr = 0x00;
    sendFrame.can_dlc = 0x00;
    qDebug() <<"send SelfCheck frame ...";
    nwrite = write(can_fd, &sendFrame, sizeof(struct can_frame));

}

void MainWidget::okPushButtonClickedSlot()
{
    /*re-register of board .stop the polling thread..*/

    if(polling->isRunning())
        polling->stopPolling();//timer in polling thread stopped!!!

    //    QPixmap pixmap(":/image/start.png");
    //    QSplashScreen splash(pixmap);
    //    splash.setFont(QFont("Times", 18, QFont::Bold));
    //    splash.show();

    //    QObject::connect(polling,SIGNAL(started()), &splash,SLOT(clearMessage()));
    //    QObject::connect(polling,SIGNAL(started()), &splash,SLOT(deleteLater()));

    qDebug()<<"in MainWidget:when click okButton of regDialog,polling stopped.start boardreg thread..";
    boardThread->start();
}

void MainWidget::cancelPushButtonClickedSlot()
{
    qDebug()<<"in MainWidget:cancelPushButtonClickedSlot().";
    boardDialog->close();
    qDebug()<<"in MainWidget:when close regDialog,polling start.";
    polling->start();
    qDebug()<<"in MainWidget:cancelPushButtonClickedSlot():polling start.";
    changeWindowFlag = true;
    oneSecondTimer->start();
    timer3s->start();
    this->show();
    keyThread->pollingStartSlot();
    qDebug()<<"in MainWidget:this->show()TTTTTTTTTTTTTTTTTTTTTTTTTT";

}

void MainWidget::regAllOverSlot()
{
    boardDialog->close();
    qDebug()<<"in menuwidget::regAllOverSlot():reset system.";

    //    emit sysResetKeyPressed();
    //    resetSystem();

    polling->start();
    qDebug()<<"in MainWidget:regAllOverSlot():polling start.";
    changeWindowFlag = true;
    oneSecondTimer->start();
    timer3s->start();
    this->show();
    keyThread->pollingStartSlot();
}

int MainWidget::isReReport(int ti,nodeID tempnode,unsigned char shijian)//QMultiHash<nodeID, int> hashTable
{
    QList<int> nodeList;
    int nodecount = 0;
    switch( ti )
    {
    case TI_BreakDown:
    case TI_BreakDownRecover:
        nodeList = hashFault.values(tempnode);
        nodecount = nodeList.count();
        while( nodecount-- > 0)
        {
            if((nodeList.at(nodecount) >> 16 & 0xffff ) == shijian )
            {
                //qDebug("in isReReport():%x",nodeList.at(nodecount) >> 16 & 0xffff);
                return (nodeList.at(nodecount) & 0xffff);//row number of the node shijian
            }
        }
        return -1;
        break;
    case TI_FireAlarm:
        nodeList = hashFire.values(tempnode);
        nodecount = nodeList.count();
        while( nodecount-- > 0)
        {
            if((nodeList.at(nodecount) >> 16 & 0xffff ) == shijian )
                return (nodeList.at(nodecount) & 0xffff);//row number of the node shijian
        }
        /*check the hashFireDelay */
        nodeList = hashFireDelay.values(tempnode);
        nodecount = nodeList.count();
        while( nodecount-- > 0)
        {
            if((nodeList.at(nodecount) >> 16 & 0xff ) == shijian )
            {
                //qDebug()<<"isReReport() hashFireDelay ,row is"<<((nodeList.at(nodecount) >> 8) & 0xff);
                return ((nodeList.at(nodecount) >> 8) & 0xff);//row number of the node shijian
            }
        }
        break;
    case TI_FeedBack:
    case TI_LinkageStart:
        nodeList = hashLink.values(tempnode);
        nodecount = nodeList.count();
        while( nodecount-- > 0)
        {
            if((nodeList.at(nodecount) >> 24 & 0xffff ) == shijian )
                return (nodeList.at(nodecount) & 0xffff);//row number of the node shijian
        }
        return -1;
        break;

    default:
        //return -1;
        break;
    }

}



unsigned char MainWidget::getFaultCode(unsigned char nodestate)
{
    //for fault_recover,convert the fault_recover shijian to fault shijian
    switch(nodestate)
    {
    case 0x59:  return 0x58;  break;
    case 0x57:  return 0x56;  break;
    case 0xdc:  return 0xdd;  break;
    case 0xd5:  return 0xd1;  break;//0D1H:主电停止; 0D5H:主电启动
    case 0xd6:  return 0xd2;  break;//0D2H:备电停止; 0D6H:备电启动
    case 0xd7:  return 0xd3;  break;
    case 0xd8:  return 0xd4;  break;
    case 0x01:  return 0x0a;  break;//正常 对应 通信故障
    case 0x35:  return 0x55;  break;
    case 0x36:  return 0xa5;  break;
    case 0x3a:  return 0xaa;  break;
    case 0xE2:  return 0xE1;  break;//0E1H:声光故障;  0E2H:声光恢复

    default:
        return (nodestate & 0x0f);
        break;
    }
}

void MainWidget::eventComeSlot(int ti, int iobrnum, int loopnum, int nodenum, unsigned char nodetype, unsigned char nodestate, QDateTime datetime)
{
    switch(ti)
    {
    case TI_FireAlarm :
        fireComeSlot(iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
        break;

    case TI_BreakDown :
        faultComeSlot(iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
        break;

    case  TI_BreakDownRecover:
        faultRecoverSlot(iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
        break;

    case TI_FeedBack :
        feedBackSlot(iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
        break;

    case  TI_FeedBackRecover:
        feedBackRecoverSlot(iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
        break;

    case  TI_LinkageStart :
        linkageStartSlot(iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
        break;

    case  TI_LinkageStop:
        linkageStopSlot(iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
        break;
    case TI_ActionFailed:
        actionFailedSlot(iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
        break;

    default:
        qDebug()<<"unknow frame type";
        break;
    }
    backgroundLabelDispaly();
    return;
}

void MainWidget::autoLinkStartSlot(int iobrnum, int loopnum, int nodenum)
{
    /* 对发送过联动启动命令的模块，人为造一个联动启动的事件。以免模块不报启动而直接报了反馈。*/
    int hostnum = 1;
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from unitset WHERE unitid == %1")
               .arg((hostnum << 24) | (iobrnum << 16) | (loopnum << 8) | nodenum));
    if (query.next())
    {
        int nodetype = query.value(1).toInt()>>8;
        linkageStartSlot(iobrnum, loopnum, nodenum, nodetype, 0x0b, QDateTime::currentDateTime());
    }
    else
    {
        qDebug()<<"in autoLinkStartSlot():"<<iobrnum<<loopnum<<nodenum<<"is not in the database.";
    }
    QSqlDatabase::database().commit();

}

void MainWidget::dealDelayHash()
{
    /* 对延时哈希表中的延时每次减1，并遍历，如果有减至0的延时，就做相应的处理。
    1）报警延时减至0，将报警信息添加到火警列表，相当于新到了一条火警，并在延时列表中将这一条删除。
     A:报警延时: hashFireDelay.insert(tempnode, nodetype<<24 | nodestate<<16 | ((delayList.count()-1)<<8) | startDelay);
    2）联动的启动延时为0，发送联动启动命令，并在延时列表中将这一条删除.
        联动的脉冲延时为0，发送联动停止命令。并在延时列表中将这一条删除.相当于触发发送联动的命令。
     B:联动启动延时: hashLinkStartDelay.insert(tempnode, pulseDelay<<16 | ((delayList.count()-1)<<8) | startDelay);
     C:联动脉冲延时: hashLinkPulseDelay.insert(tempnode,((delayList.count()-1)<<8) | pulseDelay);
 */
    /*D:重报设备哈希表: 60s内没有收到后续火警，将这一条火警放到29min延时，并将这一条在60s延时中删除*/
    QHash<nodeID, int>::iterator iterRestateP = hashRestart60s.begin();
    while (iterRestateP != hashRestart60s.end())
    {
        if( (iterRestateP.value() & 0xff) == 0)
        {
            nodeID tempnode = iterRestateP.key();//nodeID(hostnum, iobrnum, loopnum, nodenum);
            int rmRownumber = (iterRestateP.value()>>16) & 0xffff;
            hashRestart29min.insert(tempnode, rmRownumber<<16 | TWENTY_NINE_MINS);
            //qDebug()<<"hashRestart60s:60s delay over,insert into 29minHash.";
            iterRestateP = hashRestart60s.erase(iterRestateP);
        }
        else
        {
            iterRestateP.value() -= 1;
            iterRestateP++;
        }
    }
    //29min delay to delete fire
    QHash<nodeID, int>::iterator iterRestate29minP = hashRestart29min.begin();
    while (iterRestate29minP != hashRestart29min.end())
    {
        if( (iterRestate29minP.value() & 0xffff) == 0)
        {
            //29MINS later，就将这条火警删除
            nodeID tempnode = iterRestate29minP.key();//nodeID(hostnum, iobrnum, loopnum, nodenum);
            int rmRownumber = (iterRestate29minP.value()>>16) & 0xffff;
            removeFireItem(tempnode,rmRownumber);
            //qDebug()<<"hashRestart29min:29min delay over,delete fire row "<<rmRownumber;
            iterRestate29minP = hashRestart29min.erase(iterRestate29minP);
        }
        else
        {
            iterRestate29minP.value() -= 1;
            iterRestate29minP++;
        }
    }

    /* A:报警延时 deal with hashFireDelay */
    QHash<nodeID, int>::iterator iterFireP = hashFireDelay.begin();
    while (iterFireP != hashFireDelay.end())
    {
        if( (iterFireP.value() & 0xff) == 0)
        {
            nodeID tempnode = iterFireP.key();//nodeID(hostnum, iobrnum, loopnum, nodenum);
            //int hostnum = tempnode.gethostnum();
            int iobrnum = tempnode.getiobrnum();
            int loopnum = tempnode.getloopnum();
            int nodenum = tempnode.getnodenum();
            unsigned char nodestate = iterFireP.value() >> 16 & 0xff;
            unsigned char nodetype = iterFireP.value() >> 24 & 0xff;
            int rmRowNumber = (iterFireP.value()>>8) & 0xff;
            delayList.removeAt( rmRowNumber);


            //fireShow(iobrnum,loopnum,nodenum,nodetype,nodestate,QDateTime::currentDateTime());
            qDebug()<<"fireDelay over,show fire "<<iobrnum<<"-"<<loopnum<<"-"<<nodenum<<"rmRowNumber is"<<rmRowNumber;

            iterFireP = hashFireDelay.erase(iterFireP);
            emit eventNumberChangedSignal();
            //遍历三个 Hash Delay 表，找出所有key值大于当前删除行号 的节点
            updateHashRowNumber(hashFireDelay,rmRowNumber);
            updateHashRowNumber(hashLinkStartDelay,rmRowNumber);
            updateHashRowNumber(hashLinkPulseDelay,rmRowNumber);

            if(eventTabWidget->currentIndex() == delayIndex)
            {
                updateTabWidget( eventTabWidget->currentIndex());
            }
            fireShow(iobrnum,loopnum,nodenum,nodetype,nodestate,QDateTime::currentDateTime());
        }
        else
        {
            iterFireP.value() -= 1;
            int row =iterFireP.value()>>8 & 0xff;
            QString strItem = QString("%1 %2").arg(iterFireP.value() & 0xff).arg(delayList.at(row));
            delayTableWidget->setItem(row, 0, new QTableWidgetItem(strItem));
            iterFireP++;
        }
    }

    /* B:联动启动延时: hashLinkStartDelay.insert(tempnode, pulseDelay<<8 | ((delayList.count()-1)<<8) | startDelay); */
    QHash<nodeID, int>::iterator iterLinkStartP = hashLinkStartDelay.begin();
    while (iterLinkStartP != hashLinkStartDelay.end())
    {
        if( (iterLinkStartP.value() & 0xff) == 0)
        {
            nodeID tempnode = iterLinkStartP.key();//nodeID(hostnum, iobrnum, loopnum, nodenum);

            int hostnum = tempnode.gethostnum();
            int iobrnum = tempnode.getiobrnum();
            int loopnum = tempnode.getloopnum();
            int nodenum = tempnode.getnodenum();
            nodeID linknode = nodeID(hostnum,iobrnum,loopnum,nodenum,LINK_ACTION_START);
            int pulseDelay =(iterLinkStartP.value() >> 16) & 0xff;
            int rmRowNumber= (iterLinkStartP.value() >> 8 ) & 0xff;
            delayList.removeAt( rmRowNumber);

            emit nodeEnqueueSignal(LINK_NODE_ENQUEUE,linknode);
            qDebug()<<"LinkStartDelay over,send LinkStart"<<iobrnum<<"-"<<loopnum<<"-"<<nodenum<<"rmRowNumber is"<<rmRowNumber;

            if( !hashLinkageAction.contains(tempnode))
                hashLinkageAction.insert(tempnode,LINK_ACTION_START);
            else
            {
                QHash<nodeID, int>::iterator iterP = hashLinkageAction.find(tempnode);
                iterP.value() =  LINK_ACTION_START;
            }

            iterLinkStartP = hashLinkStartDelay.erase(iterLinkStartP);
            emit eventNumberChangedSignal();
            //遍历三个 Hash Delay 表，找出所有key值大于当前删除行号 的节点
            updateHashRowNumber(hashFireDelay,rmRowNumber);
            updateHashRowNumber(hashLinkStartDelay,rmRowNumber);
            updateHashRowNumber(hashLinkPulseDelay,rmRowNumber);

            if(eventTabWidget->currentIndex() == delayIndex)
            {
                updateTabWidget( eventTabWidget->currentIndex());
            }

            if( pulseDelay > 0 )
            {
                QSqlDatabase::database().transaction();
                QSqlQuery query;
                query.setForwardOnly(true);
                query.exec(QString("select * from unitset WHERE unitid == %1")
                           .arg((hostnum << 24) | (iobrnum << 16) | (loopnum << 8) | nodenum));
                if (query.next())
                {
                    int  bldnum = 0 , flrnum = 0, zonenum = 0, roomnum = 0;
                    unsigned char nodetype;
                    QString nodestr;
                    QString infoid_info;
                    QString geo_addr_info;
                    bldnum = (query.value(6).toInt() >> 24) & 0xFF;
                    flrnum = (query.value(6).toInt() >> 16) & 0xFF;
                    zonenum = (query.value(6).toInt() >> 8) & 0xFF;
                    roomnum = query.value(6).toInt() & 0xFF;
                    nodetype = query.value(7).toInt()>>8 & 0xFF;
                    nodestr = query.value(9).toString();
                    infoid_info.sprintf("%02d-%02d-%02d-%03d",hostnum, iobrnum,loopnum, nodenum);
                    geo_addr_info.sprintf("%02d/%02d/%02d", bldnum, flrnum, zonenum);
                    QString stringItem = QString("%1 %2 %3 %4 %5 %6")
                            .arg(tr("秒倒计时"))//.arg(pulseDelay))
                            .arg(infoid_info) //host-iobr-loop-node
                            .arg(tr("联动停止"))//1
                            .arg(Type::char_danyuan(nodetype) )//2
                            .arg(geo_addr_info) //bld/flr/zone
                            .arg(nodestr);//by qingwu:显示汉字标签
                    delayList.append(stringItem);
                    hashLinkPulseDelay.insert(tempnode,((delayList.count()-1)<<8)| pulseDelay);
                    emit eventNumberChangedSignal();
                    //qDebug()<<"link pulse delay,row is"<<delayList.count()-1;
                    if( (fireList.isEmpty() && linkList.isEmpty() && superviseList.isEmpty() )
                            || ( eventdialog->isActiveWindow() && (!(oneEventShowFlag ^ DELAY_PUSHBUTTON))))
                    {
                        qDebug()<<"there is no fire or link or supervise ";
                        delayEventDialog();
                    }
                    if(eventTabWidget->currentIndex() == delayIndex)
                    {
                        updateTabWidget( eventTabWidget->currentIndex());
                    }
                }
                QSqlDatabase::database().commit();
            }
        }
        else
        {
            iterLinkStartP.value() -= 1;
            int row =iterLinkStartP.value()>>8 & 0xff;
            QString strItem = QString("%1 %2").arg(iterLinkStartP.value() & 0xff).arg(delayList.at(row));
            delayTableWidget->setItem(row, 0, new QTableWidgetItem(strItem));
            iterLinkStartP++;
        }
    }

    /*C:联动脉冲延时: hashLinkPulseDelay.insert(tempnode,((delayList.count()-1)<<8) | pulseDelay); */
    QHash<nodeID, int>::iterator iterLinkPulseP = hashLinkPulseDelay.begin();
    while (iterLinkPulseP != hashLinkPulseDelay.end())
    {
        if( (iterLinkPulseP.value() & 0xff) == 0)
        {
            nodeID tempnode = iterLinkPulseP.key();//nodeID(hostnum, iobrnum, loopnum, nodenum);
            //int hostnum = tempnode.gethostnum();
            int iobrnum = tempnode.getiobrnum();
            int loopnum = tempnode.getloopnum();
            int nodenum = tempnode.getnodenum();
            nodeID linknode = nodeID(1,iobrnum,loopnum,nodenum,LINK_ACTION_STOP);
            int rmRowNumber = (iterLinkPulseP.value() >> 8) & 0xff;
            delayList.removeAt( rmRowNumber);

            emit nodeEnqueueSignal(LINK_NODE_ENQUEUE,linknode);
            qDebug()<<"LinkPulseDelay over,send LinkStop"<<iobrnum<<"-"<<loopnum<<"-"<<nodenum<<"rmRowNumber is"<<rmRowNumber;

            if( !hashLinkageAction.contains(tempnode))
                hashLinkageAction.insert(tempnode,LINK_ACTION_STOP);
            else
            {
                QHash<nodeID, int>::iterator iterP = hashLinkageAction.find(tempnode);
                iterP.value() = LINK_ACTION_STOP;
            }

            iterLinkPulseP = hashLinkPulseDelay.erase(iterLinkPulseP);//returns an iterator to the next item in the hash
            emit eventNumberChangedSignal();
            //遍历三个 Hash Delay 表，找出所有key值大于当前删除行号 的节点
            updateHashRowNumber(hashFireDelay,rmRowNumber);
            updateHashRowNumber(hashLinkStartDelay,rmRowNumber);
            updateHashRowNumber(hashLinkPulseDelay,rmRowNumber);

            if(eventTabWidget->currentIndex() == delayIndex)
            {
                updateTabWidget( eventTabWidget->currentIndex());
            }
        }
        else
        {
            iterLinkPulseP.value() -= 1;
            int row =iterLinkPulseP.value()>>8 & 0xff;
            QString strItem = QString("%1 %2").arg(iterLinkPulseP.value() & 0xff).arg(delayList.at(row));
            delayTableWidget->setItem(row, 0, new QTableWidgetItem(strItem));
            iterLinkPulseP++;
        }
    }

}


void  MainWidget::dealLinkNodeSlot(int nodeid)
{
    /*  根据火警查到对应联动信息，接下来对联动过滤：
        1，查询节点是否登记
        2，节点是否被屏蔽
        3，节点是否在故障列表
        4，节点是否已经在联动列表
        5，节点是否有延时（启动延时 和 脉冲延时） */

    int hostnum = 1;
    int iobrnum = (nodeid >> 16) & 0xFF;
    int loopnum = (nodeid >> 8) & 0xFF;
    int nodenum = (nodeid ) & 0xFF;
    nodeID tempnode = nodeID(hostnum,iobrnum,loopnum,nodenum);
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from unitset WHERE unitid == %1").arg(nodeid));
    if (query.next())
    {
        /* 要启动的联动设备 1,已经登记 2，没有屏蔽 3，不在故障列表
                    4，节点不在联动哈希表中 或者 节点在联动哈希表中且节点状态是未启动
                    5，不在启动延时列表 6，不在脉冲延时列表 */
//        if (query.value(1).toBool() && (!query.value(2).toBool())
//                && (!hashFault.contains(tempnode))
//                && (!(hashLink.contains(tempnode) && ((hashLink.find(tempnode).value()>>24 & 0xff) == 0x03)))
//                && (!hashLinkStartDelay.contains(tempnode)) && (!hashLinkPulseDelay.contains(tempnode)))
        if (        query.value(1).toBool()
                && (!query.value(2).toBool())
                && (!hashLinkStartDelay.contains(tempnode))
                && (!hashLinkPulseDelay.contains(tempnode))
                && (!hashFault.contains(tempnode))
                && ((!hashLink.contains(tempnode))
                    || ( (hashLink.find(tempnode).value()>>24 & 0xff) == 0x03) ) )
        {
            int startDelay =  query.value(8).toInt() & 0xFF;
            int pulseDelay = (query.value(8).toInt()>>8) & 0xFF;

            if(startDelay == 0 )
            {
                nodeID linknode = nodeID(hostnum,iobrnum,loopnum,nodenum,LINK_ACTION_START);
                emit nodeEnqueueSignal(LINK_NODE_ENQUEUE,linknode);
                qDebug()<<"dealLinkNodeSlot:startDelay == 0,send link:"<<iobrnum<<"-"<<loopnum<<"-"<<nodenum;
                if( !hashLinkageAction.contains(tempnode))
                    hashLinkageAction.insert(tempnode,LINK_ACTION_START);
                else
                {
                    QHash<nodeID, int>::iterator iterP = hashLinkageAction.find(tempnode);
                    iterP.value() =  LINK_ACTION_START;
                }

                if(pulseDelay > 0)
                {
                    /* the node  startDelay == 0,pulseDelay > 0, first show in the delayTableWidget,
                     write in the pulse delay hash table,after the delay time,send linkstop fmame */
                    int  bldnum = 0 , flrnum = 0, zonenum = 0, roomnum = 0;
                    unsigned char nodetype;
                    QString nodestr;
                    QString infoid_info;
                    QString geo_addr_info;
                    bldnum = (query.value(6).toInt() >> 24) & 0xFF;
                    flrnum = (query.value(6).toInt() >> 16) & 0xFF;
                    zonenum = (query.value(6).toInt() >> 8) & 0xFF;
                    roomnum = query.value(6).toInt() & 0xFF;
                    nodetype = query.value(7).toInt()>>8 & 0xFF;
                    nodestr = query.value(9).toString();
                    infoid_info.sprintf("%02d-%02d-%02d-%03d",hostnum, iobrnum,loopnum, nodenum);
                    geo_addr_info.sprintf("%02d/%02d/%02d", bldnum, flrnum, zonenum);
                    QString stringItem = QString("%1 %2 %3 %4 %5 %6")
                            .arg(tr("秒倒计时"))//.arg(pulseDelay))
                            .arg(infoid_info) //host-iobr-loop-node
                            .arg(tr("联动停止"))//1
                            .arg(Type::char_danyuan(nodetype) )//2
                            .arg(geo_addr_info) //bld/flr/zone
                            .arg(nodestr);//by qingwu:显示汉字标签
                    delayList.append(stringItem);

                    hashLinkPulseDelay.insert(tempnode,((delayList.count()-1)<<8)| pulseDelay);
                    emit eventNumberChangedSignal();
                    if( (fireList.isEmpty() && linkList.isEmpty() && superviseList.isEmpty() )
                            || ( eventdialog->isActiveWindow() && (!(oneEventShowFlag ^ DELAY_PUSHBUTTON))))
                    {
                        qDebug()<<"there is no fire or link or supervise ";
                        delayEventDialog();
                    }
                    if(eventTabWidget->currentIndex() == delayIndex)
                    {
                        updateTabWidget( eventTabWidget->currentIndex());
                    }
                    // qDebug()<<"link pulse delay,row is"<<delayList.count()-1;
                }
            }
            else
            {
                /* the node  start delay > 0,first show in the delayTableWidget,
                    when delay over,send link start Frame when pulseDelay = 0;
                    when pulseDelay >0, after send link start Frame,write in the pulse delay hash table, still show in the delayTableWidget */

                int  bldnum = 0 , flrnum = 0, zonenum = 0, roomnum = 0;
                unsigned char nodetype;
                QString nodestr;
                QString infoid_info;
                QString geo_addr_info;
                bldnum = (query.value(6).toInt() >> 24) & 0xFF;
                flrnum = (query.value(6).toInt() >> 16) & 0xFF;
                zonenum = (query.value(6).toInt() >> 8) & 0xFF;
                roomnum = query.value(6).toInt() & 0xFF;
                nodetype = query.value(7).toInt()>>8 & 0xFF;
                nodestr = query.value(9).toString();
                infoid_info.sprintf("%02d-%02d-%02d-%03d",hostnum, iobrnum,loopnum, nodenum);
                geo_addr_info.sprintf("%02d/%02d/%02d", bldnum, flrnum, zonenum);

                QString stringItem = QString("%1 %2 %3 %4 %5 %6")
                        .arg(tr("秒倒计时"))//.arg(startDelay))
                        .arg(infoid_info) //host-iobr-loop-node
                        .arg(tr("联动启动"))//1
                        .arg(Type::char_danyuan(nodetype) )//2
                        .arg(geo_addr_info) //bld/flr/zone
                        .arg(nodestr);//by qingwu:显示汉字标签
                delayList.append(stringItem);

                hashLinkStartDelay.insert(tempnode, pulseDelay<<16 | ((delayList.count()-1)<<8) | startDelay);
                emit eventNumberChangedSignal();
                if( (fireList.isEmpty() && linkList.isEmpty() && superviseList.isEmpty() )
                        || ( eventdialog->isActiveWindow() && (!(oneEventShowFlag ^ DELAY_PUSHBUTTON))))
                {
                    qDebug()<<"there is no fire or link or supervise ";
                    delayEventDialog();
                }
                if(eventTabWidget->currentIndex() == delayIndex)
                {
                    updateTabWidget( eventTabWidget->currentIndex());
                }
                // qDebug()<<"link start delay,row is"<<delayList.count()-1;
            }
        }
        else
        {
            qDebug()<<"link start node missmatch the 6 conditions.";
        }
    }
    else
    {
        qDebug()<<"link start node is not in database.";
    }
    QSqlDatabase::database().commit();
}

void MainWidget::dealHandNodeSlot(char action, int nodeid)
{
    int hostnum = 1;
    int iobrnum = (nodeid >> 16) & 0xFF;
    int loopnum = (nodeid >> 8) & 0xFF;
    int nodenum = (nodeid ) & 0xFF;
    nodeID tempnode = nodeID(hostnum,iobrnum,loopnum,nodenum);
    nodeID linknode = nodeID(hostnum,iobrnum,loopnum,nodenum,action);
    emit nodeEnqueueSignal(LINK_NODE_ENQUEUE,linknode);

    if( !hashLinkageAction.contains(tempnode))
        hashLinkageAction.insert(tempnode,action);
    else
    {
        QHash<nodeID, int>::iterator iterP = hashLinkageAction.find(tempnode);
        iterP.value() = action;
    }
}

void MainWidget::fireComeSlot(int iobrnum, int loopnum, int nodenum,unsigned char nodetype,unsigned char nodestate, QDateTime datetime)
{
    /* check hashFire for re-report fire ,make sure if the coming fire has reported before*/
    int hostnum = 1;
    nodeID tempnode = nodeID(hostnum, iobrnum, loopnum, nodenum);
    if ( (hashFire.contains( tempnode ) && (isReReport(TI_FireAlarm,tempnode,nodestate) >= 0))
            || (hashFireDelay.contains(tempnode) && (isReReport(TI_FireAlarm,tempnode,nodestate) >= 0)))
    {
        qDebug()<<"re-report fire frame of..."<<iobrnum<<loopnum<<nodenum;
        return;
    }
    else
    {
        /* a new fire comes , get zone bld flr room number form database,hostnum default set to 1. */
        QSqlDatabase::database().transaction();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid == %1")
                   .arg((hostnum << 24) | (iobrnum << 16) | (loopnum << 8) | nodenum));
        if (query.next())
        {
            int startDelay = query.value(8).toInt() & 0xFF;
            if(startDelay == 0)
                fireShow(iobrnum, loopnum, nodenum, nodetype, nodestate, datetime);
            else
            {
                /* the node  start delay > 0,first show in the delayTableWidget,
                    when delay over,show in the fireTableWidget     */
                int hostnum = 1;
                int  bldnum = 0 , flrnum = 0, zonenum = 0, roomnum = 0;
                bldnum = (query.value(6).toInt() >> 24) & 0xFF;
                flrnum = (query.value(6).toInt() >> 16) & 0xFF;
                zonenum = (query.value(6).toInt() >> 8) & 0xFF;
                roomnum = query.value(6).toInt() & 0xFF;
                QString nodestr = query.value(9).toString();
                QString infoid_info;
                QString geo_addr_info;
                infoid_info.sprintf("%02d-%02d-%02d-%03d",hostnum, iobrnum,loopnum, nodenum);
                geo_addr_info.sprintf("%02d/%02d/%02d", bldnum, flrnum, zonenum);
                QString stringItem = QString("%1 %2 %3 %4 %5 %6")
                        .arg(tr("秒倒计时"))
                        .arg(infoid_info) //host-iobr-loop-node
                        .arg(Type::char_shijian(nodestate,nodetype) )//1
                        .arg(Type::char_danyuan(nodetype) )//2
                        .arg(geo_addr_info) //bld/flr/zone
                        .arg(nodestr);//by qingwu:显示汉字标签
                delayList.append(stringItem);
                hashFireDelay.insert(tempnode, nodetype<<24 | nodestate<<16 | ((delayList.count()-1)<<8) | startDelay);
                emit eventNumberChangedSignal();
                qDebug()<<"fire show in delayTableWidget,row is"<<delayList.count()-1;

                if( (fireList.isEmpty() && linkList.isEmpty() && superviseList.isEmpty() )
                        || ( eventdialog->isActiveWindow() && (!(oneEventShowFlag ^ DELAY_PUSHBUTTON))))
                {
                    qDebug()<<"there is no fire or link or supervise ";
                    delayEventDialog();
                }
                if(eventTabWidget->currentIndex() == delayIndex)
                {
                    updateTabWidget( eventTabWidget->currentIndex());
                }
            }
        }
        else
        {
            qDebug()<<"fire node is not in database.";
        }
        QSqlDatabase::database().commit();
    }


}

void MainWidget::fireShow(int iobrnum, int loopnum, int nodenum, unsigned char nodetype, unsigned char nodestate, QDateTime datetime)
{
    /* 火警报警延时为0 或者已经延时完毕，处理火警
        a new fire comes , get zone bld flr room number form database,hostnum default set to 1. */
    //start the sound and alarm device-by linwotian
    if((!sound_alarm_shield_Flag) && (!sound_alarm_fault_Flag))
    {
        if(!sound_alarm_startOrStop_Flag)
        {
            OperationHis::Update("自动启动声光报警器");
            emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_START,LED_PUT_ON);
            this->show();//show main widget
            this->eventComeSlot(TI_LinkageStart, 0, 0, 0, 0x14, 0x0b, QDateTime::currentDateTime());
            sound_alarm_startOrStop_Flag = true;
        }
    }

    int hostnum = 1;
    nodeID tempnode = nodeID(hostnum, iobrnum, loopnum, nodenum);
    int  bldnum = 0 , flrnum = 0, zonenum = 0, roomnum = 0;
    QString nodestr;
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from unitset WHERE unitid == %1")
               .arg((hostnum << 24) | (iobrnum << 16) | (loopnum << 8) | nodenum));
    if (query.next())
    {
        bldnum = (query.value(6).toInt() >> 24) & 0xFF;
        flrnum = (query.value(6).toInt() >> 16) & 0xFF;
        zonenum = (query.value(6).toInt() >> 8) & 0xFF;
        roomnum = query.value(6).toInt() & 0xFF;
        nodestr = query.value(9).toString();
        QString infoid_info;
        QString geo_addr_info;
        infoid_info.sprintf("%02d-%02d-%02d-%03d",hostnum, iobrnum,loopnum, nodenum);
        geo_addr_info.sprintf("%02d/%02d/%02d", bldnum, flrnum, zonenum);

        QString stringItem = QString("%1 %2 %3 %4 %5 %6")
                .arg(datetime.toString("MM-dd hh:mm:ss"))
                .arg(infoid_info) //host-iobr-loop-node
                .arg(Type::char_shijian(nodestate,nodetype) )//1
                .arg(Type::char_danyuan(nodetype) )//2
                .arg(geo_addr_info) //bld/flr/zone
                .arg(nodestr);//by qingwu:显示汉字标签
        fireList.append(stringItem);
        fireAllNumber += 1;


        if( linkList.isEmpty() && superviseList.isEmpty())
        {
            fireEventDialog();//独立火警事件界面显示
        }
        else
            this->show();//show main widget

        /* if只有一条火警显示在首火警;else第一行隐藏，添加火警显示的时候，从第二行开始，也就是序号1*/
        if( fireList.count()==1 )
        {            
            firstFireTableWidget->setItem(0,0,new QTableWidgetItem(fireList.at(0)));
        }
        else
        {
            //            if(fireList.count() > LISTWIDGET_ROW_OF_FIRE)
            //                ifire = fireList.count() - LISTWIDGET_ROW_OF_FIRE;
            //            else
            //                ifire = 1;
            ifire = fireList.count() - 2;
            updateFireWidgetSlot();
            //    fireTableWidget->setItem(fireList.count()-1,0,new QTableWidgetItem(fireList.last()));
        }


        //fireTableWidget->selectRow(fireList.count() -1);
        //firenumLabel->setText(tr("%1").arg(fireList.count()));
        saveFireToDB((int)nodestate, (int)nodetype, iobrnum, loopnum, nodenum, bldnum , flrnum, zonenum , roomnum, datetime, nodestr);
        if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_FIRE_FLAG) & 0x01) )
        {
            QString line2 = QString("%1 %2 %3")
                    .arg(Type::char_shijian(nodestate,nodetype) )//1
                    .arg(Type::char_danyuan(nodetype) )//2
                    .arg(geo_addr_info); //bld/flr/zone
            QString line3 = QString("%1 %2")
                    .arg(datetime.toString("MM-dd hh:mm:ss"))
                    .arg(infoid_info); //host-iobr-loop-node
            doPrint(nodestr,line2,line3);
        }
        hashFire.insert(tempnode, nodestate<<16 | (fireList.count()-1));//上报事件的类型<<16 | 上报事件在列表中的序号，用做哈希表的value值。
        emit eventNumberChangedSignal();
        if( (!query.value(3).toBool()) || (query.value(3).toBool() && (hashRestart60s.contains(tempnode))))
        {
            /*节点不是重报节点 或者 是重报节点且60s之内上报的火警事件数目大于1*/
            emit ledStateChangeSignal(LED_OF_FIRE,LED_PUT_ON);//test,first show fire,then light on led.
            emit eventTrueComes(iobrnum, loopnum, nodenum,nodetype,nodestate);
            emit closeWVSignal();
            int louxiannum = query.value(7).toInt() & 0xFF;
            if( louxiannum>0 && louxiannum<=255 )
            {               
                //查询这个报警节点的所属楼显
                QSqlQuery query1;
                query1.setForwardOnly(true);
                query1.exec(QString("select * from unitset WHERE unitid == %1")
                            .arg((hostnum << 24) | (LOUXIANNUM << 16) | (1 << 8) | louxiannum));
                if (query1.next())
                {
                    if(query1.value(1).toBool() && (!query1.value(2).toBool()))
                    {
                        nodeID louxiannode = nodeID(louxiannum,iobrnum, loopnum, nodenum,nodetype,nodestate);
                        emit nodeEnqueueSignal(LOUXIAN_NODE_ENQUEUE,louxiannode);
                    }
                }
                else
                    qDebug()<<"in fireShow():cant find the louxian in unitset.";
            }
            if(query.value(3).toBool())
            {
                hashRestart60s.erase(hashRestart60s.find(tempnode));
            }
        }
        else
        {
            hashRestart60s.insert(tempnode, (fireList.count()-1) << 16 | SIXTY_SECONDS);
            qDebug()<<"node"<<tempnode.getnodenum()<<"is reRestate,insert into 60sDelay";
            emit closeWVSignal();//声报警信号怎么弄
        }
    }
    QSqlDatabase::database().commit();
}


void MainWidget::faultComeSlot(int iobrnum, int loopnum, int nodenum,unsigned char nodetype,unsigned char nodestate, QDateTime datetime)
{
    /* check hashFault , make sure if the coming fire has reported before */
    int hostnum = 1;
    nodeID tempnode = nodeID(hostnum,iobrnum,loopnum,nodenum);
    int shijian = nodestate;
    if ( hashFault.contains( tempnode ) && (isReReport(TI_BreakDown, tempnode,nodestate) >= 0))
    {
        qDebug()<<"re-report fault frame of..."<<iobrnum<<loopnum<<nodenum;
        return;
    }
    else
    {
        /* get zone bld flr room number form database,hostnum default set to 1. */
        int  bldnum = 0 , flrnum = 0, zonenum = 0, roomnum = 0;
        QString nodestr;
        QSqlDatabase::database().transaction();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid == %1")
                   .arg((hostnum << 24) | (iobrnum << 16) | (loopnum << 8) | nodenum));
        if (query.next())
        {
            bldnum = (query.value(6).toInt() >> 24) & 0xFF;
            flrnum = (query.value(6).toInt() >> 16) & 0xFF;
            zonenum = (query.value(6).toInt() >> 8) & 0xFF;
            roomnum = query.value(6).toInt() & 0xFF;
            nodestr = query.value(9).toString();
            QString infoid_info;
            QString geo_addr_info;
            infoid_info.sprintf("%02d-%02d-%02d-%03d",hostnum, iobrnum,loopnum, nodenum);
            geo_addr_info.sprintf("%02d/%02d/%02d", bldnum, flrnum, zonenum);
            QString stringItem = QString("%1 %2 %3 %4 %5 %6")
                    .arg(datetime.toString("MM-dd hh:mm:ss"))
                    .arg(infoid_info) //host-iobr-loop-node
                    .arg(Type::char_shijian(nodestate,nodetype) )//1
                    .arg(Type::char_danyuan(nodetype) )//2
                    .arg(geo_addr_info) //bld/flr/zone
                    .arg(nodestr);//by qingwu:显示汉字标签

            faultList.append(stringItem);

            /* 判断是否有优先级更高的事件，如果有，那么界面不跳转;否则，显示故障列表*/
            if( (fireList.isEmpty() && linkList.isEmpty() && superviseList.isEmpty() && delayList.isEmpty())
                    || ( eventdialog->isActiveWindow() && (!(oneEventShowFlag ^ FAULT_PUSHBUTTON))))
            {
                qDebug()<<"faultComeSlot():there is no fire or link or supervise or delay";
                faultEventDialog();
            }
            if(eventTabWidget->currentIndex() == faultIndex)
            {
                qDebug()<<"faultComeSlot():updateTabWidget().";
                updateTabWidget( eventTabWidget->currentIndex());
            }

            faultAllNumber += 1;
            hashFault.insert(tempnode, nodestate<<16 | (faultList.count()-1));//That is assumed the maxinum number is 0xFFFF;
            emit eventNumberChangedSignal();
            emit ledStateChangeSignal(LED_OF_FAULT,LED_PUT_ON);
            emit closeWVSignal();
            saveFaultToDB((int)nodestate,(int)nodetype, iobrnum, loopnum, nodenum, bldnum , flrnum, zonenum , roomnum, datetime, nodestr);
            if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_FAULT_FLAG) & 0x01) )
            {
                QString line2 = QString("%1 %2 %3")
                        .arg(Type::char_shijian(nodestate,nodetype) )//1
                        .arg(Type::char_danyuan(nodetype) )//2
                        .arg(geo_addr_info); //bld/flr/zone
                QString line3 = QString("%1 %2")
                        .arg(datetime.toString("MM-dd hh:mm:ss"))
                        .arg(infoid_info); //host-iobr-loop-node

                doPrint(nodestr,line2,line3);
            }
        }
        else
        {
            qDebug()<<"in mianwidget:faultComeSlot():node"<<iobrnum<<loopnum<<nodenum<<"not in database";
        }
        QSqlDatabase::database().commit();
    }
}

void MainWidget::faultRecoverSlot(int iobrnum,int loopnum,int nodenum,unsigned char nodetype,unsigned char nodestate, QDateTime datetime)
{
    /* check hashFault , make sure if the coming fire has reported before */
    int hostnum = 1;
    nodeID tempnode = nodeID(hostnum, iobrnum,loopnum,nodenum);
    unsigned char shijian =  getFaultCode(nodestate) & 0xff;

    /*  1) 如果hashFault不包含节点，那么不处理
        2) 如果hashFault包含节点，并且节点的故障类型跟故障恢复的类型相对应,那么处理
        3) 否则，哈希表包含节点，但是节点的故障类型跟 要恢复的故障类型不同 那么不处理    */
    if( !hashFault.contains( tempnode ))
    {
        qDebug()<<"in faultRecoverSlot():fault"<<iobrnum<<loopnum<<nodenum<<"is NOT in hashFault,return 1";
        return;
    }
    else if ( hashFault.contains( tempnode ) && (isReReport(TI_BreakDownRecover, tempnode, shijian) >= 0) )
    {
        //this->show();//show main widget
        qDebug()<<"fault  RECOVER  frame"<<iobrnum<<loopnum<<nodenum<<"do RECOVER !";
        /* get zone bld flr room number form database,hostnum default set to 1. */
        int zonenum = 0, bldnum = 0 ,flrnum = 0,roomnum = 0;
        QString nodestr;
        QSqlDatabase::database().transaction();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid == %1")
                   .arg((hostnum << 24) | (iobrnum << 16) | (loopnum << 8) | nodenum));
        if (query.next())
        {
            bldnum = (query.value(6).toInt() >> 24) & 0xFF;
            flrnum = (query.value(6).toInt() >> 16) & 0xFF;
            zonenum = (query.value(6).toInt() >> 8) & 0xFF;
            roomnum = query.value(6).toInt() & 0xFF;
            nodestr = query.value(9).toString();

            int rmRowNumber = isReReport(TI_BreakDownRecover,tempnode,shijian);
            qDebug()<<"delete fault of faultList ,the row number is: "<< rmRowNumber;
            faultList.removeAt(rmRowNumber);


            /* 判断是否有优先级更高的事件，如果有，那么界面不跳转;否则，显示故障列表*/
            if( (fireList.isEmpty() && linkList.isEmpty() && superviseList.isEmpty() && delayList.isEmpty())
                    || ( eventdialog->isActiveWindow() && (!(oneEventShowFlag ^ FAULT_PUSHBUTTON))))
            {
                qDebug()<<"there is no fire or link or supervise or delay";
                faultEventDialog();
            }
            if(eventTabWidget->currentIndex() == faultIndex)
            {
                updateTabWidget( eventTabWidget->currentIndex());
            }
            faultAllNumber += 1;

            //遍历hashFault表，找出所有key值大于当前删除行号 的节点，对其key值进行-1操作. 删除对应<value,key>
            QHash<nodeID, int>::iterator iterD = hashFault.find(tempnode, shijian<<16 | rmRowNumber);
            hashFault.erase(iterD);//.remove(tempnode);
            emit eventNumberChangedSignal();

            QHash<nodeID, int>::iterator iterP = hashFault.begin();
            while (iterP != hashFault.end())
            {
                if(  ( iterP.value() & 0xffff) > rmRowNumber )
                    iterP.value() -= 1;
                ++iterP;
            }

            /*故障列表为空*/
            if(faultList.isEmpty())
            {
                emit ledStateChangeSignal(LED_OF_FAULT,LED_PUT_OFF);
            }
            //emit closeWVSignal();
            saveFaultToDB((int)nodestate,(int)nodetype, iobrnum, loopnum, nodenum, bldnum , flrnum, zonenum , roomnum, datetime, nodestr);
            if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_FAULT_FLAG) & 0x01) )
            {
                eventPrint(hostnum, iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
            }
        }
        QSqlDatabase::database().commit();
    }
    else
    {
        qDebug()<<"in faultRecoverSlot():hashFault.contains fault, but isReReport()="<< isReReport(TI_BreakDownRecover, tempnode, shijian);
        return;
    }
}



void MainWidget::feedBackSlot( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate, QDateTime datetime)
{
    int hostnum = 1;
    nodeID tempnode = nodeID(hostnum, iobrnum,loopnum,nodenum);
    if((nodetype != 0x09) && (nodetype != 0x40))
    {
        /*  对于非输入输出设备（只会是输入模块，上报的状态只有 反馈 和 反馈恢复。
            控制器不会给输入设备发送命令！所以，不需要查命令表）
            1)如果 hashLink中包含节点 并且 上报的事件类型跟保存的事件类型一样——>重报信息，不处理
            2)否则，哈希表中不包含节点——>正常显示 */
        if( hashLink.contains(tempnode) && isReReport(TI_FeedBack,tempnode,nodestate) >= 0)
        {
            qDebug()<<"in feedBackSlot(),feedback RE-Report.return1...";
            return;
        }
        else if( !hashLink.contains(tempnode) )
        {
            qDebug()<<"in feedBackSlot():deal feedback1...";
            //if(eventdialog->isActiveWindow())
            eventdialog->hide();
            this->show();//show main widget

            QString stringItem = getStingItemFromDB(hostnum, iobrnum,loopnum,nodenum,nodestate,nodetype,nodetype,datetime);

            linkAllNumber += 1;
            saveLinkToDB(iobrnum, loopnum, nodenum, nodetype, nodestate, datetime);
            if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_LINK_FLAG) & 0x01) )
            {
                eventPrint(hostnum, iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
            }

            linkList.append(stringItem);
            hashLink.insert(tempnode, nodestate<<24 | (linkList.count()-1));
            emit eventNumberChangedSignal();
            busLinkAction(IS_NOT_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);

            //            if(linkList.count() > LISTWIDGET_ROW_OF_FIRE-1)
            //                ilink = linkList.count() - LISTWIDGET_ROW_OF_FIRE + 1;
            //            else
            //                ilink = 0;
            ilink = linkList.count()-1;
            updateLinkWidgetSlot();
            linknumLabel->setText(tr("%1").arg(linkList.count()));
            emit closeWVSignal();
        }
    }
    else
    {
        /* 对于输入输出设备
           1)如果哈希表中不包含设备,不管有没有发送过启动命令——> 都显示 联动反馈

         */
        if ( !hashLink.contains( tempnode ) )
        {
            //if(eventdialog->isActiveWindow())
            eventdialog->hide();
            this->show();//show main widget

            char shijianma;
            char newState;
            QString stringItem;
            qDebug()<<"in feedBackSlot():deal feedback3...";
            newState = LINK_FEEDBACK_STATE;
            shijianma = LINK_SHIJIAN_MA;
            stringItem = getStingItemFromDB(hostnum, iobrnum,loopnum,nodenum,shijianma,newState,nodetype,datetime);

            linkAllNumber += 1;
            saveLinkToDB(iobrnum, loopnum, nodenum, nodetype, nodestate, datetime);
            if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_LINK_FLAG) & 0x01) )
            {
                eventPrint(hostnum, iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
            }

            stringItem.insert(6,QString("启--:--:-- 反"));
            linkList.append(stringItem);
            hashLink.insert(tempnode, (newState<<24) | (TI_FeedBack<<16) | (linkList.count()-1));
            emit eventNumberChangedSignal();
            busLinkAction(IS_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);

            ilink = linkList.count()-1;
            updateLinkWidgetSlot();
            linknumLabel->setText(tr("%1").arg(linkList.count()));
            emit closeWVSignal();
        }
        else
        {
            /*哈希表中包含节点
            2.1）如果哈希表中包含节点，并且节点状态是 启动已反馈or联动反馈——>不处理
            2.2）如果节点状态是 启动未反馈——>显示 启动已反馈 */
            char currentState = (hashLink.find(tempnode).value()>>24) & 0x0f;
            if((currentState == LINK_START_YES_FEEDBACK_STATE) || (currentState == LINK_FEEDBACK_STATE))
            {
                qDebug()<<"in feedBackSlot(): current state is  feedback.do nothing.return2.";
                return;
            }
            else if( currentState == LINK_START_NO_FEEDBACK_STATE )
            {
                qDebug()<<"in feedBackSlot():deal feedback4...";
                //if(eventdialog->isActiveWindow())
                eventdialog->hide();
                this->show();//show main widget

                char shijianma = LINK_SHIJIAN_MA;
                char newState = LINK_START_YES_FEEDBACK_STATE;
                int rmRowNumber = hashLink.value(tempnode) & 0xffff;
                QString stringItem = linkList.at(rmRowNumber);
                stringItem.replace(17,8,datetime.toString("hh:mm:ss"));
                stringItem.replace(39,5,QString("启动已反馈"));


//                QString stringItemTemp = getStingItemFromDB(hostnum, iobrnum,loopnum,nodenum,shijianma,newState,nodetype,datetime);
//                QString qfTimeStr = "启";
//                qfTimeStr.append(linkList.at(rmRowNumber).mid(6,8));//启00:00:00
//                qfTimeStr.append(" 反");//启00:00:00 反
//                qfTimeStr.append(datetime.toString("hh:mm:ss"));//启00:00:00 反xx:xx:xx
//                QString stringItem = QString("%1 %2 %3")
//                        .arg(stringItemTemp.left(5))
//                        .arg(qfTimeStr)
//                        .arg(stringItemTemp.right(stringItemTemp.length()-15));

                linkList.removeAt(rmRowNumber);

                //遍历hashLink表，找出所有key值大于当前删除行号 的节点，对其key值进行-1操作. 删除对应<value,key>
                hashLink.erase(hashLink.find(tempnode));
                QHash<nodeID, int>::iterator iterP = hashLink.begin();
                while (iterP != hashLink.end())
                {
                    if( ( iterP.value() & 0xffff) > rmRowNumber )
                        iterP.value() -= 1;
                    ++iterP;
                }

                linkList.append(stringItem);
                hashLink.insert(tempnode,  (newState<<24) | (TI_FeedBack<<16) | (linkList.count()-1));
                linknumLabel->setText(tr("%1").arg(linkList.count()));
                emit closeWVSignal();
                ilink = linkList.count()-1;
                updateLinkWidgetSlot();

                busLinkAction(IS_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);
                if(  hashLinkageAction.contains(tempnode) )
                    hashLinkageAction.erase(hashLinkageAction.find(tempnode));

                linkAllNumber += 1;
                saveLinkToDB(iobrnum, loopnum, nodenum, nodetype, nodestate, datetime);
                if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_LINK_FLAG) & 0x01) )
                {
                    eventPrint(hostnum, iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
                }
            }
        }
    }
    dealLinkLed();
}

void MainWidget::feedBackRecoverSlot( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate, QDateTime datetime)
{
    int hostnum = 1;
    nodeID tempnode = nodeID(hostnum, iobrnum,loopnum,nodenum);
    if((nodetype != 0x09) && (nodetype != 0x40))
    {
        /* 对于非输入输出设备（只会是输入模块）
            如果哈希表中不包含节点——>不处理
            如果哈希表中包含节点，那么节点状态一定是“线路#1反馈”，找到这条信息，删除信息即可。 */
        if(!hashLink.contains(tempnode) )
        {
            qDebug()<<"in feedBackRecoverSlot():the node"<<iobrnum<<loopnum<<nodenum<<" is not in linkList,return1...";
            return;
        }
        else
        {
            qDebug()<<"in feedBackRecoverSlot(), do feedBackRecover1...";
            removeLinkItem(hostnum, iobrnum,loopnum,nodenum,nodestate,nodetype,datetime);
            busLinkAction(IS_NOT_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);
        }
    }
    else
    {
        /* B）对于输入输出设备
              如果哈希表中不包含节点——>不处理
             否则，哈希表中包含节点
             2.1）如果节点状态是 启动未反馈——>不处理
             2.2）如果节点状态是 启动已反馈——>显示 Q:xx:xx:xx 启动未反馈
             2.3）如果节点状态是 联动反馈——>删除这一条 */
        if(!hashLink.contains(tempnode) )
        {
            qDebug()<<"in feedBackRecoverSlot():the node "<<iobrnum<<loopnum<<nodenum<<"is not in linkList,return2...";
            return;
        }
        else
        {
            char currentState = (hashLink.find(tempnode).value()>>24) & 0x0f;
            if(currentState == LINK_START_NO_FEEDBACK_STATE)
            {
                qDebug()<<"in feedBackRecoverSlot():the node state is LINK_START_NO_FEEDBACK_STATE,return2...";
                return;
            }
            else if( currentState == LINK_FEEDBACK_STATE )
            {
                qDebug()<<"in feedBackRecoverSlot(), do feedBackRecover2...";
                removeLinkItem(hostnum, iobrnum,loopnum,nodenum,nodestate,nodetype,datetime);
                busLinkAction(IS_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);
            }
            else if( currentState == LINK_START_YES_FEEDBACK_STATE )
            {
                /* 单独处理直控盘：在启动已反馈的情况下收到反馈恢复，显示启动未反馈 */
                qDebug()<<"in feedBackRecoverSlot(), do feedBackRecover3...";
                /*当前状态是 启动已反馈------->显示 启动未反馈 */
                //if(eventdialog->isActiveWindow())
                eventdialog->hide();
                this->show();//show main widget

                int rmRowNumber = hashLink.value(tempnode) & 0xffff;
                char shijianma = LINK_SHIJIAN_MA;
                char newState = LINK_START_NO_FEEDBACK_STATE;
                QString stringItem = linkList.at(rmRowNumber);
                stringItem.replace(17,8,"--:--:--");
                stringItem.replace(39,5,QString("启动未反馈"));


                //                QString tempStringItem = getStingItemFromDB(hostnum, iobrnum,loopnum,nodenum,shijianma,newState,nodetype,datetime);
                //                QString qTimeStr = linkList.at(rmRowNumber).mid(8,8);//00:00:00
                //                QString rightStr = tempStringItem.right(tempStringItem.length() - 15);
                //                QString stringItem = QString("%1 %2 %3")
                //                        .arg(linkList.at(rmRowNumber).left(5)) //04-19
                //                        .arg(qTimeStr)
                //                        .arg(rightStr);

                linkList.removeAt(rmRowNumber);

                //遍历hashLink表，找出所有key值大于当前删除行号 的节点，对其key值进行-1操作. 删除对应<value,key>
                hashLink.erase(hashLink.find(tempnode));
                QHash<nodeID, int>::iterator iterP = hashLink.begin();
                while (iterP != hashLink.end())
                {
                    if( ( iterP.value() & 0xffff) > rmRowNumber )
                        iterP.value() -= 1;
                    ++iterP;
                }

                linkList.append(stringItem);
                hashLink.insert(tempnode, (newState<<24) | (TI_LinkageStop<<16) | (linkList.count()-1));
                linknumLabel->setText(tr("%1").arg(linkList.count()));
                emit closeWVSignal();
                ilink = linkList.count()-1;
                updateLinkWidgetSlot();

                busLinkAction(IS_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);
                if(  hashLinkageAction.contains(tempnode) )
                    hashLinkageAction.erase(hashLinkageAction.find(tempnode));

                linkAllNumber += 1;
                saveLinkToDB(iobrnum, loopnum, nodenum, nodetype, nodestate, datetime);
                if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_LINK_FLAG) & 0x01) )
                {
                    eventPrint(hostnum, iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
                }
            }
        }
    }
    dealLinkLedStop();
}

void MainWidget::linkageStartSlot( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate, QDateTime datetime)
{
    /* check hashFault , make sure if the coming fire has reported before */
    int hostnum = 1;
    nodeID tempnode = nodeID(hostnum, iobrnum,loopnum,nodenum);
    /*如果 1)hashLink中包含节点 并且 上报的事件类型跟保存的事件类型一样——>重报信息，不处理
           2)hashLink中不包含节点（此时就没有上报事件类型）——>显示，保存            */
    if((nodetype != 0x09) && (nodetype != 0x40))/*09:in-out; 40:zhikong;0x04:sheng-guang*/
    {
        if( hashLink.contains(tempnode) && isReReport(TI_LinkageStart,tempnode,nodestate) >= 0)
        {
            qDebug()<<"in linkageStartSlot(),link "<<iobrnum<<loopnum<<nodenum<<"RE-Report.return...";
            return;
        }
        else if( !hashLink.contains(tempnode) )
        {
            if( (hashLinkageAction.contains(tempnode) && ((hashLinkageAction.find(tempnode).value() & 0xff) == LINK_ACTION_START))
                    || (nodetype == 0x14)  /*|| (iobrnum == 34)*/ )//0x14:声光警报
            {
                qDebug()<<"in linkageStartSlot():deal linkage_start1...";
                //if(eventdialog->isActiveWindow())
                eventdialog->hide();
                this->show();//show main widget

                QString stringItem = getStingItemFromDB(hostnum, iobrnum,loopnum,nodenum,nodestate,nodetype,nodetype,datetime);

                linkAllNumber += 1;
                saveLinkToDB(iobrnum, loopnum, nodenum, nodetype, nodestate, datetime);
                if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_LINK_FLAG) & 0x01) )
                {
                    eventPrint(hostnum, iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
                }

                linkList.append(stringItem);
                hashLink.insert(tempnode, nodestate<<24 | (linkList.count()-1));
                emit eventNumberChangedSignal();
                linknumLabel->setText(tr("%1").arg(linkList.count()));
                emit closeWVSignal();

                ilink = linkList.count()-1;
                updateLinkWidgetSlot();

                busLinkAction(IS_NOT_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);
                hashLinkageAction.erase(hashLinkageAction.find(tempnode));
            }
            else
            {
                qDebug()<<"in linkageStartSlot():no LINK_ACTION_START send to node"<<iobrnum<<loopnum<<nodenum<<"return1...";
                return;
            }
        }
    }
    else
    {
        /* if the nodetype is 输入输出||直控点  0EFH 001H 001H 启动未反馈 */
        /*
            对于输入输出模块 以及 直控模块
            1）如果哈希表中不包含节点——>显示启动未反馈

        */

        if( !hashLink.contains(tempnode))
        {
            qDebug()<<"in linkageStartSlot():deal linkage_start 2...";

            //if(eventdialog->isActiveWindow())
            eventdialog->hide();
            this->show();//show main widget

            char shijianma = LINK_SHIJIAN_MA;
            char newState = LINK_START_NO_FEEDBACK_STATE;
            QString stringItem = getStingItemFromDB(hostnum, iobrnum,loopnum,nodenum,shijianma,newState,nodetype,datetime);
            stringItem.insert(6,QString("启"));
            stringItem.insert(16,QString("反--:--:-- "));

            linkList.append(stringItem);
            hashLink.insert(tempnode, (newState<<24) | (TI_LinkageStart<<16) | (linkList.count()-1));
            emit eventNumberChangedSignal();

            linknumLabel->setText(tr("%1").arg(linkList.count()));
            emit closeWVSignal();

            ilink = linkList.count()-1;
            updateLinkWidgetSlot();

            busLinkAction(IS_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);

            linkAllNumber += 1;
            saveLinkToDB(iobrnum, loopnum, nodenum, nodetype, nodestate, datetime);
            if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_LINK_FLAG) & 0x01) )
            {
                eventPrint(hostnum, iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
            }
        }
        else
        {
            /*
            2）如果包含节点 && (the nodetype is 输入输出||直控点 )
                 2.1）如果节点状态为 启动已反馈 或者 启动未反馈 不处理
                 2.2）如果节点状态为 联动反馈 那么显示启动已反馈
            */

            char currentState = (hashLink.find(tempnode).value()>>24) & 0x0f;
            if( (currentState == LINK_START_YES_FEEDBACK_STATE) || (currentState == LINK_START_NO_FEEDBACK_STATE))
            {
                qDebug()<<"in linkageStartSlot(): current state is started.do nothing.return.";
                return;
            }
            else if(currentState == LINK_FEEDBACK_STATE)
            {
                qDebug()<<"in linkageStartSlot():deal linkage_start 3...";

                //if(eventdialog->isActiveWindow())
                eventdialog->hide();
                this->show();//show main widget

                char shijianma = LINK_SHIJIAN_MA;
                char newState = LINK_START_YES_FEEDBACK_STATE;
                int rmRowNumber = hashLink.value(tempnode) & 0xffff;
                QString stringItem = linkList.at(rmRowNumber);
                stringItem.replace(7,8,datetime.toString("hh:mm:ss"));
                stringItem.replace(39,4,QString("启动已反馈"));

                //                QString stringItemTemp = getStingItemFromDB(hostnum, iobrnum,loopnum,nodenum,shijianma,newState,nodetype,datetime);
                //                QString fTimeStr = stringItemTemp.mid(6,8);//xx:xx:xx
                //                QString qfTimeStr = "Q:";
                //                qfTimeStr.append(datetime.toString("hh:mm:ss"));//Q:00:00:00
                //                qfTimeStr.append(" F:");//Q:00:00:00 F:
                //                qfTimeStr.append(fTimeStr);//Q:00:00:00 F:xx:xx:xx
                //                QString stringItem = QString("%1 %2 %3 %4")
                //                        .arg(stringItemTemp.left(5))
                //                        .arg(qfTimeStr)
                //                        .arg(stringItemTemp.mid(15,22))//01-01-01-001 联动反馈 输入输出
                //                        .arg(stringItemTemp.right(stringItemTemp.length()-47));

                linkList.removeAt(rmRowNumber);

                //遍历hashLink表，找出所有key值大于当前删除行号 的节点，对其key值进行-1操作. 删除对应<value,key>
                hashLink.erase(hashLink.find(tempnode));
                QHash<nodeID, int>::iterator iterP = hashLink.begin();
                while (iterP != hashLink.end())
                {
                    if( ( iterP.value() & 0xffff) > rmRowNumber )
                        iterP.value() -= 1;
                    ++iterP;
                }

                linkList.append(stringItem);
                hashLink.insert(tempnode,  (newState<<24) | (TI_FeedBack<<16) | (linkList.count()-1));
                linknumLabel->setText(tr("%1").arg(linkList.count()));
                emit closeWVSignal();
                ilink = linkList.count()-1;
                updateLinkWidgetSlot();

                busLinkAction(IS_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);
                if(  hashLinkageAction.contains(tempnode) )
                    hashLinkageAction.erase(hashLinkageAction.find(tempnode));

                linkAllNumber += 1;
                saveLinkToDB(iobrnum, loopnum, nodenum, nodetype, nodestate, datetime);
                if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_LINK_FLAG) & 0x01) )
                {
                    eventPrint(hostnum, iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
                }
            }
        }
    }
    dealLinkLed();
}

void MainWidget::linkageStopSlot( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate, QDateTime datetime)
{
    /* check hashFault , make sure if the coming fire has reported before */
    int hostnum = 1;
    nodeID tempnode = nodeID(hostnum, iobrnum,loopnum,nodenum);
    if((nodetype != 0x09) && (nodetype != 0x40))
    {
        /*
        A）对于非输入输出设备或者直控（只会是输出模块，上报的状态只有启动 和 停止）
        1）如果hashLink中不包含节点——>不处理
        2）如果hashLink中包含节点，那么，节点的状态一定是启动状态。删除信息即可。
        */
        if(!hashLink.contains(tempnode) )
        {
            qDebug()<<"in linkageStopSlot():the node"<<iobrnum<<loopnum<<nodenum<<"is NOT-START,return1...";
            return;
        }
        else
        {
            qDebug()<<"in linkageStopSlot(), do LINK--STOP1...";
            removeLinkItem(hostnum, iobrnum,loopnum,nodenum,nodestate,nodetype,datetime);
            busLinkAction(IS_NOT_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);
        }
    }
    else
    { /* B）对于输入输出设备 或者 直控设备
             1）如果hashLink中不包含节点

        */
        if ( !hashLink.contains(tempnode))
        {
            qDebug()<<"in linkageStopSlot():the node "<<iobrnum<<loopnum<<nodenum<<"is not in the list,return2...";
            return;
        }
        else
        {  /*
            2）如果hashLink中包含节点
                  2.1) 节点状态是非启动状态（也就是联动反馈状态），那么不处理
                  2.2）节点状态为 启动未反馈——>删除这一条，并将联动停止的信息存入数据库
                  2.3）节点状态为 启动已反馈 ，找到这一条，将状态更新为联动反馈，将联动停止信息存入数据库
            */
            char currentState = (hashLink.find(tempnode).value()>>24) & 0x0f;
            if(currentState == LINK_FEEDBACK_STATE )
            {

                qDebug()<<"in linkageStopSlot():the node"<<iobrnum<<loopnum<<nodenum<<" is no-start,return3...";
                return;
            }
            else if(currentState == LINK_START_NO_FEEDBACK_STATE)
            {
                qDebug()<<"in linkageStopSlot(), do LINK--STOP2...";
                removeLinkItem(hostnum, iobrnum,loopnum,nodenum,nodestate,nodetype,datetime);
                busLinkAction(IS_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);
            }
            else if(currentState == LINK_START_YES_FEEDBACK_STATE)
            {
                qDebug()<<"in linkageStopSlot(), do LINK--STOP3...";
                /*当前状态是 启动已反馈------->显示 联动反馈 */
                //if(eventdialog->isActiveWindow())
                eventdialog->hide();
                this->show();//show main widget

                int rmRowNumber = hashLink.value(tempnode) & 0xffff;
                char shijianma = LINK_SHIJIAN_MA;
                char newState = LINK_FEEDBACK_STATE;
                QString stringItem = linkList.at(rmRowNumber);
                stringItem.replace(7,8,QString("--:--:--"));
                stringItem.replace(39,5,QString("联动反馈"));

                //                QString tempStringItem = getStingItemFromDB(hostnum, iobrnum,loopnum,nodenum,shijianma,newState,nodetype,datetime);
                //                QString fTimeStr = linkList.at(rmRowNumber).mid(19,8);//xx:xx:xx
                //                QString rightStr = tempStringItem.right(tempStringItem.length() - 15);
                //                QString dtStr = linkList.at(rmRowNumber).left(5);//04-19
                //                QString stringItem = QString("%1 %2 %3")
                //                        .arg(dtStr)
                //                        .arg(fTimeStr)
                //                        .arg(rightStr);

                linkList.removeAt(rmRowNumber);

                //遍历hashLink表，找出所有key值大于当前删除行号 的节点，对其key值进行-1操作. 删除对应<value,key>
                hashLink.erase(hashLink.find(tempnode));
                QHash<nodeID, int>::iterator iterP = hashLink.begin();
                while (iterP != hashLink.end())
                {
                    if( ( iterP.value() & 0xffff) > rmRowNumber )
                        iterP.value() -= 1;
                    ++iterP;
                }

                linkList.append(stringItem);
                hashLink.insert(tempnode, (newState<<24) | (TI_LinkageStop<<16) | (linkList.count()-1));
                linknumLabel->setText(tr("%1").arg(linkList.count()));
                emit closeWVSignal();

                ilink = linkList.count()-1;
                updateLinkWidgetSlot();

                busLinkAction(IS_IO_FLAG, hostnum, iobrnum, loopnum, nodenum);

                linkAllNumber += 1;
                saveLinkToDB(iobrnum, loopnum, nodenum, nodetype, nodestate, datetime);
                if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_LINK_FLAG) & 0x01) )
                {
                    eventPrint(hostnum, iobrnum, loopnum, nodenum, nodetype,nodestate, datetime);
                }
            }
        }
    }
    dealLinkLedStop();
}

void MainWidget::updateFireWidgetSlot()
{
    for(int i = 0; i < fireList.count() - 1; i++)
    {
        QString strItem = QString("%1 %2").arg(i+2).arg(fireList.at(i+1));
        fireTableWidget->setItem(i, 0, new QTableWidgetItem(strItem));
    }
    fireTableWidget->selectRow(ifire);
    //qDebug()<<"________________ifire:"<<ifire;
    ifire++;
    if( ifire > (fireList.count()-2))
        ifire = 0;
}

void MainWidget::updateLinkWidgetSlot()
{
    linkTableWidget->clear();
    for(int i = 0; i < linkList.count(); i++)
    {
        QString strItem = QString("%1 %2").arg(i+1).arg(linkList.at(i));
        linkTableWidget->setItem(i, 0, new QTableWidgetItem(strItem));
    }
    linkTableWidget->selectRow(ilink);
    ilink++;
    if( ilink > linkList.count() - 1)
        ilink = 0;
}

void MainWidget::actionFailedSlot( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate, QDateTime datetime)
{
    qDebug()<<"action FAILED !"<<"the node is "<<"iobrnum :"<<iobrnum <<"loopnum:"<<loopnum<<"nodenum:"<< nodenum<<"nodetype:"<< nodetype<<"nodestate:"<< nodestate<<"datetime:"<<datetime;
}


void MainWidget::KeyInfSlot(int boardnum,int buspanlnum,int busnodenum, unsigned char nodestate, QDateTime datatime)
{
    //总线盘 buspanlnum 就是总线盘号1~64，busnodenum就是总线点号1~64,nodestate 是编址单元状态	启动＝0、停止＝1
    //queryBusset(iobrnum, loopnum, nodenum, nodestate);
    //LINK_ACTION_START 0x01  LINK_ACTION_STOP 0x02

    OperationHis::Update(QString("%1号总线盘%2号点按下").arg(buspanlnum).arg(busnodenum));
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from busset WHERE bus_num = %1 AND bus_node = %2").arg(buspanlnum).arg(busnodenum));
    if (query.next())
    {
        qDebug()<<"in KeyInfSlot():select from busset where busnum ="<<buspanlnum<<"busnode = "<<busnodenum;
        for(int i = 0;i < 1; i++)
        {
            int k = query.value(i + 18).toInt(); //查询数据库，找到按键对应的输出设备
            if (k)
            {
                int hostnum = 1;
                int iobrnum = (k >> 16) & 0xff;
                int loopnum = (k >> 8) & 0xff;
                int nodenum = k & 0xff;
                nodeID tempnode = nodeID(hostnum, iobrnum, loopnum, nodenum);
                nodeID linknode = nodeID(hostnum, iobrnum, loopnum, nodenum,nodestate+1);
                emit nodeEnqueueSignal(LINK_NODE_ENQUEUE,linknode);
                qDebug()<<"in KeyInfSlot():emit nodeEnqueueSignal(LINK_NODE_ENQUEUE";
                if( !hashLinkageAction.contains(tempnode))
                    hashLinkageAction.insert(tempnode,nodestate+1);
                else
                {
                    QHash<nodeID, int>::iterator iterP = hashLinkageAction.find(tempnode);
                    iterP.value() =  nodestate+1;
                }
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        qDebug()<<"in mainwidget::KeyInfSlot(): current node is not in busset database.";
    }
    QSqlDatabase::database().commit();
}

void MainWidget::busLinkAction(int isIOFlag, int hostnum, int iobrnum, int loopnum, int nodenum)
{
    //接收到设备状态改变，通过设备查找按键，然后在总线盘上点灯！！！
    int nodevalue = hostnum<<24 | iobrnum<<16 | loopnum<<8 | nodenum;
    nodeID tempnode = nodeID(hostnum,  iobrnum,  loopnum,  nodenum);
    int buspanelNum = 0;
    int busnodeNum = 0;
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from busset"));//遍历busset数据库，通过设备查找按键
    while (query.next())
    {
        buspanelNum = query.value(1).toInt();
        busnodeNum = query.value(2).toInt();

        for(int i = 0; i < 1; i++)
        {
            int k = query.value(i+3).toInt();//for input node
            if( k )
            {
                if( k == nodevalue )
                {
                    if(hashLink.contains(tempnode))
                    {
                        char currentState = (hashLink.find(tempnode).value()>>24) & 0xff;
                        switch(currentState)
                        {
                        /*#define NO_STATE 0x0
                                #define LINK_START_NO_FEEDBACK_STATE 0x01
                                #define LINK_START_YES_FEEDBACK_STATE 0x02
                                #define LINK_FEEDBACK_STATE 0x03*/
                        case NO_STATE:
                        {
                            nodeID  bustempnode1 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum, LED_ACTION_PUT_OUT, BUS_LED_OF_FEEDBACK);
                            emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode1);
                        }
                            break;
                        case LINK_START_NO_FEEDBACK_STATE:
                        case 0x0b://启动
                        {
                            nodeID  bustempnode1 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum, LED_ACTION_PUT_OUT, BUS_LED_OF_FEEDBACK);
                            emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode1);
                        }
                            break;
                        case LINK_START_YES_FEEDBACK_STATE:
                        {
                            nodeID  bustempnode1 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum,LED_ACTION_PUT_ON, BUS_LED_OF_FEEDBACK);
                            emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode1);
                        }
                            break;
                        case LINK_FEEDBACK_STATE:
                        case 0x55://反馈
                        {
                            nodeID  bustempnode1 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum,LED_ACTION_PUT_ON, BUS_LED_OF_FEEDBACK);
                            emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode1);
                        }
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {
                        nodeID  bustempnode1 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum,LED_ACTION_PUT_OUT, BUS_LED_OF_FEEDBACK);
                        nodeID  bustempnode2 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum, LED_ACTION_PUT_OUT, BUS_LED_OF_START);
                        emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode1);
                        emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode2);
                    }
                }
            }
            else
                break;
        }

        for(int j = 0; j < 1; j++)
        {
            int k =  query.value(j+18).toInt();
            if( k )
            {
                if( k == nodevalue )
                {
                    if(hashLink.contains(tempnode))
                    {
                        char currentState = (hashLink.find(tempnode).value()>>24) & 0x0f;
                        switch(currentState)
                        {
                        /*#define NO_STATE 0x0
                            #define LINK_START_NO_FEEDBACK_STATE 0x01
                            #define LINK_START_YES_FEEDBACK_STATE 0x02
                            #define LINK_FEEDBACK_STATE 0x03*/
                        case NO_STATE:
                        {
                            nodeID  bustempnode2 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum, LED_ACTION_PUT_OUT, BUS_LED_OF_START);
                            emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode2);
                        }
                            break;
                        case LINK_START_NO_FEEDBACK_STATE:
                        case 0x0b://启动
                        {
                            nodeID  bustempnode2 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum, LED_ACTION_PUT_ON, BUS_LED_OF_START);
                            emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode2);
                            nodeID  bustempnode3 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum, LED_ACTION_FLICK, BUS_LED_OF_START);
                            emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode3);
                        }
                            break;
                        case LINK_START_YES_FEEDBACK_STATE:
                        {
                            nodeID  bustempnode2 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum, LED_ACTION_PUT_ON, BUS_LED_OF_START);
                            emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode2);
                        }
                            break;
                        case LINK_FEEDBACK_STATE:
                        case 0x55://反馈
                        {
                            nodeID  bustempnode2 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum, LED_ACTION_PUT_OUT, BUS_LED_OF_START);
                            emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode2);
                        }
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {
                        nodeID  bustempnode1 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum,LED_ACTION_PUT_OUT, BUS_LED_OF_FEEDBACK);
                        nodeID  bustempnode2 = nodeID(1,ZHIKONGZONGXIAN,buspanelNum, busnodeNum, LED_ACTION_PUT_OUT, BUS_LED_OF_START);
                        emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode1);
                        emit nodeEnqueueSignal(BUS_NODE_ENQUEUE, bustempnode2);
                    }
                }
            }
            else
                break;
        }
    }
    QSqlDatabase::database().commit();

}

void MainWidget::saveFireToDB( int nodestate, int nodetype,int iobrnum, int loopnum, int nodenum,
                              int bldnum , int flrnum, int zonenum , int roomnum, QDateTime datetime, const QString &str)
{
    int host = 1;
    //    int firehisSize = 0;
    //    QSqlDatabase::database().transaction();
    //    QSqlQuery query1;
    //    query1.setForwardOnly(true);

    //    query1.exec("select * from firehis");
    //    while (query1.next())
    //    {
    //        firehisSize++;
    //    }
    //    QSqlDatabase::database().commit();
    //ADD
    /*Before the opening of each transaction，adding this code to improve the speed*/
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO firehis(id, type, infoid, geo_addr, time, str)"
                  "VALUES (?, ?, ?, ?, ?, ?)");

    //    query.addBindValue(firehisSize + 1);

    query.addBindValue(fireMaxId + fireAllNumber);
    query.addBindValue((nodestate<<16) | (nodetype << 8));
    query.addBindValue((host<<24) | (iobrnum<<16) | (loopnum<<8) | nodenum);
    query.addBindValue((bldnum<<24) | (flrnum<<16) | (zonenum<<8) | roomnum);
    query.addBindValue(datetime);
    query.addBindValue(str);

    bool bSuccess = query.exec();
    if (!bSuccess)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("insert firehis error"));
    }

    //    if((firehisSize) + 1 > HIS_TABLE_ROWS)
    if((fireMaxId + fireAllNumber) > HIS_TABLE_ROWS)
    {
        int fireMinId = 0;
        QSqlQuery query3;
        query3.setForwardOnly(true);

        query3.exec("select MIN(id) from firehis");
        while (query3.next())
        {
            fireMinId = query3.value(0).toInt();
        }

        QSqlQuery query2;
        query2.exec(QString("delete  from firehis where id = %1").arg(fireMinId));
        bool bSuccess = query2.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query2.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("delete fire error"));
        }
    }
    /*Close the transaction*/
    QSqlDatabase::database().commit();
}

void MainWidget::saveFaultToDB( int nodestate,int nodetype, int iobrnum, int loopnum, int nodenum,
                               int bldnum , int flrnum, int zonenum , int roomnum, QDateTime datetime, const QString &str)
{
    int host = 1;
    //    int faulthisSize = 0;
    //    QSqlDatabase::database().transaction();
    //    QSqlQuery query1;
    //    query1.setForwardOnly(true);

    //    query1.exec("select * from faulthis");
    //    while (query1.next())
    //    {
    //        faulthisSize++;
    //    }
    //    QSqlDatabase::database().commit();
    //    //ADD
    /*Before the opening of each transaction，adding this code to improve the speed*/
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO faulthis(id, type, infoid, geo_addr, time, str)"
                  "VALUES (?, ?, ?, ?, ?, ?)");

    //    query.addBindValue(faulthisSize + 1);
    query.addBindValue(faultMaxId+faultAllNumber);

    query.addBindValue((nodestate<<16) | (nodetype << 8));
    query.addBindValue((host<<24) | (iobrnum<<16) | (loopnum<<8) | nodenum);
    query.addBindValue((bldnum<<24) | (flrnum<<16) | (zonenum<<8) | roomnum);
    query.addBindValue(datetime);
    query.addBindValue(str);

    bool bSuccess = query.exec();
    if (!bSuccess)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("insert faulthis error"));
    }

    if((faultMaxId+faultAllNumber) > HIS_TABLE_ROWS)
    {
        int faultMinId = 0;
        QSqlQuery query3;
        query3.setForwardOnly(true);

        query3.exec("select MIN(id) from faulthis");
        while (query3.next())
        {
            faultMinId = query3.value(0).toInt();
        }

        QSqlQuery query2;
        query2.exec(QString("delete  from faulthis where id = %1").arg(faultMinId));
        bool bSuccess = query2.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query2.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("delete fault error"));
        }

    }

    /*Close the transaction*/
    QSqlDatabase::database().commit();
}



//void MainWidget::saveLinkToDB( int nodestate, int nodetype,int iobrnum,int loopnum,int nodenum,
//                              int bldnum ,int flrnum,int zonenum ,int roomnum,QDateTime datetime, const QString &str)

void MainWidget::saveLinkToDB(int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate, QDateTime datetime)
{
    int hostnum = 1;
    QSqlDatabase::database().transaction();
    QSqlQuery query1;
    query1.setForwardOnly(true);
    query1.exec(QString("select * from unitset WHERE unitid == %1")
                .arg((hostnum << 24) | (iobrnum << 16) | (loopnum << 8) | nodenum));
    if (query1.next())
    {
        int bldnum = (query1.value(6).toInt() >> 24) & 0xFF;
        int flrnum = (query1.value(6).toInt() >> 16) & 0xFF;
        int zonenum = (query1.value(6).toInt() >> 8) & 0xFF;
        int roomnum = query1.value(6).toInt() & 0xFF;
        QString nodestr = query1.value(9).toString();


        QSqlQuery query;
        query.prepare("INSERT INTO linkhis(id, type, infoid, geo_addr, time, str)"
                      "VALUES (?, ?, ?, ?, ?, ?)");
        //   query.addBindValue(linkhisSize + 1);
        query.addBindValue(linkMaxId+linkAllNumber);
        query.addBindValue((nodestate<<16) | (nodetype << 8));
        query.addBindValue((hostnum<<24) | (iobrnum<<16) | (loopnum<<8) | nodenum);
        query.addBindValue((bldnum<<24) | (flrnum<<16) | (zonenum<<8) | roomnum);
        query.addBindValue(datetime);
        query.addBindValue(nodestr);

        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("insert linkhis error"));
        }

        if((linkMaxId+linkAllNumber) > HIS_TABLE_ROWS)
        {
            int linkMinId = 0;
            QSqlQuery query3;
            query3.setForwardOnly(true);

            query3.exec("select MIN(id) from linkhis");
            while (query3.next())
            {
                linkMinId = query3.value(0).toInt();
            }

            QSqlQuery query2;
            query2.exec(QString("delete  from linkhis where id = %1").arg(linkMinId));
            bool bSuccess = query2.exec();
            if (!bSuccess)
            {
                QSqlError lastError = query2.lastError();
                qDebug() << lastError.driverText()
                         <<QString(QObject::tr("delete fire error"));
            }
        }
    }

    /*Close the transaction*/
    QSqlDatabase::database().commit();

    //    int linkhisSize = 0;
    //    QSqlDatabase::database().transaction();
    //    QSqlQuery query1;
    //    query1.setForwardOnly(true);

    //    query1.exec("select * from linkhis");
    //    while (query1.next())
    //    {
    //        linkhisSize++;
    //    }
    //    QSqlDatabase::database().commit();
    //ADD


}


void MainWidget::createActions()
{
    /*hide or show*/
    hideAction = new QAction (this);
    hideAction->setShortcut (/*Qt::CTRL+*/Qt::Key_F2);
    secondhideAction = new QAction (this);
    secondhideAction->setShortcut (/*Qt::CTRL+*/Qt::Key_F3);
    escapeAction = new QAction (this);
    escapeAction->setShortcut (/*Qt::CTRL+*/Qt::Key_Escape);
    connect(hideAction, SIGNAL(triggered()), this, SLOT(hideWindow()));

    upKeyAction = new QAction(this);
    upKeyAction->setShortcut (Qt::Key_Up);
    downKeyAction = new QAction(this);
    downKeyAction->setShortcut (Qt::Key_Down);
    shieldAction = new QAction(this);
    shieldAction->setShortcut (/*Qt::CTRL+*/Qt::Key_F4);
    connect(shieldAction, SIGNAL(triggered()), this, SLOT(creatShieldWidget()));

    connect(secondhideAction, SIGNAL(triggered()), this, SLOT(secondhideWindow()));
    connect(escapeAction, SIGNAL(triggered()), this, SLOT(escWindow()));

    connect(upKeyAction, SIGNAL(triggered()), this, SLOT(upKeySlot()));
    connect(downKeyAction, SIGNAL(triggered()), this, SLOT(downKeySlot()));

    connect(this, SIGNAL(hidemainshowview()), this, SLOT(secondhideWindow()));
    connect(this, SIGNAL(hidemainshowset()), this, SLOT(thirdhideWindow()));

    this->addAction(hideAction);
    this->addAction(secondhideAction);
    this->addAction(escapeAction);
    this->addAction(upKeyAction);
    this->addAction(downKeyAction);
    this->addAction(shieldAction);
}

void MainWidget::upKeySlot()
{
    QTableWidget *focusWidget= qobject_cast<QTableWidget*>(this->focusWidget());
    focusWidget->selectRow( focusWidget->currentRow() - 1 );

    ifire = fireTableWidget->currentRow();
    ilink = linkTableWidget->currentRow();

    //    if(fireTableWidget->isActiveWindow())
    //    {
    //        qDebug()<<"________________ Key_Up for fire.";
    //        fireTableWidget->selectRow( fireTableWidget->currentRow() - 1 );
    //        ifire = fireTableWidget->currentRow();
    //        if(ifire > fireList.count())
    //            ifire = fireList.count()-2;
    //    }
    //    if(linkTableWidget->isActiveWindow())
    //    {
    //        qDebug()<<"________________ Key_Up for link";
    //        linkTableWidget->selectRow( linkTableWidget->currentRow() - 1 );
    //        ilink = linkTableWidget->currentRow();
    //        if(ilink > linkList.count())
    //            ilink = linkList.count()-1;
    //    }
    timer10s->start();
    timer3s->stop();
}
void MainWidget::downKeySlot()
{
    //if(fireTableWidget->isActiveWindow())
    //QWidget *current_focus_widget;
    //current_focus_widget = this->focusWidget();// QApplication::focusWidget();

    QTableWidget *focusWidget= qobject_cast<QTableWidget*>(this->focusWidget());
    focusWidget->selectRow( focusWidget->currentRow() + 1 );

    //qDebug()<<"________________ Key_Down for fire";

    ifire = fireTableWidget->currentRow();
    if(ifire > fireList.count())
        ifire = 0;

    ilink = linkTableWidget->currentRow();
    if(ilink > linkList.count())
        ilink = 0;

    //        qDebug()<<"________________ Key_Down for fire";
    //        fireTableWidget->selectRow( fireTableWidget->currentRow() + 1 );
    //        ifire = fireTableWidget->currentRow();
    //        if(ifire > fireList.count())
    //            ifire = fireList.count()-2;

    //    if(linkTableWidget->isActiveWindow())
    //    {
    //        qDebug()<<"________________ Key_Down for link";
    //        linkTableWidget->selectRow( linkTableWidget->currentRow() + 1 );
    //        ilink = linkTableWidget->currentRow();
    //        if(ilink > linkList.count())
    //            ilink = linkList.count()-1;
    //    }
    timer10s->start();
    timer3s->stop();

}
void MainWidget::escWindow()
{
    //do nothing here.
    qDebug()<<"________________Key_Escape.";
}

void MainWidget::creatShieldWidget()
{
    qDebug()<<"________________Key_F4.";
    PasswordDialog password(4, this);
    password.exec();
    if (password.matchFlag)
    {
        OperationHis::Update("屏蔽设置");
        ShieldWidget shield(w);
        shield.exec();

    }
}


void MainWidget::createItems()
{
    fireAllNumber = 0;
    faultAllNumber = 0;
    linkAllNumber = 0;

    printerFlagOfWidget = 0x01010101;//default setting is print-all-event
    sysResetFlag = 0;//it is the start,not restart.

    linkFeedBackFlag = 0;
    oneEventShowFlag = 0;

    //    linkStateList.append(QString(""));
    //    linkStateList.append(QString("启动未反馈"));
    //    linkStateList.append(QString("启动已反馈"));
    //    linkStateList.append(QString("联动反馈"));
    //    for(int i = 0;i<linkStateList.count();i++)
    //        qDebug()<<"in MainWidget::createItems():linkStateList"<<i<<"is"<<linkStateList.at(i);

    fireTipLabel = new QLabel(tr("火警  共："),this);
    fireTipLabel->setGeometry(QRect(5,1,80,20));
    firenumLabel = new QLabel(tr("000"),this);
    firenumLabel->setGeometry(QRect(90,1,30,20));
    firstFireTableWidget = new QTableWidget(1,1,this);
    firstFireTableWidget->setGeometry(QRect(0,21,800,23));//QRect(0,25,800,20)
    firstFireTableWidget->horizontalHeader()->setStretchLastSection(true);
    firstFireTableWidget->verticalHeader()->setDefaultSectionSize(22);//set the verticalHeader size is 24
    firstFireTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    firstFireTableWidget->horizontalHeader()->setVisible(false);

    fireTableWidget = new QTableWidget(LISTWIDGET_ROW,LISTWIDGET_COLUMN,this);
    fireTableWidget->setGeometry(QRect(0,45,800,141));
    fireTableWidget->horizontalHeader()->setStretchLastSection(true);
    fireTableWidget->verticalHeader()->setDefaultSectionSize(20);//set the verticalHeader size is 24
    fireTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    fireTableWidget->horizontalHeader()->setVisible(false);
    fireTableWidget->verticalHeader()->setVisible(false);//setHidden(true);//set the verticalHeader hidden
    //fireTableWidget->setRowHidden(0,true);
    //fireTableWidget->addItem(tr("002 复合<>火警 复合探头 12-04-19 10:08 02/003/09  此处为汉字标签——总共十六个汉字     646420255"));
    //set firstfire tablewidget  header width to align at  firetablewidget...
    //firstFireTableWidget->verticalHeader()->setMinimumWidth(fireTableWidget->verticalHeader()->width());
    //fireTableWidget->verticalHeader()->setMinimumHeight(20);


    linkTipLabel = new QLabel(tr("联动  共："),this);
    linkTipLabel->setGeometry(QRect(5,186,80,20));
    linknumLabel = new QLabel(tr("000"),this);
    linknumLabel->setGeometry(QRect(90,186,30,20));
    linkTableWidget = new QTableWidget(LISTWIDGET_ROW_OF_FIRE - 1,LISTWIDGET_COLUMN,this);//LISTWIDGET_ROW_OF_FIRE - 1
    linkTableWidget->setGeometry(QRect(0,206,800,141));
    linkTableWidget->horizontalHeader()->setStretchLastSection(true);
    linkTableWidget->verticalHeader()->setDefaultSectionSize(20);//set the verticalHeader size is 24
    linkTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    linkTableWidget->horizontalHeader()->setVisible(false);
    linkTableWidget->verticalHeader()->setVisible(false);

    //superviseTipLabel = new QLabel(tr("监管  共："),this);
    //superviseTipLabel->setGeometry(QRect(5,325,80,20));
    //supervisenumLabel = new QLabel(tr("000"),this);
    //supervisenumLabel->setGeometry(QRect(90,325,30,20));

    //add by linwotian , add eventTabwidget to show the other events
    //superviseTipLabel->setVisible(false);
    //supervisenumLabel->setVisible(false);

    eventTabWidget = new QTabWidget(this);
    eventTabWidget->setGeometry(QRect(0,347,800,180));

    superviseTableWidget = new QTableWidget(LISTWIDGET_ROW,LISTWIDGET_COLUMN,this);
    superviseTableWidget->horizontalHeader()->setStretchLastSection(true);
    superviseTableWidget->verticalHeader()->setDefaultSectionSize(20);//set the verticalHeader size is 24
    superviseTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    superviseTableWidget->horizontalHeader()->setVisible(false);
    superviseTableWidget->verticalHeader()->setVisible(false);
    superviseIndex = eventTabWidget->addTab(superviseTableWidget,tr("监管"));

    faultTableWidget = new QTableWidget(LISTWIDGET_ROW,LISTWIDGET_COLUMN,this);
    faultTableWidget->horizontalHeader()->setStretchLastSection(true);
    faultTableWidget->verticalHeader()->setDefaultSectionSize(20);//set the verticalHeader size is 24
    faultTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    faultTableWidget->horizontalHeader()->setVisible(false);
    faultTableWidget->verticalHeader()->setVisible(false);
    faultIndex = eventTabWidget->addTab(faultTableWidget,tr("故障"));

    delayTableWidget = new QTableWidget(LISTWIDGET_ROW,LISTWIDGET_COLUMN,this);
    delayTableWidget->horizontalHeader()->setStretchLastSection(true);
    delayTableWidget->verticalHeader()->setDefaultSectionSize(20);//set the verticalHeader size is 24
    delayTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    delayTableWidget->horizontalHeader()->setVisible(false);
    delayTableWidget->verticalHeader()->setVisible(false);
    delayIndex = eventTabWidget->addTab(delayTableWidget,tr("延时"));


    shieldTableWidget = new QTableWidget(LISTWIDGET_ROW,LISTWIDGET_COLUMN,this);
    shieldTableWidget->horizontalHeader()->setStretchLastSection(true);
    shieldTableWidget->verticalHeader()->setDefaultSectionSize(20);//set the verticalHeader size is 24
    shieldTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    shieldTableWidget->horizontalHeader()->setVisible(false);
    shieldIndex = eventTabWidget->addTab(shieldTableWidget,tr("屏蔽"));


    handWidget = new HandWidget(this);
    handIndex = eventTabWidget->addTab(handWidget,tr("手动"));

    //    fireLabel = new QLabel(this);
    //    linkLabel = new QLabel(this);
    //    fire_linkLabel = new QLabel(this);
    superviseLabel = new QLabel(this);
    faultLabel = new QLabel(this);
    delayLabel = new QLabel(this);
    shieldLabel = new QLabel(this);
    //hand_automaticLabel = new QLabel(this);

    //    fireButton = new QPushButton (tr("火警"),this);
    //    fireButton->setGeometry(QRect(10, 465, LABEL_WIDTH, LABEL_HEIGHT));
    //    fireLabel->setGeometry(QRect(115, 465, 80, 24));//x plot+105 of button
    //    connect(fireButton, SIGNAL(clicked()), this, SLOT(fireEventDialog()));

    //    linkButton = new QPushButton (tr("联动"),this);
    //    linkButton->setGeometry(QRect(200, 465,  LABEL_WIDTH, LABEL_HEIGHT));
    //    linkLabel->setGeometry(QRect(315, 465, 80, 24));
    //    connect(linkButton, SIGNAL(clicked()), this, SLOT(linkEventDialog()));

    superviseButton = new QPushButton (tr("监管"),this);
    superviseButton->setGeometry(QRect(230, 540,  LABEL_WIDTH, LABEL_HEIGHT));
    superviseLabel->setGeometry(QRect(230+60, 540, 40, 24));
    connect(superviseButton, SIGNAL(clicked()), this, SLOT(superviseEventDialog()));

    faultButton = new QPushButton (tr("故障"),this);
    faultButton->setGeometry(QRect(10, 540,  LABEL_WIDTH, LABEL_HEIGHT));
    faultLabel->setGeometry(QRect(10+60, 540, 40, 24));
    connect(faultButton, SIGNAL(clicked()), this, SLOT(faultEventDialog()));

    delayButton = new QPushButton (tr("延时"),this);
    delayButton->setGeometry(QRect(120, 540,  LABEL_WIDTH, LABEL_HEIGHT));
    delayLabel->setGeometry(QRect(120+60, 540, 40, 24));
    connect(delayButton, SIGNAL(clicked()), this, SLOT(delayEventDialog()));

    shieldButton = new QPushButton (tr("屏蔽"),this);
    shieldButton->setGeometry(QRect(340, 540,  LABEL_WIDTH, LABEL_HEIGHT));
    shieldLabel->setGeometry(QRect(340+60, 540, 40, 24));
    connect(shieldButton, SIGNAL(clicked()), this, SLOT(shieldEventDialog()));

    //    fire_linkButton = new QPushButton (tr("火警 联动"),this);
    //    fire_linkButton->setGeometry(QRect(390, 500, 100, 24));
    //    fire_linkLabel->setGeometry(QRect(390+115, 500, 80, 24));
    //    connect(fire_linkButton, SIGNAL(clicked()), this, SLOT(fire_linkEventDialog()));

    hand_automaticButton = new QPushButton (tr("手动"),this);
    hand_automaticButton->setGeometry(QRect(450, 540, LABEL_WIDTH, LABEL_HEIGHT));
    //hand_automaticLabel->setGeometry(QRect(450+10, 540, 40, 24));
    connect(hand_automaticButton, SIGNAL(clicked()), this, SLOT(hand_automaticEventDialog()));

    QSqlDatabase::database().transaction();
    QSqlQuery hostquery;
    hostquery.setForwardOnly(true);
    hostquery.exec("select systemtype from hostset");
    while (hostquery.next())
    {

        if(hostquery.value(0).toInt())
        {
            handWidget->openFunction();
            hand_automaticButton->setEnabled(true);
        }
        else
        {
            handWidget->closeFunction();
            hand_automaticButton->setEnabled(false);
        }

    }
    QSqlDatabase::database().commit();


    connect(this,SIGNAL(eventNumberChangedSignal()),this,SLOT(eventNumberChangedSlot()));

    timeLabel = new QLabel(this);
    //timeLabel->setGeometry(QRect(580,540,180,24));
    timeLabel->setGeometry(QRect(580,0,180,24));

    remindLabel = new QLabel(tr("F2设置  F3查看"),this);
    //remindLabel->setGeometry(QRect(400,540,150,24));
    remindLabel->setGeometry(QRect(640,540,150,24));
    remindLabel->setFont(QFont("unifont",10));

    eventNumberChangedSlot();

    backgroundLabel = new QLabel(this);
    //backgroundLabel->setPixmap(QPixmap(":/image/background.PNG"));
    backgroundLabel->setGeometry(QRect(0,0 ,800,600));

    QPixmap pTimePix(tr(":/image/background.PNG"));
    QPainter painter1;
    painter1.begin(&pTimePix);
    painter1.setPen(Qt::black);
    painter1.setFont(QFont("Times", 14, QFont::Bold));
    QDateTime dt = QDateTime::currentDateTime();
    //setGeometry(QRect(580,0,180,24));
    //backgroundLabel->setText(dt.toString("yyyy-MM-dd hh:mm:ss"));
    painter1.drawText(QRect(580,540,180,24), Qt::AlignBottom, dt.toString("yyyy-MM-dd hh:mm:ss"));    //这里要确保你的坐标等等参数正确，不然你就不知道画到哪里去了
    painter1.drawText(QRect(360,380,100,60), Qt::AlignBottom,tr("系统正常"));
    painter1.end();
    backgroundLabel->setPixmap(pTimePix);
    backgroundLabel->setVisible(true);
    //backgroundLabel->setVisible(false);

}

void MainWidget::eventNumberChangedSlot()
{
    /*update the 8 labels of event number.*/

    superviseLabel->setText(tr("%1").arg(superviseList.count()));
    faultLabel->setText(tr("%1").arg(faultList.count()));
    delayLabel->setText(tr("%1").arg(delayList.count()));

    firenumLabel->setText(tr("%1").arg(fireList.count()));
    linknumLabel->setText(tr("%1").arg(linkList.count()));

    fireTableWidget->setRowCount(max(fireList.count(),LISTWIDGET_ROW_OF_FIRE));
    linkTableWidget->setRowCount(max(linkList.count(),LISTWIDGET_ROW_OF_FIRE-1));

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec("select * from shieldhis");
    int shield_count = 0;
    while (query.next())
    {
        shield_count++;
    }
    shieldLabel->setText(tr("%1").arg(shield_count));
    QSqlDatabase::database().commit();

    if (shield_count > 0)
    {
        emit ledStateChangeSignal(LED_OF_SHIELD,LED_PUT_ON);
    }
    else
    {
        emit ledStateChangeSignal(LED_OF_SHIELD,LED_PUT_OFF);
    }

    if(delayList.count() > 0)
    {
        //qDebug()<<"delayList.count() > 0 is true";
        if (hashFireDelay.isEmpty())
        {
            //qDebug()<<"hashFireDelay.isEmpty() is true";
            emit ledStateChangeSignal(LED_OF_FIRE_DELAY,LED_PUT_OFF);
        }
        else
        {
            //qDebug()<<"hashFireDelay.isEmpty() is false";
            emit ledStateChangeSignal(LED_OF_FIRE_DELAY,LED_PUT_ON);
        }

        if ((hashLinkStartDelay.isEmpty()) && (hashLinkPulseDelay.isEmpty()))
        {
            //qDebug()<<"(hashLinkStartDelay.isEmpty()) && (hashLinkPulseDelay.isEmpty()) is true";
            emit ledStateChangeSignal(LED_OF_OUTPUT_DELAY,LED_PUT_OFF);
        }
        else
        {
            //qDebug()<<"(hashLinkStartDelay.isEmpty()) && (hashLinkPulseDelay.isEmpty()) is false";
            emit ledStateChangeSignal(LED_OF_OUTPUT_DELAY,LED_PUT_ON);
        }
    }
    else
    {
        //qDebug()<<"delayList.count() > 0 is false";
        emit ledStateChangeSignal(LED_OF_FIRE_DELAY,LED_PUT_OFF);
        emit ledStateChangeSignal(LED_OF_OUTPUT_DELAY,LED_PUT_OFF);
    }


}
//void MainWidget::createStatusBar()
//{
//    statusBar = new QStatusBar (this);
//    statusBar->setGeometry(QRect(0, 576, 800, 24));

//    remindLabel = new QLabel (tr("按下F2显示设置窗口，按下F3显示查看窗口"), statusBar);
//    remindLabel->setAlignment (Qt::AlignLeft);
//    remindLabel->setMinimumSize (remindLabel->sizeHint());

////    timeLabel = new QLabel (tr("2012-05-04  00:00:00"), statusBar);
////    timeLabel->setAlignment (Qt::AlignCenter);
////    timeLabel->setMaximumSize(timeLabel->sizeHint());
////    /*此处QPoint(650, 5).y = 5，因为子控件的y必须相对于父控件而言*/
////    timeLabel->setGeometry(QRect(QPoint (650, 5), timeLabel->sizeHint()));
////    timeLabel->setFrameShape(QFrame::Box);
//}
#if 0
void MainWidget::getHashBusFromBusset()
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from busset"));//遍历busset数据库，完成hashBusset的初始化。

    while (query.next())
    {
        int buspanelNum = query.value(1).toInt();
        int busnodeNum = query.value(2).toInt();
        for(int i = 0; i < 15; i++)
        {
            int k = query.value(i+3).toInt();
            if( k )
            {
                nodeID tempnode = nodeID((k >> 24) & 0xff,(k >> 16) & 0xff,(k >>  8) & 0xff,k & 0xff);
                hashBusIn.insert(tempnode, 0x0ffff| buspanelNum<<8 | busnodeNum);//value>>16位 为0，表示设备状态为 反馈恢复，为1表示设备状态为 反馈
            }
            else
                break;
        }
        for(int j = 0; j < 15; j++)
        {
            int k =  query.value(j+18).toInt();
            if( k )
            {
                nodeID tempnode = nodeID((k >> 24) & 0xff,(k >> 16) & 0xff,(k >>  8) & 0xff,k & 0xff);
                hashBusOut.insert(tempnode,0x0ffff | buspanelNum<<8 | busnodeNum);//value>>16位 为0，表示设备状态为 未启动，为1表示设备状态为 启动
            }
            else
                break;
        }
    }
    QSqlDatabase::database().commit();
}
#endif
#if 0
int MainWidget::getNewState(char preState, char command, unsigned char reportTIState)
{
    /*   3.3，联动上报的四种状态
    #define TI_FeedBack            0x0b
    #define TI_FeedBackRecover     0x0c
    #define TI_LinkageStart        0x0d
    #define TI_LinkageStop         0x0e

    3.4，联动信息的4种显示状态
    #define LINK_START_NO_FEEDBACK_STATE 0X01  启动未反馈
    #define LINK_START_YES_FEEDBACK_STATE 0X02 启动已反馈
    #define LINK_FEEDBACK_STATE 0X03 联动反馈
    0x0  无状态

    3.5，联动下发的两种命令
    #define LINK_ACTION_START 0X01
    #define LINK_ACTION_STOP 0X02   */

    qDebug()<<"in MainWidget::getNewState().preState"<<preState<<"link_action:"<<command<<"reportTIState:"<<reportTIState;

    switch(preState)
    {
    case 0://NO_STATE
        if(reportTIState == TI_LinkageStart && command == LINK_ACTION_START)
            return LINK_START_NO_FEEDBACK_STATE;//newState = QString("启动未反馈");
        else if( reportTIState == TI_FeedBack )
        {
            if(command == LINK_ACTION_START )
                return LINK_START_YES_FEEDBACK_STATE;//newState = QString("启动已反馈");
            else if(command == 0)
                return LINK_FEEDBACK_STATE;//newState = QString("联动反馈");
        }
        break;
    case LINK_START_NO_FEEDBACK_STATE:
        if(reportTIState == TI_FeedBack)
        {
            if(command == 0 )
                return LINK_START_YES_FEEDBACK_STATE;//newState = QString("启动已反馈");
            else if(command == LINK_ACTION_STOP )
                return LINK_FEEDBACK_STATE;//newState = QString("联动反馈");
        }
        else if(reportTIState == TI_LinkageStop)
            return NO_STATE;
        break;
    case LINK_START_YES_FEEDBACK_STATE:
        if(reportTIState == TI_LinkageStop && command == LINK_ACTION_STOP)
            return LINK_FEEDBACK_STATE;//newState = QString("联动反馈");
        else if(reportTIState == TI_FeedBackRecover && command == LINK_ACTION_STOP)
            return NO_STATE;
        break;
    case LINK_FEEDBACK_STATE:
        if(reportTIState == TI_FeedBackRecover && command == 0)
            return NO_STATE;
        else if((reportTIState == TI_LinkageStart || reportTIState == TI_FeedBack) && command == LINK_ACTION_START)
            return LINK_START_YES_FEEDBACK_STATE;//newState = QString("启动已反馈");
        break;
    default:
        qDebug()<<"in MainWidget::getNewState():NONONONO this case.check.";
        return NO_STATE;
        break;
    }
}
#endif

void MainWidget::updateTabWidget(int index)
{
    switch(index)
    {
    case 0:
        superviseTableWidget->clear();
        for(int i = 0; i < superviseList.count(); i++)
        {
            superviseTableWidget->setItem(i, 0, new QTableWidgetItem(superviseList.at(i)));
            superviseTableWidget->selectRow(i);
        }
        break;
    case 1:
        faultTableWidget->clear();
        for(int i = 0; i < faultList.count(); i++)
        {
            faultTableWidget->setItem(i, 0, new QTableWidgetItem(faultList.at(i)));
            faultTableWidget->selectRow(i);
        }
        break;
    case 2:
    {
        delayTableWidget->clear();
        //        for(int i = 0; i < delayList.count(); i++)
        //        {
        //            delayTableWidget->setItem(i, 0, new QTableWidgetItem(delayList.at(i)));
        //            delayTableWidget->selectRow(i);
        //        }

        //A:报警延时: hashFireDelay.insert(tempnode, nodetype<<24 | nodestate<<16 | ((delayList.count()-1)<<8) | startDelay);
        QHash<nodeID, int>::iterator iterFireP = hashFireDelay.begin();
        while (iterFireP != hashFireDelay.end())
        {
            int row = iterFireP.value()>>8 & 0xff;
            QString strItem = QString("%1 %2").arg(iterFireP.value() & 0xff).arg(delayList.at(row));
            delayTableWidget->setItem(row, 0, new QTableWidgetItem(strItem));
            iterFireP++;
        }

        /* B:联动启动延时: hashLinkStartDelay.insert(tempnode, pulseDelay<<8 | ((delayList.count()-1)<<8) | startDelay); */
        QHash<nodeID, int>::iterator iterLinkStartP = hashLinkStartDelay.begin();
        while (iterLinkStartP != hashLinkStartDelay.end())
        {
            int row = iterLinkStartP.value()>>8 & 0xff;
            QString strItem = QString("%1 %2").arg(iterLinkStartP.value() & 0xff).arg(delayList.at(row));
            delayTableWidget->setItem(row, 0, new QTableWidgetItem(strItem));
            iterLinkStartP++;
        }

        /*C:联动脉冲延时: hashLinkPulseDelay.insert(tempnode,((delayList.count()-1)<<8) | pulseDelay); */
        QHash<nodeID, int>::iterator iterLinkPulseP = hashLinkPulseDelay.begin();
        while (iterLinkPulseP != hashLinkPulseDelay.end())
        {
            int row = iterLinkPulseP.value()>>8 & 0xff;
            QString strItem = QString("%1 %2").arg(iterLinkPulseP.value() & 0xff).arg(delayList.at(row));
            delayTableWidget->setItem(row, 0, new QTableWidgetItem(strItem));
            iterLinkPulseP++;
        }

    }
        break;
    case 3:
    {
        qDebug()<<"in mainWidget::updateTabWidget:shieldIndex .";
        shieldTableWidget->clear();

        QSqlDatabase::database().transaction();
        int row = 0;
        QSqlQuery query;
        //           query.setForwardOnly(true);
        query.exec("select * from shieldhis");

        int shield_count = 0;
        while (query.next())
        {
            shield_count++;
        }
        shieldLabel->setText(tr("%1").arg(shield_count));
        if (shield_count > 0)
        {
            emit ledStateChangeSignal(LED_OF_SHIELD,LED_PUT_ON);
        }
        else
        {
            emit ledStateChangeSignal(LED_OF_SHIELD,LED_PUT_OFF);
        }
        int j =0;
        while(query.previous()&&(j < 1000))
        {
            j++;
        }
        while (query.next())
        {
            if (query.value(1).toInt() == SoundAndAlarmDeviceID)
            {
                QDateTime dt = query.value(0).toDateTime();
                QString dtstr = dt.toString("MM-dd hh:mm:ss");
                QString shield_info = QString("%1  声光报警器   已经屏蔽")
                        .arg(dtstr); //time

                shieldTableWidget->setItem(row++, 0, new QTableWidgetItem(shield_info));

            }
            else
            {
                QDateTime dt = query.value(0).toDateTime();
                QString dtstr = dt.toString("MM-dd hh:mm:ss");
                QString shield_info = QString("%1  %2号主机%3号接口板%4号回路%5号节点   %6   已经屏蔽")
                        .arg(dtstr) //time
                        .arg((query.value(1).toInt() >> 24) & 0xFF)//host
                        .arg((query.value(1).toInt() >> 16) & 0xFF)//iobr
                        .arg((query.value(1).toInt() >> 8) & 0xFF)//loop
                        .arg(query.value(1).toInt() & 0xFF)//node
                        .arg(Type::char_danyuan(query.value(2).toInt()));//type
                shieldTableWidget->setItem(row++, 0, new QTableWidgetItem(shield_info));
            }
        }

        QSqlDatabase::database().commit();
    }

        break;
    case 4:
    {
        qDebug()<<"in mainWidget::updateTabWidget:handIndex .";
        PasswordDialog password(3, this);
        password.exec();
        if (password.matchFlag)
        {
            eventTabWidget->setCurrentIndex(4);
        }
        else
        {
            eventTabWidget->setCurrentIndex(0);
        }

    }
        break;
    default:
        qDebug()<<"in mainWidget::updateTabWidget:index undefined.";
        break;
    }
}

#if 0
void MainWidget::updateHashBusSlot(int buspanelNum, int busnodeNum, int inMode[], int outMode[])
{
    int i;
    for(i = 0; i < 15; i++)
    {
        int tempvalue = 0xffff | buspanelNum<<8 | busnodeNum;
        if( inMode[i] )
        {
            hashBusIn.insert( nodeID((inMode[i] >> 24) & 0xff,(inMode[i] >> 16) & 0xff,(inMode[i] >>  8) & 0xff,inMode[i] & 0xff),tempvalue);
        }
        else
            break;
    }
    for(i = 0; i < 15; i++)
    {
        int tempvalue = 0xffff | buspanelNum<<8 | busnodeNum;
        if( outMode[i] )
        {
            hashBusOut.insert( nodeID((outMode[i] >> 24) & 0xff,(outMode[i] >> 16) & 0xff,(outMode[i] >>  8) & 0xff,outMode[i] & 0xff),tempvalue);
        }
        else
            break;
    }
}
#endif

void MainWidget::timerEvent (QTimerEvent *)// *event)
{
    QDateTime dt = QDateTime::currentDateTime();
    timeLabel->setText(dt.toString("yyyy-MM-dd hh:mm:ss"));
    if(backgroundLabel->isVisible())
    {
        QPixmap pTimePix(tr(":/image/background.PNG"));
        QPainter painter1;
        painter1.begin(&pTimePix);
        painter1.setPen(Qt::black);
        painter1.setFont(QFont("Times", 14, QFont::Bold));
        QDateTime dt = QDateTime::currentDateTime();
        //setGeometry(QRect(580,0,180,24));
        //backgroundLabel->setText(dt.toString("yyyy-MM-dd hh:mm:ss"));
        painter1.drawText(QRect(580,540,180,24), Qt::AlignBottom, dt.toString("yyyy-MM-dd hh:mm:ss"));    //这里要确保你的坐标等等参数正确，不然你就不知道画到哪里去了
        painter1.drawText(QRect(360,380,100,60), Qt::AlignBottom,tr("系统正常"));
        painter1.end();
        backgroundLabel->setPixmap(pTimePix);
    }
}

void MainWidget::hideWindow()
{
    PasswordDialog password(2, w);
    password.exec();
    if (password.matchFlag)
    {
        //this->hide();
        eventdialog->hide();
        menutype = 0;
        w->show();
    }
}

void MainWidget::secondhideWindow()
{
    //this->hide();
    menutype = 1;
    v->show();
}

void MainWidget::thirdhideWindow()
{
    //this->hide();
    menutype = 0;
    w->show();
}


void MainWidget::fireEventDialog()
{
    eventdialog->show();
    oneEventShowFlag = 0;
    oneEventShowFlag |= FIRE_PUSHBUTTON;//0x01
    if(fireList.count() > 0)
        emit oneEventShowSignal(fireList.at(0), fireList, oneEventShowFlag);
    else
    {
        QString str = "";
        emit oneEventShowSignal(str, fireList, oneEventShowFlag);
    }
}

void MainWidget::linkEventDialog()
{
    eventdialog->show();
    oneEventShowFlag = 0;
    oneEventShowFlag |= LINK_PUSHBUTTON;
    if(fireList.count() > 0)
        emit oneEventShowSignal(fireList.at(0), linkList, oneEventShowFlag);
    else
    {
        QString str = "";
        emit oneEventShowSignal(str, linkList, oneEventShowFlag);
    }    
}

void MainWidget::superviseEventDialog()
{
    eventdialog->show();
    oneEventShowFlag = 0;
    oneEventShowFlag |= SUPERVISE_PUSHBUTTON;
    if(fireList.count() > 0)
        emit oneEventShowSignal(fireList.at(0), superviseList, oneEventShowFlag);
    else
    {
        QString str = "";
        emit oneEventShowSignal(str, superviseList, oneEventShowFlag);
    }    

    if(superviseList.isEmpty())
    {
        timer4s->start();
    }
}


void MainWidget::faultEventDialog()
{    
    eventdialog->show();
    oneEventShowFlag = 0;
    oneEventShowFlag |= FAULT_PUSHBUTTON;
    if(fireList.count() > 0)
        emit oneEventShowSignal(fireList.at(0), faultList, oneEventShowFlag);
    else
    {
        qDebug()<<"faultEventDialog():fireList is null,there is no fire.";
        QString str = "";
        emit oneEventShowSignal(str, faultList, oneEventShowFlag);
    }

    if(faultList.isEmpty())
    {
        timer4s->start();
    }
}

void MainWidget::delayEventDialog()
{    
    eventdialog->show();
    oneEventShowFlag = 0;
    oneEventShowFlag |= DELAY_PUSHBUTTON;

    //qingwu:2012.11.05
    if(fireList.count() > 0)
        emit oneEventShowSignal(fireList.at(0), delayList, oneEventShowFlag);
    else
    {
        qDebug()<<"fireList is null,there is no fire.";
        QString str = "";
        emit oneEventShowSignal(str, delayList, oneEventShowFlag);
    }

    if(delayList.isEmpty())
    {
        timer4s->start();
    }
}

void MainWidget::shieldEventDialog()
{    
    eventdialog->show();
    oneEventShowFlag = 0;
    oneEventShowFlag |= SHIELD_PUSHBUTTON;

    //qingwu:2012.11.05
    if(fireList.count() > 0)
        emit oneEventShowSignal(fireList.at(0), shieldList, oneEventShowFlag);
    else
    {
        qDebug()<<"fireList is null,there is no fire.";
        QString str = "";
        emit oneEventShowSignal(str, shieldList, oneEventShowFlag);
    }

    if(shieldList.isEmpty())
    {
//        timer4s->start();
    }
}

void MainWidget::fire_linkEventDialog()
{

}

void MainWidget::hand_automaticEventDialog()
{    
    eventdialog->show();
    oneEventShowFlag = 0;
    oneEventShowFlag |= HANDAOTO_PUSHBUTTON;

    //qingwu:2012.11.05
    if(fireList.count() > 0)
        emit oneEventShowSignal(fireList.at(0),handautoList, oneEventShowFlag);
    else
    {
        qDebug()<<"fireList is null,there is no fire.";
        QString str = "";
        emit oneEventShowSignal(str, handautoList, oneEventShowFlag);
    }
    if(handautoList.isEmpty())
    {
//        timer4s->start();
    }
}

void MainWidget::printerFlagOfWidgetSetSlot(int pFlag)//add by qingwu
{
    OperationHis::Update("打印设置");
    printerFlagOfWidget = pFlag;
    qDebug("in MainWidget: printerFlagOfWidget is : %x",printerFlagOfWidget);
}

void MainWidget::linkFeedBackSetSlot(int pFlag)
{
    linkFeedBackFlag = pFlag ;
    qDebug()<<"in MainWidget::linkFeedBackSetSlot:linkFeedBackFlag is:"<<linkFeedBackFlag;

}
void MainWidget::updateHashRowNumber(QMultiHash<nodeID, int> &hashTable,int rmRowNumber)
{
    /*  △△△ 确保传入的哈希表都是QMultiHash。rmRowNumber begin from 0.
    遍历 QMultiHash 表，找出所有key值大于当前删除行号的节点，对于哈希表中剩余的项的value值中的行号，
    如果有比当前删除行的行号大的，要进行行号减1的操作.将行号更新到原先的value中。
    If there is already an item with the same key in the hash,QMutiHash的insert()操作会增加一条信息，
    因此在insert行号减1的<value,key>之后，需要将之前的那条<value,key>信息删除 */
    //qDebug()<<"before update row number------------";
    //hashTest(hashTable);
    QHash<nodeID, int>::iterator iterP;
    iterP = hashTable.begin();
    while (iterP != hashTable.end())
    {
        if( ((iterP.value()>>8) & 0xff) > rmRowNumber )
            iterP.value() -= 0x100;
        ++iterP;
    }
    //for test
    //qDebug()<<"after update row number------------";
    //hashTest(hashTable);
}
#if 0
void MainWidget::hashTest(QMultiHash<nodeID, int> hashTable)
{
    QHash<nodeID, int>::iterator iterP = hashTable.begin();
    while (iterP != hashTable.end())
    {
        nodeID tempnode = iterP.key();
        int iobrnum = tempnode.getiobrnum();
        int loopnum = tempnode.getloopnum();
        int nodenum = tempnode.getnodenum();
        int rowNumber = iterP.value()>>8 & 0xff;
        qDebug()<<"iobrnum"<<iobrnum<<"loopnum"<<loopnum<<"nodenum"<<nodenum<<"rownumber"<<rowNumber;
        ++iterP;
    }
}
#endif

void  MainWidget::removeFireItem(nodeID tempnode, int rmRowNumber)
{
    qDebug()<<"in removeFireItem():remove fire of row:"<<rmRowNumber;
    //fireTableWidget->removeRow(rmRowNumber);
    //fireTableWidget->insertRow(fireTableWidget->rowCount());
    fireList.removeAt(rmRowNumber);
    //firenumLabel->setText(tr("%1").arg(fireList.count()));
    //fireTableWidget->selectRow(fireList.count() - 1);


    //遍历hashFire表，找出所有key值大于当前删除行号 的节点，对其key值进行-1操作. 删除对应<value,key>
    hashFire.erase(hashFire.find(tempnode));
    emit eventNumberChangedSignal();
    QHash<nodeID, int>::iterator iterP = hashFire.begin();
    while (iterP != hashFire.end())
    {
        if( ( iterP.value() & 0xffff) > rmRowNumber )
            iterP.value() -= 1;
        ++iterP;
    }
}

void  MainWidget::removeLinkItem(int hostnum,int iobrnum,int loopnum,int nodenum,char shijianma,char danyuanma,QDateTime datetime)
{
    //if(eventdialog->isActiveWindow())
    eventdialog->hide();
    this->show();//show main widget

    nodeID tempnode = nodeID(hostnum, iobrnum,loopnum,nodenum);
    int  bldnum = 0 , flrnum = 0, zonenum = 0, roomnum = 0;
    QString nodestr;
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from unitset WHERE unitid == %1")
               .arg((hostnum << 24) | (iobrnum << 16) | (loopnum << 8) | nodenum));
    if (query.next())
    {
        bldnum = (query.value(6).toInt() >> 24) & 0xFF;
        flrnum = (query.value(6).toInt() >> 16) & 0xFF;
        zonenum = (query.value(6).toInt() >> 8) & 0xFF;
        roomnum = query.value(6).toInt() & 0xFF;
        nodestr = query.value(9).toString();
        QString infoid_info;
        QString geo_addr_info;
        infoid_info.sprintf("%02d-%02d-%02d-%03d",hostnum, iobrnum,loopnum, nodenum);
        geo_addr_info.sprintf("%02d/%02d/%02d", bldnum, flrnum, zonenum);

        int rmRowNumber = hashLink.value(tempnode) & 0xffff;
        //linkTableWidget->removeRow(rmRowNumber);
        //linkTableWidget->insertRow(linkTableWidget->rowCount());
        qDebug()<<"remove row number of linkList is:"<<rmRowNumber;
        linkList.removeAt(rmRowNumber);
        ilink = linkList.count()-1;
        updateLinkWidgetSlot();


        linknumLabel->setText(tr("%1").arg(linkList.count()));
        //linkTableWidget->selectRow(linkList.count() - 1);

        //遍历hashLink表，找出所有key值大于当前删除行号 的节点，对其key值进行-1操作. 删除对应<value,key>
        hashLink.erase(hashLink.find(tempnode));
        emit eventNumberChangedSignal();
        QHash<nodeID, int>::iterator iterP = hashLink.begin();
        while (iterP != hashLink.end())
        {
            if(  ( iterP.value() & 0xffff) > rmRowNumber )
                iterP.value() -= 1;
            ++iterP;
        }

        hashLinkageAction.erase(hashLinkageAction.find(tempnode));

        linkAllNumber += 1;
        saveLinkToDB(iobrnum, loopnum, nodenum, danyuanma, shijianma, datetime);

        //saveLinkToDB((int)shijianma,(int)danyuanma, iobrnum, loopnum, nodenum, bldnum , flrnum, zonenum , roomnum, datetime, nodestr);
        if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_LINK_FLAG) & 0x01) )
        {
            QString line2 = QString("%1 %2 %3")
                    .arg(Type::char_shijian(shijianma,danyuanma) )//1
                    .arg(Type::char_danyuan(danyuanma) )//2
                    .arg(geo_addr_info); //bld/flr/zone
            QString line3 = QString("%1 %2")
                    .arg(datetime.toString("MM-dd hh:mm:ss"))
                    .arg(infoid_info); //host-iobr-loop-node
            doPrint(nodestr,line2,line3);
        }
    }
    QSqlDatabase::database().commit();

    //    /*link列表为空*/
    //    if(linkList.isEmpty())
    //    {
    //        //emit ledStateChangeSignal(LED_OF_LINK_START,LED_PUT_OFF);
    //        qDebug()<<"------------------link-led put off";
    //    }
    //    else
    //    {
    //        QHash<nodeID, int>::iterator iterP = hashLink.begin();
    //        while (iterP != hashLink.end())
    //        {
    //            if( (((iterP.value()>>24) & 0xff) == LINK_START_NO_FEEDBACK_STATE) ||
    //                    (((iterP.value()>>24) & 0xff) == 0x0B))//对非输入输出设备
    //            {
    //                emit ledStateChangeSignal(LED_OF_LINK_START,LED_PUT_FLICK);
    //                qDebug()<<"------------------link-led put flick";
    //                return;
    //            }
    //            ++iterP;
    //        }
    //        emit ledStateChangeSignal(LED_OF_LINK_START,LED_PUT_ON);
    //        qDebug()<<"------------------link-start-led put on";

    //    }
}

QString  MainWidget::getStingItemFromDB(int hostnum,int iobrnum,int loopnum,int nodenum,char shijianma,char danyuanma,char nodetype,QDateTime datetime)
{
    int  bldnum = 0 , flrnum = 0, zonenum = 0, roomnum = 0;
    // char danyuanPrint = 0;
    QString nodestr;
    QString infoid_info;
    //QString geo_addr_info;
    QString stringItem;
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from unitset WHERE unitid == %1")
               .arg((hostnum << 24) | (iobrnum << 16) | (loopnum << 8) | nodenum));
    if (query.next())
    {
        bldnum = (query.value(6).toInt() >> 24) & 0xFF;
        flrnum = (query.value(6).toInt() >> 16) & 0xFF;
        zonenum = (query.value(6).toInt() >> 8) & 0xFF;
        roomnum = query.value(6).toInt() & 0xFF;
        nodestr = query.value(9).toString();
        infoid_info.sprintf("%02d-%02d-%02d-%03d",hostnum, iobrnum,loopnum, nodenum);
        //geo_addr_info.sprintf("%02d/%02d/%02d", bldnum, flrnum, zonenum);
        stringItem = QString("%1 %2 %3 %4 %5")
                .arg(datetime.toString("MM-dd hh:mm:ss"))
                .arg(infoid_info) //host-iobr-loop-node
                .arg(Type::char_shijian(shijianma,danyuanma) )//1
                .arg(Type::char_danyuan(nodetype) )//2
                //.arg(geo_addr_info) //bld/flr/zone
                .arg(nodestr);//by qingwu:显示汉字标签
    }
    QSqlDatabase::database().commit();

//    linkAllNumber += 1;
//    saveLinkToDB((int)shijianma,(int)danyuanma, iobrnum, loopnum, nodenum, bldnum , flrnum, zonenum , roomnum, datetime, nodestr);

//    if( (printerFlagOfWidget & 0x01) || ((printerFlagOfWidget>>PRINT_LINK_FLAG) & 0x01) )
//    {
//        QString line2 = QString("%1 %2")
//                .arg(Type::char_shijian(shijianma,danyuanma) )//1
//                .arg(Type::char_danyuan(nodetype) );//2
//        //.arg(geo_addr_info); //bld/flr/zone
//        QString line3 = QString("%1 %2")
//                .arg(datetime.toString("MM-dd hh:mm:ss"))
//                .arg(infoid_info); //host-iobr-loop-node
//        doPrint(nodestr,line2,line3);
//    }
    return stringItem;

}

void MainWidget::linkShow()
{
    //linkTableWidget->setItem(linkList.count()-1,0,new QTableWidgetItem(linkList.last()));
    //linkTableWidget->selectRow(linkList.count()-1);
    linknumLabel->setText(tr("%1").arg(linkList.count()));//then ,1 is 1,not 001.if you want to show as 001,.arg(linknumChar)
    emit closeWVSignal();

    //    QHash<nodeID, int>::iterator iterP = hashLink.begin();
    //    while (iterP != hashLink.end())
    //    {
    //        if( (((iterP.value()>>24) & 0xff) == LINK_START_NO_FEEDBACK_STATE) ||
    //                (((iterP.value()>>24) & 0xff) == 0x0B))//对非输入输出设备
    //        {
    //            emit ledStateChangeSignal(LED_OF_LINK_START,LED_PUT_FLICK);
    //            qDebug()<<"------------------link-led put flick";
    //            return;
    //        }
    //        ++iterP;
    //    }
    //    emit ledStateChangeSignal(LED_OF_LINK_START,LED_PUT_ON);
    //    qDebug()<<"------------------link-led put on";
}

void MainWidget::dealLinkLed()
{
    //deal 2 link light:LED_OF_LINK_START and #define LED_OF_LINK_FEEDBACK 0x20
    /* LED_OF_LINK_START
      0x0B:start 对非输入输出设备
        1）遍历hashLink，如果有启动未反馈，那么就闪灯；
        2）遍历hashLink，如果有启动已反馈，那么点亮启动灯；否则不对启动灯操作。
    */
    char linkStartLedFlick = false;
    if(hashLink.isEmpty())
    {
        emit linkSoundOffSignal();
        emit ledStateChangeSignal(LED_OF_LINK_FEEDBACK,LED_PUT_OFF);
        qDebug()<<"------------------led feedback put off";
        return;
    }

    QHash<nodeID, int>::iterator iterP = hashLink.begin();
    while (iterP != hashLink.end())
    {
        if( (((iterP.value()>>24) & 0xff) == LINK_START_NO_FEEDBACK_STATE) ||
                (((iterP.value()>>24) & 0xff) == 0x0B))//对非输入输出设备
        {
            emit ledStateChangeSignal(LED_OF_LINK_START,LED_PUT_FLICK);
            qDebug()<<"------------------led link-start put flick";
            linkStartLedFlick = true;
            break;
        }
        ++iterP;
    }

    if( !linkStartLedFlick )
    {
        iterP = hashLink.begin();
        while (iterP != hashLink.end())
        {
            if( (((iterP.value()>>24) & 0xff) == LINK_START_YES_FEEDBACK_STATE) ||
                    (((iterP.value()>>24) & 0xff) == 0x0B))//对非输入输出设备
            {
                emit ledStateChangeSignal(LED_OF_LINK_START,LED_PUT_ON);
                qDebug()<<"------------------led link-start put on";
                break;
            }
            ++iterP;
        }
    }
    /* LED_OF_LINK_FEEDBACK
        #define LINK_START_YES_FEEDBACK_STATE 0x02
        #define LINK_FEEDBACK_STATE 0x03
        0x55:feedback 对非输入输出设备
        遍历hashLink，如果有 联动反馈 或者 启动已反馈 状态的联动节点，那么就发送点亮反馈灯命令，否则，发送灭灯命令。
    */
    iterP = hashLink.begin();
    while (iterP != hashLink.end())
    {
        if( (((iterP.value()>>24) & 0xff) == LINK_FEEDBACK_STATE) ||
                (((iterP.value()>>24) & 0xff) == LINK_START_YES_FEEDBACK_STATE) ||
                (((iterP.value()>>24) & 0xff) == 0x55))//对非输入输出设备
        {
            emit ledStateChangeSignal(LED_OF_LINK_FEEDBACK,LED_PUT_ON);
            qDebug()<<"------------------led feedback put on";
            return;
        }
        ++iterP;
    }
    emit ledStateChangeSignal(LED_OF_LINK_FEEDBACK,LED_PUT_OFF);
    qDebug()<<"------------------led feedback put off";

}
void MainWidget::dealLinkLedStop()
{
    /*  将联动停止 和 联动反馈恢复 槽函数最后的deallinkled函数 用新函数dealLinkLedStop()代替
     ** 对启动灯和反馈灯进行相应的处理。
     ** 联动停止：遍历hashlink，如果有 启动未反馈 或者 启动已反馈 或者 联动启动（对非输入输出），那么对启动灯不处理。
     ** 反馈恢复：遍历hashlink，如果有 联动反馈 或者 启动已反馈 ，那么不处理；如果都没有，就灭反馈灯。
    */
    if(hashLink.isEmpty())
    {
        emit linkSoundOffSignal();
        emit ledStateChangeSignal(LED_OF_LINK_FEEDBACK,LED_PUT_OFF);
        qDebug()<<"in dealLinkLedStop():led feedback put off";
        return;
    }

    QHash<nodeID, int>::iterator iterP = hashLink.begin();
    while (iterP != hashLink.end())
    {
        int state = (iterP.value()>>24) & 0xff;
        if( (state == LINK_START_NO_FEEDBACK_STATE) || (state == LINK_START_YES_FEEDBACK_STATE) || (state == 0x0B))//对非输入输出设备
        {
            qDebug()<<"in dealLinkLedStop():link-start-led state no change.";
            break;
        }
        ++iterP;
    }

    iterP = hashLink.begin();
    while (iterP != hashLink.end())
    {
        int state = (iterP.value()>>24) & 0xff;
        if( (state == LINK_FEEDBACK_STATE) || (state == LINK_START_YES_FEEDBACK_STATE) || (state == 0x55))//对非输入输出设备
        {
            qDebug()<<"in dealLinkLedStop():link-feedback-led state no change.";
            return;
        }
        ++iterP;
    }
    emit ledStateChangeSignal(LED_OF_LINK_FEEDBACK,LED_PUT_OFF);
    qDebug()<<"------------------led feedback put off";
}

void  MainWidget::eventPrint(int hostnum,int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate,QDateTime datetime)
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from unitset WHERE unitid == %1")
               .arg((hostnum << 24) | (iobrnum << 16) | (loopnum << 8) | nodenum));
    if (query.next())
    {
        int bldnum = (query.value(6).toInt() >> 24) & 0xFF;
        int flrnum = (query.value(6).toInt() >> 16) & 0xFF;
        int zonenum = (query.value(6).toInt() >> 8) & 0xFF;
        //int roomnum = query.value(6).toInt() & 0xFF;
        QString nodestr = query.value(9).toString();
        QString infoid_info;
        QString geo_addr_info;
        infoid_info.sprintf("%02d-%02d-%02d-%03d",hostnum, iobrnum,loopnum, nodenum);
        geo_addr_info.sprintf("%02d/%02d/%02d", bldnum, flrnum, zonenum);

        QString line2 = QString("%1 %2 %3")
                .arg(Type::char_shijian(nodestate,nodetype) )//1
                .arg(Type::char_danyuan(nodetype) )//2
                .arg(geo_addr_info); //bld/flr/zone
        QString line3 = QString("%1 %2")
                .arg(datetime.toString("MM-dd hh:mm:ss"))
                .arg(infoid_info); //host-iobr-loop-node
        doPrint(nodestr,line2,line3);
    }
    QSqlDatabase::database().commit();

}

void MainWidget::doPrint( QString str1, QString str2, QString str3)
{
    //printer the event selected
    if(fd_printer < 0)
    {
        qDebug()<<"in MainWidget::doPrint():fd_printer="<<fd_printer<<"return here.";
        return;
    }
    char ToHanzi[2];
    char p = 10;
    char zouzhi[3] = { 27, 74, 12};
    ToHanzi[0] = 28; ToHanzi[1] = 38;
    write(fd_printer,ToHanzi,sizeof(ToHanzi)/sizeof(char));//to hanzi

    QTextCodec *gbk = QTextCodec::codecForName("gb18030");
    QByteArray encodedString1 = gbk->fromUnicode(str1.toLocal8Bit());
    QByteArray encodedString2 = gbk->fromUnicode(str2.toLocal8Bit());
    QByteArray encodedString3 = gbk->fromUnicode(str3.toLocal8Bit());
    char *pstr1 = encodedString1.data();
    char *pstr2 = encodedString2.data();
    char *pstr3 = encodedString3.data();
    write(fd_printer, pstr1, encodedString1.count());//string to print
    write(fd_printer,&p,1);//print current row and line feed
    write(fd_printer, pstr2, encodedString2.count());//string to print
    write(fd_printer,&p,1);//print current row and line feed
    write(fd_printer, pstr3, encodedString3.count());//string to print
    write(fd_printer,&p,1);//print current row and line feed

    write(fd_printer,zouzhi, sizeof(zouzhi)/sizeof(char));
    //qDebug()<<"print data over!";
}


void MainWidget::initMaxId()
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);

    query.exec("select MAX(id) from firehis");
    while (query.next())
    {
        fireMaxId = query.value(0).toInt();
    }
    query.exec("select MAX(id) from linkhis");
    while (query.next())
    {
        linkMaxId = query.value(0).toInt();
    }
    query.exec("select MAX(id) from faulthis");
    while (query.next())
    {
        faultMaxId = query.value(0).toInt();
    }

    //     query.exec("select MAX(id) from supervisehis");
    //      while (query.next())
    //      {
    //          superviseMaxId = query.value(0).toInt();
    //      }

    //     qDebug()<<"fireMaxId"<<fireMaxId;
    //     qDebug()<<"linkMaxId"<<linkMaxId;
    //     qDebug()<<"faultMaxId"<<faultMaxId;
    //     qDebug()<<"superviseMaxId"<<superviseMaxId;

    QSqlDatabase::database().commit();

}

void MainWidget::currentFireChangedSlot()
{
    qDebug()<<"______________________mouse clicked";
    ifire = fireTableWidget->currentRow();
    //QTableWidgetItem item = fireTableWidget->itemAt(1,0);

    if(ifire > fireList.count())
        ifire = fireList.count()-2;
    timer10s->start();
    timer3s->stop();
}

//void MainWidget::currentCellActivatedSlot(int currentRow,int ,int preRow,int)
//{
//    //qDebug()<<"----currentRow"<<currentRow<<"preRow"<<preRow;

//    if(((currentRow - ifire) != 1) && (currentRow != 0))
//    {
//        ifire = currentRow;
//        if(currentRow > fireList.count())
//            ifire = fireList.count()-2;
//        timer10s->start();
//        //qDebug()<<"currentCellActivatedSlot():10s-start";
//        timer3s->stop();
//    }
//}
void MainWidget::timer3sStart()
{
    timer3s->start();
    //qDebug()<<"in timer3sStart():3s-start";
}

void MainWidget::eventDialogRejectedSlot()
{
    this->show();
}
void MainWidget::singleBoardNumSlot(int num)
{
    //connect(boardDialog,SIGNAL(singleBoardNumSignal(int)),polling,SLOT(stopPolling()));
    //connect(boardDialog,SIGNAL(singleBoardNumSignal(int)),boardThread,SLOT(signalBoardRegSlot(int)));

    qDebug()<<"in MainWidget:singleBoardNumSlot():polling stopped.start single boardreg thread..";
    if(polling->isRunning())
        polling->stopPolling();//timer in polling thread stopped!!!

    boardThread->singleBoardRegSlot(num);
    boardThread->start();

}
void MainWidget::updateHandwidgetType(int systemtype)
{
    qDebug()<<"in MainWidget::updateHandwidgetType:systemtype is:"<<systemtype;
    if(systemtype)
    {
        emit ledStateChangeSignal(LED_OF_MANUAL,LED_PUT_ON);
        emit ledStateChangeSignal(LED_OF_AUTOMATIC,LED_PUT_OFF);
        handWidget->openFunction();
        hand_automaticButton->setEnabled(true);
    }
    else
    {
        emit ledStateChangeSignal(LED_OF_MANUAL,LED_PUT_OFF);
        emit ledStateChangeSignal(LED_OF_AUTOMATIC,LED_PUT_ON);
        handWidget->closeFunction();
        hand_automaticButton->setEnabled(false);
    }
}

void MainWidget::updateSoundAndAlarmDeviceState(int sound_alarm_state)
{

    //应该在单元数据库中加上修改，但是由于现在在显示屏蔽时不需要用到，故没有加上，如果以后需要的话请在此处加上
    if(sound_alarm_state)
    {
        sound_alarm_shield_Flag = false;
        emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_SHIELD,LED_PUT_OFF);

        QSqlDatabase::database().transaction();
        QSqlQuery queryDel;
        queryDel.exec(QString("delete  from shieldhis where unitid = %1").arg(SoundAndAlarmDeviceID));
        bool bSuccess = queryDel.exec();
        if (!bSuccess)
        {
            QSqlError lastError = queryDel.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("in MainWidget :delete  soundandalarmdevice from shieldhis error"));
        }
        QSqlDatabase::database().commit();

        /*when sg is no longer shield,check sg fault state!*/
        if(sound_alarm_fault_Flag)
        {
            emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_FAULT,LED_PUT_ON);
            this->eventComeSlot(TI_BreakDown, 0, 0, 0, 0x14, 0xE1, QDateTime::currentDateTime());

            /*声光启动后，报故障，此时应将声光停止*/
            if(sound_alarm_startOrStop_Flag)
            {
                OperationHis::Update("声光报警器因故障停止");
                emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_START,LED_PUT_OFF);
                this->eventComeSlot(TI_LinkageStop, 0, 0, 0, 0x14, 0x0b, QDateTime::currentDateTime());
                sound_alarm_startOrStop_Flag = false;
            }
        }
        else
        {
            emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_FAULT,LED_PUT_OFF);
            this->eventComeSlot(TI_BreakDownRecover, 0, 0, 0, 0x14, 0xE2, QDateTime::currentDateTime());
        }
    }
    else
    {
        sound_alarm_shield_Flag = true;
        emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_SHIELD,LED_PUT_ON);

        QSqlDatabase::database().transaction();
        QDateTime datetime = QDateTime::currentDateTime();
        QSqlQuery query;
        query.setForwardOnly(true);

        query.prepare("INSERT INTO shieldhis( time,unitid,type)"
                      "VALUES ( ?, ?,?)");

        query.addBindValue(datetime);
        query.addBindValue(SoundAndAlarmDeviceID);
        query.addBindValue(0x00FFFF00);

        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("in MainWidget : insert soundandalarmdevice into the shieldhis error"));
        }



        QSqlDatabase::database().commit();
    }
    this->updateTabWidget(3);
}

void MainWidget::sgFlagChangeSlot(char flag)
{
    sound_alarm_fault_Flag = flag;

    if(!sound_alarm_shield_Flag)
    {
        if(sound_alarm_fault_Flag)
        {
            emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_FAULT,LED_PUT_ON);
            this->eventComeSlot(TI_BreakDown, 0, 0, 0, 0x14, 0xE1, QDateTime::currentDateTime());

            /*声光启动后，报故障，此时应将声光停止*/
            if(sound_alarm_startOrStop_Flag)
            {
                OperationHis::Update("声光报警器因故障停止");
                emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_START,LED_PUT_OFF);
                this->eventComeSlot(TI_LinkageStop, 0, 0, 0, 0x14, 0x0b, QDateTime::currentDateTime());
                sound_alarm_startOrStop_Flag = false;
            }
            qDebug()<<"in sgFlagChangeSlot():TI_BreakDown-0x14-0xE1";
        }
        else
        {
            emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_FAULT,LED_PUT_OFF);
            this->eventComeSlot(TI_BreakDownRecover, 0, 0, 0, 0x14, 0xE2, QDateTime::currentDateTime());
            qDebug()<<"in sgFlagChangeSlot():TI_BreakDownRecover-0x14-0xE2";
        }
    }



}

void MainWidget::updateGasFireControlState(int gasfirecontrolstate)
{
    if(gasfirecontrolstate)
    {
        emit ledStateChangeSignal(LED_OF_GAS_FIRE_CONTROL_ALLOW,LED_PUT_ON);
    }
    else
    {
        emit ledStateChangeSignal(LED_OF_GAS_FIRE_CONTROL_ALLOW,LED_PUT_OFF);
    }

}


void MainWidget::backgroundLabelDispaly()
{

    if(hashFire.isEmpty() && hashLink.isEmpty() && faultList.isEmpty() && superviseList.isEmpty() && delayList.isEmpty())
    {
        backgroundLabel->setVisible(true);
    }
    else
    {
        backgroundLabel->setVisible(false);
    }
}


void MainWidget::initSystemLight()
{

    QSqlDatabase::database().transaction();
    QSqlQuery hostquery;
    hostquery.setForwardOnly(true);
    hostquery.exec("select systemtype from hostset");
    while (hostquery.next())
    {

        if(hostquery.value(0).toInt())
        {
            emit ledStateChangeSignal(LED_OF_MANUAL,LED_PUT_ON);
            emit ledStateChangeSignal(LED_OF_AUTOMATIC,LED_PUT_OFF);
        }
        else
        {
            emit ledStateChangeSignal(LED_OF_MANUAL,LED_PUT_OFF);
            emit ledStateChangeSignal(LED_OF_AUTOMATIC,LED_PUT_ON);
        }

    }


    QSqlQuery sound_alarm_state_query;
    sound_alarm_state_query.setForwardOnly(true);
    sound_alarm_state_query.exec("select sound_alarm_state from hostset");
    while (sound_alarm_state_query.next())
    {
        if(sound_alarm_state_query.value(0).toInt())
        {
            sound_alarm_shield_Flag = false;
            emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_SHIELD,LED_PUT_OFF);
        }
        else
        {
            sound_alarm_shield_Flag = true;
            emit ledStateChangeSignal(LED_OF_SOUND_AND_ALARM_DEVICE_SHIELD,LED_PUT_ON);
        }
    }

    QSqlQuery gasfirecontrolstatequery;
    gasfirecontrolstatequery.setForwardOnly(true);
    gasfirecontrolstatequery.exec("select gasfirecontrolstate from hostset");
    while (gasfirecontrolstatequery.next())
    {

        if(gasfirecontrolstatequery.value(0).toInt())
        {
            emit ledStateChangeSignal(LED_OF_GAS_FIRE_CONTROL_ALLOW,LED_PUT_ON);
        }
        else
        {
            emit ledStateChangeSignal(LED_OF_GAS_FIRE_CONTROL_ALLOW,LED_PUT_OFF);
        }

    }


    QSqlQuery shieldquery;
    shieldquery.setForwardOnly(true);
    shieldquery.exec("select * from shieldhis");
    int shield_count = 0;
    while (shieldquery.next())
    {
        shield_count++;
    }


    if (shield_count > 0)
    {
        emit ledStateChangeSignal(LED_OF_SHIELD,LED_PUT_ON);
    }
    else
    {
        emit ledStateChangeSignal(LED_OF_SHIELD,LED_PUT_OFF);
    }


    QSqlDatabase::database().commit();


}


void MainWidget::systemSelfcheck()
{
    selfcheckFlag = true;
    qDebug()<<"self check start";
    //自检
    keyThread->oneSecondTimer->stop();
    keyThread->allSoundOffSlot();
    keyThread->put_off_all_light();
    keyThread->Delay(50);
    keyThread->put_on_selfchecklight();

    QTimer *selfcheckTimer= new QTimer(this);
    selfcheckTimer->setInterval(15000);//12s
    selfcheckTimer->setSingleShot(true);


    selfcheckTimer->start();
    if(polling->isRunning())
    {
        polling->stopPolling();
    }

    QSplashScreen *backgroundsplash = new QSplashScreen;
    backgroundsplash->setPixmap(QPixmap(":/image/background-zijian.png"));
    backgroundsplash->show();

    QSplashScreen *backgroundsplash2 = new QSplashScreen;
    backgroundsplash2->setPixmap(QPixmap(":/image/background-zijian.png"));



    QSplashScreen *whitesplash = new QSplashScreen;
    whitesplash->setPixmap(QPixmap(":/image/white.PNG"));
//    whitesplash->show();


    QSplashScreen *blacksplash = new QSplashScreen;
    blacksplash->setPixmap(QPixmap(":/image/black.PNG"));
    //blacksplash->show();

    QTimer *selfcheckTimer0= new QTimer(backgroundsplash);
    selfcheckTimer0->setInterval(500);//2s
    selfcheckTimer0->setSingleShot(true);

    QTimer *selfcheckTimer1= new QTimer(whitesplash);
    selfcheckTimer1->setInterval(2000);//2s
    selfcheckTimer1->setSingleShot(true);


    QTimer *selfcheckTimer2= new QTimer(blacksplash);
    selfcheckTimer2->setInterval(2000);//2s
    selfcheckTimer2->setSingleShot(true);

    QTimer *selfcheckTimer3= new QTimer(this);
    selfcheckTimer3->setInterval(2000);//2s
    selfcheckTimer3->setSingleShot(true);

    QTimer *selfcheckTimer4= new QTimer(backgroundsplash2);
    selfcheckTimer4->setInterval(11000);//2s
    selfcheckTimer4->setSingleShot(true);



//    selfcheckTimer1->start();
    selfcheckTimer0->start();


    QObject::connect(selfcheckTimer0,SIGNAL(timeout()), whitesplash,SLOT(show()));
    QObject::connect(selfcheckTimer0,SIGNAL(timeout()), backgroundsplash,SLOT(deleteLater()));
    QObject::connect(selfcheckTimer0,SIGNAL(timeout()), selfcheckTimer1,SLOT(start()));


    QObject::connect(selfcheckTimer1,SIGNAL(timeout()), blacksplash,SLOT(show()));

    QObject::connect(selfcheckTimer1,SIGNAL(timeout()), whitesplash,SLOT(deleteLater()));



    QObject::connect(selfcheckTimer1,SIGNAL(timeout()), selfcheckTimer2,SLOT(start()));
    QObject::connect(selfcheckTimer2,SIGNAL(timeout()), blacksplash,SLOT(deleteLater()));
    QObject::connect(selfcheckTimer2,SIGNAL(timeout()), backgroundsplash2,SLOT(show()));
    QObject::connect(selfcheckTimer2,SIGNAL(timeout()), selfcheckTimer4,SLOT(start()));
    QObject::connect(selfcheckTimer4,SIGNAL(timeout()), backgroundsplash2,SLOT(deleteLater()));

    QObject::connect(selfcheckTimer2,SIGNAL(timeout()), keyThread,SLOT(put_on_all_light()));
    QObject::connect(selfcheckTimer2,SIGNAL(timeout()), selfcheckTimer3,SLOT(start()));
    QObject::connect(selfcheckTimer2,SIGNAL(timeout()), this,SLOT(sendSelfcheckFrameOnce()));

    QObject::connect(selfcheckTimer3,SIGNAL(timeout()), keyThread,SLOT(put_off_all_light()));
    QObject::connect(selfcheckTimer3,SIGNAL(timeout()), keyThread,SLOT(put_on_selfchecklight()));


    QObject::connect(selfcheckTimer3,SIGNAL(timeout()), keyThread,SLOT(sound_take_turn_on()));


    QObject::connect(selfcheckTimer,SIGNAL(timeout()), polling,SLOT(start()));
    QObject::connect(selfcheckTimer,SIGNAL(timeout()), keyThread,SLOT(put_off_selfchecklight()));

    //         QObject::connect(selfcheckTimer,SIGNAL(timeout()), keyThread,SLOT(allSoundOffSlot()));
    QObject::connect(selfcheckTimer,SIGNAL(timeout()), keyThread,SLOT(put_on_the_lights_before_selfcheck()));
    QObject::connect(selfcheckTimer,SIGNAL(timeout()), keyThread,SLOT(start()));
    QObject::connect(selfcheckTimer,SIGNAL(timeout()), keyThread->oneSecondTimer,SLOT(start()));
    QObject::connect(selfcheckTimer,SIGNAL(timeout()), this,SLOT(setSelfcheckFlagFalseSlot()));

}

void MainWidget::setSelfcheckFlagFalseSlot()
{
    selfcheckFlag = false;
}
void MainWidget::setLockkeyFlagFalseSlot()
{
    lockkeyFlag = false;
}
void MainWidget::setLockkeyFlagTrueSlot()
{
    lockkeyFlag = true;
}

void MainWidget::sendSelfcheckFrameOnce()
{
    int can_fd = ::open("/dev/can", O_RDWR);
    if(can_fd < 0)
    {
        qDebug()<<"in sendSelfcheckFrameOnce: can't open can_dev, can_fd = "<< can_fd <<endl;
        return;
    }
    else
        qDebug() <<"in sendSelfcheckFrameOnce: open  can_dev successfully! can_fd = "<< can_fd <<endl;
    sendSelfCheckFrame(can_fd);
    ::close(can_fd);

}

void MainWidget::ChangeWidgettimer30sTimeoutSlot()
{
    QWidget *nowActiveQwidget =  static_cast<QWidget *>(QApplication::activeWindow());
    QDialog *nowActiveQDialog =  static_cast<QDialog *>(QApplication::activeWindow());
    if((changeWindowFlag) && (nowActiveQwidget != this)&&(nowActiveQDialog != eventdialog))
    {
        this->testnowwidget(nowActiveQwidget);
    }

}

void MainWidget::testnowwidget(QWidget *nowQwidget)
{
    if( nowQwidget == this)
    {
        //do nothing
        // qDebug() <<"QApplication::activeWindow()->windowTitle()";
    }
    else
    {
        this->testnowwidget(nowQwidget->parentWidget());
        nowQwidget->close();
    }
}

void MainWidget::restartChangeWidgettimer30sSlot()
{
    //restart the qtimer
    changeWidgettimer30s->stop();
    changeWidgettimer30s->start();
}






