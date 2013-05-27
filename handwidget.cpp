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

#include "handwidget.h"
#include "info.h"
#include "type.h"

#define LISTWIDGET_COLUMN 1
#define  LISTWIDGET_ROW  256

HandWidget::HandWidget(QWidget *parent) :
    QWidget(parent)
{
    createItems();
    this->setFixedSize (QSize(800, 195));
    handTableWidget->setGeometry(QRect(0,30,790,125));

    allwidgetFrame = new QFrame(this);
    allwidgetFrame->setGeometry(QRect(0, 0 ,795, 100));
    allwidgetFrame->setFrameStyle(QFrame::Box);
    allwidgetFrame->setVisible(false);


    nohandwidgetLabel = new QLabel(tr("请首先在本机设置中设置允许使用手动联动后才能使用该功能"), this);
    nohandwidgetLabel->setAlignment(Qt::AlignCenter);
    nohandwidgetLabel->setGeometry(QRect(0, 0, 800, 85));
    nohandwidgetLabel->setVisible(false);



}
HandWidget::~HandWidget()
{
}
HandWidget::HandWidget(int index, QWidget *parent):
    QWidget(parent)
{
    int id =index;
    if(id == 1)
    {
    createItems();
    handTableWidget->setGeometry(QRect(0,30,790,365));
    this->setFixedSize (QSize(800, 430));
    }
}


void HandWidget::createItems()
{

    handwidgetFrame = new QFrame(this);
    handwidgetFrame->setGeometry(QRect(0, 1 ,798, 28));
    handwidgetFrame->setFrameStyle(QFrame::Box);

    hostLabel = new QLabel(tr("主机"), this);
    hostLabel->setAlignment(Qt::AlignCenter);
    hostLabel->setGeometry(QRect(2, 3, 33, 24));
    hostLabel->setFrameStyle(QFrame::Box);
    hostSpinBox = new QSpinBox(this);
    hostSpinBox->setGeometry(QRect(38, 3, 50, 24));
    hostSpinBox->setRange(1,64);

    iobrLabel = new QLabel(tr("接口板"), this);
    iobrLabel->setAlignment(Qt::AlignCenter);
    iobrLabel->setGeometry(QRect(91, 3, 50, 24));
    iobrLabel->setFrameStyle(QFrame::Box);
    iobrSpinBox = new QSpinBox(this);
    iobrSpinBox->setGeometry(QRect(144, 3, 50, 24));
    iobrSpinBox->setRange(1,32);

    loopLabel = new QLabel(tr("回路"), this);
    loopLabel->setAlignment(Qt::AlignCenter);
    loopLabel->setGeometry(QRect(197, 3, 35, 24));
    loopLabel->setFrameStyle(QFrame::Box);
    loopSpinBox = new QSpinBox(this);
    loopSpinBox->setGeometry(QRect(235, 3, 50, 24));
    loopSpinBox->setRange(1,20);

    startnodeLabel = new QLabel(tr("首地址"), this);
    startnodeLabel->setAlignment(Qt::AlignCenter);
    startnodeLabel->setGeometry(QRect(288, 3, 45, 24));
    startnodeLabel->setFrameStyle(QFrame::Box);
    startnodeSpinBox = new QSpinBox(this);
    startnodeSpinBox->setGeometry(QRect(336, 3, 55, 24));
    startnodeSpinBox->setRange(1,256);

    endnodeLabel = new QLabel(tr("终地址"), this);
    endnodeLabel->setAlignment(Qt::AlignCenter);
    endnodeLabel->setGeometry(QRect(394, 3, 50, 24));
    endnodeLabel->setFrameStyle(QFrame::Box);
    endnodeSpinBox = new QSpinBox(this);
    endnodeSpinBox->setGeometry(QRect(447, 3, 55, 24));
    endnodeSpinBox->setRange(1,256);

    equipLabel = new QLabel(tr("设备类型"), this);
    equipLabel->setAlignment(Qt::AlignCenter);
    equipLabel->setGeometry(QRect(505, 3, 65, 24));
    equipLabel->setFrameStyle(QFrame::Box);

    equipComboBox = new QComboBox(this);
    equipComboBox->setGeometry(QRect(572, 2, 90, 24));
    for (int i = 0;i < (int)sizeof(char_liandong) /((int)sizeof(char)*4);i++)
    {
        equipComboBox->addItem(tr(char_liandong[i]));
    }


    findPushButton = new QPushButton(tr("查找"), this);
    findPushButton->setGeometry(QRect(665, 2, 40, 24));
    connect(findPushButton, SIGNAL(clicked()),this, SLOT(findPushButtonClicked()));

    startPushButton = new QPushButton(tr("启动"), this);
    startPushButton->setGeometry(QRect(708, 2, 40, 24));
    connect(startPushButton, SIGNAL(clicked()),this, SLOT(startPushButtonClicked()));

    stopPushButton = new QPushButton(tr("停止"), this);
    stopPushButton->setGeometry(QRect(751, 2, 40, 24));
    connect(stopPushButton, SIGNAL(clicked()),this, SLOT(stopPushButtonClicked()));



    handTableWidget = new QTableWidget(LISTWIDGET_ROW,LISTWIDGET_COLUMN,this);

    handTableWidget->horizontalHeader()->setStretchLastSection(true);
    handTableWidget->verticalHeader()->setDefaultSectionSize(20);//set the verticalHeader size is 24
    handTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    handTableWidget->horizontalHeader()->setVisible(false);


}

void HandWidget::startPushButtonClicked()
{

    QSqlDatabase::database().transaction();
    int host = hostSpinBox->value();
    int iobr = iobrSpinBox->value();
    int loop = loopSpinBox->value();
    int nodestart = startnodeSpinBox->value();
    int nodeend = endnodeSpinBox->value();
    int equiptype = Type::int_liandongId(equipComboBox->currentText());

    if(nodestart > nodeend)
    {
        return ;
    }
    else
    {
        handTableWidget->clear();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg((host << 24) | (iobr << 16) | (loop << 8) | nodestart)
                   .arg((host << 24) | (iobr << 16) | (loop << 8) | nodeend));

        int id = 0;
        char infoid_info[30];
        while (query.next())
        {
            if((((query.value(7).toInt() >> 16) & 0xFF) == equiptype) && query.value(1).toBool() &&  (!query.value(2).toBool()) )
            {

                sprintf(infoid_info, "%02d%02d%02d%03d",host,iobr, loop, (query.value(5).toInt() & 0xFF));
                QString equip_info = QString("%1号主机%2号接口板%3号回路%4号节点   %5 %6  发出手动启动命令")
                        .arg(host)
                        .arg(iobr)
                        .arg(loop)
                        .arg((query.value(5).toInt() & 0xFF))
                        .arg(Type::char_liandong(equiptype))
                        .arg(infoid_info);
                emit handNode(NODE_START, query.value(5).toInt());

                handTableWidget->setItem(id++, 0, new QTableWidgetItem(equip_info));
            }
        }
    }
    QSqlDatabase::database().commit();
}
void HandWidget::stopPushButtonClicked()
{

    QSqlDatabase::database().transaction();
    int host = hostSpinBox->value();
    int iobr = iobrSpinBox->value();
    int loop = loopSpinBox->value();
    int nodestart = startnodeSpinBox->value();
    int nodeend = endnodeSpinBox->value();
    int equiptype = Type::int_liandongId(equipComboBox->currentText());

    if(nodestart > nodeend)
    {
        return ;
    }
    else
    {
        handTableWidget->clear();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg((host << 24) | (iobr << 16) | (loop << 8) | nodestart)
                   .arg((host << 24) | (iobr << 16) | (loop << 8) | nodeend));

        int id = 0;
        char infoid_info[30];
        while (query.next())
        {
            if((((query.value(7).toInt() >> 16) & 0xFF) == equiptype) && query.value(1).toBool() &&  !(query.value(2).toBool()) )
            {

                sprintf(infoid_info, "%02d%02d%02d%03d",host,iobr, loop, (query.value(5).toInt() & 0xFF));
                QString equip_info = QString("%1号主机%2号接口板%3号回路%4号节点   %5 %6  发出手动停止命令")
                        .arg(host)
                        .arg(iobr)
                        .arg(loop)
                        .arg((query.value(5).toInt() & 0xFF))
                        .arg(Type::char_liandong(equiptype))
                        .arg(infoid_info);

                emit handNode(NODE_STOP,query.value(5).toInt());
                handTableWidget->setItem(id++, 0, new QTableWidgetItem(equip_info));
            }
        }
    }
    QSqlDatabase::database().commit();

}
void HandWidget::findPushButtonClicked()
{

    QSqlDatabase::database().transaction();
    int host = hostSpinBox->value();
    int iobr = iobrSpinBox->value();
    int loop = loopSpinBox->value();
    int nodestart = startnodeSpinBox->value();
    int nodeend = endnodeSpinBox->value();
    int equiptype = Type::int_liandongId(equipComboBox->currentText());

    if(nodestart > nodeend)
    {
        return ;
    }
    else
    {
        handTableWidget->clear();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg((host << 24) | (iobr << 16) | (loop << 8) | nodestart)
                   .arg((host << 24) | (iobr << 16) | (loop << 8) | nodeend));

        int id = 0;
        char infoid_info[30];

        while (query.next())
        {
            if((((query.value(7).toInt() >> 16) & 0xFF) == equiptype) && query.value(1).toBool() && (!query.value(2).toBool()) )
            {

                sprintf(infoid_info, "%02d%02d%02d%03d",host,iobr, loop, (query.value(5).toInt() & 0xFF));
//                QString equip_info = QString("%1 %2 %3")
//                        .arg(infoid_info) //time
//                        .arg(Type::char_liandong(equiptype))
//                        .arg(query.value(9).toString());


//                QString equip_info = QString("%1号主机%2号接口板%3号回路%4号节点 %5  %6")
//                        .arg(host)
//                        .arg(iobr)
//                        .arg(loop)
//                        .arg((query.value(5).toInt() & 0xFF))
//                        .arg(Type::char_liandong(equiptype))
//                        .arg(query.value(9).toString());

                QString equip_info = QString("%1号主机%2号接口板%3号回路%4号节点   %5 %6 ")
                        .arg(host)
                        .arg(iobr)
                        .arg(loop)
                        .arg((query.value(5).toInt() & 0xFF))
                        .arg(Type::char_liandong(equiptype))
                        .arg(infoid_info);

                handTableWidget->setItem(id++, 0, new QTableWidgetItem(equip_info));
            }
        }
    }
    QSqlDatabase::database().commit();
}


void HandWidget::openFunction()
{
    allwidgetFrame->setVisible(false);
    handwidgetFrame->setVisible(false);
    hostLabel->setVisible(true);
    iobrLabel->setVisible(true);
    loopLabel->setVisible(true);
    startnodeLabel->setVisible(true);
    endnodeLabel->setVisible(true);
    equipLabel->setVisible(true);
    nohandwidgetLabel->setVisible(false);

    hostSpinBox->setVisible(true);
    iobrSpinBox->setVisible(true);
    loopSpinBox->setVisible(true);
    startnodeSpinBox->setVisible(true);
    endnodeSpinBox->setVisible(true);
    equipComboBox->setVisible(true);

    startPushButton->setVisible(true);
    stopPushButton->setVisible(true);
    findPushButton->setVisible(true);
    handTableWidget->setVisible(true);

}
void HandWidget::closeFunction()
{
    allwidgetFrame->setVisible(true);
    handwidgetFrame->setVisible(false);
    hostLabel->setVisible(false);
    iobrLabel->setVisible(false);
    loopLabel->setVisible(false);
    startnodeLabel->setVisible(false);
    endnodeLabel->setVisible(false);
    equipLabel->setVisible(false);
    nohandwidgetLabel->setVisible(true);

    hostSpinBox->setVisible(false);
    iobrSpinBox->setVisible(false);
    loopSpinBox->setVisible(false);
    startnodeSpinBox->setVisible(false);
    endnodeSpinBox->setVisible(false);
    equipComboBox->setVisible(false);

    startPushButton->setVisible(false);
    stopPushButton->setVisible(false);
    findPushButton->setVisible(false);
    handTableWidget->setVisible(false);

}
