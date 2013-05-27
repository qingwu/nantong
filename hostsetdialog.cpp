#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QFrame>
#include <QtGui/QComboBox>
#include <QtGui/QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include<QtCore/QVariant>
#include <QtDebug>

#include "hostsetdialog.h"
#include "serial.h"

HostSetDialog::HostSetDialog(QWidget *parent) :
    QDialog(parent)
{
    createItems();
    setWindowTitle(tr("本机设置"));
    this->setFixedSize (QSize(400, 500));
    connect(this,SIGNAL(handTypeChange(int)),this->parentWidget()->parentWidget(),SLOT(updateHandwidgetType(int)));
    connect(this,SIGNAL(lockkeyTimeChange(int)),this->parentWidget()->parentWidget(),SIGNAL(updateLockkeyTime(int)));
    connect(this,SIGNAL(sound_alarm_statechange(int)),this->parentWidget()->parentWidget(),SLOT(updateSoundAndAlarmDeviceState(int)));
    connect(this,SIGNAL(gasfirecontrolstatechange(int)),this->parentWidget()->parentWidget(),SLOT(updateGasFireControlState(int)));
}
HostSetDialog::~HostSetDialog()
{
}

void HostSetDialog::createItems()
{
    linkFeedBackFlag = 0;

    hostFrame = new QFrame(this);
    hostFrame->setGeometry(QRect(0, 5 ,400, 450));
    hostFrame->setFrameStyle(QFrame::Box);

    hostSetLabel = new QLabel(tr("本机设置"), this);
    hostSetLabel->setGeometry(QRect(160, 15 , 80, 24));
    hostSetLabel->setFrameShape(QFrame::Box);

    hostNumLabel = new QLabel(tr("本机号码:"), this);
    hostNumLabel->setGeometry(QRect(70, 50 , 80, 24));
    passwordResetLabel = new QLabel(tr("复位密码:"), this);
    passwordResetLabel->setGeometry(QRect(70, 90 , 80, 24));
    passwordOkResetLabel = new QLabel(tr("复位密码确定:"), this);
    passwordOkResetLabel->setGeometry(QRect(70, 130 , 100, 24));
    passwordSetLabel = new QLabel(tr("设置密码:"),this);
    passwordSetLabel->setGeometry(QRect(70, 170 , 80, 24));
    passwordOkSetLabel = new QLabel(tr("设置密码确定:"),this);
    passwordOkSetLabel->setGeometry(QRect(70, 210 , 100, 24));
    linkFeedBackLabel = new QLabel(tr("联动反馈:"), this);
    linkFeedBackLabel->setGeometry(QRect(70, 250 , 80, 24));
    linkStyleLabel = new QLabel(tr("联动方式:"), this);
    linkStyleLabel->setGeometry(QRect(70, 290 , 80, 24));
    lockKeyLabel = new QLabel(tr("锁键时间:"), this);
    lockKeyLabel->setGeometry(QRect(70, 330 , 80, 24));
    soundAndAlarmDeviceLabel = new QLabel(tr("声光报警器:"), this);
    soundAndAlarmDeviceLabel->setGeometry(QRect(70, 370 , 80, 24));
    gasFireControlLabel = new QLabel(tr("气体灭火:"), this);
    gasFireControlLabel->setGeometry(QRect(70, 410 , 80, 24));



    hostNumCombox  = new QComboBox(this);
    hostNumCombox->setGeometry(QRect(190, 50 , 80, 24));


    resetLineEdit = new QLineEdit(this);
    resetLineEdit->setGeometry(QRect(190, 90 , 80, 24));
    resetLineEdit->setEchoMode(QLineEdit::Password);
    resetOkLineEdit = new QLineEdit(this);
    resetOkLineEdit->setGeometry(QRect(190, 130 , 80, 24));
    resetOkLineEdit->setEchoMode(QLineEdit::Password);
    setLineEdit = new QLineEdit(this);
    setLineEdit->setGeometry(QRect(190, 170 , 80, 24));
    setLineEdit->setEchoMode(QLineEdit::Password);
    setOkLineEdit = new QLineEdit(this);
    setOkLineEdit->setGeometry(QRect(190, 210 , 80, 24));
    setOkLineEdit->setEchoMode(QLineEdit::Password);
    linkFeedBackCombox = new QComboBox(this);
    linkFeedBackCombox->setGeometry(QRect(190, 250 , 80, 24));
    linkFeedBackCombox->addItem(tr("否"));
    linkFeedBackCombox->addItem(tr("是"));
    linkStyleCombox = new QComboBox(this);
    linkStyleCombox->setGeometry(QRect(190, 290 , 80, 24));
    linkStyleCombox->addItem(tr("自动"));
    linkStyleCombox->addItem(tr("手动"));

    lockKeyCombox = new QComboBox(this);
    lockKeyCombox->setGeometry(QRect(190, 330 , 80, 24));
    lockKeyCombox->addItem(tr("1分钟"));
    lockKeyCombox->addItem(tr("2分钟"));
    lockKeyCombox->addItem(tr("3分钟"));
    lockKeyCombox->addItem(tr("4分钟"));
    lockKeyCombox->addItem(tr("5分钟"));

    soundAndAlarmDeviceCombox = new QComboBox(this);
    soundAndAlarmDeviceCombox->setGeometry(QRect(190, 370 , 80, 24));
    soundAndAlarmDeviceCombox->addItem(tr("屏蔽"));
    soundAndAlarmDeviceCombox->addItem(tr("允许"));

    gasFireControlCombox = new QComboBox(this);
    gasFireControlCombox->setGeometry(QRect(190, 410 , 80, 24));
    gasFireControlCombox->addItem(tr("屏蔽"));
    gasFireControlCombox->addItem(tr("允许"));



    QSqlDatabase::database().transaction();
    QSqlQuery hostquery;
    hostquery.setForwardOnly(true);
    hostquery.exec("select * from hostset");
    int hostnum = 1;
    while (hostquery.next())
    {

        hostnum = hostquery.value(1).toInt();
        hostNumCombox->addItem(tr("%1").arg(hostquery.value(1).toInt()));
        linkStyleCombox->setCurrentIndex(hostquery.value(2).toInt());
        //        qDebug()<<"hostnum"<<hostnum;
        lockKeyCombox->setCurrentIndex(hostquery.value(3).toInt());
        soundAndAlarmDeviceCombox->setCurrentIndex(hostquery.value(4).toInt());
        gasFireControlCombox->setCurrentIndex(hostquery.value(5).toInt());

    }
    QSqlDatabase::database().commit();


    for(int i = 1; i <= 64; i++)
    {
        if(i != hostnum)
            hostNumCombox->addItem(tr("%1").arg(i));
    }

    okButton = new QPushButton(tr("确定"), this);
    okButton->setGeometry(QRect(70, 470, 80, 24));
    cancelButton = new QPushButton(tr("取消"), this);
    cancelButton->setGeometry(QRect(220, 470, 80, 24));
    cancelButton->setFocus();//set default button

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec("select * from password");
    while (query.next())
    {
        switch(query.value(0).toInt())
        {
        case 1:
        {
            resetLineEdit->setText(query.value(1).toString());
            resetOkLineEdit->setText(query.value(1).toString());
        }
        break;
        case 2:
        {
            setLineEdit->setText(query.value(1).toString());
            setOkLineEdit->setText(query.value(1).toString());
        }
        break;
        }
    }
    QSqlDatabase::database().commit();

    connect(linkFeedBackCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(linkFeedBackChangeSlot(int)));
    connect(this,SIGNAL(linkFeedBackSetSaveSignal(int)),this->parentWidget()->parentWidget(),SLOT(linkFeedBackSetSlot(int)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okSaveSlot()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
}

void HostSetDialog::linkFeedBackChangeSlot(int index)
{
    switch(index)
    {
    case 0:
        linkFeedBackFlag = 0;
        //qDebug()<<"in HostSetDialog:linkFeedBackFlag is:"<<linkFeedBackFlag;
        break;
    case 1:
        linkFeedBackFlag = 1;
        //qDebug()<<"in HostSetDialog:linkFeedBackFlag is:"<<linkFeedBackFlag;
        break;
    }
}
void HostSetDialog::okSaveSlot()
{
    if ((resetLineEdit->text() != resetOkLineEdit->text()) || (setLineEdit->text() != setOkLineEdit->text()))
    {
        QMessageBox *box = new QMessageBox(tr("Warning"),
                                           tr("The first time and once again enter the password does not match!"),
                                           QMessageBox::Warning,
                                           QMessageBox::Ok,
                                           QMessageBox::NoButton,
                                           QMessageBox::NoButton,
                                           this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        box->exec();
    }
    else
    {
        QMessageBox *box1 = new QMessageBox(tr("Question"),
                                            tr("确定保存修改？"),
                                            QMessageBox::Question,
                                            QMessageBox::Yes | QMessageBox::Default,
                                            QMessageBox::NoButton,
                                            QMessageBox::Cancel | QMessageBox::Escape,
                                            this);
        box1->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->parentWidget(), SIGNAL(closedialog()), box1, SLOT(close()));
        switch(box1->exec())
        {
        case QMessageBox::Yes:
        {
            // save and exit
            emit linkFeedBackSetSaveSignal(linkFeedBackFlag);
            QSqlDatabase::database().transaction();
            QSqlQuery query;
            query.clear();
            for (int i = 1;i <= 2; i ++)
            {
                query.prepare(QString("UPDATE password SET "
                                      "passwordinfo=?"
                                      "WHERE id = %1").arg(i));
                switch(i)
                {
                case 1:
                    query.addBindValue(resetOkLineEdit->text());
                    break;
                case 2:
                    query.addBindValue(setOkLineEdit->text());
                    break;
                }
                bool bSuccess = query.exec();
                if (!bSuccess)
                {
                    QSqlError lastError = query.lastError();
                    qDebug() << lastError.driverText()
                             << QString(QObject::tr("password update error"));
                }
            }
            QSqlDatabase::database().commit();

            QSqlDatabase::database().transaction();
            QSqlQuery hostquery;
            hostquery.clear();
            hostquery.prepare("UPDATE hostset SET "
                              "hostnum = ?,systemtype = ?,lockkeytime = ?,sound_alarm_state = ?,gasfirecontrolstate = ?"
                              "WHERE id = 1");

            hostquery.addBindValue(hostNumCombox->currentText().toInt());
            hostquery.addBindValue(linkStyleCombox->currentIndex());
            hostquery.addBindValue(lockKeyCombox->currentIndex());
            hostquery.addBindValue(soundAndAlarmDeviceCombox->currentIndex());
            hostquery.addBindValue(gasFireControlCombox->currentIndex());

            bool bSuccess = hostquery.exec();
            if (!bSuccess)
            {
                QSqlError lastError = hostquery.lastError();
                qDebug() << lastError.driverText()
                         << QString(QObject::tr("hostset update error"));
            }
            emit handTypeChange(linkStyleCombox->currentIndex());
            emit lockkeyTimeChange(lockKeyCombox->currentIndex());
            emit sound_alarm_statechange(soundAndAlarmDeviceCombox->currentIndex());
            emit gasfirecontrolstatechange(gasFireControlCombox->currentIndex());

            QSqlDatabase::database().commit();

            this->close();
        }
        break;
        case QMessageBox::Cancel:
        {
            // don't save and exit
            this->close();
        }
        break;
        }
    }
}
