#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QFrame>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>
#include <QtGui/QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include<QtCore/QVariant>
#include <QtSql/QSqlError>
#include <QtDebug>

#include "info.h"
#include "type.h"
#include "zzlinksetwidget.h"

zzLinkSetWidget::zzLinkSetWidget(QWidget *parent) :
    QWidget(parent)
{

}

zzLinkSetWidget::zzLinkSetWidget(bool editFlag, QWidget *parent) :
    QWidget(parent)
{
    qFill(linkin, linkin + 31, 0);
    qFill(linkout, linkout + 31, 0);
    editEnableFlag = editFlag;
    forthlinkid = 1;
    forthtype = 3;
    forthsetup = 0;
    forthlinkinFrame = new QFrame(this);
    forthlinkinFrame->setGeometry(QRect(0, 0 ,390, 450));
    forthlinkinFrame->setFrameStyle(QFrame::Box);

    forthlinkInTipLabel = new QLabel(tr("联动输入"), this);
    forthlinkInTipLabel->setGeometry(QRect(40,5,80,20));
    forthlinkInTipLabel->setAlignment(Qt::AlignCenter);
    forthlinkInTipLabel->setFrameShape(QFrame::Box);

    forthlinkinTipLabel2 = new QLabel(tr("条件数"), this);
    forthlinkinTipLabel2 ->setGeometry(QRect(40,35 ,60,20));
    forthlinkinTipLabel2->setAlignment(Qt::AlignCenter);
    forthlinkinTipLabel2->setFrameShape(QFrame::Box);
    forthlinkinNum = new QLineEdit(this);
    forthlinkinNum->setGeometry(QRect(105,35 ,35,20));
    forthlinkinNum->setValidator(new QIntValidator(1,31,this));
    forthnum = 1 ;
    forthlinkinNum->setText(QString("%1").arg(forthnum));
    QObject::connect(forthlinkinNum,SIGNAL(textChanged(QString)),this,SLOT(linknumreturn(QString)));


    forthhideComboBox = new QComboBox(this);
    forthhideComboBox->setGeometry(QRect(175,35 ,80,20));
    forthhideComboBox->addItem(tr("屏蔽"));
    forthhideComboBox->addItem(tr("不屏蔽"));
    QObject::connect(forthhideComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(shieldreturn(int)));

    //    forthhideCheckBox = new QCheckBox(tr("屏蔽"),ZforthlinkInGroup);
    //    forthhideCheckBox->setGeometry(QRect(175,35 ,80,20));
    //    forthhideCheckBox->setCheckState(Qt::Checked);
    //    QObject::connect(forthhideCheckBox,SIGNAL(stateChanged(int)),this,SLOT(shieldreturn(int)));

    linkTabWidgetforth1 = new QTabWidget(this);
    linkTabWidgetforth1->setGeometry(QRect(5,60,380,385));

    for(int j = 0; j < 3; j++)
    {
        int k = 0;
        forthlinkInGroup[j] = new QGroupBox(this);
        forthlinkInGroup[j]->setGeometry(QRect(0, 5 ,365, 470));
        //   forthlinkInGroup1->setStyleSheet( "border:1px outset;" );
        forthlinkinNumLabel = new QLabel(tr("NO"), forthlinkInGroup[j]);
        forthlinkinNumLabel->setGeometry(QRect(0,5,29,20));
        forthlinkinNumLabel->setFrameShape(QFrame::Box);
        forthlinkinHostLabel = new QLabel(tr("主机"), forthlinkInGroup[j]);
        forthlinkinHostLabel->setGeometry(QRect(34,5,44,20));
        forthlinkinHostLabel->setFrameShape(QFrame::Box);
        forthlinkinIobrLabel = new QLabel(tr("接口板"),forthlinkInGroup[j]);
        forthlinkinIobrLabel->setGeometry(QRect(83,5,59,20));
        forthlinkinIobrLabel->setFrameShape(QFrame::Box);
        forthlinkinLoopLabel = new QLabel(tr("回路"), forthlinkInGroup[j]);
        forthlinkinLoopLabel->setGeometry(QRect(146,5,44,20));
        forthlinkinLoopLabel->setFrameShape(QFrame::Box);
        forthlinkinStartLabel = new QLabel(tr("起始"), forthlinkInGroup[j]);
        forthlinkinStartLabel->setGeometry(QRect(195,5,39,20));
        forthlinkinStartLabel->setFrameShape(QFrame::Box);
        forthlinkinEndLabel = new QLabel(tr("终止"),forthlinkInGroup[j]);
        forthlinkinEndLabel->setGeometry(QRect(239,5,39,20));
        forthlinkinEndLabel->setFrameShape(QFrame::Box);
        forthlinkinDeviceLabel = new QLabel(tr("设备类型"), forthlinkInGroup[j]);
        forthlinkinDeviceLabel->setAlignment(Qt::AlignCenter);
        forthlinkinDeviceLabel->setGeometry(QRect(283,5,92,20));
        forthlinkinDeviceLabel->setFrameShape(QFrame::Box);

        for(int i = 0; (i < 11 && (i+j*11) < 31); i++)
        {
            forthlinkinLabel[i+j*11] = new  QLabel(tr("%1").arg(i+j*11+1), forthlinkInGroup[j]);
            forthlinkinLabel[i+j*11]->setGeometry(QRect(0,30+i*30,29,20));
            forthlinkinLabel[i+j*11]->setFrameShape(QFrame::Box);
            forthlinkinHostSpinBox[i+j*11] = new QSpinBox(forthlinkInGroup[j]);
            forthlinkinHostSpinBox[i+j*11]->setGeometry(QRect(34,30+i*30,44,20));
            forthlinkinHostSpinBox[i+j*11]->setRange(1,64);
            forthlinkinIobrSpinBox[i+j*11] = new QSpinBox(forthlinkInGroup[j]);
            forthlinkinIobrSpinBox[i+j*11]->setGeometry(QRect(83,30+i*30,59,20));
            forthlinkinIobrSpinBox[i+j*11]->setRange(1,35);
            forthlinkinLoopSpinBox[i+j*11] = new QSpinBox(forthlinkInGroup[j]);
            forthlinkinLoopSpinBox[i+j*11]->setGeometry(QRect(146,30+i*30,44,20));
            forthlinkinLoopSpinBox[i+j*11]->setRange(1,20);
            forthlinkinStart[i+j*11] = new QLineEdit(forthlinkInGroup[j]);
            forthlinkinStart[i+j*11]->setGeometry(QRect(195,30+i*30,39,20));
            forthlinkinStart[i+j*11]->setValidator(new QIntValidator(1,255,forthlinkInGroup[j]));

            forthlinkinEnd[i+j*11] = new QLineEdit(forthlinkInGroup[j]);
            forthlinkinEnd[i+j*11]->setGeometry(QRect(239,30+i*30,39,20));
            forthlinkinEnd[i+j*11]->setValidator(new QIntValidator(1,255,forthlinkInGroup[j]));

            forthlinkinDeviceCombo[i+j*11] = new QComboBox(forthlinkInGroup[j]);
            forthlinkinDeviceCombo[i+j*11]->setGeometry(QRect(283,30+i*30,92,20));
            k = i+j*11+1;
        }
        linkTabWidgetforth1->addTab(forthlinkInGroup[j],tr("组联动%1--%2").arg(j*11+1).arg(k));
    }

    forthlinkoutFrame = new QFrame(this);
    forthlinkoutFrame->setGeometry(QRect(400, 0 ,390, 450));
    forthlinkoutFrame->setFrameStyle(QFrame::Box);

    forthlinkOutTipLabel = new QLabel(tr("联动输出"), this);
    forthlinkOutTipLabel->setGeometry(QRect(440,5,80,20));
    //  forthlinkOutTipLabel->setGeometry(QRect(QPoint (120, 10), linkOutTipLabel->sizeHint()));
    forthlinkOutTipLabel->setFrameShape(QFrame::Box);

    linkTabWidgetforth2 = new QTabWidget(this);
    //   linkTabWidgetforth->setMaximumSize(300,300);
    linkTabWidgetforth2->setGeometry(QRect(405,60,380,385));

    for(int j=0;j<3;j++)
    {
        int k=0;
        forthlinkOutGroup[j] = new QGroupBox(this);
        forthlinkOutGroup[j]->setGeometry(QRect(400, 5 ,365, 470));
        //   forthlinkOutGroup1->setStyleSheet( "border:1px outset;" );
        forthlinkoutNumLabel = new QLabel(tr("NO"), forthlinkOutGroup[j]);
        forthlinkoutNumLabel->setGeometry(QRect(0,5,29,20));
        forthlinkoutNumLabel->setFrameShape(QFrame::Box);
        forthlinkoutHostLabel = new QLabel(tr("主机"), forthlinkOutGroup[j]);
        forthlinkoutHostLabel->setGeometry(QRect(34,5,44,20));
        forthlinkoutHostLabel->setFrameShape(QFrame::Box);
        forthlinkoutIobrLabel = new QLabel(tr("接口板"),forthlinkOutGroup[j]);
        forthlinkoutIobrLabel->setGeometry(QRect(83,5,59,20));
        forthlinkoutIobrLabel->setFrameShape(QFrame::Box);
        forthlinkoutLoopLabel = new QLabel(tr("回路"), forthlinkOutGroup[j]);
        forthlinkoutLoopLabel->setGeometry(QRect(146,5,44,20));
        forthlinkoutLoopLabel->setFrameShape(QFrame::Box);
        forthlinkoutStartLabel = new QLabel(tr("起始"), forthlinkOutGroup[j]);
        forthlinkoutStartLabel->setGeometry(QRect(195,5,39,20));
        forthlinkoutStartLabel->setFrameShape(QFrame::Box);
        forthlinkoutEndLabel = new QLabel(tr("终止"),forthlinkOutGroup[j]);
        forthlinkoutEndLabel->setGeometry(QRect(239,5,39,20));
        forthlinkoutEndLabel->setFrameShape(QFrame::Box);
        forthlinkoutDeviceLabel = new QLabel(tr("设备类型"), forthlinkOutGroup[j]);
        forthlinkoutDeviceLabel->setAlignment(Qt::AlignCenter);
        forthlinkoutDeviceLabel->setGeometry(QRect(283,5,92,20));
        forthlinkoutDeviceLabel->setFrameShape(QFrame::Box);

        for(int i=0;(i<11&&(i+j*11)<31);i++)
        {
            forthlinkoutLabel[i+j*11] = new  QLabel(tr("%1").arg(i+j*11+1), forthlinkOutGroup[j]);
            forthlinkoutLabel[i+j*11]->setGeometry(QRect(0,30+i*30,29,20));
            forthlinkoutLabel[i+j*11]->setFrameShape(QFrame::Box);
            forthlinkoutHostSpinBox[i+j*11] = new QSpinBox(forthlinkOutGroup[j]);
            forthlinkoutHostSpinBox[i+j*11]->setGeometry(QRect(34,30+i*30,44,20));
            forthlinkoutHostSpinBox[i+j*11]->setRange(1,64);
            forthlinkoutIobrSpinBox[i+j*11] = new QSpinBox(forthlinkOutGroup[j]);
            forthlinkoutIobrSpinBox[i+j*11]->setGeometry(QRect(83,30+i*30,59,20));
            forthlinkoutIobrSpinBox[i+j*11]->setRange(1,35);
            forthlinkoutLoopSpinBox[i+j*11] = new QSpinBox(forthlinkOutGroup[j]);
            forthlinkoutLoopSpinBox[i+j*11]->setGeometry(QRect(146,30+i*30,44,20));
            forthlinkoutLoopSpinBox[i+j*11]->setRange(1,20);
            forthlinkoutStart[i+j*11] = new QLineEdit(forthlinkOutGroup[j]);
            forthlinkoutStart[i+j*11]->setGeometry(QRect(195,30+i*30,39,20));
            forthlinkoutStart[i+j*11]->setValidator(new QIntValidator(1,255,forthlinkOutGroup[j]));

            forthlinkoutEnd[i+j*11] = new QLineEdit(forthlinkOutGroup[j]);
            forthlinkoutEnd[i+j*11]->setGeometry(QRect(239,30+i*30,39,20));
            forthlinkoutEnd[i+j*11]->setValidator(new QIntValidator(1,255,forthlinkOutGroup[j]));

            forthlinkoutDeviceCombo[i+j*11] = new QComboBox(forthlinkOutGroup[j]);
            forthlinkoutDeviceCombo[i+j*11]->setGeometry(QRect(283,30+i*30,92,20));
            k=i+j*11+1;
        }
        linkTabWidgetforth2->addTab(forthlinkOutGroup[j],tr("组联动%1--%2").arg(j*11+1).arg(k));
    }

    forthokButton = new QPushButton(tr("确定"),this);
    //     forthokButton->setMaximumSize(40,24);
    forthokButton->setGeometry(QRect(325,455,40,24));
    connect(forthokButton, SIGNAL(clicked()),this, SLOT(forthokButtonClicked()));

    forthcancelButton = new QPushButton(tr("取消"),this);
    //     forthcancelButton->setMaximumSize(40,24);
    forthcancelButton->setGeometry(QRect(425,455,40,24));
    connect(forthcancelButton, SIGNAL(clicked()),this, SLOT(forthcancelButtonClicked()));

    if(!editEnableFlag)
    {
        forthlinkinNum->setReadOnly(true);
        forthhideComboBox->setEnabled(false);
        forthokButton->setVisible(false);
        forthcancelButton->setVisible(false);
        QPushButton *okButton = new QPushButton(tr("OK"),this);
        okButton->setGeometry(QRect(375,455,40,24));
        connect(okButton, SIGNAL(clicked()),this->parentWidget(), SLOT(close()));

        for(int j = 0; j<31;j++)
        {
            forthlinkinHostSpinBox[j]->setReadOnly(true);
            forthlinkinIobrSpinBox[j]->setReadOnly(true);
            forthlinkinLoopSpinBox[j]->setReadOnly(true);
            forthlinkinStart[j]->setReadOnly(true);
            forthlinkinEnd[j]->setReadOnly(true);
            forthlinkinDeviceCombo[j]->setEnabled(false);
        }
        for(int j = 0; j<31;j++)
        {
            forthlinkoutHostSpinBox[j]->setReadOnly(true);
            forthlinkoutIobrSpinBox[j]->setReadOnly(true);
            forthlinkoutLoopSpinBox[j]->setReadOnly(true);
            forthlinkoutStart[j]->setReadOnly(true);
            forthlinkoutEnd[j]->setReadOnly(true);
            forthlinkoutDeviceCombo[j]->setEnabled(false);
        }
    }
}

zzLinkSetWidget::~zzLinkSetWidget()
{

}


void zzLinkSetWidget::linkDisplay(int id)
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    forthlinkid = id;


    query.exec(QString("select * from linkset WHERE linkid = %1").arg(id));

    while (query.next())
    {
        forthshield = query.value(1).toInt();
//        qDebug()<<"query.value(1).toInt()"<<query.value(1).toInt();
//        qDebug()<<"forthshield"<<forthshield;
        forthhideComboBox->setCurrentIndex(forthshield);
        //        if (query.value(1).toInt())
        //        {
        //            forthhideCheckBox->setCheckState(Qt::Checked);
        //        }
        //        else
        //        {
        //            forthhideCheckBox->setCheckState(Qt::Unchecked);
        //        }

        forthlinkinNum->setText(QString("%1").arg(query.value(4).toString()));
        forthnum = query.value(4).toInt();

        for(int j = 0;j< 31;j++)
        {
            long long int m = query.value(j+5).toLongLong();
            linkin[j] = m;
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
                forthlinkinHostSpinBox[j]->setValue(host);
                forthlinkinIobrSpinBox[j]->setValue(iobr);
                forthlinkinLoopSpinBox[j]->setValue(loop);
                forthlinkinStart[j]->setText(QString("%1").arg(znodestar));
                forthlinkinEnd[j]->setText(QString("%1").arg(znodeend));
                forthlinkinDeviceCombo[j]->clear();
                forthlinkinDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(zequiptype)));
                for (int n = 0x00; n<= 0x56; n++)
                {
                    if(n != zequiptype)
                    {
                        if(Type::char_danyuan(n) != "不存在  ")
                        {
                            forthlinkinDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(n)));
                        }
                    }
                }
            }
            else
            {
                forthlinkinDeviceCombo[j]->addItem(tr(" "));
                for (int n = 0x00; n<= 0x56; n++)
                {
                    if(Type::char_danyuan(n) != "不存在  ")
                    {
                        forthlinkinDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(n)));
                    }
                }
            }
        }

        for(int j = 0;j< 31;j++)
        {
            long long int m = query.value(j+36).toLongLong();
            linkout[j] = m;
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
                forthlinkoutHostSpinBox[j]->setValue(host);
                forthlinkoutIobrSpinBox[j]->setValue(iobr);
                forthlinkoutLoopSpinBox[j]->setValue(loop);
                forthlinkoutStart[j]->setText(QString("%1").arg(znodestar));
                forthlinkoutEnd[j]->setText(QString("%1").arg(znodeend));
                forthlinkoutDeviceCombo[j]->clear();
                forthlinkoutDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(zequiptype)));
                for (int n = 0x00; n<= 0x56; n++)
                {
                    if(n != zequiptype)
                    {
                        if(Type::char_danyuan(n) != "不存在  ")
                        {
                            forthlinkoutDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(n)));
                        }
                    }
                }
            }
            else
            {
                forthlinkoutDeviceCombo[j]->addItem(tr(" "));
                for (int n = 0x00; n<= 0x56; n++)
                {
                    if(Type::char_danyuan(n) != "不存在  ")
                    {
                        forthlinkoutDeviceCombo[j]->addItem(tr("%1").arg(Type::char_danyuan(n)));
                    }
                }
            }
        }
    }
    QSqlDatabase::database().commit();
}

void zzLinkSetWidget::linknumreturn(QString test)
{
    forthnum = test.toInt();
    if(forthnum == 0)
    {
        forthnum = 1;
        forthlinkinNum->setText(QString("%1").arg(forthnum));
    }
}

void zzLinkSetWidget::shieldreturn(int shield)
{
    forthshield = shield;
    //    if(!shield)
    //    {
    //        forthshield = shield;
    //    }
    //    else
    //    {
    //        forthshield = shield-1;
    //    }
}


void zzLinkSetWidget::forthokButtonClicked()
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
        int host = 0, iobr = 0, loop  = 0, zequiptype = 0,zsetupnum = 0;
        long long int znodestar = 0, znodeend = 0;
        long long int flag = 0;
        for(int j = 0; j < 31; j++)
        {
            if(forthlinkinStart[j]->text().toInt() == 0)
            {
                flag = 0;
            }
            else if(forthlinkinEnd[j]->text().toInt() == 0)
            {
                flag = 0;
            }
            else if(Type::int_danyuanId(forthlinkinDeviceCombo[j]->currentText()) == 0xFF)
            {
                flag = 0;
            }
            else if(forthlinkinStart[j]->text().toInt() > forthlinkinEnd[j]->text().toInt())
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
                host = forthlinkinHostSpinBox[j]->value();
                iobr = forthlinkinIobrSpinBox[j]->value();
                loop = forthlinkinLoopSpinBox[j]->value();
                znodestar = forthlinkinStart[j]->text().toInt() & 0xFF;
                znodeend = forthlinkinEnd[j]->text().toInt() & 0xFF;
                zequiptype = Type::int_danyuanId(forthlinkinDeviceCombo[j]->currentText());
                flag =  ((long  long  int)host<<48) | ((long  long  int)iobr<<40)
                        | ((long  long  int)loop<<32) | (znodestar<<24) | (znodeend<<16) | (zequiptype<<8) | zsetupnum;
                //printf("flag%llx\n",flag);
            }
            linkin[j] = flag;
        }

        for(int j = 0; j < 31; j++)
        {
            if(forthlinkoutStart[j]->text().toInt() == 0)
            {
                flag = 0;
            }
            else if(forthlinkoutEnd[j]->text().toInt() == 0)
            {
                flag = 0;
            }
            else if(Type::int_danyuanId(forthlinkoutDeviceCombo[j]->currentText()) == 0xFF)
            {
                flag = 0;
            }
            else if(forthlinkoutStart[j]->text().toInt() > forthlinkoutEnd[j]->text().toInt())
            {
                //                QMessageBox::warning(this,tr("警告"),
                //                                     QString(tr("联动输出设置中的组联动中\n因为第%1项起始地址大于终止地址，\n保存失败，请返回修改！")
                //                                             .arg(j+1)),QMessageBox::Ok);
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
                host = forthlinkoutHostSpinBox[j]->value();
                iobr = forthlinkoutIobrSpinBox[j]->value();
                loop = forthlinkoutLoopSpinBox[j]->value();
                znodestar = forthlinkoutStart[j]->text().toInt() & 0xFF;
                znodeend = forthlinkoutEnd[j]->text().toInt() & 0xFF;
                zequiptype = Type::int_danyuanId(forthlinkoutDeviceCombo[j]->currentText());
                flag =  ((long  long  int)host<<48) | ((long  long  int)iobr<<40)
                        | ((long  long  int)loop<<32) | (znodestar<<24) | (znodeend<<16) | (zequiptype<<8) | zsetupnum;
                //printf("flag%llx\n",flag);
            }
            linkout[j] = flag;            
        }
        this->updateLinkset();
    }
}

void zzLinkSetWidget::forthcancelButtonClicked()
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

void zzLinkSetWidget::updateLinkset()
{
    emit  changeTypeLabel(3);
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
                          "WHERE linkid =%1").arg(forthlinkid));

//    printf("forthnum%dforthlinkid%d",forthnum,forthlinkid);

    query.bindValue(0,forthshield);
    query.bindValue(1,forthsetup);
    query.bindValue(2,forthtype);
    query.bindValue(3,forthnum);

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
    emit transferzzLinknum(forthlinkid,forthnum);
    emit databaseChange();
}

void zzLinkSetWidget::clearforthWidget()
{
    for(int j = 0; j<31;j++)
    {
        forthlinkinHostSpinBox[j]->clear();
        forthlinkinIobrSpinBox[j]->clear();
        forthlinkinLoopSpinBox[j]->clear();
        forthlinkinStart[j]->clear();
        forthlinkinEnd[j]->clear();
        forthlinkinDeviceCombo[j]->clear();
    }    
    for(int j = 0; j<31;j++)
    {
        forthlinkoutHostSpinBox[j]->clear();
        forthlinkoutIobrSpinBox[j]->clear();
        forthlinkoutLoopSpinBox[j]->clear();
        forthlinkoutStart[j]->clear();
        forthlinkoutEnd[j]->clear();
        forthlinkoutDeviceCombo[j]->clear();
    }
    //
    for(int i = 0;i<31;i++)
    {
        forthlinkinDeviceCombo[i]->addItem(tr(" "));
        for (int n = 0x00; n<= 0x56; n++)
        {
            if(Type::char_danyuan(n) != "不存在  ")
            {
                forthlinkinDeviceCombo[i]->addItem(tr("%1").arg(Type::char_danyuan(n)));
            }
        }
    }

    for(int i = 0;i<31;i++)
    {
        forthlinkoutDeviceCombo[i]->addItem(tr(" "));
        for (int n = 0x00; n<= 0x56; n++)
        {
            if(Type::char_danyuan(n) != "不存在  ")
            {
                forthlinkoutDeviceCombo[i]->addItem(tr("%1").arg(Type::char_danyuan(n)));
            }
        }
    }     
    //    forthhideCheckBox->setCheckState(Qt::Checked);
    forthhideComboBox->setCurrentIndex(0);
    forthnum = 1 ;
    forthlinkinNum->setText(QString("%1").arg(forthnum));    
}




