#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTableWidget>
#include <QtGui/QHeaderView>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtCore/QTextCodec>
#include <QtGui/QMessageBox>
#include <QtDebug>

#include <time.h>
#include "historydialog.h"
#include "type.h"
#include "serial.h"

#define TABLE_ROWS 10000
#define TABLE_CLOS 1

HistoryDialog::HistoryDialog(QWidget *parent) :
    QDialog(parent)
{
    type = 0;
    createItems();
    this->setFixedSize(QSize(800, 580));// (QSize(700, 480));
}

HistoryDialog::HistoryDialog(int index, QWidget *parent):
    QDialog(parent)
{
    type = index;
    createItems();
    this->setFixedSize(QSize(800, 580));// (QSize(700, 480));
}

HistoryDialog::~HistoryDialog()
{
}

void HistoryDialog::createItems()
{
    table = new QTableWidget(TABLE_ROWS, TABLE_CLOS, this);
    QStringList label;
    label<<tr("内容");
    table->setHorizontalHeaderLabels(label);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setDefaultSectionSize(24);//set the verticalHeader size is 24
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    table->setGeometry(QRect(0, 0, 800, 528));//table->setGeometry(QRect(0, 0, 700, 435));

    from = new QLabel(tr("从:"), this);
    from->setGeometry(QRect(60, 550, 30, 24));
    fromLineEdit = new QLineEdit(this);
    fromLineEdit->setGeometry(QRect(100, 550, 60, 24));
    to = new QLabel(tr("到:"), this);
    to->setGeometry(QRect(170, 550, 30, 24));
    toLineEdit = new QLineEdit(this);
    toLineEdit->setGeometry(QRect(210, 550, 60, 24));

    printButton = new QPushButton(tr("打印"), this);
    printButton->setGeometry(QRect(340, 550, 80, 24));
    exitButton = new QPushButton(tr("退出"), this);
    exitButton->setGeometry(QRect(440, 550, 80, 24));
    exitButton->setFocus();//set default button
    connect(exitButton, SIGNAL(clicked()), this, SLOT(close()));
    switch (type)
    {
        case 0:
            setWindowTitle(tr("所有历史"));
            break;
        case 1:
            setWindowTitle(tr("历史火警"));
            break;
        case 2:
            setWindowTitle(tr("历史故障"));
            break;
        case 3:
            setWindowTitle(tr("历史联动"));
            break;
        case 4:
            setWindowTitle(tr("历史反馈"));
            break;
        case 5:
            setWindowTitle(tr("历史操作"));
            break;
    }
    readHistory();
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
    connect(printButton,SIGNAL(clicked()),this,SLOT(doPrint()));
    connect(table,SIGNAL(cellClicked(int,int)),this,SLOT(selectedCellChangedSlot(int)));
}

void HistoryDialog::readHistory()
{
    QString infoid_info;
    QString geo_addr_info;

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);

    int eventMinId = 0;

    switch (type)
    {
    case 0:
    {

    }
    break;
    case 1:
    {
        QSqlQuery query3;
        query3.setForwardOnly(true);

        query3.exec("select MIN(id) from firehis");
        while (query3.next())
        {
            eventMinId = query3.value(0).toInt();
        }

        query.exec("select * from firehis");
    }
    break;
    case 2:
    {
        QSqlQuery query3;
        query3.setForwardOnly(true);

        query3.exec("select MIN(id) from faulthis");
        while (query3.next())
        {
            eventMinId = query3.value(0).toInt();
        }
        query.exec("select * from faulthis");
    }
    break;
    case 3:
    {
        QSqlQuery query3;
        query3.setForwardOnly(true);

        query3.exec("select MIN(id) from linkhis");
        while (query3.next())
        {
            eventMinId = query3.value(0).toInt();
        }
        query.exec("select * from linkhis");
    }
    break;

    case 4:
    {
    }
    break;
    case 5:
    {
        QSqlQuery query3;
        query3.setForwardOnly(true);

        query3.exec("select MIN(id) from operationhis");
        while (query3.next())
        {
            eventMinId = query3.value(0).toInt();
        }

        query.exec("select * from operationhis");
        int id = 0;
        while (query.next())
        {
            id = query.value(0).toInt() - eventMinId;
            QDateTime dt = query.value(1).toDateTime();
            QString dtstr = dt.toString("MM-dd hh:mm:ss");
            QString history_info = QString("%1 %2 ")
                    .arg(dtstr) //time
                    .arg(query.value(2).toString());//by qingwu:显示汉字标签

            //Display format:    04-19 10:08 进入单元设置界面
            table->setItem(id, 0, new QTableWidgetItem(history_info));
        }
        QSqlDatabase::database().commit();
        table->selectRow(id);
        fromLineEdit->setValidator(new QIntValidator(1,id+1,this));
        toLineEdit->setValidator(new QIntValidator(1,id+1,this));
    }
    return;
    break;
    }
    int id = 0;
    while (query.next())
    {
        id = query.value(0).toInt() - eventMinId;
        int typeinfo= query.value(1).toInt();
        int infoid = query.value(2).toInt();
        int geo_addr = query.value(3).toInt();
        QDateTime dt = query.value(4).toDateTime();
//        QDate date = dt.date();
//        QTime time = dt.time();
        infoid_info.sprintf("%02d-%02d-%02d-%03d",(infoid >> 24) & 0xFF, (infoid >> 16) & 0xFF, (infoid >> 8) & 0xFF, infoid &0xFF);
        geo_addr_info.sprintf("%02d/%02d/%02d", (geo_addr >> 24) & 0xFF, (geo_addr >> 16) & 0xFF, (geo_addr >> 8) & 0xFF);
        QString dtstr = dt.toString("MM-dd hh:mm:ss");
//        QString("%1-%2 %3:%4")
//                .arg(date.month()).arg(date.day())
//                .arg(time.hour()).arg(time.minute());//month-day hour:min
        QString history_info = QString("%1 %2 %3 %4 %5 %6")
                .arg(dtstr) //time
                .arg(infoid_info) //host-iobr-loop-node
                .arg(Type::char_shijian((typeinfo >> 16) & 0xFF, (typeinfo >> 8) & 0xFF))
                .arg(Type::char_danyuan((typeinfo >> 8) & 0xFF))
                .arg(geo_addr_info)//bld/flr/zone
                .arg(query.value(5).toString());//by qingwu:显示汉字标签

        //Display format:    04-19 10:08 01-01-01-001 复合<>火警 复合探头 02/03/09 01主机01接口01回路001单元
        table->setItem(id, 0, new QTableWidgetItem(history_info));
    }
    QSqlDatabase::database().commit();
    table->selectRow(id);
    fromLineEdit->setValidator(new QIntValidator(1,id+1,this));
    toLineEdit->setValidator(new QIntValidator(1,id+1,this));
    //qDebug()<<"history max id is "<< id;

}

void HistoryDialog::doPrint()
{
    if(fromLineEdit->text().isEmpty() || toLineEdit->text().isEmpty()
            || (fromLineEdit->text().toInt() > toLineEdit->text().toInt()))
    {
        rangeFaultMessage();
        qDebug()<<"in historydialog:doPrint():range fault.return.";
        return;
    }

    int fd_p;
    int i = 0;
    char ToHanzi[2] = {28,38};
    char zouzhi[3] = { 27, 74, 20};
    char p = 10;
    char dev_name[] = "/dev/s3c_serial1";
    char *dev = dev_name;
    fd_p = OpenDev(dev);
    if (fd_p>0)
        qDebug()<<"in HistoryDialog:s3c_serial1 open.fd_p="<<fd_p;//printf("in HistoryDialog:s3c_serial1 open \n");
    else
    {
        printerFaultMessage();
        qDebug()<<"in HistoryDialog:Can't Open Serial Port!fd_p="<<fd_p;//printf("in HistoryDialog:Can't Open Serial Port!\n");
        return;
    }
    set_Opt( fd_p , 9600 , 8 , 1 , 'n' );

    write(fd_p,ToHanzi,sizeof(ToHanzi)/sizeof(char));//to hanzi


    for(i = fromLineEdit->text().toInt()-1; i < toLineEdit->text().toInt(); i++)
    {
        QString str1,str2,str3;
        QString str = table->item(i,0)->text();
        str1 = str.left(27);
        str2 = str.mid(28,20);
        if(str.length() > 47)
            str3 = str.right(str.length()-47);
        else
            str3="";
        QTextCodec *gbk = QTextCodec::codecForName("gb18030");
        QByteArray encodedString1 = gbk->fromUnicode(str1.toLocal8Bit());
        QByteArray encodedString2 = gbk->fromUnicode(str2.toLocal8Bit());
        QByteArray encodedString3 = gbk->fromUnicode(str3.toLocal8Bit());
        char *pstr1 = encodedString1.data();
        char *pstr2 = encodedString2.data();
        char *pstr3 = encodedString3.data();
        write(fd_p, pstr1, encodedString1.count());//string to print
        write(fd_p,&p,1);//print current row and line feed
        write(fd_p, pstr2, encodedString2.count());//string to print
        write(fd_p,&p,1);//print current row and line feed
        write(fd_p, pstr3, encodedString3.count());//string to print
        write(fd_p,&p,1);//print current row and line feed

        write(fd_p,zouzhi, sizeof(zouzhi)/sizeof(char));
        qDebug()<<"print data over!";
    }
    ::close(fd_p);
}

void HistoryDialog::rangeFaultMessage()
{
    QMessageBox *box = new QMessageBox(tr("输入范围错误"),
                                       tr("请输入正确的打印范围！"),
                                       QMessageBox::Information,
                                       QMessageBox::Ok,
                                       QMessageBox::NoButton,
                                       QMessageBox::NoButton,
                                       this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
    box->exec();
}
void HistoryDialog::printerFaultMessage()
{
    QMessageBox *box = new QMessageBox(tr("打印机错误"),
                                       tr("打印机打开错误,请检查！"),
                                       QMessageBox::Information,
                                       QMessageBox::Ok,
                                       QMessageBox::NoButton,
                                       QMessageBox::NoButton,
                                       this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
    box->exec();
}
void HistoryDialog::selectedCellChangedSlot(int row)
{
    fromLineEdit->setText(QString("%1").arg(row+1));
    toLineEdit->setText(QString("%1").arg(row+1));
}
