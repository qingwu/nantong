#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QFrame>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <QtSql/QSqlError>
#include <QtDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include<QtCore/QVariant>
#include <QtGui/QMessageBox>
#include <linkdialog.h>

//#include <QtAlgorithms>

#include "ddlinksetwidget.h"
#include "info.h"
#include "type.h"

ddLinkSetWidget::ddLinkSetWidget(QWidget *parent) :
    QWidget(parent)
{

}

ddLinkSetWidget::ddLinkSetWidget(bool editFlag, QWidget *parent) :
    QWidget(parent)
{
    qFill(linkin, linkin + 31, 0);
    qFill(linkout, linkout + 31, 0);
    editEnableFlag = editFlag;
    firstlinkid = 1;
    firsttype = 0;
    firstsetup = 0;


    firstlinkinFrame = new QFrame(this);
    firstlinkinFrame->setGeometry(QRect(0, 0 ,390, 450));
    firstlinkinFrame->setFrameStyle(QFrame::Box);


    firstlinkinTipLabel = new QLabel(tr("联动输入"), this);
    firstlinkinTipLabel->setAlignment(Qt::AlignCenter);
    firstlinkinTipLabel->setFrameStyle(QFrame::Box);
    firstlinkinTipLabel ->setGeometry(QRect(40,5,80,20));

    firstlinkinTipLabel2 = new QLabel(tr("条件数"), this);
    firstlinkinTipLabel2->setAlignment(Qt::AlignCenter);
    firstlinkinTipLabel2->setFrameStyle(QFrame::Box);
    firstlinkinTipLabel2 ->setGeometry(QRect(40,31 ,60,20));
    firstlinkinNum = new QLineEdit(this);
    firstlinkinNum->setGeometry(QRect(110,30 ,35,20));
    firstlinkinNum->setValidator(new QIntValidator(1,31,this));
    firstnum = 1 ;
    firstlinkinNum->setText(QString("%1").arg(firstnum));
    //    firstlinkinNum->setReadOnly(true);
    QObject::connect(firstlinkinNum,SIGNAL(textChanged(QString)),this,SLOT(linknumreturn(QString)));

    firsthideComboBox = new QComboBox(this);
    firsthideComboBox->setGeometry(QRect(40,55 ,80,20));
    firsthideComboBox->addItem(tr("屏蔽"));
    firsthideComboBox->addItem(tr("不屏蔽"));
    QObject::connect(firsthideComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(shieldreturn(int)));

    //    firsthideCheckBox = new QCheckBox(tr("屏蔽"),firstlinkInGroup);
    //    firsthideCheckBox->setGeometry(QRect(40,55 ,80,20));
    //    firsthideCheckBox->setCheckState(Qt::Checked);
    //    QObject::connect(firsthideCheckBox,SIGNAL(stateChanged(int)),this,SLOT(shieldreturn(int)));

    firstlinkinTipLabel3 = new QLabel(tr("已选部位列表"), this);
    firstlinkinTipLabel3->setAlignment(Qt::AlignCenter);
    firstlinkinTipLabel3->setFrameStyle(QFrame::Box);
    firstlinkinTipLabel3->setGeometry(QRect(5,112 ,210,20));

    firstlinkinTipLabel8 = new QLabel(tr("主机"), this);
    firstlinkinTipLabel8->setGeometry(QRect(260,2,60,20));
    firstlinkinTipLabel8->setAlignment(Qt::AlignCenter);
    firstlinkinTipLabel8->setFrameStyle(QFrame::Box);

    firstHostinSpinBox = new QSpinBox(this);
    firstHostinSpinBox->setGeometry(QRect(325,2,60,20));
    firstHostinSpinBox->setRange(1,64);
    firstHostinSpinBox->setValue(1);
    firstHostinSpinBox->setAlignment(Qt::AlignRight);
    QObject::connect(firstHostinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet1(int)));

    firstlinkinTipLabel4 = new QLabel(tr("接口板"), this);
    firstlinkinTipLabel4->setGeometry(QRect(260,25 ,60,20));
    firstlinkinTipLabel4->setAlignment(Qt::AlignCenter);
    firstlinkinTipLabel4->setFrameStyle(QFrame::Box);

    firstIobrinSpinBox = new QSpinBox(this);
    firstIobrinSpinBox->setGeometry(QRect(325,25,60,20));
    firstIobrinSpinBox->setRange(1,35);
    firstIobrinSpinBox->setValue(1);
    firstIobrinSpinBox->setAlignment(Qt::AlignRight);
    QObject::connect(firstIobrinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet1(int)));

    firstlinkinTipLabel5 = new QLabel(tr("回  路"), this);
    firstlinkinTipLabel5->setGeometry(QRect(260,47,60,20));
    firstlinkinTipLabel5->setAlignment(Qt::AlignCenter);
    firstlinkinTipLabel5->setFrameStyle(QFrame::Box);

    firstLoopinSpinBox = new QSpinBox(this);
    firstLoopinSpinBox->setGeometry(QRect(325,47,60,20));

    firstLoopinSpinBox->setRange(1,20);
    firstLoopinSpinBox->setValue(1);
    firstLoopinSpinBox->setAlignment(Qt::AlignRight);
    QObject::connect(firstLoopinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet1(int)));

    firstlinkinTipLabel7 = new QLabel(tr("线  路"), this);
    firstlinkinTipLabel7->setGeometry(QRect(260,69,60,20));
    firstlinkinTipLabel7->setAlignment(Qt::AlignCenter);
    firstlinkinTipLabel7->setFrameStyle(QFrame::Box);

    firstLineinSpinBox = new QSpinBox(this);
    firstLineinSpinBox->setGeometry(QRect(325,69,60,20));

    firstLineinSpinBox->setRange(1,2);
    firstLineinSpinBox->setValue(1);
    firstLineinSpinBox->setAlignment(Qt::AlignRight);

    firstlinkinTipLabel6 = new QLabel(tr("待选部位列表"), this);
    firstlinkinTipLabel6->setAlignment(Qt::AlignCenter);
    firstlinkinTipLabel6->setGeometry(QRect(260,112 ,120,20));
    firstlinkinTipLabel6->setFrameStyle(QFrame::Box);

    firstlinkinListWidget2 = new QListWidget(this);
    firstlinkinListWidget2->setGeometry(QRect(260,135 ,120,310));
    this->readUnitSet1(1);//初始化主机号接口板回路都为1的单元设备

    firstLinkinInsertButton = new QPushButton(tr("添加"),this);
    firstLinkinInsertButton->setGeometry(QRect(220,190 ,35,20));
    QObject::connect(firstLinkinInsertButton,SIGNAL(clicked()),this,SLOT(firstLinkinInsertClicked()));

    firstlinkinListWidget = new QListWidget(this);
    firstlinkinListWidget->setGeometry(QRect(5,135 ,210,310));

    firstLinkinOutsertButton = new QPushButton(tr("删除"),this);
    firstLinkinOutsertButton->setGeometry(QRect(220,230 ,35,20));
    QObject::connect(firstLinkinOutsertButton,SIGNAL(clicked()),this,SLOT(firstLinkinOutsertClicked()));


    firstlinkoutFrame = new QFrame(this);
    firstlinkoutFrame->setGeometry(QRect(400, 0 ,390, 450));
    firstlinkoutFrame->setFrameStyle(QFrame::Box);

    firstlinkOutTipLabel = new QLabel(tr("联动输出"), this);
    firstlinkOutTipLabel->setAlignment(Qt::AlignCenter);
    firstlinkOutTipLabel->setFrameStyle(QFrame::Box);
    firstlinkOutTipLabel ->setGeometry(QRect(440,5,80,20));

    firstlinkoutTipLabel3 = new QLabel(tr("已选部位列表"), this);
    firstlinkoutTipLabel3->setAlignment(Qt::AlignCenter);
    firstlinkoutTipLabel3->setFrameStyle(QFrame::Box);
    firstlinkoutTipLabel3->setGeometry(QRect(405,112 ,210,20));

    firstlinkoutTipLabel8 = new QLabel(tr("主机"), this);
    firstlinkoutTipLabel8->setGeometry(QRect(660,2,60,20));
    firstlinkoutTipLabel8->setAlignment(Qt::AlignCenter);
    firstlinkoutTipLabel8->setFrameStyle(QFrame::Box);
    firstHostoutSpinBox = new QSpinBox(this);
    firstHostoutSpinBox->setGeometry(QRect(725,2,60,20));

    firstHostoutSpinBox->setRange(1,64);
    firstHostoutSpinBox->setValue(1);
    firstHostoutSpinBox->setAlignment(Qt::AlignRight);
    QObject::connect(firstHostoutSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet2(int)));

    firstlinkoutTipLabel4 = new QLabel(tr("接口板"), this);
    firstlinkoutTipLabel4->setGeometry(QRect(660,25 ,60,20));
    firstlinkoutTipLabel4->setAlignment(Qt::AlignCenter);
    firstlinkoutTipLabel4->setFrameStyle(QFrame::Box);
    firstIobroutSpinBox = new QSpinBox(this);
    firstIobroutSpinBox->setGeometry(QRect(725,25,60,20));
    firstIobroutSpinBox->setRange(1,35);
    firstIobroutSpinBox->setValue(1);
    firstIobroutSpinBox->setAlignment(Qt::AlignRight);
    QObject::connect(firstIobroutSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet2(int)));

    firstlinkoutTipLabel5 = new QLabel(tr("回  路"), this);
    firstlinkoutTipLabel5->setGeometry(QRect(660,47,60,20));
    firstlinkoutTipLabel5->setAlignment(Qt::AlignCenter);
    firstlinkoutTipLabel5->setFrameStyle(QFrame::Box);
    firstLoopoutSpinBox = new QSpinBox(this);
    firstLoopoutSpinBox->setGeometry(QRect(725,47,60,20));
    firstLoopoutSpinBox->setRange(1,20);
    firstLoopoutSpinBox->setValue(1);
    firstLoopoutSpinBox->setAlignment(Qt::AlignRight);
    QObject::connect(firstLoopoutSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet2(int)));

    firstlinkoutTipLabel7 = new QLabel(tr("线  路"), this);
    firstlinkoutTipLabel7->setGeometry(QRect(660,69,60,20));
    firstlinkoutTipLabel7->setAlignment(Qt::AlignCenter);
    firstlinkoutTipLabel7->setFrameStyle(QFrame::Box);
    firstLineoutSpinBox = new QSpinBox(this);
    firstLineoutSpinBox->setGeometry(QRect(725,69,60,20));
    firstLineoutSpinBox->setRange(1,2);
    firstLineoutSpinBox->setValue(1);
    firstLineoutSpinBox->setAlignment(Qt::AlignRight);

    firstlinkoutTipLabel6 = new QLabel(tr("待选部位列表"), this);
    firstlinkoutTipLabel6->setAlignment(Qt::AlignCenter);
    firstlinkoutTipLabel6->setGeometry(QRect(660,112 ,120,20));
    firstlinkoutTipLabel6->setFrameStyle(QFrame::Box);

    firstlinkoutListWidget2 = new QListWidget(this);
    firstlinkoutListWidget2->setGeometry(QRect(660,135 ,120,310));
    this->readUnitSet2(1);//初始化主机号接口板回路都为1的单元设备
    //上面是第一个窗口的

    firstLinkoutInsertButton = new QPushButton(tr("添加"),this);
    firstLinkoutInsertButton->setGeometry(QRect(620,190 ,35,20));
    QObject::connect(firstLinkoutInsertButton,SIGNAL(clicked()),this,SLOT(firstLinkoutInsertClicked()));

    firstlinkoutListWidget = new QListWidget(this);
    firstlinkoutListWidget->setGeometry(QRect(405,135 ,210,310));

    firstLinkoutOutsertButton = new QPushButton(tr("删除"),this);
    firstLinkoutOutsertButton->setGeometry(QRect(620,230 ,35,20));
    QObject::connect(firstLinkoutOutsertButton,SIGNAL(clicked()),this,SLOT(firstLinkoutOutsertClicked()));

    firstokButton = new QPushButton(tr("确定"),this);
    //     firstokButton->setMaximumSize(40,24);
    firstokButton->setGeometry(QRect(325,455,40,24));
    connect(firstokButton, SIGNAL(clicked()),this, SLOT(firstokButtonClicked()));

    firstcancelButton = new QPushButton(tr("取消"),this);
    //     firstcancelButton->setMaximumSize(40,24);
    firstcancelButton->setGeometry(QRect(425,455,40,24));
    connect(firstcancelButton, SIGNAL(clicked()),this, SLOT(firstcancelButtonClicked()));

    if(!editEnableFlag)
    {
        firstlinkinNum->setReadOnly(true);
        firsthideComboBox->setEnabled(false);
        firstLinkinInsertButton->setVisible(false);
        firstLinkinOutsertButton->setVisible(false);
        firstLinkoutInsertButton->setVisible(false);
        firstLinkoutOutsertButton->setVisible(false);
        firstokButton->setVisible(false);
        firstcancelButton->setVisible(false);
        QPushButton *okButton = new QPushButton(tr("OK"),this);
        okButton->setGeometry(QRect(375,455,40,24));
        connect(okButton, SIGNAL(clicked()),this->parentWidget(), SLOT(close()));


    }
}

ddLinkSetWidget::~ddLinkSetWidget()
{

}
void ddLinkSetWidget::readUnitSet1(int)
{
    char danyuan_info[20];
    firstlinkinListWidget2->clear();

    int host = firstHostinSpinBox->value();
    int iobr = firstIobrinSpinBox->value();
    int loop = firstLoopinSpinBox->value();
    int node = 1;
    int unitid1 = (host<<24) | (iobr<<16) | (loop<<8) | node;
    int unitid2 = ((host<<24) | (iobr<<16) | (loop<<8) | node)+254;
    //    printf("unitid%x %x\n",unitid1,unitid2);
    QSqlDatabase::database().transaction();
    //    QSqlQuery query10;
    //    query10.setForwardOnly(true);
    //    query10.exec(QString("select iobrregist from iobrinfo WHERE id = %1 ").arg(iobr));
    //    while(query10.next())
    //    {
    //        //        printf("第%d条query1是%d",iobr,query1.value(1).toInt());
    //        //        qDebug()<<"第"<<iobr<<"条query1是 "<<query10.value(0).toBool();

    //        if(query10.value(0).toBool())
    //        {

    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg(unitid1).arg(unitid2));
    while (query.next())
    {
        if(query.value(1).toBool())
        {
            if(! query.value(2).toBool())
            {
                int danyuannode =  (query.value(5).toInt() & 0xFF);
                //memset(danyuan_info, 0x0, 20);
                sprintf(danyuan_info, "%03d",danyuannode);
                QString history_info = QString("%1%2").arg(danyuan_info)
                        .arg(Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF));
                //         printf("danyuan%d",danyuannode);
                firstlinkinListWidget2->addItem(tr("%1").arg(history_info));
            }
        }
    }
    //        }
    //    }
    QSqlDatabase::database().commit();
}


void ddLinkSetWidget::readUnitSet2(int)
{
    char danyuan_info[20];
    firstlinkoutListWidget2->clear();

    int host = firstHostoutSpinBox->value();
    int iobr = firstIobroutSpinBox->value();
    int loop = firstLoopoutSpinBox->value();
    int node = 1;
    int unitid1 = (host<<24) | (iobr<<16) | (loop<<8) | node;
    int unitid2 = ((host<<24) | (iobr<<16) | (loop<<8) | node)+254;
    //    printf("unitid%x %x\n",unitid1,unitid2);
    QSqlDatabase::database().transaction();

    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg(unitid1).arg(unitid2));
    while (query.next())
    {
        if(query.value(1).toBool())
        {
            if(! query.value(2).toBool())
            {
                //                int danyuannode =  (query.value(5).toInt() & 0xFF)+1;
                int danyuannode =  (query.value(5).toInt() & 0xFF);
                //memset(danyuan_info, 0x0, 20);
                sprintf(danyuan_info, "%03d",danyuannode);
                //        int danyuannode =  query.value(0).toInt() ;
                QString history_info = QString("%1%2").arg(danyuan_info)
                        .arg(Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF));
                //         printf("danyuan%d",danyuannode);
                firstlinkoutListWidget2->addItem(tr("%1").arg(history_info));
            }
        }
    }
    QSqlDatabase::database().commit();
}


void ddLinkSetWidget::linkDisplay(int id)
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    firstlinkid = id;

    char dlinkininfo1 [20];
    char dlinkininfo2 [20];
    query.exec(QString("select * from linkset WHERE linkid = %1").arg(id));

    while (query.next())
    {
        firstshield = query.value(1).toInt();
//        qDebug()<<"query.value(1).toInt()"<<query.value(1).toInt();
//        qDebug()<<"firstshield"<<firstshield;
        firsthideComboBox->setCurrentIndex(firstshield);

        firstlinkinNum->setText(QString("%1").arg(query.value(4).toString()));
        firstnum = query.value(4).toInt();

        for(int i = 0;i < 31; i++)
        {
            long long int k = query.value(i+5).toLongLong();
            linkin[i] = k ;
            if(k)
            {
                int host = (k >> 32) & 0xFF;
                int iobr = (k >> 24) & 0xFF;
                int loop = (k>>16) & 0xFF;
                int node = (k>>8) & 0xFF;
                int unitid = (host<<24) | (iobr<<16) | (loop<<8) | node;
                QString linkindeviceinfo;
                QSqlQuery query2;
                query2.setForwardOnly(true);
                query2.exec(QString("select * from unitset WHERE unitid = %1").arg(unitid));
                while (query2.next())
                {
                    linkindeviceinfo = Type::char_danyuan((query2.value(7).toInt() >> 8) & 0xFF);
                }
                memset(dlinkininfo1, 0x0, 20);
                sprintf(dlinkininfo1, "%02lld%02lld%02lld%03lld",(k >> 32) & 0xFF,(k >> 24) & 0xFF,
                        (k>>16) & 0xFF, (k>>8) & 0xFF);
                memset(dlinkininfo2, 0x0, 20);
                sprintf(dlinkininfo2, "#%01lld", k &0xFF);
                QString dlinkininfo = QString("%1%2 %3").arg(dlinkininfo1).arg(linkindeviceinfo).arg(dlinkininfo2);
                firstlinkinListWidget->addItem(dlinkininfo);
            }
            else
            {
                break;
            }
        }
        for(int j = 0;j< 31;j++)
        {
            long long int m = query.value(j+36).toLongLong();
            linkout[j] = m;
            if(m)
            {
                int host = (m >> 32) & 0xFF;
                int iobr = (m >> 24) & 0xFF;
                int loop = (m>>16) & 0xFF;
                int node = (m>>8) & 0xFF;
                int unitid = (host<<24) | (iobr<<16) | (loop<<8) | node;
                QString linkindeviceinfo;
                QSqlQuery query3;
                query3.setForwardOnly(true);
                query3.exec(QString("select * from unitset WHERE unitid = %1").arg(unitid));
                while (query3.next())
                {
                    linkindeviceinfo = Type::char_danyuan((query3.value(7).toInt() >> 8) & 0xFF);
                }
                memset(dlinkininfo1, 0x0, 20);
                sprintf(dlinkininfo1, "%02lld%02lld%02lld%03lld",(m >> 32) & 0xFF,
                        (m >> 24) & 0xFF, (m>>16) & 0xFF, (m>>8) & 0xFF);
                memset(dlinkininfo2, 0x0, 20);
                sprintf(dlinkininfo2, "#%01lld", m &0xFF);
                QString dlinkininfo = QString("%1%2 %3").arg(dlinkininfo1).arg(linkindeviceinfo).arg(dlinkininfo2);
                firstlinkoutListWidget->addItem(dlinkininfo);
            }
            else
            {
                break;
            }
        }
    }
    QSqlDatabase::database().commit();
}

void ddLinkSetWidget::firstLinkinInsertClicked()
{
    if(firstlinkinListWidget2->currentRow() != (-1))
    {
        char linkininfo1[20];
        char linkininfo2[20];

        int host = firstHostinSpinBox->value();
        int iobr = firstIobrinSpinBox->value();
        int loop = firstLoopinSpinBox->value();
        int node = firstlinkinListWidget2->currentItem()->text().mid(0,3).toInt();
        int unitid = (host<<24) | (iobr<<16) | (loop<<8) | node;
        QString linkindeviceinfo;

        QSqlDatabase::database().transaction();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid = %1").arg(unitid));
        while (query.next())
        {
            linkindeviceinfo = Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF);
        }
        QSqlDatabase::database().commit();

//        //memset(linkininfo1, 0x0, 20);
//        sprintf(linkininfo1, "%02d-%02d-%02d-%03d",firstHostinSpinBox->value()
//                ,firstIobrinSpinBox->value(),firstLoopinSpinBox->value(),node);
//        //memset(linkininfo1, 0x0, 20);
//        sprintf(linkininfo2, "#%01d",firstLineinSpinBox->value());
//        QString dtstr = QString("%1 %2 %3").arg(linkininfo1).arg(linkindeviceinfo).arg(linkininfo2);

//        long long int k = ((long  long  int)dtstr.left(2).toInt()<<32) | (dtstr.mid(3,2).toInt()<<24)
//                | (dtstr.mid(6,2).toInt()<<16) | dtstr.mid(9,3).toInt()<<8 | dtstr.right(1).toInt();

        sprintf(linkininfo1, "%02d%02d%02d%03d",firstHostinSpinBox->value()
                ,firstIobrinSpinBox->value(),firstLoopinSpinBox->value(),node);

        sprintf(linkininfo2, "#%01d",firstLineinSpinBox->value());
        QString dtstr = QString("%1%2 %3").arg(linkininfo1).arg(linkindeviceinfo).arg(linkininfo2);

        long long int k = ((long  long  int)dtstr.left(2).toInt()<<32) | (dtstr.mid(2,2).toInt()<<24)
                | (dtstr.mid(4,2).toInt()<<16) | dtstr.mid(6,3).toInt()<<8 | dtstr.right(1).toInt();

        if (firstlinkinListWidget->count()+1 > 31)
        {
            //            QMessageBox *box = new QMessageBox(this);
            //            connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
            //            box->setWindowTitle(tr("警告"));
            //            box->setIcon(QMessageBox::Warning);
            //            box->setText(tr("点联动条数已达31条的上限，不能再添加"));
            //            box->setStandardButtons(QMessageBox::Ok);
            //            box->exec();
            QMessageBox *box = new QMessageBox(tr("警告"),
                                               tr("点联动条数已达31条的上限，不能再添加!"),
                                               QMessageBox::Warning,
                                               QMessageBox::Ok,
                                               QMessageBox::NoButton,
                                               QMessageBox::NoButton,
                                               this);
            box->setAttribute(Qt::WA_DeleteOnClose);
            connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
            box->exec();
        }
        else
        {
            if(qFind(linkin, linkin +firstlinkinListWidget->count()+1,k) == (linkin +firstlinkinListWidget->count()+1))
            {
                linkin[firstlinkinListWidget->count()] =  k;
                firstlinkinListWidget->addItem(dtstr);
            }
            else
            {
                //                QMessageBox *box = new QMessageBox(this);
                //                connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
                //                box->setWindowTitle(tr("警告"));
                //                box->setIcon(QMessageBox::Warning);
                //                box->setText(tr("该点联动已添加，不能重复添加"));
                //                box->setStandardButtons(QMessageBox::Ok);
                //                box->exec();
                QMessageBox *box = new QMessageBox(tr("警告"),
                                                   tr("该点联动已添加，不能重复添加!"),
                                                   QMessageBox::Warning,
                                                   QMessageBox::Ok,
                                                   QMessageBox::NoButton,
                                                   QMessageBox::NoButton,
                                                   this);
                box->setAttribute(Qt::WA_DeleteOnClose);
                connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
                box->exec();
            }
        }
    }
    else
    {
        //        QMessageBox *box = new QMessageBox(this);
        //        connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        //        box->setWindowTitle(tr("警告"));
        //        box->setIcon(QMessageBox::Warning);
        //        box->setText(tr("请在已选框中选择要删除的设备"));
        //        box->setStandardButtons(QMessageBox::Ok);
        //        box->exec();
        QMessageBox *box = new QMessageBox(tr("警告"),
                                           tr("请在待选框中选择要添加的设备!"),
                                           QMessageBox::Warning,
                                           QMessageBox::Ok,
                                           QMessageBox::NoButton,
                                           QMessageBox::NoButton,
                                           this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        box->exec();
    }
}


void ddLinkSetWidget::firstLinkinOutsertClicked()
{

    if(firstlinkinListWidget->currentRow() != (-1))
    {

        QString dtstr = firstlinkinListWidget->currentItem()->text();
        //     int m = t.int_liandongId(dtstr.mid(13,(dtstr.lastIndexOf("#"))-14));
//        long long int k = (dtstr.left(2).toLongLong()<<32) | (dtstr.mid(3,2).toLongLong()<<24)
//                | (dtstr.mid(6,2).toInt()<<16) | dtstr.mid(9,3).toInt()<<8 |dtstr.right(1).toInt();
        long long int k = ((long  long  int)dtstr.left(2).toInt()<<32) | (dtstr.mid(2,2).toInt()<<24)
                | (dtstr.mid(4,2).toInt()<<16) | dtstr.mid(6,3).toInt()<<8 | dtstr.right(1).toInt();


        long long int *j;
        for(j = qFind(linkin, linkin +firstlinkinListWidget->count(),k); j <linkin +firstlinkinListWidget->count()-1; j++)
        {
            *j =*(j+1);
        }
        *j = 0;
        firstlinkinListWidget->takeItem(firstlinkinListWidget->currentRow());
    }
    else
    {
        //        QMessageBox *box = new QMessageBox(this);
        //        connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        //        box->setWindowTitle(tr("警告"));
        //        box->setIcon(QMessageBox::Warning);
        //        box->setText(tr("请在已选框中选择要删除的设备"));
        //        box->setStandardButtons(QMessageBox::Ok);
        //        box->exec();
        QMessageBox *box = new QMessageBox(tr("警告"),
                                           tr("请在已选框中选择要删除的设备!"),
                                           QMessageBox::Warning,
                                           QMessageBox::Ok,
                                           QMessageBox::NoButton,
                                           QMessageBox::NoButton,
                                           this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        box->exec();
    }
}

void ddLinkSetWidget::firstLinkoutInsertClicked()
{
    if(firstlinkoutListWidget2->currentRow() != (-1))
    {
        char linkoutinfo1[20];
        char linkoutinfo2[20];

        int host = firstHostoutSpinBox->value();
        int iobr = firstIobroutSpinBox->value();
        int loop = firstLoopoutSpinBox->value();
        int node = firstlinkoutListWidget2->currentItem()->text().mid(0,3).toInt();
        int unitid = (host<<24) | (iobr<<16) | (loop<<8) | node;
        QString linkoutdeviceinfo;

        QSqlDatabase::database().transaction();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid = %1").arg(unitid));
        while (query.next())
        {
            linkoutdeviceinfo = Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF);
        }
        QSqlDatabase::database().commit();

        //memset(linkoutinfo1, 0x0, 20);
//        sprintf(linkoutinfo1, "%02d-%02d-%02d-%03d",firstHostoutSpinBox->value(),
//                firstIobroutSpinBox->value(),firstLoopoutSpinBox->value(),node);
//        //memset(linkoutinfo2, 0x0, 20);
//        sprintf(linkoutinfo2, "#%01d",firstLineoutSpinBox->value());
//        QString dtstr = QString("%1 %2 %3").arg(linkoutinfo1).arg(linkoutdeviceinfo).arg(linkoutinfo2);

//        long long int k = ((long  long  int)dtstr.left(2).toInt()<<32) | (dtstr.mid(3,2).toInt()<<24) |
//                (dtstr.mid(6,2).toInt()<<16) | dtstr.mid(9,3).toInt()<<8 | dtstr.right(1).toInt();


        sprintf(linkoutinfo1, "%02d%02d%02d%03d",firstHostoutSpinBox->value(),
                firstIobroutSpinBox->value(),firstLoopoutSpinBox->value(),node);
        //memset(linkoutinfo2, 0x0, 20);
        sprintf(linkoutinfo2, "#%01d",firstLineoutSpinBox->value());
        QString dtstr = QString("%1%2 %3").arg(linkoutinfo1).arg(linkoutdeviceinfo).arg(linkoutinfo2);

        long long int k = ((long  long  int)dtstr.left(2).toInt()<<32) | (dtstr.mid(2,2).toInt()<<24) |
                (dtstr.mid(4,2).toInt()<<16) | dtstr.mid(6,3).toInt()<<8 | dtstr.right(1).toInt();

        //    printf("值为%llx",k);

        if (firstlinkoutListWidget->count()+1 > 31)
        {
            //            QMessageBox *box = new QMessageBox(this);
            //            connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
            //            box->setWindowTitle(tr("警告"));
            //            box->setIcon(QMessageBox::Warning);
            //            box->setText(tr("点联动条数已达31条的上限，不能再添加"));
            //            box->setStandardButtons(QMessageBox::Ok);
            //            box->exec();
            QMessageBox *box = new QMessageBox(tr("警告"),
                                               tr("点联动条数已达31条的上限，不能再添加!"),
                                               QMessageBox::Warning,
                                               QMessageBox::Ok,
                                               QMessageBox::NoButton,
                                               QMessageBox::NoButton,
                                               this);
            box->setAttribute(Qt::WA_DeleteOnClose);
            connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
            box->exec();
        }
        else
        {
            if(qFind(linkout, linkout +firstlinkoutListWidget->count()+1,k) == (linkout +firstlinkoutListWidget->count()+1))
            {
                linkout[firstlinkoutListWidget->count()] =  k;
                firstlinkoutListWidget->addItem(dtstr);
            }
            else
            {
                //                QMessageBox *box = new QMessageBox(this);
                //                connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
                //                box->setWindowTitle(tr("警告"));
                //                box->setIcon(QMessageBox::Warning);
                //                box->setText(tr("该点联动已添加，不能重复添加"));
                //                box->setStandardButtons(QMessageBox::Ok);
                //                box->exec();
                QMessageBox *box = new QMessageBox(tr("警告"),
                                                   tr("该点联动已添加，不能重复添加!"),
                                                   QMessageBox::Warning,
                                                   QMessageBox::Ok,
                                                   QMessageBox::NoButton,
                                                   QMessageBox::NoButton,
                                                   this);
                box->setAttribute(Qt::WA_DeleteOnClose);
                connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
                box->exec();
            }
        }

    }
    else
    {
        //        QMessageBox *box = new QMessageBox(this);
        //        connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        //        box->setWindowTitle(tr("警告"));
        //        box->setIcon(QMessageBox::Warning);
        //        box->setText(tr("请在待选框中选择要添加的设备"));
        //        box->setStandardButtons(QMessageBox::Ok);
        //        box->exec();
        QMessageBox *box = new QMessageBox(tr("警告"),
                                           tr("请在待选框中选择要添加的设备!"),
                                           QMessageBox::Warning,
                                           QMessageBox::Ok,
                                           QMessageBox::NoButton,
                                           QMessageBox::NoButton,
                                           this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        box->exec();
    }
}


void ddLinkSetWidget::firstLinkoutOutsertClicked()
{

    if(firstlinkoutListWidget->currentRow() != (-1))
    {

        QString dtstr = firstlinkoutListWidget->currentItem()->text();
        //     int m = t.int_liandongId(dtstr.mid(13,(dtstr.lastIndexOf("#"))-14));
//        long long int k = (dtstr.left(2).toLongLong()<<32) | (dtstr.mid(3,2).toLongLong()<<24) |
//                (dtstr.mid(6,2).toInt()<<16) | dtstr.mid(9,3).toInt()<<8 |dtstr.right(1).toInt();


        long long int k = ((long  long  int)dtstr.left(2).toInt()<<32) | (dtstr.mid(2,2).toInt()<<24) |
                (dtstr.mid(4,2).toInt()<<16) | dtstr.mid(6,3).toInt()<<8 | dtstr.right(1).toInt();

        long long int *j;
        for(j = qFind(linkout, linkout +firstlinkoutListWidget->count(),k); j <linkout +firstlinkoutListWidget->count()-1; j++)
        {
            *j =*(j+1);

        }
        *j = 0;
        firstlinkoutListWidget->takeItem(firstlinkoutListWidget->currentRow());

    }
    else
    {
        //        QMessageBox *box = new QMessageBox(this);
        //        connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        //        box->setWindowTitle(tr("警告"));
        //        box->setIcon(QMessageBox::Warning);
        //        box->setText(tr("请在已选框中选择要删除的设备"));
        //        box->setStandardButtons(QMessageBox::Ok);
        //        box->exec();
        QMessageBox *box = new QMessageBox(tr("警告"),
                                           tr("请在已选框中选择要删除的设备!"),
                                           QMessageBox::Warning,
                                           QMessageBox::Ok,
                                           QMessageBox::NoButton,
                                           QMessageBox::NoButton,
                                           this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        box->exec();
    }
}



void ddLinkSetWidget::firstokButtonClicked()
{
    //    QMessageBox *box = new QMessageBox(this);
    //    connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
    //    box->setWindowTitle(tr("提示"));
    //    box->setIcon(QMessageBox::Information);
    //    box->setText(tr("设置将会保存"));
    //    box->setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    //    if(box->exec() == QMessageBox::Yes)
    //    {
    //        this->updateLinkset();
    //    }
    QMessageBox *box = new QMessageBox(tr("提示"),
                                       tr("设置将会保存!"),
                                       QMessageBox::Information,
                                       QMessageBox::Yes | QMessageBox::Default,
                                       QMessageBox::NoButton,
                                       QMessageBox::No | QMessageBox::Escape,
                                       this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
    switch(box->exec())
    {
    case QMessageBox::Yes:
    {
        this->updateLinkset();

    }
    break;
    case  QMessageBox::No:
    {
        //this->close();
    }
    break;
    }
}


void ddLinkSetWidget::updateLinkset()
{
    emit  changeTypeLabel(0);

    QSqlQuery query;
    //    query.exec("UPDATE linkset SET linknum = 5 WHERE linkid = 2 ");

    query.clear();
    query.prepare(QString("UPDATE linkset SET "
                          "linkshield=?, linksetup=?, linktype=?, linknum=?, "
                          "linkINInfo1=?, linkINInfo2=?, linkINInfo3=?, linkINInfo4=?, linkINInfo5=?, linkINInfo6=?, linkINInfo7=?, linkINInfo8=?, linkINInfo9=?, linkINInfo10=?, "
                          "linkINInfo11=?, linkINInfo12=?, linkINInfo13=?, linkINInfo14=?, linkINInfo15=?, linkINInfo16=?, linkINInfo17=?, linkINInfo18=?, linkINInfo19=?, "
                          "linkINInfo20=?, linkINInfo21=?, linkINInfo22=?, linkINInfo23=?, linkINInfo24=?, linkINInfo25=?, linkINInfo26=?, linkINInfo27=?, linkINInfo28=?, "
                          "linkINInfo29=?, linkINInfo30=?, linkINInfo31=?, linkOUTInfo1=?, linkOUTInfo2=?, linkOUTInfo3=?, linkOUTInfo4=?, linkOUTInfo5=?, linkOUTInfo6=?, "
                          "linkOUTInfo7=?, linkOUTInfo8=?, linkOUTInfo9=?, linkOUTInfo10=?, linkOUTInfo11=?, linkOUTInfo12=?, linkOUTInfo13=?, linkOUTInfo14=?, "
                          "linkOUTInfo15=?, linkOUTInfo16=?, linkOUTInfo17=?, linkOUTInfo18=?, linkOUTInfo19=?, linkOUTInfo20=?, linkOUTInfo21=?, linkOUTInfo22=?, "
                          "linkOUTInfo23=?, linkOUTInfo24=?, linkOUTInfo25=?, linkOUTInfo26=?, linkOUTInfo27=?, linkOUTInfo28=?, linkOUTInfo29=?, linkOUTInfo30=?, linkOUTInfo31=? "
                          "WHERE linkid =%1").arg(firstlinkid));

    //   query.prepare(QString("UPDATE linkset SET linknum=?,linksetup=?, linkshield=?, linktype=?  WHERE linkid = %1").arg(firstlinkid));
    printf("firstnum%dfirstlinkid%d",firstnum,firstlinkid);

    query.bindValue(0,firstshield);
    // qDebug()<<"shujukuzhong"<<firstshield;
    //      printf("firstshield%d",firstshield);
    query.bindValue(1,firstsetup);
    query.bindValue(2,firsttype);
    query.bindValue(3,firstnum);

    for (int j=0; j<31; j++)
    {
        query.bindValue(j+4,linkin[j]);
        //       qDebug() <<j+5 ;
    }
    for (int j=0; j<31; j++)
    {
        query.bindValue(j+35,linkout[j]);
    }
    bool bSuccess = query.exec();
    if (!bSuccess)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("update error"));
    }
    emit transferddLinknum(firstlinkid,firstnum);
    emit databaseChange();
}


void ddLinkSetWidget::firstcancelButtonClicked()
{
    //    QMessageBox *box = new QMessageBox(this);
    //    connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
    //    box->setWindowTitle(tr("警告"));
    //    box->setIcon(QMessageBox::Warning);
    //    box->setText(tr("设置将不会保存"));
    //    box->setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    //    if(box->exec() == QMessageBox::Yes)
    //    {
    //        this->parentWidget()->parentWidget()->parentWidget()->close();
    //    }
    QMessageBox *box = new QMessageBox(tr("提示"),
                                       tr("设置将不会保存!"),
                                       QMessageBox::Information,
                                       QMessageBox::Yes | QMessageBox::Default,
                                       QMessageBox::NoButton,
                                       QMessageBox::No | QMessageBox::Escape,
                                       this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
    switch(box->exec())
    {
    case QMessageBox::Yes:
    {
        this->parentWidget()->parentWidget()->parentWidget()->close();
    }
    break;
    case  QMessageBox::No:
    {
        //this->close();
    }
    break;
    }
}


void ddLinkSetWidget::linknumreturn(QString test)
{
    firstnum = test.toInt();
    if(firstnum == 0)
    {
        firstnum = 1;
        firstlinkinNum->setText(QString("%1").arg(firstnum));
    }
}

void ddLinkSetWidget::shieldreturn(int shield)
{
    firstshield = shield;
    //    if(!shield)
    //    {
    //        firstshield = shield;
    //        qDebug()<<firstshield;
    //        // printf("屏蔽%d\n",firstsetup);
    //    }
    //    else
    //    {
    //        firstshield = shield-1;
    //        qDebug()<<firstshield;
    //        //   printf("屏蔽%d\n",firstsetup);
    //    }
}

void ddLinkSetWidget::clearfirstWidget()
{
    firstlinkinListWidget->clear();
    firstlinkoutListWidget->clear();
    //    firsthideCheckBox->setCheckState(Qt::Checked);
    firsthideComboBox->setCurrentIndex(0);
    firstnum = 1 ;
    firstlinkinNum->setText(QString("%1").arg(firstnum));
}
