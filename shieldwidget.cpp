#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include<QtCore/QVariant>
#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtGui/QTableWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QFrame>
#include <QtCore/QDateTime>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtGui/QMessageBox>
#include <QtDebug>

#include "shieldwidget.h"
#include "info.h"
#include "type.h"

#define LISTWIDGET_COLUMN 1
#define  LISTWIDGET_ROW  256

ShieldWidget::ShieldWidget(QWidget *parent) :
    QDialog(parent)
{
    messageid = 0;
    createItems();
    setWindowTitle(tr("屏蔽设置"));
    shieldTableWidget->setGeometry(QRect(5,100,590,303));
    this->setFixedSize (QSize(600, 410));
    connect(this,SIGNAL(shieldhisChange(int)),this->parentWidget()->parentWidget(),SLOT(updateTabWidget(int)));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));

}
ShieldWidget::~ShieldWidget()
{
}
//ShieldWidget::ShieldWidget(int index, QWidget *parent):
//    QDialog(parent)
//{
//    int id =index;
//    if(id == 1)
//    {
//    createItems();
//    shieldTableWidget->setGeometry(QRect(0,30,790,365));
//    this->setFixedSize (QSize(800, 430));
//    }
//}

void ShieldWidget::createItems()
{


    shieldwidgetFrame = new QFrame(this);
    shieldwidgetFrame->setGeometry(QRect(0, 1 ,598, 408));
    shieldwidgetFrame->setFrameStyle(QFrame::Box);

    hostLabel = new QLabel(tr("主机"), this);
    hostLabel->setAlignment(Qt::AlignCenter);
    hostLabel->setGeometry(QRect(22, 8 , 33, 24));
    hostLabel->setFrameStyle(QFrame::Box);
    hostSpinBox = new QSpinBox(this);
    hostSpinBox->setGeometry(QRect(58, 8, 50, 24));
    hostSpinBox->setRange(1,64);

    iobrLabel = new QLabel(tr("接口板"), this);
    iobrLabel->setAlignment(Qt::AlignCenter);
    iobrLabel->setGeometry(QRect(141, 8, 50, 24));
    iobrLabel->setFrameStyle(QFrame::Box);
    iobrSpinBox = new QSpinBox(this);
    iobrSpinBox->setGeometry(QRect(194, 8, 50, 24));
    iobrSpinBox->setRange(1,33);

    loopLabel = new QLabel(tr("回路"), this);
    loopLabel->setAlignment(Qt::AlignCenter);
    loopLabel->setGeometry(QRect(277, 8, 35, 24));
    loopLabel->setFrameStyle(QFrame::Box);
    loopSpinBox = new QSpinBox(this);
    loopSpinBox->setGeometry(QRect(325, 8, 50, 24));
    loopSpinBox->setRange(1,20);

    startnodeLabel = new QLabel(tr("首地址"), this);
    startnodeLabel->setAlignment(Qt::AlignCenter);
    startnodeLabel->setGeometry(QRect(22, 35, 45, 24));
    startnodeLabel->setFrameStyle(QFrame::Box);
    startnodeSpinBox = new QSpinBox(this);
    startnodeSpinBox->setGeometry(QRect(72, 35, 55, 24));
    startnodeSpinBox->setRange(1,256);

    endnodeLabel = new QLabel(tr("终地址"), this);
    endnodeLabel->setAlignment(Qt::AlignCenter);
    endnodeLabel->setGeometry(QRect(135, 35, 50, 24));
    endnodeLabel->setFrameStyle(QFrame::Box);
    endnodeSpinBox = new QSpinBox(this);
    endnodeSpinBox->setGeometry(QRect(190, 35, 55, 24));
    endnodeSpinBox->setRange(1,256);

    equipLabel = new QLabel(tr("设备类型"), this);
    equipLabel->setAlignment(Qt::AlignCenter);
    equipLabel->setGeometry(QRect(250, 35, 65, 24));
    equipLabel->setFrameStyle(QFrame::Box);


    equipComboBox = new QComboBox(this);
    equipComboBox->setGeometry(QRect(325, 34, 90, 24));
    for (int i = 0;i < (int)sizeof(char_danyuan) /((int)sizeof(char)*4);i++)
    {
        equipComboBox->addItem(tr(char_danyuan[i]));
    }

    buttonLabel = new QLabel(tr("功能按键:"), this);
    buttonLabel->setAlignment(Qt::AlignCenter);
    buttonLabel->setGeometry(QRect(22, 65, 65, 24));
    buttonLabel->setFrameStyle(QFrame::Box);

    shieldPushButton = new QPushButton(tr("进行屏蔽"), this);
    shieldPushButton->setGeometry(QRect(100, 65, 65, 24));
    connect(shieldPushButton, SIGNAL(clicked()),this, SLOT(shieldPushButtonClicked()));

    unshieldPushButton = new QPushButton(tr("取消屏蔽"), this);
    unshieldPushButton->setGeometry(QRect(180, 65, 65, 24));
    connect(unshieldPushButton, SIGNAL(clicked()),this, SLOT(unshieldPushButtonClicked()));

    clearPushButton = new QPushButton(tr("清空列表"), this);
    clearPushButton->setGeometry(QRect(260, 65, 65, 24));
    connect(clearPushButton, SIGNAL(clicked()),this, SLOT(clearPushButtonClicked()));




    shieldTableWidget = new QTableWidget(LISTWIDGET_ROW,LISTWIDGET_COLUMN,this);
    QStringList label;
    label<<tr("屏蔽操作列表");
    shieldTableWidget->setHorizontalHeaderLabels(label);
    shieldTableWidget->horizontalHeader()->setStretchLastSection(true);
    shieldTableWidget->verticalHeader()->setDefaultSectionSize(20);//set the verticalHeader size is 24
    shieldTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    //    shieldTableWidget->horizontalHeader()->setVisible(false);



}

void ShieldWidget::shieldPushButtonClicked()
{
    QSqlDatabase::database().transaction();
    int host = hostSpinBox->value();
    int iobr = iobrSpinBox->value();
    int loop = loopSpinBox->value();
    int nodestart = startnodeSpinBox->value();
    int nodeend = endnodeSpinBox->value();
    int equiptype = Type::int_danyuanId(equipComboBox->currentText());

    if(nodestart > nodeend)
    {
        return ;
    }
    else if (!equiptype)
    {
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg((host << 24) | (iobr << 16) | (loop << 8) | nodestart)
                   .arg((host << 24) | (iobr << 16) | (loop << 8) | nodeend));
        while (query.next())
        {
            if(query.value(1).toBool() && (! query.value(2).toBool()))
            {

                QSqlQuery shieldquery;
                shieldquery.setForwardOnly(true);
                shieldquery.clear();
                shieldquery.prepare(QString("UPDATE unitset SET "
                                            "shield= ?"
                                            "WHERE unitid = %1").arg(query.value(5).toInt()));
                shieldquery.addBindValue(true);

                this->insertintoShieldHis(query.value(5).toInt(),((query.value(7).toInt() >> 8) & 0xFF));
                emit shieldhisChange(3);

                bool bSuccess2 = shieldquery.exec();
                if (!bSuccess2)
                {
                    QSqlError lastError = shieldquery.lastError();
                    qDebug() << lastError.driverText()
                             <<QString(QObject::tr("update error"));
                }
            }
        }
        QDateTime datetime = QDateTime::currentDateTime();
        QString dtstr = datetime.toString("MM-dd hh:mm:ss");
        QString equip_info = QString("%1  %2号主机%3号接口板%4号回路从%5号节点到%6号节点  %7 进行屏蔽")
                .arg(dtstr)
                .arg(host)
                .arg(iobr)
                .arg(loop)
                .arg(nodestart)
                .arg(nodeend)
                .arg(Type::char_danyuan(equiptype));

        shieldTableWidget->setItem(messageid, 0, new QTableWidgetItem(equip_info));
        shieldTableWidget->selectRow(messageid++);

    }
    else
    {
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg((host << 24) | (iobr << 16) | (loop << 8) | nodestart)
                   .arg((host << 24) | (iobr << 16) | (loop << 8) | nodeend));
        while (query.next())
        {
            if((((query.value(7).toInt() >> 8) & 0xFF) == equiptype) && query.value(1).toBool() && (! query.value(2).toBool()))
            {

                QSqlQuery shieldquery;
                shieldquery.setForwardOnly(true);
                shieldquery.clear();


                shieldquery.prepare(QString("UPDATE unitset SET "
                                            "shield= ?"
                                            "WHERE unitid = %1").arg(query.value(5).toInt()));
                shieldquery.addBindValue(true);

                this->insertintoShieldHis(query.value(5).toInt(),((query.value(7).toInt() >> 8) & 0xFF));
                emit shieldhisChange(3);

                bool bSuccess2 = shieldquery.exec();
                if (!bSuccess2)
                {
                    QSqlError lastError = shieldquery.lastError();
                    qDebug() << lastError.driverText()
                             <<QString(QObject::tr("update error"));
                }
            }
        }
        QDateTime datetime = QDateTime::currentDateTime();
        QString dtstr = datetime.toString("MM-dd hh:mm:ss");

        QString equip_info = QString("%1  %2号主机%3号接口板%4号回路从%5号节点到%6号节点  %7 进行屏蔽")
                .arg(dtstr)
                .arg(host)
                .arg(iobr)
                .arg(loop)
                .arg(nodestart)
                .arg(nodeend)
                .arg(Type::char_danyuan(equiptype));

        shieldTableWidget->setItem(messageid, 0, new QTableWidgetItem(equip_info));
        shieldTableWidget->selectRow(messageid++);

    }

    QSqlDatabase::database().commit();

}
void ShieldWidget::unshieldPushButtonClicked()
{
    QSqlDatabase::database().transaction();
    int host = hostSpinBox->value();
    int iobr = iobrSpinBox->value();
    int loop = loopSpinBox->value();
    int nodestart = startnodeSpinBox->value();
    int nodeend = endnodeSpinBox->value();
    int equiptype = Type::int_danyuanId(equipComboBox->currentText());

    if(nodestart > nodeend)
    {
        return ;
    }
    else if (!equiptype)
    {
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg((host << 24) | (iobr << 16) | (loop << 8) | nodestart)
                   .arg((host << 24) | (iobr << 16) | (loop << 8) | nodeend));
        while (query.next())
        {
            if(query.value(1).toBool() && (query.value(2).toBool()))
            {

                QSqlQuery shieldquery;
                shieldquery.setForwardOnly(true);
                shieldquery.clear();
                shieldquery.prepare(QString("UPDATE unitset SET "
                                            "shield= ?"
                                            "WHERE unitid = %1").arg(query.value(5).toInt()));
                shieldquery.addBindValue(false);

                this->deletefromShieldHis(query.value(5).toInt());
                emit shieldhisChange(3);

                bool bSuccess2 = shieldquery.exec();
                if (!bSuccess2)
                {
                    QSqlError lastError = shieldquery.lastError();
                    qDebug() << lastError.driverText()
                             <<QString(QObject::tr("update error"));
                }
            }
        }
        QDateTime datetime = QDateTime::currentDateTime();
        QString dtstr = datetime.toString("MM-dd hh:mm:ss");

        QString equip_info = QString("%1  %2号主机%3号接口板%4号回路从%5号节点到%6号节点  %7 取消屏蔽")
                .arg(dtstr)
                .arg(host)
                .arg(iobr)
                .arg(loop)
                .arg(nodestart)
                .arg(nodeend)
                .arg(Type::char_danyuan(equiptype));

        shieldTableWidget->setItem(messageid, 0, new QTableWidgetItem(equip_info));
        shieldTableWidget->selectRow(messageid++);

    }
    else
    {
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg((host << 24) | (iobr << 16) | (loop << 8) | nodestart)
                   .arg((host << 24) | (iobr << 16) | (loop << 8) | nodeend));
        while (query.next())
        {
            if((((query.value(7).toInt() >> 8) & 0xFF) == equiptype) && query.value(1).toBool() && (query.value(2).toBool()))
            {

                QSqlQuery shieldquery;
                shieldquery.setForwardOnly(true);
                shieldquery.clear();


                shieldquery.prepare(QString("UPDATE unitset SET "
                                            "shield= ?"
                                            "WHERE unitid = %1").arg(query.value(5).toInt()));
                shieldquery.addBindValue(false);

                this->deletefromShieldHis(query.value(5).toInt());
                emit shieldhisChange(3);
                bool bSuccess2 = shieldquery.exec();
                if (!bSuccess2)
                {
                    QSqlError lastError = shieldquery.lastError();
                    qDebug() << lastError.driverText()
                             <<QString(QObject::tr("update error"));
                }
            }
        }
        QDateTime datetime = QDateTime::currentDateTime();
        QString dtstr = datetime.toString("MM-dd hh:mm:ss");

        QString equip_info = QString("%1  %2号主机%3号接口板%4号回路从%5号节点到%6号节点  %7 取消屏蔽")
                .arg(dtstr)
                .arg(host)
                .arg(iobr)
                .arg(loop)
                .arg(nodestart)
                .arg(nodeend)
                .arg(Type::char_danyuan(equiptype));

        shieldTableWidget->setItem(messageid, 0, new QTableWidgetItem(equip_info));
        shieldTableWidget->selectRow(messageid++);

    }

    QSqlDatabase::database().commit();

}
void ShieldWidget::clearPushButtonClicked()
{
    messageid = 0;
    shieldTableWidget->clear();
    QStringList label;
    label<<tr("屏蔽操作列表");
    shieldTableWidget->setHorizontalHeaderLabels(label);
}

void ShieldWidget::insertintoShieldHis(int unitid, int type)
{

        QSqlDatabase::database().transaction();
        QDateTime datetime = QDateTime::currentDateTime();
        QSqlQuery query;
        query.setForwardOnly(true);

        query.prepare("INSERT INTO shieldhis( time,unitid,type)"
                      "VALUES ( ?, ?,?)");

        query.addBindValue(datetime);
        query.addBindValue(unitid);
        query.addBindValue(type);

        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("in ShieldWidget : insert shieldhis error"));
        }

        QSqlDatabase::database().commit();

}
void ShieldWidget::deletefromShieldHis(int unitid)
{
    QSqlDatabase::database().transaction();
    QSqlQuery queryDel;
    queryDel.exec(QString("delete  from shieldhis where unitid = %1").arg(unitid));
    bool bSuccess = queryDel.exec();
    if (!bSuccess)
    {
        QSqlError lastError = queryDel.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("in ShieldWidget :delete shieldhis error"));
    }
    QSqlDatabase::database().commit();

}

