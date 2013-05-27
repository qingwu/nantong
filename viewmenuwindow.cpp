#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <QtGui/QFrame>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>
#include <QtGui/QTableWidget>
#include <QtGui/QKeyEvent>
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtDebug>

#include "viewmenuwindow.h"
#include "aboutdialog.h"
#include "historydialog.h"
#include "unitsetdialog.h"
#include "bussetdialog.h"
#include "netsetdialog.h"
#include "linkdialog.h"
#include "passworddialog.h"
#include "reginfodialog.h"
#include "operationhis.h"

ViewMenuWindow::ViewMenuWindow(QWidget *parent) :
    QDialog(parent)
{
    linkwin2 = new LinkDialog(false,this);
    createActions();
    createMenus();
    createStatusBar();
    addGroupButtons();
    timerEvent(0);
    startTimer(1000);
    setWindowTitle(tr("火灾报警控制器-查看菜单窗口"));
    this->setFixedSize (QSize(800, 600));
    connect(this, SIGNAL(hideviewshowset()), this->parentWidget(), SIGNAL(hidemainshowset()));
}
ViewMenuWindow::~ViewMenuWindow()
{
}
void ViewMenuWindow::createActions()
{
    hideAction = new QAction (this);
    hideAction->setShortcut (/*Qt::CTRL+*/Qt::Key_F1);
    connect(hideAction, SIGNAL(triggered()), this, SLOT(hideWindow()));

    secondhideAction = new QAction (this);
    secondhideAction->setShortcut (/*Qt::CTRL+*/Qt::Key_F2);
    connect(secondhideAction, SIGNAL(triggered()), this, SLOT(secondhideWindow()));
    this->addAction(hideAction);
    this->addAction(secondhideAction);

    /*systemInfoMenu*/
    unitInfoAction = new QAction (tr("&单元信息"), this);
    unitInfoAction->setStatusTip (tr("查询单元信息"));
    connect(unitInfoAction, SIGNAL(triggered()), this, SLOT(unitInfo()));

    busInfoAction = new QAction (tr("&总线信息"), this);
    busInfoAction->setStatusTip (tr("查询总线信息"));
    connect(busInfoAction, SIGNAL(triggered()), this, SLOT(busInfo()));

    linkInfoAction = new QAction (tr("&联动信息"), this);
    linkInfoAction->setStatusTip (tr("查询联动信息"));
    connect(linkInfoAction, SIGNAL(triggered()), this, SLOT(linkInfo()));

    locationInfoAction = new QAction (tr("&位置信息"), this);
    locationInfoAction->setStatusTip (tr("查询位置信息"));
    connect(locationInfoAction, SIGNAL(triggered()), this, SLOT(locationInfo()));

    networkInfoAction = new QAction (tr("&联网信息"), this);
    networkInfoAction->setStatusTip (tr("查询联网信息"));
    connect(networkInfoAction, SIGNAL(triggered()), this, SLOT(networkInfo()));

    /*historySeekMenu*/
    allHisAction = new QAction (tr("&所有历史"), this);
    allHisAction->setStatusTip (tr("查询所有历史"));
    connect(allHisAction, SIGNAL(triggered()), this, SLOT(allHis()));

    fireHisAction = new QAction (tr("&历史火警"), this);
    fireHisAction->setStatusTip (tr("查询历史火警"));
    connect(fireHisAction, SIGNAL(triggered()), this, SLOT(fireHis()));

    faultHisAction = new QAction (tr("&历史故障"), this);
    faultHisAction->setStatusTip (tr("查询历史故障"));
    connect(faultHisAction, SIGNAL(triggered()), this, SLOT(faultHis()));

    linkHisAction = new QAction (tr("&历史联动"), this);
    linkHisAction->setStatusTip (tr("查询历史联动"));
    connect(linkHisAction, SIGNAL(triggered()), this, SLOT(linkHis()));

    feedbackHisAction = new QAction (tr("&历史反馈"), this);
    feedbackHisAction->setStatusTip (tr("查询历史反馈"));
    connect(feedbackHisAction, SIGNAL(triggered()), this, SLOT(feedbackHis()));

    operateHisAction = new QAction (tr("&历史操作"), this);
    operateHisAction->setStatusTip (tr("查询历史操作"));
    connect(operateHisAction, SIGNAL(triggered()), this, SLOT(operateHis()));

    /*aboutMenu*/
    systemAboutAction = new QAction (tr("&关于系统"), this);
    systemAboutAction->setStatusTip (tr("查询系统信息"));
    connect(systemAboutAction, SIGNAL(triggered()), this, SLOT(systemAbout()));

    runAboutAction = new QAction (tr("&运行信息"), this);
    runAboutAction->setStatusTip (tr("查询运行信息"));
    connect(runAboutAction, SIGNAL(triggered()), this, SLOT(runAbout()));
}
void ViewMenuWindow::createMenus()
{
    menuBar = new QMenuBar (this);
    systemInfoMenu = menuBar->addMenu(tr("&系统信息"));
    systemInfoMenu->addAction(unitInfoAction);
    systemInfoMenu->addAction(busInfoAction);
    systemInfoMenu->addSeparator();
    systemInfoMenu->addAction(linkInfoAction);
    systemInfoMenu->addAction(locationInfoAction);
    systemInfoMenu->addAction(networkInfoAction);

    historySeekMenu  = menuBar->addMenu(tr("&历史查询"));
    historySeekMenu->addAction(allHisAction);//new add
    historySeekMenu->addAction(fireHisAction);
    historySeekMenu->addAction(faultHisAction);
    historySeekMenu->addAction(linkHisAction);
    historySeekMenu->addAction(feedbackHisAction);
    historySeekMenu->addAction(operateHisAction);

    aboutMenu = menuBar->addMenu(tr("&关于"));
    aboutMenu->addAction(systemAboutAction);
    aboutMenu->addAction(runAboutAction);
}
void ViewMenuWindow::createStatusBar()
{
    statusBar = new QStatusBar (this);
    statusBar->setGeometry(QRect(0, 576, 800, 24));

    remindLabel = new QLabel (tr("按下F1显示事件窗口，按下F2显示设置窗口"), statusBar);
    remindLabel->setAlignment (Qt::AlignLeft);
    remindLabel->setMinimumSize (remindLabel->sizeHint());

    timeLabel = new QLabel (tr("2012-05-04  00:00:00"), statusBar);
    timeLabel->setAlignment (Qt::AlignCenter);
    timeLabel->setMaximumSize(timeLabel->sizeHint());
    /*此处QPoint(650, 5).y = 5，因为子控件的y必须相对于父控件而言*/
    timeLabel->setGeometry(QRect(QPoint (650, 5), timeLabel->sizeHint()));
    timeLabel->setFrameShape(QFrame::Box);
}
void ViewMenuWindow::addGroupButtons()
{
    buttonFrame = new QFrame(this);
    buttonFrame->setGeometry(QRect(0, 30 ,800, 540));
    buttonFrame->setFrameStyle(QFrame::Box);


    unitInfoButton = new QPushButton (tr("&1 单元信息"), this);
    unitInfoButton->setGeometry(QRect(10 , 60, 100, 24));
    busInfoButton = new QPushButton (tr("&2 总线信息"), this);
    busInfoButton->setGeometry(QRect(10 , 120, 100, 24));
    linkInfoButton = new QPushButton (tr("&3 联动信息"), this);
    linkInfoButton->setGeometry(QRect(10 , 180, 100, 24));
    locationInfoButton = new QPushButton (tr("&4 位置信息"), this);
    locationInfoButton->setGeometry(QRect(10, 240, 100, 24));
    networkInfoButton = new QPushButton (tr("&4 联网信息"), this);
    networkInfoButton->setGeometry(QRect(10 , 300, 100, 24));

    allHisButton = new QPushButton (tr("&1 所有历史"), this);
    allHisButton->setGeometry(QRect(250 , 60, 100, 24));
    fireHisButton = new QPushButton (tr("&2 历史火警"), this);
    fireHisButton->setGeometry(QRect(250 , 120, 100, 24));
    faultHisButton = new QPushButton (tr("&3 历史故障"), this);
    faultHisButton->setGeometry(QRect(250 , 180, 100, 24));
    linkHisButton = new QPushButton (tr("&4 历史联动"), this);
    linkHisButton->setGeometry(QRect(250 , 240, 100, 24));
    feedbackHisButton = new QPushButton (tr("&5 历史反馈"), this);
    feedbackHisButton->setGeometry(QRect(250 ,300, 100, 24));
    operateHisButton = new QPushButton (tr("&6 历史操作"), this);
    operateHisButton->setGeometry(QRect(250 , 360, 100, 24));

    systemAboutButton = new QPushButton (tr("&1 关于系统"), this);
    systemAboutButton->setGeometry(QRect(490 , 60, 100, 24));
    runAboutButton = new QPushButton (tr("&2 运行信息"), this);
    runAboutButton->setGeometry(QRect(490 , 120, 100, 24));
    /*Connect*/
    connect(unitInfoButton, SIGNAL(clicked()), this, SLOT(unitInfo()));
    connect(busInfoButton, SIGNAL(clicked()), this, SLOT(busInfo()));
    connect(linkInfoButton, SIGNAL(clicked()), this, SLOT(linkInfo()));
    connect(locationInfoButton, SIGNAL(clicked()), this, SLOT(locationInfo()));
    connect(networkInfoButton, SIGNAL(clicked()), this, SLOT(networkInfo()));

    connect(allHisButton, SIGNAL(clicked()), this, SLOT(allHis()));
    connect(fireHisButton, SIGNAL(clicked()), this, SLOT(fireHis()));
    connect(faultHisButton, SIGNAL(clicked()), this, SLOT(faultHis()));
    connect(linkHisButton, SIGNAL(clicked()), this, SLOT(linkHis()));
    connect(feedbackHisButton, SIGNAL(clicked()), this, SLOT(feedbackHis()));
    connect(operateHisButton, SIGNAL(clicked()), this, SLOT(operateHis()));

    connect(systemAboutButton, SIGNAL(clicked()), this, SLOT(systemAbout()));
    connect(runAboutButton, SIGNAL(clicked()), this, SLOT(runAbout()));
}
void ViewMenuWindow::timerEvent (QTimerEvent *)// *event)
{
    QDateTime dt = QDateTime::currentDateTime();
    timeLabel->setText(dt.toString("yyyy-MM-dd hh:mm:ss"));
}

void ViewMenuWindow::hideWindow()
{
    this->hide();
    this->parentWidget()->show();
}

void ViewMenuWindow::secondhideWindow()
{
    PasswordDialog password(2, this);
    password.exec();
    if (password.matchFlag)
    {
        this->hide();
        this->parentWidget()->show();
        emit hideviewshowset();
    }
}

void ViewMenuWindow::unitInfo()
{
    OperationHis::Update("进入单元查询界面");
    UnitSetDialog unitinfoWin(false,this);
    unitinfoWin.exec();
}
void ViewMenuWindow::busInfo()
{
    OperationHis::Update("进入总线查询界面");
    BusSetDialog busset(false,this);
    busset.exec();
}

void ViewMenuWindow::locationInfo()
{
}
void ViewMenuWindow::networkInfo()
{
    NetSetDialog netinfo(false,this);
    netinfo.exec();
}
void ViewMenuWindow::linkInfo()
{
    OperationHis::Update("进入联动查询界面");
    linkwin2->readLinktype(linkwin2->numLineedit->text().toInt());
    linkwin2->readUnitset();
    linkwin2->show();
}

void ViewMenuWindow::allHis()
{
    OperationHis::Update("进入所有历史查询界面");
    HistoryDialog history(0, this);
    history.exec();
}
void ViewMenuWindow::fireHis()
{
    OperationHis::Update("进入火警历史查询界面");
    HistoryDialog history(1, this);
    history.exec();//Modal dialog
}
void ViewMenuWindow::faultHis()
{
    OperationHis::Update("进入故障历史查询界面");
    HistoryDialog history(2, this);
    history.exec();//Modal dialog
}
void ViewMenuWindow::linkHis()
{
    OperationHis::Update("进入联动历史查询界面");
    HistoryDialog history(3, this);
    history.exec();//Modal dialog
}
void ViewMenuWindow::feedbackHis()
{
    HistoryDialog history(4, this);
    history.exec();//Modal dialog
}
void ViewMenuWindow::operateHis()
{
    HistoryDialog history(5, this);
    history.exec();//Modal dialog
}

void ViewMenuWindow::systemAbout()
{
    OperationHis::Update("进入系统信息界面");
    AboutDialog about(this);
    about.exec();
}
void ViewMenuWindow::runAbout()
{
    OperationHis::Update("进入运行信息查询界面");
    RegInfoDialog runabout(this);
    runabout.exec();

}
