#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QTableWidget>
#include <QtGui/QHeaderView>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include<QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QTimer>
#include <QtDebug>

#include "type.h"
#include "defMacro.h"
#include "eventdialog.h"
#include "handwidget.h"

eventDialog::eventDialog(QWidget *parent) :
    QDialog(parent)
{
    createItems();
    this->setFixedSize(QSize(800, 600));
    timerEvent(0);
    startTimer(1000);
}

eventDialog::eventDialog(int index, int exitnum, int fireexitnum,QWidget *parent) :
    QDialog(parent)
{
    type = index;
    exitnumber = exitnum;
    fireexitnumber = fireexitnum;
    createItems();

    //this->setGeometry(QRect(10,30,800,550));
    this->setFixedSize(QSize(800, 600));

    //    timer3s = new QTimer(this);
    //    timer3s->setInterval(1000);
    //    timer3s->setSingleShot(true);

    //connect(timer3s,SIGNAL(timeout()),this,SLOT(close()));
}
eventDialog::eventDialog(QString firstFireStr, QList<QString> eventList , int index, int exitnum, int fireexitnum,QWidget *parent) :
    QDialog(parent)
{
    type = index;
    exitnumber = exitnum;
    fireexitnumber = fireexitnum;
    createItems();
    readEvent(firstFireStr, eventList);
    //this->setGeometry(QRect(10,30,800,550));
    this->setFixedSize (QSize(800, 550));
}
eventDialog::~eventDialog()
{
    delete(timeLabel);
    delete(firstfireTipLabel);
    delete(firstFireTableWidget);
    delete(eventTableWidget);
    delete(handwidget);
    delete(shieldTableWidget);
    //delete(timer3s);

}
void eventDialog::createItems()
{
    firstfireTipLabel = new QLabel(tr("首火警"),this);
    firstfireTipLabel->setGeometry(QRect(5,1,70,20));
    //    firenumLabel = new QLabel(tr("000"),this);
    //    firenumLabel->setGeometry(QRect(85,1,30,20));
    firstFireTableWidget = new QTableWidget(1,1,this);
    firstFireTableWidget->setGeometry(QRect(0,23,800,24));
    firstFireTableWidget->horizontalHeader()->setStretchLastSection(true);
    firstFireTableWidget->verticalHeader()->setDefaultSectionSize(24);//set the verticalHeader size is 20-->24
    firstFireTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    firstFireTableWidget->horizontalHeader()->setVisible(false);

    handwidget = new HandWidget(1,this);
    handwidget->setGeometry(QRect(0, 50, 800, 430));
    handwidget->setVisible(false);


    eventTableWidget = new QTableWidget(1000, 1, this);
    eventTableWidget->horizontalHeader()->setStretchLastSection(true);
    eventTableWidget->verticalHeader()->setDefaultSectionSize(22);//set the verticalHeader size is 22
    eventTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    eventTableWidget->setGeometry(QRect(0, 50, 800, 430));
    eventTableWidget->clear();

    shieldTableWidget = new QTableWidget(1000, 1, this);
    shieldTableWidget->horizontalHeader()->setStretchLastSection(true);
    shieldTableWidget->verticalHeader()->setDefaultSectionSize(22);//set the verticalHeader size is 20-->22
    shieldTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    shieldTableWidget->setGeometry(QRect(0, 50, 800, 430));
    shieldTableWidget->clear();
    shieldTableWidget->setVisible(false);


    firstFireTableWidget->verticalHeader()->setMinimumWidth(eventTableWidget->verticalHeader()->width());

    timeLabel = new QLabel(this);
    timeLabel->setGeometry(QRect(580,540,180,24));

#if 0
    switch (type)
    {
    case 0:
        setWindowTitle(tr("火警 事件"));
        break;
    case 1:
        setWindowTitle(tr("联动 事件"));
        break;
    case 2:
        setWindowTitle(tr("监管 事件"));
        break;
    case 3:
        setWindowTitle(tr("故障 事件"));
        break;
    case 4:
        setWindowTitle(tr("延时 事件"));
        break;
    case 5:
        setWindowTitle(tr("屏蔽 事件"));
        break;
    case 6:
        setWindowTitle(tr("火警 联动"));
        break;
    case 7:
        setWindowTitle(tr("手动 自动"));
        break;
    }
#endif

}

void eventDialog::oneEventShowSLot(QString firstFireStr, QList<QString> &eventlist, char flag)
{
    this->show();
    firstFireTableWidget->clear();
    eventTableWidget->clear();
    shieldTableWidget->clear();
    mySetWindowTitle(flag);
    readEvent(firstFireStr, eventlist);
    //    if(eventlist.isEmpty())
    //    {
    //        timer3s->start();
    //        qDebug()<<"no event to be show. event-dialog close 3s later.";
    //        return;
    //    }
}

void eventDialog::mySetWindowTitle(char flag)
{
    if( (flag & 0x01))
    {
        setWindowTitle(tr("火警 事件"));        
        QStringList label;
        label<<tr("火 警");
        eventTableWidget->setHorizontalHeaderLabels(label);

        eventTableWidget->setVisible(true);
        handwidget->setVisible(false);
        shieldTableWidget->setVisible(false);
    }
    else if((flag>>1 & 0x01))
    {
        setWindowTitle(tr("联动 事件"));
        QStringList label;
        label<<tr("联  动");
        eventTableWidget->setHorizontalHeaderLabels(label);
        shieldTableWidget->setVisible(false);
        eventTableWidget->setVisible(true);
        handwidget->setVisible(false);
    }
    else if((flag>>2 & 0x01))
    {
        setWindowTitle(tr("监管 事件"));
        QStringList label;
        label<<tr("监  管");
        eventTableWidget->setHorizontalHeaderLabels(label);
        shieldTableWidget->setVisible(false);
        eventTableWidget->setVisible(true);
        handwidget->setVisible(false);
    }
    else if(!(flag ^ FAULT_PUSHBUTTON))//((flag>>3 & 0x01))
    {
        setWindowTitle(tr("故障 事件"));
        QStringList label;
        label<<tr("故  障");
        eventTableWidget->setHorizontalHeaderLabels(label);
        shieldTableWidget->setVisible(false);
        eventTableWidget->setVisible(true);
        handwidget->setVisible(false);
    }
    else if((flag>>4 & 0x01))
    {
        setWindowTitle(tr("延时 事件"));
        QStringList label;
        label<<tr("延  时");
        eventTableWidget->setHorizontalHeaderLabels(label);
        shieldTableWidget->setVisible(false);
        eventTableWidget->setVisible(true);
        handwidget->setVisible(false);
    }
    else if((flag>>5 & 0x01))
    {
        setWindowTitle(tr("屏蔽 事件"));
        QStringList label;
        label<<tr("屏  蔽");
        eventTableWidget->setHorizontalHeaderLabels(label);
        eventTableWidget->setVisible(false);
        handwidget->setVisible(false);
        shieldTableWidget->setHorizontalHeaderLabels(label);
        shieldTableWidget->setVisible(true);

        QSqlDatabase::database().transaction();
        int row = 0;
        QSqlQuery query;
        //        query.setForwardOnly(true);
        query.exec("select * from shieldhis");

        //should add the function that when the row >1000,how to deal with
        while (query.next())
        {
        }
        int j =0;
        while(query.previous()&&(j < 1000))
        {
            j++;
        }
        while (query.next())
        {
            if (query.value(1).toInt() == 0x01000000)
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
    else if((flag>>6 & 0x01))
    {
        setWindowTitle(tr("手动 自动"));
        QStringList label;
        label<<tr("手动 自动");
        eventTableWidget->setHorizontalHeaderLabels(label);
        shieldTableWidget->setVisible(false);
        eventTableWidget->setVisible(false);
        handwidget->setVisible(true);
    }

}

void eventDialog::readEvent(QString firstFireStr, QList<QString> eventList)
{
    QStringList label;
    label << windowTitle();
    label << tr("共")<<tr("%1").arg(eventList.count())<< tr("条");
    eventTableWidget->setHorizontalHeaderLabels(label);

    //要不要判断首火警是不是为空？
    firstFireTableWidget->setItem(0,0,new QTableWidgetItem(firstFireStr));

    for(int i = 0; i < eventList.count(); i++)
    {
        eventTableWidget->setItem(i, 0, new QTableWidgetItem(eventList.at(i)));
        eventTableWidget->selectRow(i);
    }
}

void eventDialog::updateOneEventSlot(QString newstr)
{
    eventTableWidget->setItem(eventTableWidget->rowCount(),0,new QTableWidgetItem(newstr));
    eventTableWidget->selectRow(eventTableWidget->rowCount());
}

void eventDialog::timerEvent (QTimerEvent *)// *event)
{
    QDateTime dt = QDateTime::currentDateTime();
    timeLabel->setText(dt.toString("yyyy-MM-dd hh:mm:ss"));
}

void eventDialog::clearEventDialogSlot()
{
    firstFireTableWidget->clear();
    eventTableWidget->clear();

}
#if 0
void eventDialog::readEvent()
{

    char infoid_info[30];
    char geo_addr_info[30];

    QSqlDatabase::database().transaction();
    QSqlQuery query2;//显示首火警
    query2.setForwardOnly(true);

    query2.exec(QString("select * from firehis WHERE id = %1").arg(fireexitnumber+1));
    while (query2.next())
    {

        char hostnumChar[4], iobrnumChar[4], loopnumChar[4], nodenumChar[4];
        int id = query2.value(0).toInt() - fireexitnumber - 1;
        int typeinfo= query2.value(1).toInt();
        int infoid = query2.value(2).toInt();

        memset(hostnumChar, 0x0, 4);
        memset(iobrnumChar, 0x0, 4);
        memset(loopnumChar, 0x0, 4);
        memset(nodenumChar, 0x0, 4);

        int hostnum = (infoid >> 24) & 0xFF;
        int  iobrnum = (infoid >> 16) & 0xFF;
        int  loopnum = (infoid >> 8) & 0xFF;
        int  nodenum = infoid & 0xFF;

        sprintf(hostnumChar,"%02d",hostnum);
        sprintf(iobrnumChar,"%02d",iobrnum);
        sprintf(loopnumChar,"%01d",loopnum);
        sprintf(nodenumChar,"%03d",nodenum);

        int geo_addr = query2.value(3).toInt();
        QDateTime dt = query2.value(4).toDateTime();
        QDate date = dt.date();
        QTime time = dt.time();
        memset(infoid_info, 0x0, 30);
        memset(geo_addr_info, 0x0, 30);

        sprintf(infoid_info, "%02d%02d%01d%03d",(infoid >> 24) & 0xFF, (infoid >> 16) & 0xFF, (infoid >> 8) & 0xFF, infoid &0xFF);
        sprintf(geo_addr_info, "%02d/%03d/%02d", (geo_addr >> 24) & 0xFF, (geo_addr >> 16) & 0xFF, (geo_addr >> 8) & 0xFF);
        QString dtstr = QString("%1-%2-%3 %4:%5").arg(date.year()).arg(date.month()).arg(date.day())
                .arg(time.hour()).arg(time.minute());
        QString history_info = QString("%1 %2 %3 %4 %5号接口板%6号回路%7号节点 %8 %9").arg(Type::char_shijian((typeinfo >> 16) & 0xFF, (typeinfo >> 8) & 0xFF)).arg(Type::char_danyuan((typeinfo >> 8) & 0xFF))
                .arg(dtstr).arg(geo_addr_info).arg(iobrnumChar).arg(loopnumChar).arg(nodenumChar).arg(query2.value(5).toString()).arg(infoid_info);
        firstFireTableWidget->setItem(id, 0, new QTableWidgetItem(history_info));
        //qDebug()<<"exitnumber"<<exitnumber;

    }



    QSqlQuery query;//显示事件
    query.setForwardOnly(true);
    switch (type)
    {
    case 0:
    {
        query.exec(QString("select * from firehis WHERE id > %1").arg(exitnumber));
        //  qDebug()<<"exitnumber"<<exitnumber;
    }
    break;
    case 1:
    {

    }
    break;
    case 2:
    {

    }
    break;
    case 3:
    {
        query.exec(QString("select * from faulthis where id > %1").arg(exitnumber));
    }
    break;
    case 4:
    {
    }
    break;
    case 5:
    {
    }
    break;

    case 6:
    {
    }
    break;
    case 7:
    {
    }
    break;
    }

    while (query.next())
    {
        char hostnumChar[4], iobrnumChar[4], loopnumChar[4], nodenumChar[4];


        int id = query.value(0).toInt() - exitnumber - 1;
        //   qDebug()<<"query.value(0).toInt()"<<query.value(0).toInt();
        //  qDebug()<<"id"<<id;
        int typeinfo= query.value(1).toInt();
        int infoid = query.value(2).toInt();

        memset(hostnumChar, 0x0, 4);
        memset(iobrnumChar, 0x0, 4);
        memset(loopnumChar, 0x0, 4);
        memset(nodenumChar, 0x0, 4);

        int hostnum = (infoid >> 24) & 0xFF;
        int  iobrnum = (infoid >> 16) & 0xFF;
        int  loopnum = (infoid >> 8) & 0xFF;
        int  nodenum = infoid & 0xFF;

        sprintf(hostnumChar,"%02d",hostnum);
        sprintf(iobrnumChar,"%02d",iobrnum);
        sprintf(loopnumChar,"%01d",loopnum);
        sprintf(nodenumChar,"%03d",nodenum);

        int geo_addr = query.value(3).toInt();
        QDateTime dt = query.value(4).toDateTime();
        QDate date = dt.date();
        QTime time = dt.time();
        memset(infoid_info, 0x0, 30);
        memset(geo_addr_info, 0x0, 30);

        sprintf(infoid_info, "%02d%02d%01d%03d",(infoid >> 24) & 0xFF, (infoid >> 16) & 0xFF, (infoid >> 8) & 0xFF, infoid &0xFF);

        sprintf(geo_addr_info, "%02d/%03d/%02d", (geo_addr >> 24) & 0xFF, (geo_addr >> 16) & 0xFF, (geo_addr >> 8) & 0xFF);
        QString dtstr = QString("%1-%2-%3 %4:%5").arg(date.year()).arg(date.month()).arg(date.day())
                .arg(time.hour()).arg(time.minute());
        QString history_info = QString("%1 %2 %3 %4 %5号接口板%6号回路%7号节点 %8 %9").arg(Type::char_shijian((typeinfo >> 16) & 0xFF, (typeinfo >> 8) & 0xFF)).arg(Type::char_danyuan((typeinfo >> 8) & 0xFF))
                .arg(dtstr).arg(geo_addr_info).arg(iobrnumChar).arg(loopnumChar).arg(nodenumChar).arg(query.value(5).toString()).arg(infoid_info);
        eventTableWidget->setItem(id, 0, new QTableWidgetItem(history_info));
        //qDebug()<<"exitnumber"<<exitnumber;
    }
    QSqlDatabase::database().commit();

}
#endif
