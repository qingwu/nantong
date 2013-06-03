#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <QtGui/QFrame>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTableWidget>
#include <QtGui/QKeyEvent>
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtGui/QLineEdit>
#include <QtDebug>

#include "menuwindow.h"
#include "timesetdialog.h"
#include "hostsetdialog.h"
#include "unitsetdialog.h"
#include "bussetdialog.h"
#include "netsetdialog.h"
#include "serialtestdialog.h"
#include "linkdialog.h"
#include "soundtestdialog.h"
#include "singletestdialog.h"
#include "nettestdialog.h"
#include "link.h"
#include "printerdialog.h"
#include "operationhis.h"
#include "shieldwidget.h"

#define WINDOW_STYTLE

MenuWindow::MenuWindow(QWidget *parent)
    : QDialog(parent)
{
    printerFlagOfWindow = 0x01010101;
    linkwin = new  LinkDialog(true,this);
    //linkwin2 = new LinkDialog(false,this);
    nowLink = new Link();
    connect(linkwin,SIGNAL(transferLinknum(int,int)),nowLink,SLOT(updatelinknum(int,int)));
    //connect(nowLink,SIGNAL(tranferLinkout(int,int,int,int ,int)),this,SIGNAL(linkComeSignal(int,int,int,int ,int)));
    connect(nowLink,SIGNAL(transferLinkNode(int)),this,SIGNAL(linkNodeSignal(int)));

    connect(this,SIGNAL(transferEvent(int,int,int,unsigned char,unsigned char)),nowLink,SLOT(DealwithFire(int,int,int,unsigned char,unsigned char)));

    createActions();
    createMenus();
    createStatusBar();
    addGroupButtons();
    timerEvent(0);
    startTimer(1000);
    setWindowTitle (tr("火灾报警控制器-设置菜单窗口"));

#ifndef WINDOW_STYTLE
    setWindowFlags(windowFlags() &~Qt::WindowMinMaxButtonsHint);
    setWindowFlags(windowFlags() &~Qt::WindowCloseButtonHint);
    this->resize(QSize(800, 480));
#else
    this->setFixedSize (QSize(800, 600));
#endif
    //connect(registerSetButton,SIGNAL(clicked()),this,SIGNAL(regButtonClicked()));
    connect(this, SIGNAL(hidesetshowview()), this->parentWidget(), SIGNAL(hidemainshowview()));
    connect(this,SIGNAL(unitSetDialogClose(int)),this->parentWidget(),SLOT(updateTabWidget(int)));
}

MenuWindow::~MenuWindow()
{
    //delete(this);
}

void MenuWindow::createActions()
{
    hideAction = new QAction (this);
    hideAction->setShortcut (/*Qt::CTRL+*/Qt::Key_F1);
    connect(hideAction, SIGNAL(triggered()), this, SLOT(hideWindow()));
    secondhideAction = new QAction (this);
    secondhideAction->setShortcut (/*Qt::CTRL+*/Qt::Key_F3);
    connect(secondhideAction, SIGNAL(triggered()), this, SLOT(secondhideWindow()));
    this->addAction(hideAction);
    this->addAction(secondhideAction);
    /*systemSetMenu*/
    unitSetAction = new QAction (tr("&单元设置"), this);
    unitSetAction->setStatusTip (tr("设置模块/单元"));

    connect(unitSetAction, SIGNAL(triggered()), this, SLOT(unitSet()));

    busSetAction = new QAction (tr("&总线设置"), this);
    busSetAction->setStatusTip (tr("设置总线"));
    connect(busSetAction, SIGNAL(triggered()), this, SLOT(busSet()));

    linkSetAction = new QAction (tr("&联动设置"), this);
    linkSetAction->setStatusTip (tr("设置联动"));
    connect(linkSetAction, SIGNAL(triggered()), this, SLOT(linkSet()));

    printerSetAction = new QAction (tr("&打印设置"), this);
    printerSetAction->setStatusTip (tr("打印设置"));
    connect(printerSetAction, SIGNAL(triggered()), this, SLOT(printerSet()));

    networkSetAction = new QAction (tr("&联网设置"), this);
    networkSetAction->setStatusTip (tr("设置联网"));
    connect(networkSetAction, SIGNAL(triggered()), this, SLOT(networkSet()));

    hostSetAction = new QAction (tr("&本机设置"), this);
    hostSetAction->setStatusTip (tr("设置此机"));
    connect(hostSetAction, SIGNAL(triggered()), this, SLOT(hostSet()));

    timeSetAction = new QAction (tr("&时间设置"), this);
    timeSetAction->setStatusTip (tr("设置时间"));
    connect(timeSetAction, SIGNAL(triggered()), this, SLOT(timeSet()));

    registerSetAction = new QAction (tr("&登记设置"), this);
    registerSetAction->setStatusTip (tr("设置登记"));
    //connect( registerSetAction, SIGNAL(triggered()), this, SIGNAL(regButtonClicked()));

    shieldSetAction = new QAction (tr("&屏蔽设置"), this);
       shieldSetAction->setStatusTip (tr("设置屏蔽"));
       connect(shieldSetAction, SIGNAL(triggered()), this, SLOT(shieldSet()));

    /*systemDebugMenu*/
    singleTestAction = new QAction (tr("&单步测试"), this);
    singleTestAction->setStatusTip (tr("单步测试"));
    connect(singleTestAction, SIGNAL(triggered()), this, SLOT(singleTest()));

    soundTestAction = new QAction (tr("&声光测试"), this);
    soundTestAction->setStatusTip (tr("声光测试"));
    connect(soundTestAction, SIGNAL(triggered()), this, SLOT(soundTest()));

    serialTestAction = new QAction (tr("&串口测试"), this);
    serialTestAction->setStatusTip (tr("串口测试"));
    connect(serialTestAction, SIGNAL(triggered()), this, SLOT(serialTest()));

    networkTestAction = new QAction (tr("&联网测试"), this);
    networkTestAction->setStatusTip (tr("联网测试"));
    connect(networkTestAction, SIGNAL(triggered()), this, SLOT(networkTest()));

    logicTestAction = new QAction (tr("&逻辑测试"), this);
    logicTestAction->setStatusTip (tr("逻辑测试"));
    connect(logicTestAction, SIGNAL(triggered()), this, SLOT(logicTest()));
}

void MenuWindow::createMenus()
{
    menuBar = new QMenuBar (this);
    /*已经设置menuBar为this的子控件，默认起点为QPoint(0, 0)
      *故此处没有setGeometry(QRect(0, 0, 800, 24));
      *设置其父类为this，那么也不用addWidget()了
      */
    systemSetMenu = menuBar->addMenu(tr("&系统设置"));
    systemSetMenu->addAction(unitSetAction);
    systemSetMenu->addAction(busSetAction);
    systemSetMenu->addSeparator();
    systemSetMenu->addAction(linkSetAction);
    systemSetMenu->addAction(printerSetAction);
    systemSetMenu->addAction(networkSetAction);
    systemSetMenu->addSeparator();
    systemSetMenu->addAction(hostSetAction);
    systemSetMenu->addAction(timeSetAction);
    systemSetMenu->addAction(registerSetAction);
        systemSetMenu->addAction(shieldSetAction);

    systemDebugMenu = menuBar->addMenu(tr("&系统调试"));
    systemDebugMenu->addAction(singleTestAction);
    systemDebugMenu->addAction(soundTestAction);
    systemDebugMenu->addAction(serialTestAction);
    systemDebugMenu->addAction(networkTestAction);
    systemDebugMenu->addAction(logicTestAction);
}

void MenuWindow::createStatusBar()
{
    statusBar = new QStatusBar (this);
    statusBar->setGeometry(QRect(0, 576, 800, 24));

    remindLabel = new QLabel (tr("按下F1显示事件窗口，按下F3显示查看窗口"), statusBar);
    remindLabel->setAlignment (Qt::AlignLeft);
    remindLabel->setMinimumSize (remindLabel->sizeHint());

    timeLabel = new QLabel (tr("2012-05-04  00:00:00"), statusBar);
    timeLabel->setAlignment (Qt::AlignCenter);
    timeLabel->setMaximumSize(timeLabel->sizeHint());
    /*此处QPoint(650, 5).y = 5，因为子控件的y必须相对于父控件而言*/
    timeLabel->setGeometry(QRect(QPoint (650, 5), timeLabel->sizeHint()));
    timeLabel->setFrameShape(QFrame::Box);
}

void MenuWindow::addGroupButtons()
{
    buttonFrame = new QFrame(this);
    buttonFrame->setGeometry(QRect(0, 30 ,800, 540));
    buttonFrame->setFrameStyle(QFrame::Box);


    unitSetButton = new QPushButton (tr("&1 单元设置"), this);
    unitSetButton->setGeometry(QRect(10, 60, 100, 24));
    busSetButton = new QPushButton (tr("&2 总线设置"), this);
    busSetButton->setGeometry(QRect(10, 120, 100, 24));
    linkSetButton = new QPushButton (tr("&3 联动设置"), this);
    linkSetButton->setGeometry(QRect(10, 180, 100, 24));
    printerSetButton = new QPushButton (tr("&4 打印设置"), this);
    printerSetButton->setGeometry(QRect(10, 240, 100, 24));
    networkSetButton = new QPushButton (tr("&5 联网设置"), this);
    networkSetButton->setGeometry(QRect(10 , 300, 100, 24));
    hostSetButton = new QPushButton (tr("&6 本机设置"), this);
    hostSetButton->setGeometry(QRect(10 , 360, 100, 24));
    timeSetButton = new QPushButton (tr("&7 时间设置"), this);
    timeSetButton->setGeometry(QRect(10 , 420, 100, 24));
    registerSetButton = new QPushButton (tr("&8 登记设置"), this);
    registerSetButton->setGeometry(QRect(10 , 480, 100, 24));
    shieldSetButton = new QPushButton (tr("&9 屏蔽设置"), this);
    shieldSetButton->setGeometry(QRect(10 , 540, 100, 24));



    singleTestButton = new QPushButton (tr("&1 单步测试"), this);
    singleTestButton->setGeometry(QRect(250 , 60, 100, 24));
    soundTestButton = new QPushButton (tr("&2 声光测试"), this);
    soundTestButton->setGeometry(QRect(250 , 120, 100, 24));
    serialTestButton = new QPushButton (tr("&3 串口测试"), this);
    serialTestButton->setGeometry(QRect(250 , 180, 100, 24));
    networkTestButton = new QPushButton (tr("&4 联网测试"), this);
    networkTestButton->setGeometry(QRect(250 , 240, 100, 24));
    logicTestButton = new QPushButton (tr("&5 逻辑测试"), this);
    logicTestButton->setGeometry(QRect(250 , 300, 100, 24));
    /*Connect*/
    connect(unitSetButton, SIGNAL(clicked()), this, SLOT(unitSet()));
    connect(busSetButton, SIGNAL(clicked()), this, SLOT(busSet()));
    connect(linkSetButton, SIGNAL(clicked()), this, SLOT(linkSet()));
    connect(printerSetButton, SIGNAL(clicked()), this, SLOT(printerSet()));
    connect(networkSetButton, SIGNAL(clicked()), this, SLOT(networkSet()));
    connect(hostSetButton, SIGNAL(clicked()), this, SLOT(hostSet()));
    connect(timeSetButton, SIGNAL(clicked()), this, SLOT(timeSet()));
        connect(shieldSetButton, SIGNAL(clicked()), this, SLOT(shieldSet()));

    connect(singleTestButton, SIGNAL(clicked()), this, SLOT(singleTest()));
    connect(soundTestButton, SIGNAL(clicked()), this, SLOT(soundTest()));
    connect(serialTestButton, SIGNAL(clicked()), this, SLOT(serialTest()));
    connect(networkTestButton, SIGNAL(clicked()), this, SLOT(networkTest()));
    connect(logicTestButton, SIGNAL(clicked()), this, SLOT(logicTest()));
}
void MenuWindow::timerEvent (QTimerEvent *)// *event)
{
    QDateTime dt = QDateTime::currentDateTime();
    timeLabel->setText(dt.toString("yyyy-MM-dd hh:mm:ss"));
}

void MenuWindow::hideWindow()
{
    this->hide();
    this->parentWidget()->show();
}
void MenuWindow::secondhideWindow()
{
    this->hide();
    this->parentWidget()->show();
    emit hidesetshowview();
}
void MenuWindow::unitSet()
{
    OperationHis::Update("进入单元设置界面");
    UnitSetDialog unitset(true,this);
    unitset.exec();
        emit unitSetDialogClose(3);
    unitset.move(Qt::AlignTop,Qt::AlignLeft);
}
void MenuWindow::busSet()
{
    OperationHis::Update("进入总线设置界面");
    BusSetDialog busset(true,this);
    busset.exec();
    busset.move(0,0);
}

void MenuWindow::linkSet()
{
    OperationHis::Update("进入联动设置界面");
    linkwin->readLinktype(linkwin->numLineedit->text().toInt());
    linkwin->readUnitset();
    linkwin->nowcopynum = 1;
    linkwin->exec();
}
void MenuWindow::printerSet()
{
    OperationHis::Update("进入打印设置界面");
    printerDialog printset(printerFlagOfWindow, this);//printerDialog printset(this);
    printset.exec();
}
void MenuWindow::getprinterFlag(int flag)//yang ju
{
    printerFlagOfWindow = flag;
    qDebug("in MenuWindow: printerFlagOfWindow is : %x",printerFlagOfWindow);
}
void MenuWindow::networkSet()
{
    OperationHis::Update("进入网络设置界面");
    NetSetDialog netset(true,this);
    netset.exec();
}
void MenuWindow::hostSet()
{
    OperationHis::Update("进入主机设置界面");
    HostSetDialog host(this);
    host.exec();
}
void MenuWindow::timeSet()
{
    OperationHis::Update("进入时间设置界面");
    TimeSetDialog time(this);
    time.exec();
}
void MenuWindow::shieldSet()
{
    OperationHis::Update("进入屏蔽设置界面");
    ShieldWidget shield(this);
    shield.exec();

}

void MenuWindow::singleTest()
{
    singleTestDialog singlewin(this);
    singlewin.exec();
}
void MenuWindow::soundTest()
{
    soundTestDialog soundwin(this);
    soundwin.exec();
}
void MenuWindow::serialTest()
{
    serialTestDialog serialPortTest(this);
    serialPortTest.exec();
}
void MenuWindow::networkTest()
{
    netTestDialog netTest(this);
    netTest.exec();
}
void MenuWindow::logicTest()
{
}
void MenuWindow::nowLinkClear()
{
    nowLink->Clear();
}
