#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>
#include <QtGui/QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include<QtCore/QVariant>
#include <QtDebug>
//#include <QtAlgorithms>

#include "dzlinksetwidget.h"
#include "info.h"
#include "type.h"

dzLinkSetWidget::dzLinkSetWidget(QWidget *parent) :
    QWidget(parent)
{

}

dzLinkSetWidget::dzLinkSetWidget(bool editFlag,QWidget *parent) :
    QWidget(parent)
{

    qFill(linkin, linkin + 31, 0);
    qFill(linkout, linkout + 31, 0);
    editEnableFlag = editFlag;
    secondlinkid = 1;
    secondtype = 1;
    secondsetup = 0;

    secondlinkinFrame = new QFrame(this);
    secondlinkinFrame->setGeometry(QRect(0, 0 ,390, 450));
    secondlinkinFrame->setFrameStyle(QFrame::Box);

    secondlinkinTipLabel = new QLabel(tr("联动输入"), this);
    secondlinkinTipLabel ->setGeometry(QRect(40,5,80,20));
    secondlinkinTipLabel->setAlignment(Qt::AlignCenter);
    secondlinkinTipLabel->setFrameStyle(QFrame::Box);
    //secondlinkinTipLabel->setGeometry(QRect(QPoint (120, 10), linkinTipLabel->sizeHint()));
    // secondlinkinTipLabel->setFrameShape(QFrame::NoFrame);

    secondlinkinTipLabel2 = new QLabel(tr("条件数"), this);
    secondlinkinTipLabel2 ->setGeometry(QRect(40,31 ,60,20));
    secondlinkinTipLabel2->setAlignment(Qt::AlignCenter);
    secondlinkinTipLabel2->setFrameStyle(QFrame::Box);
    secondlinkinNum = new QLineEdit(this);
    secondlinkinNum->setGeometry(QRect(110,30 ,35,20));
    secondlinkinNum->setValidator(new QIntValidator(1,31,this));
    secondnum = 1 ;
    secondlinkinNum->setText(QString("%1").arg(secondnum));
    QObject::connect(secondlinkinNum,SIGNAL(textChanged(QString)),this,SLOT(linknumreturn(QString)));

    //secondlinkinTipLabel->setGeometry(QRect(QPoint (120, 10), linkinTipLabel->sizeHint()));
    // secondlinkinTipLabel2->setFrameShape(QFrame::Box);

    secondhideComboBox = new QComboBox(this);
    secondhideComboBox->setGeometry(QRect(40,55 ,80,20));
    secondhideComboBox->addItem(tr("屏蔽"));
    secondhideComboBox->addItem(tr("不屏蔽"));
    QObject::connect(secondhideComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(shieldreturn(int)));


    //    secondhideCheckBox = new QCheckBox(tr("屏蔽"),secondlinkInGroup);
    //    secondhideCheckBox->setGeometry(QRect(40,55 ,80,20));
    //    secondhideCheckBox->setCheckState(Qt::Checked);
    //    QObject::connect(secondhideCheckBox,SIGNAL(stateChanged(int)),this,SLOT(shieldreturn(int)));


    secondlinkinTipLabel3 = new QLabel(tr("已选部位列表"), this);
    secondlinkinTipLabel3->setAlignment(Qt::AlignCenter);
    secondlinkinTipLabel3->setGeometry(QRect(5,112 ,210,20));
    secondlinkinTipLabel3->setFrameStyle(QFrame::Box);

    secondlinkinTipLabel8 = new QLabel(tr("主机"), this);
    secondlinkinTipLabel8->setGeometry(QRect(260,2,60,20));
    secondlinkinTipLabel8->setAlignment(Qt::AlignCenter);
    secondlinkinTipLabel8->setFrameStyle(QFrame::Box);

    secondHostinSpinBox = new QSpinBox(this);
    secondHostinSpinBox->setGeometry(QRect(325,2,60,20));
    secondHostinSpinBox->setRange(1,64);
    secondHostinSpinBox->setValue(1);
    secondHostinSpinBox->setAlignment(Qt::AlignRight);

    QObject::connect(secondHostinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet(int)));


    secondlinkinTipLabel4 = new QLabel(tr("接口板"), this);
    secondlinkinTipLabel4->setGeometry(QRect(260,25 ,60,20));
    secondlinkinTipLabel4->setAlignment(Qt::AlignCenter);
    secondlinkinTipLabel4->setFrameStyle(QFrame::Box);

    secondIobrinSpinBox = new QSpinBox(this);
    secondIobrinSpinBox->setGeometry(QRect(325,25,60,20));
    secondIobrinSpinBox->setRange(1,35);
    secondIobrinSpinBox->setValue(1);
    secondIobrinSpinBox->setAlignment(Qt::AlignRight);

    QObject::connect(secondIobrinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet(int)));

    secondlinkinTipLabel5 = new QLabel(tr("回  路"), this);
    secondlinkinTipLabel5->setGeometry(QRect(260,47,60,20));
    secondlinkinTipLabel5->setAlignment(Qt::AlignCenter);
    secondlinkinTipLabel5->setFrameStyle(QFrame::Box);

    secondLoopinSpinBox = new QSpinBox(this);
    secondLoopinSpinBox->setGeometry(QRect(325,47,60,20));
    secondLoopinSpinBox->setRange(1,20);
    secondLoopinSpinBox->setValue(1);
    secondLoopinSpinBox->setAlignment(Qt::AlignRight);

    QObject::connect(secondLoopinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet(int)));

    secondlinkinTipLabel7 = new QLabel(tr("线  路"), this);
    secondlinkinTipLabel7->setGeometry(QRect(260,69,60,20));
    secondlinkinTipLabel7->setAlignment(Qt::AlignCenter);
    secondlinkinTipLabel7->setFrameStyle(QFrame::Box);

    secondLineinSpinBox = new QSpinBox(this);
    secondLineinSpinBox->setGeometry(QRect(325,69,60,20));

    secondLineinSpinBox->setRange(1,2);
    secondLineinSpinBox->setValue(1);
    secondLineinSpinBox->setAlignment(Qt::AlignRight);

    secondlinkinTipLabel6 = new QLabel(tr("待选部位列表"), this);
    secondlinkinTipLabel6->setAlignment(Qt::AlignCenter);
    secondlinkinTipLabel6->setGeometry(QRect(260,112 ,120,20));
    secondlinkinTipLabel6->setFrameStyle(QFrame::Box);


    secondlinkinListWidget2 = new QListWidget(this);
    secondlinkinListWidget2->setGeometry(QRect(260,135 ,120,310));
    this->readUnitSet(1);//初始化主机号接口板回路都为1的单元设备

    secondLinkinInsertButton = new QPushButton(tr("添加"),this);
    secondLinkinInsertButton->setGeometry(QRect(220,190 ,35,20));
    QObject::connect(secondLinkinInsertButton,SIGNAL(clicked()),this,SLOT(secondLinkinInsertClicked()));




    secondlinkinListWidget = new QListWidget(this);
    secondlinkinListWidget->setGeometry(QRect(5,135,210,310));


    secondLinkinOutsertButton = new QPushButton(tr("删除"),this);
    secondLinkinOutsertButton->setGeometry(QRect(220,230 ,35,20));
    QObject::connect(secondLinkinOutsertButton,SIGNAL(clicked()),this,SLOT(secondLinkinOutsertClicked()));




    secondlinkoutFrame = new QFrame(this);
    secondlinkoutFrame->setGeometry(QRect(400, 0 ,390, 450));
    secondlinkoutFrame->setFrameStyle(QFrame::Box);



    secondlinkOutTipLabel = new QLabel(tr("联动输出"), this);
    secondlinkOutTipLabel->setGeometry(QRect(440,5,80,20));
    secondlinkOutTipLabel->setAlignment(Qt::AlignCenter);
    secondlinkOutTipLabel->setFrameStyle(QFrame::Box);
    //  secondlinkOutTipLabel->setGeometry(QRect(QPoint (120, 10), linkOutTipLabel->sizeHint()));
    secondlinkOutTipLabel->setFrameShape(QFrame::Box);

    linkTabWidgetSecond = new QTabWidget(this);
    //   linkTabWidgetSecond->setMaximumSize(300,300);
    linkTabWidgetSecond->setGeometry(QRect(405,60,380,385));


    for(int j = 0; j < 3; j++)
    {
        int k = 0;
        secondlinkOutGroup[j] = new QGroupBox(this);
        secondlinkOutGroup[j]->setGeometry(QRect(400, 5 ,365, 470));
        //   secondlinkOutGroup1->setStyleSheet( "border:1px outset;" );
        secondlinkoutNumLabel = new QLabel(tr("NO"), secondlinkOutGroup[j]);
        secondlinkoutNumLabel->setGeometry(QRect(0,5,29,20));
        secondlinkoutNumLabel->setFrameShape(QFrame::Box);
        secondlinkoutHostLabel = new QLabel(tr("主机"), secondlinkOutGroup[j]);
        secondlinkoutHostLabel->setGeometry(QRect(34,5,44,20));
        secondlinkoutHostLabel->setFrameShape(QFrame::Box);
        secondlinkoutIobrLabel = new QLabel(tr("接口板"),secondlinkOutGroup[j]);
        secondlinkoutIobrLabel->setGeometry(QRect(83,5,59,20));
        secondlinkoutIobrLabel->setFrameShape(QFrame::Box);
        secondlinkoutLoopLabel = new QLabel(tr("回路"), secondlinkOutGroup[j]);
        secondlinkoutLoopLabel->setGeometry(QRect(146,5,44,20));
        secondlinkoutLoopLabel->setFrameShape(QFrame::Box);
        secondlinkoutStartLabel = new QLabel(tr("起始"), secondlinkOutGroup[j]);
        secondlinkoutStartLabel->setGeometry(QRect(195,5,39,20));
        secondlinkoutStartLabel->setFrameShape(QFrame::Box);
        secondlinkoutEndLabel = new QLabel(tr("终止"),secondlinkOutGroup[j]);
        secondlinkoutEndLabel->setGeometry(QRect(239,5,39,20));
        secondlinkoutEndLabel->setFrameShape(QFrame::Box);
        secondlinkoutDeviceLabel = new QLabel(tr("设备类型"), secondlinkOutGroup[j]);
        secondlinkoutDeviceLabel->setAlignment(Qt::AlignCenter);
        secondlinkoutDeviceLabel->setGeometry(QRect(283,5,92,20));
        secondlinkoutDeviceLabel->setFrameShape(QFrame::Box);


        for(int i = 0; (i < 11 && (i+j*11) <31); i++)
        {
            secondlinkoutLabel[i+j*11] = new  QLabel(tr("%1").arg(i+j*11+1), secondlinkOutGroup[j]);
            secondlinkoutLabel[i+j*11]->setGeometry(QRect(0,30+i*30,29,20));
            secondlinkoutLabel[i+j*11]->setFrameShape(QFrame::Box);
            secondlinkoutHostSpinBox[i+j*11] = new QSpinBox(secondlinkOutGroup[j]);
            secondlinkoutHostSpinBox[i+j*11]->setGeometry(QRect(34,30+i*30,44,20));
            secondlinkoutHostSpinBox[i+j*11]->setRange(1,64);
            secondlinkoutIobrSpinBox[i+j*11] = new QSpinBox(secondlinkOutGroup[j]);
            secondlinkoutIobrSpinBox[i+j*11]->setGeometry(QRect(83,30+i*30,59,20));
            secondlinkoutIobrSpinBox[i+j*11]->setRange(1,35);
            secondlinkoutLoopSpinBox[i+j*11] = new QSpinBox(secondlinkOutGroup[j]);
            secondlinkoutLoopSpinBox[i+j*11]->setGeometry(QRect(146,30+i*30,44,20));
            secondlinkoutLoopSpinBox[i+j*11]->setRange(1,20);
            secondlinkoutStart[i+j*11] = new QLineEdit(secondlinkOutGroup[j]);
            secondlinkoutStart[i+j*11]->setGeometry(QRect(195,30+i*30,39,20));
            secondlinkoutStart[i+j*11]->setValidator(new QIntValidator(1,255,secondlinkOutGroup[j]));

            secondlinkoutEnd[i+j*11] = new QLineEdit(secondlinkOutGroup[j]);
            secondlinkoutEnd[i+j*11]->setGeometry(QRect(239,30+i*30,39,20));
            secondlinkoutEnd[i+j*11]->setValidator(new QIntValidator(1,255,secondlinkOutGroup[j]));
            secondlinkoutDeviceCombo[i+j*11] = new QComboBox(secondlinkOutGroup[j]);
            secondlinkoutDeviceCombo[i+j*11]->setGeometry(QRect(283,30+i*30,92,20));
            k = i+j*11+1;
        }
        linkTabWidgetSecond->addTab(secondlinkOutGroup[j],tr("组联动%1--%2").arg(j*11+1).arg(k));
    }



    //    for(int j = 0; j < 4; j++)
    //    {
    //        int k = 0;
    //        secondlinkOutGroup[j] = new QGroupBox(ZsecondlinkOutGroup);
    //        secondlinkOutGroup[j]->setGeometry(QRect(0, 5 ,365, 470));
    //        //   secondlinkOutGroup1->setStyleSheet( "border:1px outset;" );
    //        secondlinkoutNumLabel = new QLabel(tr("NO"), secondlinkOutGroup[j]);
    //        secondlinkoutNumLabel->setGeometry(QRect(0,5,25,20));
    //        secondlinkoutNumLabel->setFrameShape(QFrame::Box);
    //        secondlinkoutHostLabel = new QLabel(tr("主机"), secondlinkOutGroup[j]);
    //        secondlinkoutHostLabel->setGeometry(QRect(30,5,45,20));
    //        secondlinkoutHostLabel->setFrameShape(QFrame::Box);
    //        secondlinkoutIobrLabel = new QLabel(tr("接口板"),secondlinkOutGroup[j]);
    //        secondlinkoutIobrLabel->setGeometry(QRect(80,5,55,20));
    //        secondlinkoutIobrLabel->setFrameShape(QFrame::Box);
    //        secondlinkoutLoopLabel = new QLabel(tr("回路"), secondlinkOutGroup[j]);
    //        secondlinkoutLoopLabel->setGeometry(QRect(140,5,45,20));
    //        secondlinkoutLoopLabel->setFrameShape(QFrame::Box);
    //        secondlinkoutStartLabel = new QLabel(tr("起始"), secondlinkOutGroup[j]);
    //        secondlinkoutStartLabel->setGeometry(QRect(190,5,35,20));
    //        secondlinkoutStartLabel->setFrameShape(QFrame::Box);
    //        secondlinkoutEndLabel = new QLabel(tr("终止"),secondlinkOutGroup[j]);
    //        secondlinkoutEndLabel->setGeometry(QRect(230,5,35,20));
    //        secondlinkoutEndLabel->setFrameShape(QFrame::Box);
    //        secondlinkoutDeviceLabel = new QLabel(tr("设备类型"), secondlinkOutGroup[j]);
    //        secondlinkoutDeviceLabel->setAlignment(Qt::AlignCenter);
    //        secondlinkoutDeviceLabel->setGeometry(QRect(270,5,85,20));
    //        secondlinkoutDeviceLabel->setFrameShape(QFrame::Box);


    //        for(int i = 0; (i < 8 && (i+j*8) <31); i++)
    //        {
    //            secondlinkoutLabel[i+j*8] = new  QLabel(tr("%1").arg(i+j*8+1), secondlinkOutGroup[j]);
    //            secondlinkoutLabel[i+j*8]->setGeometry(QRect(0,30+i*30,25,20));
    //            secondlinkoutLabel[i+j*8]->setFrameShape(QFrame::Box);
    //            secondlinkoutHostSpinBox[i+j*8] = new QSpinBox(secondlinkOutGroup[j]);
    //            secondlinkoutHostSpinBox[i+j*8]->setGeometry(QRect(30,30+i*30,45,20));
    //            secondlinkoutHostSpinBox[i+j*8]->setRange(1,64);
    //            secondlinkoutIobrSpinBox[i+j*8] = new QSpinBox(secondlinkOutGroup[j]);
    //            secondlinkoutIobrSpinBox[i+j*8]->setGeometry(QRect(80,30+i*30,55,20));
    //            secondlinkoutIobrSpinBox[i+j*8]->setRange(1,35);
    //            secondlinkoutLoopSpinBox[i+j*8] = new QSpinBox(secondlinkOutGroup[j]);
    //            secondlinkoutLoopSpinBox[i+j*8]->setGeometry(QRect(140,30+i*30,45,20));
    //            secondlinkoutLoopSpinBox[i+j*8]->setRange(1,20);
    //            secondlinkoutStart[i+j*8] = new QLineEdit(secondlinkOutGroup[j]);
    //            secondlinkoutStart[i+j*8]->setGeometry(QRect(190,30+i*30,35,20));
    //            secondlinkoutStart[i+j*8]->setValidator(new QIntValidator(1,255,secondlinkOutGroup[j]));

    //            secondlinkoutEnd[i+j*8] = new QLineEdit(secondlinkOutGroup[j]);
    //            secondlinkoutEnd[i+j*8]->setGeometry(QRect(230,30+i*30,35,20));
    //            secondlinkoutEnd[i+j*8]->setValidator(new QIntValidator(1,255,secondlinkOutGroup[j]));
    //            secondlinkoutDeviceCombo[i+j*8] = new QComboBox(secondlinkOutGroup[j]);
    //            secondlinkoutDeviceCombo[i+j*8]->setGeometry(QRect(270,30+i*30,85,20));
    //            k = i+j*8+1;
    //        }
    //        linkTabWidgetSecond->addTab(secondlinkOutGroup[j],tr("组联动%1--%2").arg(j*8+1).arg(k));
    //    }

    secondokButton = new QPushButton(tr("确定"),this);
    //     secondokButton->setMaximumSize(40,24);
    secondokButton->setGeometry(QRect(325,455,40,24));
    connect(secondokButton, SIGNAL(clicked()),this, SLOT(secondokButtonClicked()));

    secondcancelButton = new QPushButton(tr("取消"),this);
    //     secondcancelButton->setMaximumSize(40,24);
    secondcancelButton->setGeometry(QRect(425,455,40,24));
    //   connect(secondcancelButton, SIGNAL(clicked()),parentWidget(), SLOT(close()));
    connect(secondcancelButton, SIGNAL(clicked()),this, SLOT(secondcancelButtonClicked()));

    if(!editEnableFlag)
    {
        secondlinkinNum->setReadOnly(true);
        secondhideComboBox->setEnabled(false);
        secondLinkinInsertButton->setVisible(false);
        secondLinkinOutsertButton->setVisible(false);
        secondokButton->setVisible(false);
        secondcancelButton->setVisible(false);
        QPushButton *okButton = new QPushButton(tr("OK"),this);
        okButton->setGeometry(QRect(375,455,40,24));
        connect(okButton, SIGNAL(clicked()),this->parentWidget(), SLOT(close()));

        for(int j = 0; j<31;j++)
        {
            secondlinkoutHostSpinBox[j]->setReadOnly(true);
            secondlinkoutIobrSpinBox[j]->setReadOnly(true);
            secondlinkoutLoopSpinBox[j]->setReadOnly(true);
            secondlinkoutStart[j]->setReadOnly(true);
            secondlinkoutEnd[j]->setReadOnly(true);
            secondlinkoutDeviceCombo[j]->setEnabled(false);
        }

    }



}

dzLinkSetWidget::~dzLinkSetWidget()
{
}


void dzLinkSetWidget::linkDisplay(int id)
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    secondlinkid = id;

    char dlinkininfo1 [20];
    char dlinkininfo2 [20];
    query.exec(QString("select * from linkset WHERE linkid = %1").arg(id));

    while (query.next())
    {
        secondshield = query.value(1).toInt();
//        qDebug()<<"query.value(1).toInt()"<<query.value(1).toInt();
//        qDebug()<<"secondshield"<<secondshield;
        secondhideComboBox->setCurrentIndex(secondshield);

        secondlinkinNum->setText(QString("%1").arg(query.value(4).toString()));
        secondnum = query.value(4).toInt();

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
                //memset(dlinkininfo1, 0x0, 20);
                sprintf(dlinkininfo1, "%02lld%02lld%02lld%03lld",(k >> 32) & 0xFF,(k >> 24) & 0xFF,
                        (k>>16) & 0xFF, (k>>8) & 0xFF);
                //memset(dlinkininfo2, 0x0, 20);
                sprintf(dlinkininfo2, "#%01lld", k &0xFF);
                QString dlinkininfo = QString("%1%2 %3").arg(dlinkininfo1).arg(linkindeviceinfo).arg(dlinkininfo2);
                secondlinkinListWidget->addItem(dlinkininfo);
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
            //       printf("  %016llx",m);
            if(m)
            {
                int host = 0, iobr = 0, loop  = 0, znodestar = 0, znodeend = 0,zequiptype = 0,zsetupnum = 0;
                host = (m >> 48) & 0xFF;
                iobr = (m >> 40) & 0xFF;
                loop = (m >> 32) & 0xFF;
                znodestar = ((m >> 24) & 0xFF) ;
                znodeend = ((m >> 16) & 0xFF) ;
                zequiptype = (m >> 8) & 0xFF;
                zsetupnum = m  & 0xFF;
                secondlinkoutHostSpinBox[j]->setValue(host);
                secondlinkoutIobrSpinBox[j]->setValue(iobr);
                secondlinkoutLoopSpinBox[j]->setValue(loop);
                secondlinkoutStart[j]->setText(QString("%1").arg(znodestar));
                secondlinkoutEnd[j]->setText(QString("%1").arg(znodeend));
                secondlinkoutDeviceCombo[j]->clear();
                secondlinkoutDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(zequiptype)));
                for (int n = 0x00; n<= 0x56; n++)
                {
                    if(n != zequiptype)
                    {
                        if(Type::char_danyuan(n) != "不存在  ")
                        {
                            secondlinkoutDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(n)));
                        }
                    }
                }

            }
            else
            {
                secondlinkoutDeviceCombo[j]->addItem(tr(" "));
                for (int n = 0x00; n<= 0x56; n++)
                {
                    if(Type::char_danyuan(n) != "不存在  ")
                    {
                        secondlinkoutDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(n)));
                    }
                }
            }

        }

    }
    QSqlDatabase::database().commit();

}



void dzLinkSetWidget::readUnitSet(int)
{

    char danyuan_info[20];
    secondlinkinListWidget2->clear();

    int host = secondHostinSpinBox->value();
    int iobr = secondIobrinSpinBox->value();
    int loop = secondLoopinSpinBox->value();
    int node = 1;
    int unitid1 = (host<<24) | (iobr<<16) | (loop<<8) | node;
    int unitid2 = ((host<<24) | (iobr<<16) | (loop<<8) | node)+254;

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
                QString history_info = QString("%1%2").arg(danyuan_info).arg(Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF));
                //         printf("danyuan%d",danyuannode);
                secondlinkinListWidget2->addItem(tr("%1").arg(history_info));
            }
        }
    }
    QSqlDatabase::database().commit();
}

void dzLinkSetWidget::secondLinkinInsertClicked()
{
    if(secondlinkinListWidget2->currentRow() != (-1))
    {
        char linkininfo1[20];
        char linkininfo2[20];

        int host = secondHostinSpinBox->value();
        int iobr = secondIobrinSpinBox->value();
        int loop = secondLoopinSpinBox->value();
        int node = secondlinkinListWidget2->currentItem()->text().mid(0,3).toInt();
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
        //memset(linkininfo1, 0x0, 20);
        sprintf(linkininfo1, "%02d%02d%02d%03d",secondHostinSpinBox->value()
                ,secondIobrinSpinBox->value(),secondLoopinSpinBox->value(),node);
        //memset(linkininfo2, 0x0, 20);
        sprintf(linkininfo2, "#%01d",secondLineinSpinBox->value());
        QString dtstr = QString("%1%2 %3").arg(linkininfo1).arg(linkindeviceinfo).arg(linkininfo2);


        long long int k = ((long  long  int)dtstr.left(2).toInt()<<32) | (dtstr.mid(2,2).toInt()<<24)
                | (dtstr.mid(4,2).toInt()<<16) | dtstr.mid(6,3).toInt()<<8 | dtstr.right(1).toInt();
//        long long int k = ((long  long  int)dtstr.left(2).toInt()<<32) | (dtstr.mid(3,2).toInt()<<24)
//                | (dtstr.mid(6,2).toInt()<<16) | dtstr.mid(9,3).toInt()<<8 | dtstr.right(1).toInt();

        if (secondlinkinListWidget->count()+1 > 31)
        {
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
            if(qFind(linkin, linkin +secondlinkinListWidget->count()+1,k) == (linkin +secondlinkinListWidget->count()+1))
            {
                linkin[secondlinkinListWidget->count()] =  k;
                secondlinkinListWidget->addItem(dtstr);
            }
            else
            {
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
        //QMessageBox::warning(this,tr("警告"),tr("请在待选框中选择要添加的设备"),QMessageBox::Ok);
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

void dzLinkSetWidget::secondLinkinOutsertClicked()
{

    if(secondlinkinListWidget->currentRow() != (-1))
    {

        QString dtstr = secondlinkinListWidget->currentItem()->text();

        long long int k = ((long  long  int)dtstr.left(2).toInt()<<32) | (dtstr.mid(2,2).toInt()<<24)
                | (dtstr.mid(4,2).toInt()<<16) | dtstr.mid(6,3).toInt()<<8 | dtstr.right(1).toInt();
//        long long int k = (dtstr.left(2).toLongLong()<<32) | (dtstr.mid(3,2).toLongLong()<<24)
//                | (dtstr.mid(6,2).toInt()<<16) | dtstr.mid(9,3).toInt()<<8 |dtstr.right(1).toInt();

        long long int *j;
        for(j = qFind(linkin, linkin +secondlinkinListWidget->count(),k); j <linkin +secondlinkinListWidget->count()-1; j++)
        {
            *j =*(j+1);
        }
        *j = 0;
        secondlinkinListWidget->takeItem(secondlinkinListWidget->currentRow());
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

void dzLinkSetWidget::linknumreturn(QString test)
{
    secondnum = test.toInt();
    if(secondnum == 0)
    {
        secondnum = 1;
        secondlinkinNum->setText(QString("%1").arg(secondnum));
    }
}

void dzLinkSetWidget::shieldreturn(int shield)
{
    secondshield = shield;
    //    if(!shield)
    //    {
    //        secondshield = shield;
    //    }
    //    else
    //    {
    //        secondshield = shield-1;
    //    }
}

void dzLinkSetWidget::secondokButtonClicked()
{

    //    QMessageBox *box = new QMessageBox(this);
    //    connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));


    //    box->setWindowTitle(tr("提示"));
    //    box->setIcon(QMessageBox::Information);
    //    box->setText(tr("设置将会保存"));
    //    box->setStandardButtons(QMessageBox::No | QMessageBox::Yes);
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
   //     int host = 0, iobr = 0, loop  = 0, znodestar = 0, znodeend = 0,zequiptype = 0,zsetupnum = 0;
        int host = 0, iobr = 0, loop  = 0,zequiptype = 0,zsetupnum = 0;
        long long int znodestar = 0, znodeend = 0;
        long long int flag = 0;
        for(int j = 0; j < 31; j++)
        {
            if(secondlinkoutStart[j]->text().toInt() == 0)
            {
                flag = 0;
            }
            else if(secondlinkoutEnd[j]->text().toInt() == 0)
            {
                flag = 0;
            }
            else if(Type::int_danyuanId(secondlinkoutDeviceCombo[j]->currentText()) == 0xFF)
            {
                flag = 0;
            }
            else if(secondlinkoutStart[j]->text().toInt() > secondlinkoutEnd[j]->text().toInt())
            {
                //                    QMessageBox *box = new QMessageBox(this);
                //                    connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
                //                    box->setWindowTitle(tr("警告"));
                //                    box->setIcon(QMessageBox::Warning);
                //                    box->setText(QString(tr("联动输出设置中的组联动中\n因为第%1项起始地址大于终止地址，\n保存失败，请返回修改！")
                //                                        .arg(j+1)));
                //                    box->setStandardButtons(QMessageBox::Ok);
                //                    box->exec();
                QMessageBox *box = new QMessageBox(tr("警告"),
                                                   tr("联动输出设置中的组联动中\n"
                                                      "因为第%1项起始地址大于终止地址，\n"
                                                      "保存失败，请返回修改！!").arg(j + 1),
                                                   QMessageBox::Warning,
                                                   QMessageBox::Ok,
                                                   QMessageBox::NoButton,
                                                   QMessageBox::NoButton,
                                                   this);
                box->setAttribute(Qt::WA_DeleteOnClose);
                connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
                box->exec();
                return ;
            }
            else
            {
                host = secondlinkoutHostSpinBox[j]->value();
                iobr = secondlinkoutIobrSpinBox[j]->value();
                loop = secondlinkoutLoopSpinBox[j]->value();
//                znodestar = secondlinkoutStart[j]->text().toInt() & 0xFF;
//                znodeend = secondlinkoutEnd[j]->text().toInt() & 0xFF;
                znodestar = secondlinkoutStart[j]->text().toInt() ;
                znodeend = secondlinkoutEnd[j]->text().toInt() ;
                zequiptype = Type::int_danyuanId(secondlinkoutDeviceCombo[j]->currentText());
                flag =  ((long  long  int)host<<48) | ((long  long  int)iobr<<40) | ((long  long  int)loop<<32)
                        | (znodestar<<24) | (znodeend<<16) | (zequiptype<<8) | zsetupnum;
//                qDebug()<<" host";
//                qDebug("%16llx",(long  long  int)host<<48) ;
//                qDebug()<<" iobr";
//                qDebug("%16llx",(long  long  int)iobr<<40) ;
//                qDebug()<<" loop";
//                qDebug("%16llx",(long  long  int)loop<<32) ;
//                qDebug()<<" znodestar";
//                qDebug("%16llx",(znodestar<<24)) ;
//                qDebug()<<" znodestar";
//                qDebug("%16llx",(znodestar)) ;
//                qDebug()<<" znodeend";
//                qDebug("%16llx",(znodeend<<24)) ;
//                qDebug()<<" znodeend";
//                qDebug("%16llx",(znodeend)) ;
//                qDebug()<<"You pressed buttons ";
//                qDebug("%llx",flag);
                // printf("现在flag%llx\n",flag);
            }
            linkout[j] = flag;

        }
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

void dzLinkSetWidget::secondcancelButtonClicked()
{
    //    int r = QMessageBox::warning(this,tr("警告"),tr("设置将不会保存"),QMessageBox::No | QMessageBox::Yes);
    //    if (r == QMessageBox::Yes)
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

void dzLinkSetWidget::updateLinkset()
{
    emit  changeTypeLabel(1);
    QSqlQuery query;
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
                          "WHERE linkid =%1").arg(secondlinkid));

    //   query.prepare(QString("UPDATE linkset SET linknum=?,linksetup=?, linkshield=?, linktype=?  WHERE linkid = %1").arg(secondlinkid));
   // printf("secondnum%dsecondlinkid%d",secondnum,secondlinkid);
    //qDebug()<<"shashiqing"<<secondtype;

    query.bindValue(0,secondshield);
    query.bindValue(1,secondsetup);
    query.bindValue(2,secondtype);
    query.bindValue(3,secondnum);

    for (int j=0; j<31; j++)
    {
        query.bindValue(j+4,linkin[j]);
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
    emit transferdzLinknum(secondlinkid,secondnum);
    emit databaseChange();
}

void dzLinkSetWidget::clearsecondWidget()
{
    secondlinkinListWidget->clear();
    for(int j = 0; j<31;j++)
    {
        secondlinkoutHostSpinBox[j]->clear();
        secondlinkoutIobrSpinBox[j]->clear();
        secondlinkoutLoopSpinBox[j]->clear();
        secondlinkoutStart[j]->clear();
        secondlinkoutEnd[j]->clear();
        secondlinkoutDeviceCombo[j]->clear();
    }

    for(int i = 0;i<31;i++)
    {
        secondlinkoutDeviceCombo[i]->addItem(tr(" "));
        for (int n = 0x00; n<= 0x56; n++)
        {
            if(Type::char_danyuan(n) != "不存在  ")
            {
                secondlinkoutDeviceCombo[i]->addItem(tr("%1").arg(Type::char_danyuan(n)));
            }
        }
    }
    secondhideComboBox->setCurrentIndex(0);
    // secondhideCheckBox->setCheckState(Qt::Checked);
    secondnum = 1 ;
    secondlinkinNum->setText(QString("%1").arg(secondnum));
}
