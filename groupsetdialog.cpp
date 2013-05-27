#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include<QtCore/QVariant>
#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtCore/QDateTime>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtGui/QMessageBox>
#include <QtDebug>


#include "groupsetdialog.h"
#include "info.h"
#include "type.h"

GroupsetDialog::GroupsetDialog(QWidget *parent) :
    QDialog(parent)
{
}
GroupsetDialog::~GroupsetDialog()
{
}
GroupsetDialog::GroupsetDialog(int iobr, int loop, QWidget *parent):
    QDialog(parent)
{
    iobrnum = iobr;
    loopnum = loop;
    createItems();
    setWindowTitle(tr("批设置"));
    this->setFixedSize (QSize(400, 500));

    connect(this->parentWidget()->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
}
void GroupsetDialog::createItems()
{
    tipLabel = new QLabel(tr("对%1号接口板%2号回路进行批设置").arg(iobrnum).arg(loopnum),this);
    tipLabel->setGeometry(QRect(110, 0 , 195, 24));
    nodeLabel1 = new QLabel(tr("节点地址从:"), this);
    nodeLabel1->setGeometry(QRect(80, 60, 90, 24));
    //    startNode = new QLineEdit(this);
    //    startNode->setGeometry(QRect(180, 60, 45, 24));
    //    startNode->setValidator(new QIntValidator(1,255,this));
    nodeLabel2 = new QLabel(tr("到"), this);
    nodeLabel2->setGeometry(QRect(245, 60, 30, 24));
    //    endNode = new QLineEdit(this);
    //    endNode->setGeometry(QRect(260, 60, 45, 24));
    //    endNode->setValidator(new QIntValidator(1,255,this));

    startNode = new QSpinBox(this);
    startNode->setGeometry(QRect(180, 60, 60, 24));
    startNode->setRange(1,255);


    endNode = new QSpinBox(this);
    endNode->setGeometry(QRect(280, 60, 60, 24));
    endNode->setRange(1,255);



    equipLabel = new QLabel(tr("设备类型"), this);
    equipLabel->setGeometry(QRect(80, 100, 80, 24));
    equipComboBox = new QComboBox(this);
    equipComboBox->setGeometry(QRect(180, 100, 100, 24));
    for (int i = 0;i < (int)sizeof(char_liandong) /((int)sizeof(char)*4);i++)
    {
        equipComboBox->addItem(tr(char_liandong[i]));
    }

    maskLabel = new QLabel(tr("屏蔽"), this);
    maskLabel->setGeometry(QRect(80, 140, 40, 24));
    maskComboBox = new QComboBox(this);
    maskComboBox->setGeometry(QRect(180, 140, 60, 24));
    maskComboBox->addItem(tr("√"));
    maskComboBox->addItem(tr("×"));

    startDelayLabel = new QLabel(tr("报启延时"), this);
    startDelayLabel->setGeometry(QRect(80, 180, 65, 24));
    startDelaySpinBox = new QSpinBox(this);
    startDelaySpinBox->setGeometry(QRect(150, 180, 60, 24));
    startDelaySpinBox->setRange(0,100);
    ImpulseDelayLabel = new QLabel(tr("脉冲延时"), this);
    ImpulseDelayLabel->setGeometry(QRect(215, 180, 65, 24));
    ImpulseDelaySpinBox = new QSpinBox(this);
    ImpulseDelaySpinBox->setGeometry(QRect(285, 180, 60, 24));
    ImpulseDelaySpinBox->setRange(0,100);

    reDetectorLabel = new QLabel(tr("重报探头"), this);
    reDetectorLabel->setGeometry(QRect(80, 220, 65, 24));
    reDetectorComboBox = new QComboBox(this);
    reDetectorComboBox->setGeometry(QRect(150, 220, 60, 24));
    reDetectorComboBox->addItem(tr("是"));
    reDetectorComboBox->addItem(tr("否"));
    flrIndicatorLabel = new QLabel(tr("所属楼显"), this);
    flrIndicatorLabel->setGeometry(QRect(215, 220, 65, 24));
    flrIndicatorSpinBox = new QSpinBox(this);
    flrIndicatorSpinBox->setGeometry(QRect(285, 220, 60, 24));

    buildingLabel = new QLabel(tr("栋号"), this);
    buildingLabel->setGeometry(QRect(80, 260, 40, 24));
    buildingSpinBox = new QSpinBox(this);
    buildingSpinBox->setGeometry(QRect(150, 260, 60, 24));
    buildingSpinBox->setRange(0,100);
    floorLabel = new QLabel(tr("层号"), this);
    floorLabel->setGeometry(QRect(215, 260, 40, 24));
    floorSpinBox = new QSpinBox(this);
    floorSpinBox->setGeometry(QRect(285, 260, 60, 24));
    floorSpinBox->setRange(0,100);
    zoneLabel = new QLabel(tr("区号"), this);
    zoneLabel->setGeometry(QRect(80, 300, 40, 24));
    zoneSpinBox = new QSpinBox(this);
    zoneSpinBox->setGeometry(QRect(150, 300, 60, 24));
    zoneSpinBox->setRange(0,100);
    roomLabel = new QLabel(tr("房号"), this);
    roomLabel->setGeometry(QRect(215, 300, 40, 24));
    roomSpinBox = new QSpinBox(this);
    roomSpinBox->setGeometry(QRect(285, 300, 60, 24));
    roomSpinBox->setRange(0,100);

    alarmLabel = new QLabel(tr("报警级别"), this);
    alarmLabel->setGeometry(QRect(80, 340, 80, 24));
    alarmComboBox = new QComboBox(this);
    alarmComboBox->setGeometry(QRect(180, 340, 80, 24));
    alarmComboBox->addItem(tr("火警"));
    alarmComboBox->addItem(tr("预警"));

    positionLabel = new QLabel(tr("位置描述:"), this);
    positionLabel->setGeometry(QRect(80, 380, 80, 24));
    posititionLineEdit = new QLineEdit(this);
    posititionLineEdit->setGeometry(QRect(80, 410, 240, 24));


    okButton = new QPushButton(tr("确定"),this);
    okButton->setGeometry(QRect(80,460,40,24));
    connect(okButton, SIGNAL(clicked()),this, SLOT(okButtonClicked()));

    cancelButton = new QPushButton(tr("取消"),this);
    cancelButton->setGeometry(QRect(230,460,40,24));
    connect(cancelButton, SIGNAL(clicked()),this, SLOT(close()));



}

void GroupsetDialog::okButtonClicked()
{

    QSqlDatabase::database().transaction();
    int host =1;
    int nodestart = startNode->value();
    int nodeend = endNode->value();


    if(nodestart > nodeend)
    {
        QMessageBox *box = new QMessageBox(tr("警告"),
                                           tr("设置中起始地址大于终止地址，\n"
                                              "请返回修改！!"),
                                           QMessageBox::Warning,
                                           QMessageBox::Ok,
                                           QMessageBox::NoButton,
                                           QMessageBox::NoButton,
                                           this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        box->exec();
        return ;

    }
    else
    {
        QTime time;
        time.restart();

        QSqlQuery query;
        query.clear();
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg((host << 24) | (iobrnum << 16) | (loopnum << 8) | nodestart)
                   .arg((host << 24) | (iobrnum << 16) | (loopnum << 8) | nodeend));

        while (query.next())
        {
            if(query.value(1).toBool())
            {
                int equipstyle = query.value(7).toInt() & 0x0000FF00;
                QSqlQuery query2;
                query2.clear();
                query2.prepare(QString("UPDATE unitset SET "
                                       "shield=?, restated=?, level=?,"
                                       "geo_addr=?, type=?, delay=?, str=?"
                                       "WHERE unitid = %1").arg(query.value(5).toInt()));

                query2.addBindValue(maskComboBox->currentIndex()==0?true : false);
                query2.addBindValue(reDetectorComboBox->currentIndex()==0?true : false);
                query2.addBindValue(alarmComboBox->currentIndex()==0?true : false);
                query2.addBindValue((buildingSpinBox->value() << 24) | (floorSpinBox->value() << 16 | (zoneSpinBox->value() << 8))| roomSpinBox->value());
                query2.addBindValue(((Type::int_liandongId(equipComboBox->currentText())) << 16) | equipstyle | (flrIndicatorSpinBox->value()));
                query2.addBindValue((startDelaySpinBox->value() << 8)| ImpulseDelaySpinBox->value());
                query2.addBindValue(QString(posititionLineEdit->text()));
                bool bSuccess2 = query2.exec();
                if (!bSuccess2)
                {
                    QSqlError lastError = query2.lastError();
                    qDebug() << lastError.driverText()
                             <<QString(QObject::tr("update error"));
                }
                if(maskComboBox->currentIndex()==0?true : false)
                     {
                         //shiled the node,maybe add into the shieldhis
                         this->insertintoShieldHis(query.value(5).toInt(),(equipstyle >>8));
                     }
                     else
                     {
                         //unshiled the node,maybe delete from the shieldhis
                         this->deletefromShieldHis(query.value(5).toInt());
                     }
            }
        }
        qDebug() << QObject::tr("groupupdate耗时：%1 ms").arg(time.elapsed());
        QSqlDatabase::database().commit();
        this->close();
    }

}


void GroupsetDialog::insertintoShieldHis(int unitid, int type)
{


    QSqlDatabase::database().transaction();


    QSqlQuery filter;
    filter.setForwardOnly(true);
    filter.exec(QString("select * from shieldhis WHERE unitid = %1").arg(unitid));

    if (!(filter.next()))
    {

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
                     <<QString(QObject::tr("in GroupsetDialog : insert shieldhis error"));
        }
    }

        QSqlDatabase::database().commit();

}
void GroupsetDialog::deletefromShieldHis(int unitid)
{
    QSqlDatabase::database().transaction();

    QSqlQuery filter;
    filter.setForwardOnly(true);
    filter.exec(QString("select * from shieldhis WHERE unitid = %1").arg(unitid));

    if ((filter.next()))
    {
    QSqlQuery queryDel;
    queryDel.exec(QString("delete  from shieldhis where unitid = %1").arg(unitid));
    bool bSuccess = queryDel.exec();
    if (!bSuccess)
    {
        QSqlError lastError = queryDel.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("in GroupsetDialog :delete shieldhis error"));
    }
    QSqlDatabase::database().commit();
    }

}
