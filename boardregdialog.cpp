#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QFont>
#include <QtGui/QMessageBox>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtCore/QVariant>
#include <QValidator>
#include <QAction>
#include <QDebug>
#include <QTimer>

#include "boardregdialog.h"

BoardRegDialog::BoardRegDialog(QWidget *parent)
    : QDialog(parent)
{
    pollingStartTimer15s = new QTimer(this);
    pollingStartTimer15s->setInterval(15000);//15s
    pollingStartTimer15s->setSingleShot(true);
    connect(pollingStartTimer15s,SIGNAL(timeout()),this,SIGNAL(cancelPushButtonClicked()));

    isSingleRegFlag = false;
    this->setFixedSize(400,240);
    setWindowTitle(tr("接口板登记"));
    infoLabel = new QLabel(this);
    infoLabel->setText(tr("是否重新登记？\n如果不需要重新登记，请点击“取消”，开始巡检\n"));
    infoLabel->setAlignment(Qt::AlignCenter);
    QFont f("unifont", 12, QFont::Light);
    infoLabel->setFont(f);
    okPushButton = new QPushButton(tr("确定"),this);
    cancelPushButton = new QPushButton(tr("取消"),this);
    infoLabel->setGeometry(QRect(20,40,350,60));
    okPushButton->setGeometry(QRect(100,150,80,35));
    cancelPushButton->setGeometry(QRect(200,150,80,35));

    okToPollingButton = new QPushButton(tr("确定"),this);
    okToPollingButton->setGeometry(QRect(160,150,80,35));
    okToPollingButton->setVisible(false);

    backgroundLabel = new QLabel(this->parentWidget());
    backgroundLabel->setPixmap(QPixmap(":/image/background.PNG"));
    backgroundLabel->setGeometry(QRect(0,0 ,800,600));
    backgroundLabel->setVisible(false);

    timeLabel = new QLabel(this);
    timeLabel->setText(tr("%1秒后将跳过登记，开始巡检").arg(15));
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setFont(f);
    timeLabel->setGeometry(QRect(20,105,350,30));
    timeLeft = 15;


    timer1s = new QTimer(this);
    timer1s->setInterval(1000);//1s
    connect(timer1s,SIGNAL(timeout()),this,SLOT(updateTimeLabelSlot()));


    createEscapeAction();
    createSingleBoardDialog();
    setSignalVisibleFalse();

    connect(this,SIGNAL(cancelPushButtonClicked()),this,SLOT(stopTimerSlot()));
    connect(this,SIGNAL(okPushButtonClicked()),this,SLOT(stopTimerSlot()));
    connect(okPushButton,SIGNAL(clicked()),this,SIGNAL(okPushButtonClicked()));
    connect(cancelPushButton,SIGNAL(clicked()),this,SIGNAL(cancelPushButtonClicked()));
    connect(okToPollingButton,SIGNAL(clicked()),this,SIGNAL(regAllOverSignal()));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
}
void BoardRegDialog::stopTimerSlot()
{
    timeLabel->setVisible(false);
    pollingStartTimer15s->stop();
    timer1s->stop();
}
void BoardRegDialog::statrTimer15s()
{
    timer1s->start();
    pollingStartTimer15s->start();
    qDebug()<<"in boardRegDialog:15stimer start";
}
void BoardRegDialog::updateTimeLabelSlot()
{
    timeLeft -= 1;
    timeLabel->setVisible(true);    
    timeLabel->setText(tr("%1秒后将跳过登记，开始巡检").arg(timeLeft));//setText(QString(tr()));    

}

void BoardRegDialog::regStartMessage()
{
    //qDebug()<<"*************************************reg-start-message";
    //if(isSingleRegFlag)//不管是单接口登记还是全体接口板登记，都隐藏
    setSignalVisibleFalse();
    infoLabel->setText("接口板登记中...");
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setVisible(true);
    okPushButton->setVisible(false);
    cancelPushButton->setVisible(false);
    okToPollingButton->setVisible(false);
}

void BoardRegDialog::regFaultMessage()
{
    QMessageBox *box = new QMessageBox(tr("接口板登记"),
                                       tr("登记出错：can设备打开失败..."),
                                       QMessageBox::Information,
                                       QMessageBox::Ok,
                                       QMessageBox::NoButton,
                                       QMessageBox::NoButton,
                                       this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
    box->exec();
}

void BoardRegDialog::currentRegBoardSlot(int i)
{
    //qDebug()<<"*****************************currentRegBoardSlot";
    infoLabel->setVisible(true);
    infoLabel->setText(tr("%1 号接口板登记中...").arg(i));
    infoLabel->setAlignment(Qt::AlignCenter);
    if(isSingleRegFlag)
    {
        setSignalVisibleFalse();
    }
}
void BoardRegDialog::setSignalVisibleFalse()
{
    singleRegInfoLabel->setVisible(false);
    allRegInfoLabel->setVisible(false);
    singleBoardNumLabel->setVisible(false);
    singleBoardNumLineEdit->setVisible(false);
    singleOkPushButton->setVisible(false);
    allRegPushButton->setVisible(false);
    singleCancelPushButton->setVisible(false);

}
void BoardRegDialog::setSignalVisibleTrue()
{
    singleRegInfoLabel->setVisible(true);
    allRegInfoLabel->setVisible(true);
    singleBoardNumLabel->setVisible(true);
    singleBoardNumLineEdit->setVisible(true);
    singleOkPushButton->setVisible(true);
    allRegPushButton->setVisible(true);
    singleCancelPushButton->setVisible(true);

}
void BoardRegDialog::regOverSlot(int boardRegNum)
{
    //qDebug()<<"****************************************regOverSlot";
    infoLabel->setText(tr("登记结果:共%1个接口板在线\n""按下 确定 开始巡检...").arg(boardRegNum));
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setVisible(true);
    okPushButton->setVisible(false);
    cancelPushButton->setVisible(false);
    okToPollingButton->setVisible(true);
    okToPollingButton->setFocus();
}

void BoardRegDialog::singleRegOverSlot(int boardnum)
{
    //qDebug()<<"****************************************singleRegOverSlot";
    setSignalVisibleFalse();

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from iobrinfo WHERE id = %1").arg(boardnum));
    while (query.next())
    {
        if(query.value(1).toBool())
            infoLabel->setText(tr("%1号接口板登记结束,接口板在线\n""按下 确定 开始巡检").arg(boardnum));
        else
            infoLabel->setText(tr("%1号接口板登记结束,接口板不在线\n""按下 确定 开始巡检").arg(boardnum));
    }
    QSqlDatabase::database().commit();

    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setVisible(true);
    okPushButton->setVisible(false);
    cancelPushButton->setVisible(false);
    okToPollingButton->setVisible(true);
    okToPollingButton->setFocus();
}
void BoardRegDialog::createSingleBoardDialog()
{
    isSingleRegFlag = true;
    setWindowTitle(tr("接口板登记设置"));
    QFont f("unifont", 12, QFont::Light);
    singleRegInfoLabel = new QLabel(this);
    singleRegInfoLabel->setText(tr(">>>> 单接口板登记 : 请输入要登记的接口板号,\n     并点击确定."));
    //singleRegInfoLabel->setAlignment(Qt::AlignCenter);
    singleRegInfoLabel->setFont(f);
    singleRegInfoLabel->setGeometry(QRect(30,20,420,40));

    singleBoardNumLabel = new QLabel(this);
    singleBoardNumLabel->setText(tr("接口板号 :"));
    singleBoardNumLabel->setGeometry(QRect(70,80,70,24));

    singleBoardNumLineEdit = new QLineEdit(this);
    singleBoardNumLineEdit->setGeometry(QRect(160,80,60,27));
    singleBoardNumLineEdit->setValidator(new QIntValidator(1,34,this));

    singleOkPushButton = new QPushButton(tr("确定"),this);
    singleOkPushButton->setGeometry(QRect(250,80,85,27));

    allRegInfoLabel = new QLabel(this);
    allRegInfoLabel->setText(tr(">>>> 全体接口板登记，请点击下方按钮:"));
    allRegInfoLabel->setGeometry(QRect(30,120,400,40));
    allRegInfoLabel->setFont(f);

    allRegPushButton = new QPushButton(tr("全体接口板登记"),this);
    allRegPushButton->setGeometry(QRect(70,170,140,27));

    singleCancelPushButton = new QPushButton(tr("取消"),this);
    singleCancelPushButton->setGeometry(QRect(250,170,85,27));

    connect(singleOkPushButton,SIGNAL(clicked()),this,SLOT(emitSingleBoardNum()));
    connect(allRegPushButton,SIGNAL(clicked()),this,SIGNAL(okPushButtonClicked()));
    connect(singleCancelPushButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
}

void BoardRegDialog::emitSingleBoardNum()
{
    int num = singleBoardNumLineEdit->text().toInt();
    emit singleBoardNumSignal(num);
    qDebug()<<"signel board to reg is: "<<num;

}

void BoardRegDialog::refreshDialog()
{
    backgroundLabel->setVisible(true);

    infoLabel->setVisible(false);
    okPushButton->setVisible(false);
    cancelPushButton->setVisible(false);
    okToPollingButton->setVisible(false);

    setSignalVisibleTrue();
    this->exec();

    backgroundLabel->setVisible(false);
}

void BoardRegDialog::createEscapeAction()
{
    //qDebug()<<"in boardRegDialog:escape clicked! emit cancelPushButtonClicked signal.";
    escapeAction = new QAction (this);
    escapeAction->setShortcut (/*Qt::CTRL+*/Qt::Key_Escape);
    connect(escapeAction, SIGNAL(triggered()), this, SIGNAL(cancelPushButtonClicked()));
    this->addAction(escapeAction);
}

BoardRegDialog::~BoardRegDialog()
{
    delete(infoLabel);
    delete(okPushButton);
    delete(cancelPushButton);
    delete(singleRegInfoLabel);
    delete(allRegInfoLabel);
    delete(singleBoardNumLabel);
    delete(singleBoardNumLineEdit);
    delete(singleOkPushButton);
    delete(allRegPushButton);
    delete(singleCancelPushButton);
    delete(escapeAction);
}
