#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QFrame>
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

#include "info.h"
#include "type.h"
#include "zdlinksetwidget.h"

zdLinkSetWidget::zdLinkSetWidget(QWidget *parent) :
    QWidget(parent)
{

}

zdLinkSetWidget::zdLinkSetWidget(bool editFlag, QWidget *parent) :
    QWidget(parent)
{
    qFill(linkin, linkin + 31, 0);
    qFill(linkout, linkout + 31, 0);
    editEnableFlag = editFlag;
    thirdlinkid = 1;
    thirdtype = 2;
    thirdsetup = 0;

    thirdlinkinFrame = new QFrame(this);
    thirdlinkinFrame->setGeometry(QRect(0, 0 ,390, 450));
    thirdlinkinFrame->setFrameStyle(QFrame::Box);


    thirdlinkInTipLabel = new QLabel(tr("联动输入"), this);
    thirdlinkInTipLabel->setGeometry(QRect(40,5,80,20));
    thirdlinkInTipLabel->setAlignment(Qt::AlignCenter);
    thirdlinkInTipLabel->setFrameShape(QFrame::Box);

    thirdlinkinTipLabel2 = new QLabel(tr("条件数"), this);
    thirdlinkinTipLabel2 ->setGeometry(QRect(40,35 ,60,20));
    thirdlinkinTipLabel2->setAlignment(Qt::AlignCenter);
    thirdlinkinTipLabel2->setFrameShape(QFrame::Box);
    thirdlinkinNum = new QLineEdit(this);
    thirdlinkinNum->setGeometry(QRect(105,35 ,35,20));
    thirdlinkinNum->setValidator(new QIntValidator(1,31,this));
    thirdnum = 1 ;
    thirdlinkinNum->setText(QString("%1").arg(thirdnum));
    QObject::connect(thirdlinkinNum,SIGNAL(textChanged(QString)),this,SLOT(linknumreturn(QString)));


    thirdhideComboBox = new QComboBox(this);
    thirdhideComboBox->setGeometry(QRect(175,35 ,80,20));
    thirdhideComboBox->addItem(tr("屏蔽"));
    thirdhideComboBox->addItem(tr("不屏蔽"));
    QObject::connect(thirdhideComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(shieldreturn(int)));


    //    thirdhideCheckBox = new QCheckBox(tr("屏蔽"),ZthirdlinkInGroup);
    //    thirdhideCheckBox->setGeometry(QRect(175,35 ,80,20));
    //    thirdhideCheckBox->setCheckState(Qt::Checked);
    //    QObject::connect(thirdhideCheckBox,SIGNAL(stateChanged(int)),this,SLOT(shieldreturn(int)));

    linkTabWidgetThird = new QTabWidget(this);
    linkTabWidgetThird->setGeometry(QRect(5,60,380,385));

    for(int j = 0; j < 3; j++)
    {
        int k = 0;
        thirdlinkInGroup[j] = new QGroupBox(this);
        thirdlinkInGroup[j]->setGeometry(QRect(0, 5 ,365, 470));
        //   thirdlinkInGroup1->setStyleSheet( "border:1px outset;" );
        thirdlinkinNumLabel = new QLabel(tr("NO"), thirdlinkInGroup[j]);
        thirdlinkinNumLabel->setGeometry(QRect(0,5,29,20));
        thirdlinkinNumLabel->setFrameShape(QFrame::Box);
        thirdlinkinHostLabel = new QLabel(tr("主机"), thirdlinkInGroup[j]);
        thirdlinkinHostLabel->setGeometry(QRect(34,5,44,20));
        thirdlinkinHostLabel->setFrameShape(QFrame::Box);
        thirdlinkinIobrLabel = new QLabel(tr("接口板"),thirdlinkInGroup[j]);
        thirdlinkinIobrLabel->setGeometry(QRect(83,5,59,20));
        thirdlinkinIobrLabel->setFrameShape(QFrame::Box);
        thirdlinkinLoopLabel = new QLabel(tr("回路"), thirdlinkInGroup[j]);
        thirdlinkinLoopLabel->setGeometry(QRect(146,5,44,20));
        thirdlinkinLoopLabel->setFrameShape(QFrame::Box);
        thirdlinkinStartLabel = new QLabel(tr("起始"), thirdlinkInGroup[j]);
        thirdlinkinStartLabel->setGeometry(QRect(195,5,39,20));
        thirdlinkinStartLabel->setFrameShape(QFrame::Box);
        thirdlinkinEndLabel = new QLabel(tr("终止"),thirdlinkInGroup[j]);
        thirdlinkinEndLabel->setGeometry(QRect(239,5,39,20));
        thirdlinkinEndLabel->setFrameShape(QFrame::Box);
        thirdlinkinDeviceLabel = new QLabel(tr("设备类型"), thirdlinkInGroup[j]);
        thirdlinkinDeviceLabel->setAlignment(Qt::AlignCenter);
        thirdlinkinDeviceLabel->setGeometry(QRect(283,5,92,20));
        thirdlinkinDeviceLabel->setFrameShape(QFrame::Box);

        for(int i = 0;(i < 11 && (i+j*11) < 31); i++)
        {
            thirdlinkinLabel[i+j*11] = new  QLabel(tr("%1").arg(i+j*11+1), thirdlinkInGroup[j]);
            thirdlinkinLabel[i+j*11]->setGeometry(QRect(0,30+i*30,29,20));
            thirdlinkinLabel[i+j*11]->setFrameShape(QFrame::Box);
            thirdlinkinHostSpinBox[i+j*11] = new QSpinBox(thirdlinkInGroup[j]);
            thirdlinkinHostSpinBox[i+j*11]->setGeometry(QRect(34,30+i*30,44,20));
            thirdlinkinHostSpinBox[i+j*11]->setRange(1,64);
            thirdlinkinIobrSpinBox[i+j*11] = new QSpinBox(thirdlinkInGroup[j]);
            thirdlinkinIobrSpinBox[i+j*11]->setGeometry(QRect(83,30+i*30,59,20));
            thirdlinkinIobrSpinBox[i+j*11]->setRange(1,35);
            thirdlinkinLoopSpinBox[i+j*11] = new QSpinBox(thirdlinkInGroup[j]);
            thirdlinkinLoopSpinBox[i+j*11]->setGeometry(QRect(146,30+i*30,44,20));
            thirdlinkinLoopSpinBox[i+j*11]->setRange(1,20);
            thirdlinkinStart[i+j*11] = new QLineEdit(thirdlinkInGroup[j]);
            thirdlinkinStart[i+j*11]->setGeometry(QRect(195,30+i*30,39,20));
            thirdlinkinStart[i+j*11]->setValidator(new QIntValidator(1,255,thirdlinkInGroup[j]));

            thirdlinkinEnd[i+j*11] = new QLineEdit(thirdlinkInGroup[j]);
            thirdlinkinEnd[i+j*11]->setGeometry(QRect(239,30+i*30,39,20));
            thirdlinkinEnd[i+j*11]->setValidator(new QIntValidator(1,255,thirdlinkInGroup[j]));

            thirdlinkinDeviceCombo[i+j*11] = new QComboBox(thirdlinkInGroup[j]);
            thirdlinkinDeviceCombo[i+j*11]->setGeometry(QRect(283,30+i*30,92,20));
            k = i+j*11+1;
        }
        linkTabWidgetThird->addTab(thirdlinkInGroup[j],tr("组联动%1--%2").arg(j*11+1).arg(k));
    }

    thirdlinkoutFrame = new QFrame(this);
    thirdlinkoutFrame->setGeometry(QRect(400, 0 ,390, 450));
    thirdlinkoutFrame->setFrameStyle(QFrame::Box);

    thirdlinkOutTipLabel = new QLabel(tr("联动输出"), this);
    thirdlinkOutTipLabel ->setGeometry(QRect(440,5,80,20));
    thirdlinkOutTipLabel->setAlignment(Qt::AlignCenter);
    thirdlinkOutTipLabel->setFrameShape(QFrame::Box);
    //thirdlinkoutTipLabel->setGeometry(QRect(QPoint (120, 10), linkinTipLabel->sizeHint()));
    // thirdlinkoutTipLabel->setFrameShape(QFrame::NoFrame);

    thirdlinkoutTipLabel3 = new QLabel(tr("已选部位列表"), this);
    thirdlinkoutTipLabel3->setAlignment(Qt::AlignCenter);
    thirdlinkoutTipLabel3->setGeometry(QRect(405,112 ,210,20));
    thirdlinkoutTipLabel3->setFrameShape(QFrame::Box);

    thirdlinkoutTipLabel8 = new QLabel(tr("主机"), this);
    thirdlinkoutTipLabel8->setGeometry(QRect(660,2,60,20));
    thirdlinkoutTipLabel8->setAlignment(Qt::AlignCenter);
    thirdlinkoutTipLabel8->setFrameShape(QFrame::Box);
    thirdHostoutSpinBox = new QSpinBox(this);
    thirdHostoutSpinBox->setGeometry(QRect(725,2,60,20));
    thirdHostoutSpinBox->setRange(1,64);
    thirdHostoutSpinBox->setValue(1);
    thirdHostoutSpinBox->setAlignment(Qt::AlignRight);
    QObject::connect(thirdHostoutSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet(int)));

    thirdlinkoutTipLabel4 = new QLabel(tr("接口板"), this);
    thirdlinkoutTipLabel4->setGeometry(QRect(660,25 ,60,20));
    thirdlinkoutTipLabel4->setAlignment(Qt::AlignCenter);
    thirdIobroutSpinBox = new QSpinBox(this);
    thirdIobroutSpinBox->setGeometry(QRect(725,25,60,20));
    thirdIobroutSpinBox->setRange(1,35);
    thirdIobroutSpinBox->setValue(1);
    thirdIobroutSpinBox->setAlignment(Qt::AlignRight);
    QObject::connect(thirdIobroutSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet(int)));

    thirdlinkoutTipLabel5 = new QLabel(tr("回  路"), this);
    thirdlinkoutTipLabel5->setGeometry(QRect(660,47,60,20));
    thirdlinkoutTipLabel5->setAlignment(Qt::AlignCenter);
    thirdlinkoutTipLabel5->setFrameShape(QFrame::Box);
    thirdLoopoutSpinBox = new QSpinBox(this);
    thirdLoopoutSpinBox->setGeometry(QRect(725,47,60,20));

    thirdLoopoutSpinBox->setRange(1,20);
    thirdLoopoutSpinBox->setValue(1);
    thirdLoopoutSpinBox->setAlignment(Qt::AlignRight);
    QObject::connect(thirdLoopoutSpinBox,SIGNAL(valueChanged(int)),this,SLOT(readUnitSet(int)));

    thirdlinkoutTipLabel7 = new QLabel(tr("线  路"), this);
    thirdlinkoutTipLabel7->setGeometry(QRect(660,69,60,20));
    thirdlinkoutTipLabel7->setAlignment(Qt::AlignCenter);
    thirdlinkoutTipLabel7->setFrameShape(QFrame::Box);
    thirdLineoutSpinBox = new QSpinBox(this);
    thirdLineoutSpinBox->setGeometry(QRect(725,69,60,20));

    thirdLineoutSpinBox->setRange(1,2);
    thirdLineoutSpinBox->setValue(1);
    thirdLineoutSpinBox->setAlignment(Qt::AlignRight);

    thirdlinkoutTipLabel6 = new QLabel(tr("待选部位列表"), this);
    thirdlinkoutTipLabel6->setAlignment(Qt::AlignCenter);
    thirdlinkoutTipLabel6->setGeometry(QRect(660,112 ,120,20));
    thirdlinkoutTipLabel6->setFrameShape(QFrame::Box);

    thirdlinkoutListWidget2 = new QListWidget(this);
    thirdlinkoutListWidget2->setGeometry(QRect(660,135 ,120,310));
    this->readUnitSet(1);//初始化主机号接口板回路都为1的单元设备

    thirdLinkoutInsertButton = new QPushButton(tr("添加"),this);
    thirdLinkoutInsertButton->setGeometry(QRect(620,190 ,35,20));
    QObject::connect(thirdLinkoutInsertButton,SIGNAL(clicked()),this,SLOT(thirdLinkoutInsertClicked()));

    thirdlinkoutListWidget = new QListWidget(this);
    thirdlinkoutListWidget->setGeometry(QRect(405,135 ,210,310));

    thirdLinkoutOutsertButton = new QPushButton(tr("删除"),this);
    thirdLinkoutOutsertButton->setGeometry(QRect(620,230 ,35,20));
    QObject::connect(thirdLinkoutOutsertButton,SIGNAL(clicked()),this,SLOT(thirdLinkoutOutsertClicked()));

    thirdokButton = new QPushButton(tr("确定"),this);
    //     thirdokButton->setMaximumSize(40,24);
    thirdokButton->setGeometry(QRect(325,455,40,24));
    connect(thirdokButton, SIGNAL(clicked()),this, SLOT(thirdokButtonClicked()));

    thirdcancelButton = new QPushButton(tr("取消"),this);
    //     thirdcancelButton->setMaximumSize(40,24);
    thirdcancelButton->setGeometry(QRect(425,455,40,24));
    connect(thirdcancelButton, SIGNAL(clicked()),this, SLOT(thirdcancelButtonClicked()));

    if(!editEnableFlag)
    {
        thirdlinkinNum->setReadOnly(true);
        thirdhideComboBox->setEnabled(false);
        thirdLinkoutInsertButton->setVisible(false);
        thirdLinkoutOutsertButton->setVisible(false);
        thirdokButton->setVisible(false);
        thirdcancelButton->setVisible(false);
        QPushButton *okButton = new QPushButton(tr("OK"),this);
        okButton->setGeometry(QRect(375,455,40,24));
        connect(okButton, SIGNAL(clicked()),this->parentWidget(), SLOT(close()));

        for(int j = 0; j<31;j++)
        {
            thirdlinkinHostSpinBox[j]->setReadOnly(true);
            thirdlinkinIobrSpinBox[j]->setReadOnly(true);
            thirdlinkinLoopSpinBox[j]->setReadOnly(true);
            thirdlinkinStart[j]->setReadOnly(true);
            thirdlinkinEnd[j]->setReadOnly(true);
            thirdlinkinDeviceCombo[j]->setEnabled(false);
        }
    }
}

zdLinkSetWidget::~zdLinkSetWidget()
{

}

void zdLinkSetWidget::linkDisplay(int id)
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    thirdlinkid = id;

    char dlinkininfo1 [20];
    char dlinkininfo2 [20];
    query.exec(QString("select * from linkset WHERE linkid = %1").arg(id));

    while (query.next())
    {
        thirdshield = query.value(1).toInt();
//        qDebug()<<"query.value(1).toInt()"<<query.value(1).toInt();
//        qDebug()<<"thirdshield"<<thirdshield;
        thirdhideComboBox->setCurrentIndex(thirdshield);

        thirdlinkinNum->setText(QString("%1").arg(query.value(4).toString()));
        thirdnum = query.value(4).toInt();

        for(int i = 0;i < 31; i++)
        {
            long long int k = query.value(i+36).toLongLong();
            linkout[i] = k ;

            if(k)
            {
                int host = (k >> 32) & 0xFF;
                int iobr = (k >> 24) & 0xFF;
                int loop = (k>>16) & 0xFF;
                int node = (k>>8) & 0xFF;
                int unitid = (host<<24) | (iobr<<16) | (loop<<8) | node;
                QString linkindeviceinfo;
                QSqlQuery query3;
                query3.setForwardOnly(true);
                query3.exec(QString("select * from unitset WHERE unitid = %1").arg(unitid));
                while (query3.next())
                {
                    linkindeviceinfo = Type::char_danyuan((query3.value(7).toInt() >> 8) & 0xFF);
                }
                //memset(dlinkininfo1, 0x0, 20);
                sprintf(dlinkininfo1, "%02lld%02lld%02lld%03lld",(k >> 32) & 0xFF,(k >> 24) & 0xFF
                        , (k>>16) & 0xFF, (k>>8) & 0xFF);
                //memset(dlinkininfo2, 0x0, 20);
                sprintf(dlinkininfo2, "#%01lld", k &0xFF);
                QString dlinkininfo = QString("%1%2 %3").arg(dlinkininfo1).arg(linkindeviceinfo).arg(dlinkininfo2);
                thirdlinkoutListWidget->addItem(dlinkininfo);
            }
            else
            {
                break;
            }
        }
        for(int j = 0;j< 31;j++)
        {
            long long int m = query.value(j+5).toLongLong();
            linkin[j] = m;
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
                thirdlinkinHostSpinBox[j]->setValue(host);
                thirdlinkinIobrSpinBox[j]->setValue(iobr);
                thirdlinkinLoopSpinBox[j]->setValue(loop);
                thirdlinkinStart[j]->setText(QString("%1").arg(znodestar));
                thirdlinkinEnd[j]->setText(QString("%1").arg(znodeend));
                thirdlinkinDeviceCombo[j]->clear();
                thirdlinkinDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(zequiptype)));
                for (int n = 0x00; n<= 0x56; n++)
                {
                    if(n != zequiptype)
                    {
                        if(Type::char_danyuan(n) != "不存在  ")
                        {
                            thirdlinkinDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(n)));
                        }
                    }
                }

            }
            else
            {
                thirdlinkinDeviceCombo[j]->addItem(tr(" "));
                for (int n = 0x00; n<= 0x56; n++)
                {

                    if(Type::char_danyuan(n) != "不存在  ")
                    {
                        thirdlinkinDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(n)));
                    }

                }
            }

        }

    }
    QSqlDatabase::database().commit();

}


void zdLinkSetWidget::readUnitSet(int)
{
    char danyuan_info[20];
    thirdlinkoutListWidget2->clear();

    int host = thirdHostoutSpinBox->value();
    int iobr = thirdIobroutSpinBox->value();
    int loop = thirdLoopoutSpinBox->value();
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
                thirdlinkoutListWidget2->addItem(tr("%1").arg(history_info));
            }
        }
    }
    //        }
    //    }
    QSqlDatabase::database().commit();
}

void zdLinkSetWidget::thirdLinkoutInsertClicked()
{
    if(thirdlinkoutListWidget2->currentRow() != (-1))
    {
        char linkoutinfo1[20];
        char linkoutinfo2[20];

        int host = thirdHostoutSpinBox->value();
        int iobr = thirdIobroutSpinBox->value();
        int loop = thirdLoopoutSpinBox->value();
        int node = thirdlinkoutListWidget2->currentItem()->text().mid(0,3).toInt();
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
        sprintf(linkoutinfo1, "%02d%02d%02d%03d",thirdHostoutSpinBox->value()
                ,thirdIobroutSpinBox->value(),thirdLoopoutSpinBox->value(),node);
        //memset(linkoutinfo2, 0x0, 20);
        sprintf(linkoutinfo2, "#%01d",thirdLineoutSpinBox->value());
        QString dtstr = QString("%1%2 %3").arg(linkoutinfo1).arg(linkoutdeviceinfo).arg(linkoutinfo2);
        long long int k = ((long  long  int)dtstr.left(2).toInt()<<32) | (dtstr.mid(2,2).toInt()<<24)
                | (dtstr.mid(4,2).toInt()<<16) | dtstr.mid(6,3).toInt()<<8 | dtstr.right(1).toInt();

        if (thirdlinkoutListWidget->count()+1 > 31)
        {
            //QMessageBox::warning(this,tr("警告"),tr("点联动条数已达31条的上限，不能再添加"),QMessageBox::Ok);
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
            if(qFind(linkout, linkout +thirdlinkoutListWidget->count()+1,k) == (linkout +thirdlinkoutListWidget->count()+1))
            {
                linkout[thirdlinkoutListWidget->count()] =  k;
                thirdlinkoutListWidget->addItem(dtstr);
            }
            else
            {
                //QMessageBox::warning(this,tr("警告"),tr("该点联动已添加，不能重复添加"),QMessageBox::Ok);
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

void zdLinkSetWidget::thirdLinkoutOutsertClicked()
{

    if(thirdlinkoutListWidget->currentRow() != (-1))
    {

        QString dtstr = thirdlinkoutListWidget->currentItem()->text();
        long long int k = (dtstr.left(2).toLongLong()<<32) | (dtstr.mid(2,2).toLongLong()<<24)
                | (dtstr.mid(4,2).toInt()<<16) | dtstr.mid(6,3).toInt()<<8 |dtstr.right(1).toInt();

        long long int *j;
        for(j = qFind(linkout, linkout +thirdlinkoutListWidget->count(),k); j <linkout +thirdlinkoutListWidget->count()-1; j++)
        {
            *j =*(j+1);
        }
        *j = 0;
        thirdlinkoutListWidget->takeItem(thirdlinkoutListWidget->currentRow());

    }
    else
    {
        //QMessageBox::warning(this,tr("警告"),tr("请在已选框中选择要删除的设备"),QMessageBox::Ok);
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

void zdLinkSetWidget::linknumreturn(QString test)
{
    thirdnum = test.toInt();
    if(thirdnum == 0)
    {
        thirdnum = 1;
        thirdlinkinNum->setText(QString("%1").arg(thirdnum));
    }
}

void zdLinkSetWidget::shieldreturn(int shield)
{
    thirdshield = shield;
    //    if(!shield)
    //    {
    //        thirdshield = shield;
    //    }
    //    else
    //    {
    //        thirdshield = shield-1;
    //    }
}

void zdLinkSetWidget::thirdokButtonClicked()
{

    //    int r = QMessageBox::information(this,tr("提示"),tr("设置将会保存"),QMessageBox::No | QMessageBox::Yes);
    QMessageBox *box = new QMessageBox(tr("提示"),
                                       tr("设置将会保存!"),
                                       QMessageBox::Information,
                                       QMessageBox::Yes | QMessageBox::Default,
                                       QMessageBox::NoButton,
                                       QMessageBox::No | QMessageBox::Escape,
                                       this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    connect(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
    int r = box->exec();
    if (r == QMessageBox::Yes)
    {
//        int host = 0, iobr = 0, loop  = 0, znodestar = 0, znodeend = 0,zequiptype = 0,zsetupnum = 0;
        int host = 0, iobr = 0, loop  = 0,zequiptype = 0,zsetupnum = 0;
        long long int  znodestar = 0, znodeend = 0;

        long long int flag = 0;
        for(int j = 0; j < 31; j++)
        {
            if(thirdlinkinStart[j]->text().toInt() == 0)
            {
                flag = 0;
            }
            else if(thirdlinkinEnd[j]->text().toInt() == 0)
            {
                flag = 0;
            }
            else if(Type::int_danyuanId(thirdlinkinDeviceCombo[j]->currentText()) == 0xFF)
            {
                flag = 0;
            }
            else if(thirdlinkinStart[j]->text().toInt() > thirdlinkinEnd[j]->text().toInt())
            {
                //                QMessageBox::warning(this,tr("警告"),
                //                                     QString(tr("联动输入设置中的组联动中\n因为第%1项起始地址大于终止地址，\n保存失败，请返回修改！")
                //                                             .arg(j+1)),QMessageBox::Ok);
                QMessageBox *box = new QMessageBox(tr("警告"),
                                                   tr("联动输入设置中的组联动中\n"
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
                host = thirdlinkinHostSpinBox[j]->value();
                iobr = thirdlinkinIobrSpinBox[j]->value();
                loop = thirdlinkinLoopSpinBox[j]->value();
                znodestar = thirdlinkinStart[j]->text().toInt() & 0xFF;
                znodeend = thirdlinkinEnd[j]->text().toInt() & 0xFF;
                zequiptype = Type::int_danyuanId(thirdlinkinDeviceCombo[j]->currentText());
                flag =  ((long  long  int)host<<48) | ((long  long  int)iobr<<40)
                        | ((long  long  int)loop<<32) | (znodestar<<24) | (znodeend<<16) | (zequiptype<<8) | zsetupnum;
                //printf("flag%llx\n",flag);
            }
            linkin[j] = flag;
        }
        this->updateLinkset();
    }
}




void zdLinkSetWidget::thirdcancelButtonClicked()
{
    //    int r = QMessageBox::warning(this,tr("警告"),tr("设置将不会保存"),QMessageBox::No | QMessageBox::Yes);
    //    if (r == QMessageBox::Yes)
    //    {
    //        this->parentWidget()->parentWidget()->parentWidget()->close();
    //    }
    QMessageBox *box = new QMessageBox(tr("警告"),
                                       tr("设置将不会保存!"),
                                       QMessageBox::Warning,
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


void zdLinkSetWidget::updateLinkset()
{
    emit  changeTypeLabel(2);
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
                          "WHERE linkid =%1").arg(thirdlinkid));

//    printf("thirdnum%dthirdlinkid%d",thirdnum,thirdlinkid);

    query.bindValue(0,thirdshield);
    query.bindValue(1,thirdsetup);
    query.bindValue(2,thirdtype);
    query.bindValue(3,thirdnum);

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
    emit transferzdLinknum(thirdlinkid,thirdnum);
    emit databaseChange();
}

void zdLinkSetWidget::clearthirdWidget()
{
    thirdlinkoutListWidget->clear();
    for(int j = 0; j<31;j++)
    {
        thirdlinkinHostSpinBox[j]->clear();
        thirdlinkinIobrSpinBox[j]->clear();
        thirdlinkinLoopSpinBox[j]->clear();
        thirdlinkinStart[j]->clear();
        thirdlinkinEnd[j]->clear();
        thirdlinkinDeviceCombo[j]->clear();
    }

    for(int i = 0;i<31;i++)
    {
        thirdlinkinDeviceCombo[i]->addItem(tr(" "));
        for (int n = 0x00; n<= 0x56; n++)
        {
            if(Type::char_danyuan(n) != "不存在  ")
            {
                thirdlinkinDeviceCombo[i]->addItem(tr("%1").arg(Type::char_danyuan(n)));
            }
        }
    }
    thirdhideComboBox->setCurrentIndex(0);
    //    thirdhideCheckBox->setCheckState(Qt::Checked);
    thirdnum = 1 ;
    thirdlinkinNum->setText(QString("%1").arg(thirdnum));
}
