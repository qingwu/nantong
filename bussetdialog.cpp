#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QListWidget>
#include <QtGui/QFrame>
#include <QtGui/QMessageBox>
#include <QtCore/QDateTime>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtDebug>

#include "bussetdialog.h"
#include "type.h"

BusSetDialog::BusSetDialog(QWidget *parent) :
    QDialog(parent)
{

}
BusSetDialog::BusSetDialog(bool editFlag, QWidget *parent) :
    QDialog(parent)
{
    qFill(inMode, inMode + 15, 0);
    qFill(outMode, outMode + 15, 0);
    editEnableFlag = editFlag;
    createWidget();
    this->setWindowTitle(tr("总线设置"));
    this->resize(QSize(800, 480));
}
void BusSetDialog::createWidget()
{
    mainFrame = new QFrame(this);
    mainFrame->setGeometry(QRect(40,40,720,400));
    mainFrame->setFrameShape(QFrame::Box);
    candidateFrame = new QFrame(mainFrame);
    candidateFrame->setFrameShape(QFrame::Box);
    candidateFrame->setGeometry(QRect(270,40,190,350));

    inputListWidget = new QListWidget(mainFrame);
    inputListWidget->setGeometry(QRect(12,40,198,340));

    candidateListWidget = new QListWidget(candidateFrame);
    candidateListWidget->setGeometry(QRect(20,100,150,240));
    //new QListWidgetItem(tr("Oak"), candidateListWidget);

    outputListWidget = new QListWidget(mainFrame);
    outputListWidget->setGeometry(QRect(515,40,198,340));

    buspanelLabel = new QLabel(tr("总线盘号"),this);
    //buspanelLabel->resize(QSize(60,20));
    buspanelLabel->setFrameShape(QFrame::Box);
    buspanelLabel->setGeometry(QRect(60,10,80,20));
    busnodeLabel = new QLabel(tr("总线点号"),this);
    busnodeLabel->setFrameShape(QFrame::Box);
    //busnodeLabel->resize(QSize(60,20));
    busnodeLabel->setGeometry(QRect(230,10,80,20));

    inputLabel = new QLabel(tr("输入"),mainFrame);/*模块*/
    inputLabel->setFrameShape(QFrame::Box);
    inputLabel->setAlignment(Qt::AlignCenter);
    inputLabel->setGeometry(QRect(40,10,140,20));
    outputLabel = new QLabel(tr("输出"),mainFrame);/*模块*/
    outputLabel->setAlignment(Qt::AlignCenter);
    outputLabel->setFrameShape(QFrame::Box);
    outputLabel->setGeometry(QRect(530,10,140,20));
    candidateLabel = new QLabel(tr("待选模块列表"),mainFrame);
    candidateLabel->setAlignment(Qt::AlignCenter);
    candidateLabel->setFrameShape(QFrame::Box);
    candidateLabel->setGeometry(QRect(290,10,140,20));

    hostLabel = new QLabel(tr("主机号"),candidateFrame);
    hostLabel->setFrameShape(QFrame::Box);
    hostLabel->setGeometry(QRect(20,10,70,20));
    iobrLabel = new QLabel(tr("接口板号"),candidateFrame);
    iobrLabel->setFrameShape(QFrame::Box);
    iobrLabel->setGeometry(QRect(20,38,70,20));
    loopLabel = new QLabel(tr("回路号"),candidateFrame);
    loopLabel->setFrameShape(QFrame::Box);
    loopLabel->setGeometry(QRect(20,68,70,20));

    //add by linwotian:
    buspanelComboBox = new QComboBox(this);
    buspanelComboBox->setGeometry(QRect(150, 10, 60, 20));

    copybuspanelComboBox = new QComboBox(this);
    copybuspanelComboBox->setGeometry(QRect(460, 10, 60, 20));

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.clear();
    query.exec(QString("select * from zongxianinfo WHERE zongxianregist = 'true'"));
    while (query.next())
    {
        buspanelComboBox->addItem(tr("%1").arg(query.value(0).toInt()));
        copybuspanelComboBox->addItem(tr("%1").arg(query.value(0).toInt()));
    }
    QSqlDatabase::database().commit();
    connect(buspanelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(readBusSet(int)));


    busnodeSpinBox = new QSpinBox (this);
    busnodeSpinBox->setRange(1, 64);
    busnodeSpinBox->setGeometry(QRect(320,10,60,20));
    connect(busnodeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(readBusSet(int)));

    copyLabel = new QLabel(tr("从第"),this);
    copyLabel->setFrameShape(QFrame::Box);
    copyLabel->setGeometry(QRect(420,10,40,20));

    copyLabel2 = new QLabel(tr("总线盘的第"),this);
    copyLabel2->setFrameShape(QFrame::Box);
    copyLabel2->setGeometry(QRect(520,10,100,20));

    copybusnodeSpinbox = new QSpinBox (this);
    copybusnodeSpinbox->setRange(1, 64);
    copybusnodeSpinbox->setGeometry(QRect(620,10,60,20));

    copyLabel3 = new QLabel(tr("总线点"),this);
    copyLabel3->setFrameShape(QFrame::Box);
    copyLabel3->setGeometry(QRect(680,10,55,20));

    copyPushButton = new QPushButton(tr("复制"),this);
    copyPushButton->setGeometry(QRect(740,8,50,27));
    connect(copyPushButton, SIGNAL(clicked()), this, SLOT(copyPushButtonClicked()));

    if(!editEnableFlag)
    {
        copyLabel->setVisible(false);
        copyLabel2->setVisible(false);
        copyLabel3->setVisible(false);
        copybuspanelComboBox->setVisible(false);
        copybusnodeSpinbox->setVisible(false);
        copyPushButton->setVisible(false);
    }

    hostSpinBox = new QSpinBox (candidateFrame);
    hostSpinBox->setRange(1,20);
    hostSpinBox->setGeometry(QRect(100,6,60,28));
    connect(hostSpinBox, SIGNAL(valueChanged(int)), this, SLOT(readUnitSet(int)));

    iobrSpinBox = new QSpinBox (candidateFrame);
    iobrSpinBox->setRange(1,32);
    iobrSpinBox->setGeometry(QRect(100,35,60,28));
    connect(iobrSpinBox, SIGNAL(valueChanged(int)), this, SLOT(readUnitSet(int)));

    loopSpinBox = new QSpinBox (candidateFrame);
    loopSpinBox->setRange(1,4);
    loopSpinBox->setGeometry(QRect(100,64,60,28));
    connect(loopSpinBox, SIGNAL(valueChanged(int)), this, SLOT(readUnitSet(int)));

    okPushButton = new QPushButton(tr("确定"),this);
    okPushButton->setGeometry(QRect(275,450,85,27));
    connect(okPushButton, SIGNAL(clicked()), this, SLOT(okSlot()));

    cancelPushButton = new QPushButton(tr("取消"),this);
    cancelPushButton->setGeometry(QRect(435,450,85,27));
    connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
    /*初始化主机号接口板回路都为1时待选化列表*/
    this->readUnitSet(1);
    this->readBusSet(1);
    if(editEnableFlag)
    {
        cToinPushButton = new QPushButton(tr("添加"),mainFrame);
        cToinPushButton->setGeometry(QRect(215,180,40,27));
        connect(cToinPushButton,SIGNAL(clicked()),this,SLOT(cToinClicked()));
        inTocPushButton = new QPushButton(tr("删除"),mainFrame);
        inTocPushButton->setGeometry(QRect(215,215,40,27));
        connect(inTocPushButton,SIGNAL(clicked()),this,SLOT(inTocClicked()));


        cTooutPushButton = new QPushButton(tr("添加"),mainFrame);
        cTooutPushButton->setGeometry(QRect(465,180,40,27));
        connect(cTooutPushButton,SIGNAL(clicked()),this,SLOT(cTooutClicked()));
        outTocPushButton = new QPushButton(tr("删除"),mainFrame);
        outTocPushButton->setGeometry(QRect(465,215,40,27));
        connect(outTocPushButton,SIGNAL(clicked()),this,SLOT(outTocClicked()));
    }
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
}

void BusSetDialog::readUnitSet(int)
{
    char danyuan_num[20];
    candidateListWidget->clear();

    int host = hostSpinBox->value();
    int iobr = iobrSpinBox->value();
    int loop = loopSpinBox->value();
    int node = 1;
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2")
               .arg((host<<24) | (iobr<<16) | (loop<<8) | node).arg((host<<24) | (iobr<<16) | (loop<<8) | 0xFF));
    while (query.next())
    {
        if(query.value(1).toBool())
        {
            if(! query.value(2).toBool())
            {
                memset(danyuan_num, 0x0, 20);
                sprintf(danyuan_num, "%03d",query.value(5).toInt() & 0xFF);
                QString danyuan_info = QString("%1 %2").arg(danyuan_num).arg(Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF));
                candidateListWidget->addItem(tr("%1").arg(danyuan_info));
            }
        }
    }
    QSqlDatabase::database().commit();
}
void BusSetDialog::readBusSet(int)
{
    inputListWidget->clear();
    outputListWidget->clear();
    qFill(inMode, inMode + 15, 0);
    qFill(outMode, outMode + 15, 0);
    char inModenum[20];
    char outModenum[20];

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from busset WHERE bus_num = %1 AND bus_node = %2")
               .arg(buspanelComboBox->currentText().toInt()).arg(busnodeSpinBox->value()));
    if (query.next())
    {
        for(int i = 0;i < 1; i++)
        {
            int k = query.value(i + 3).toInt();
            //qDebug() << query.value(i + 3).toInt();
            inMode[i] = k;
            if (k)
            {
                 /*找出节点所对应的设备类型,在列表中显示*/
                QSqlDatabase::database().transaction();
                QSqlQuery query1;
                query1.setForwardOnly(true);
                query1.exec(QString("select * from unitset WHERE unitid = %1").arg(k));
                if (query1.next())
                {
                    memset(inModenum, 0x0, 20);
                    sprintf(inModenum, "%02d-%02d-%02d-%03d",(k >> 24) & 0xFF, (k >> 16) & 0xFF,
                                                (k >> 8) & 0xFF, k & 0xFF);
                    QString inModeinfo = QString("%1 %2").arg(inModenum)
                            .arg(Type::char_danyuan((query1.value(7).toInt() >> 8) & 0xFF));
                    inputListWidget->addItem(inModeinfo);
                }
                QSqlDatabase::database().commit();
            }
            else
            {
                break;
            }
        }
        for(int j = 0; j < 1; j++)
        {
            int k = query.value(j + 18).toInt();
            outMode[j] = k;
            if (k)
            {
                QSqlDatabase::database().transaction();
                QSqlQuery query2;
                query2.setForwardOnly(true);
                query2.exec(QString("select * from unitset WHERE unitid = %1").arg(k));
                if (query2.next())
                {
                    memset(outModenum, 0x0, 20);
                    sprintf(outModenum, "%02d-%02d-%02d-%03d",(k >> 24) & 0xFF, (k >> 16) & 0xFF,
                                                (k >> 8) & 0xFF, k & 0xFF);
                    QString outModeinfo = QString("%1 %2").arg(outModenum)
                            .arg(Type::char_danyuan((query2.value(7).toInt() >> 8) & 0xFF));
                    outputListWidget->addItem(outModeinfo);
                }
                QSqlDatabase::database().commit();
            }
            else
            {
                break;
            }
        }
    }
    QSqlDatabase::database().commit();
}

void BusSetDialog::inTocClicked()
{
    if(inputListWidget->count() > 0)
    {
        int *j;
        QString dtstr = inputListWidget->item(0)->text();

        int k = (dtstr.left(2).toInt() << 24)
                | (dtstr.mid(3,2).toInt() << 16)
                | (dtstr.mid(6,2).toInt() << 8)
                | dtstr.mid(9,3).toInt();

        for (j = qFind(inMode, inMode +inputListWidget->count(),k); j <inMode +inputListWidget->count()-1; j++)
        {
            *j =*(j+1);
        }
        *j = 0;
        inputListWidget->takeItem(0);
    }

//    if (inputListWidget->currentRow() != (-1))
//    {

//        int *j;
//        QString dtstr = inputListWidget->currentItem()->text();

//        int k = (dtstr.left(2).toInt() << 24)
//                | (dtstr.mid(3,2).toInt() << 16)
//                | (dtstr.mid(6,2).toInt() << 8)
//                | dtstr.mid(9,3).toInt();

//        for (j = qFind(inMode, inMode +inputListWidget->count(),k); j <inMode +inputListWidget->count()-1; j++)
//        {
//            *j =*(j+1);
//        }
//        *j = 0;
//        inputListWidget->takeItem(inputListWidget->currentRow());
//    }
//    else
//    {
//        //QMessageBox::warning(this,tr("警告"),tr("请在输入模块中选择要删除的设备"),QMessageBox::Ok);
//        QMessageBox *box = new QMessageBox(tr("警告"),
//                                           tr("请在输入模块中选择要删除的设备!"),
//                                           QMessageBox::Warning,
//                                           QMessageBox::Ok,
//                                           QMessageBox::NoButton,
//                                           QMessageBox::NoButton,
//                                           this);
//        box->setAttribute(Qt::WA_DeleteOnClose);
//        connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
//        box->exec();
//    }

}

void BusSetDialog::cToinClicked()
{
    if (candidateListWidget->currentRow() != (-1))
    {
        char inModeinfo[20];
        memset(inModeinfo, 0x0, 20);
        sprintf(inModeinfo, "%02d-%02d-%02d-", hostSpinBox->value(), iobrSpinBox->value(), loopSpinBox->value());
        QString dtstr = QString("%1%2").arg(inModeinfo).arg(candidateListWidget->currentItem()->text());

        //add by qingwu:这里k的赋值可以放到else中?
        /*long long */int k = (dtstr.left(2).toInt() << 24)
                    | (dtstr.mid(3,2).toInt() << 16)
                    | (dtstr.mid(6,2).toInt() << 8)
                    | dtstr.mid(9,3).toInt();

        if (inputListWidget->count()+1 > 1)
        {
            //QMessageBox::warning(this,tr("警告"),tr("节点数已达1条上限，不能再添加"),QMessageBox::Ok);
            QMessageBox *box = new QMessageBox(tr("警告"),
                                               tr("节点数已达1条上限，不能再添加!"),
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
            if(qFind(inMode, inMode +inputListWidget->count()+1,k) == (inMode + inputListWidget->count()+1))
            {
                inMode[inputListWidget->count()] =  k;
                inputListWidget->addItem(dtstr);
            }
            else
            {
                //QMessageBox::warning(this,tr("警告"),tr("已添加，请重新选择！"),QMessageBox::Ok);
                QMessageBox *box = new QMessageBox(tr("警告"),
                                                   tr("已添加，请重新选择！"),
                                                   QMessageBox::Warning,
                                                   QMessageBox::Ok,
                                                   QMessageBox::NoButton,
                                                   QMessageBox::NoButton,
                                                   this);
                box->setAttribute(Qt::WA_DeleteOnClose);
                connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
                box->exec();
            }
        }
    }
    else
    {
        //QMessageBox::warning(this,tr("警告"),tr("请在待选模块中选择要添加的设备"),QMessageBox::Ok);
        QMessageBox *box = new QMessageBox(tr("警告"),
                                           tr("请在待选模块中选择要添加的设备!"),
                                           QMessageBox::Warning,
                                           QMessageBox::Ok,
                                           QMessageBox::NoButton,
                                           QMessageBox::NoButton,
                                           this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        box->exec();
    }
}

void BusSetDialog::cTooutClicked()
{
    if (candidateListWidget->currentRow() != (-1))
    {
//        char outModeinfo[20];
//        memset(outModeinfo, 0x0, 20);
//        sprintf(outModeinfo, "%02d-%02d-%02d-", hostSpinBox->value(), iobrSpinBox->value(), loopSpinBox->value());

        int host = hostSpinBox->value();
        int iobr = iobrSpinBox->value();
        int loop = loopSpinBox->value();
        int node = candidateListWidget->currentItem()->text().mid(0,3).toInt();
        int k = (host<<24) | (iobr<<16) | (loop<<8) | node;
        QString dtstr;
        dtstr.sprintf("%02d-%02d-%02d-",host,iobr,loop);// QString("%1%2").arg(outModeinfo).arg(candidateListWidget->currentItem()->text());
        dtstr.append(candidateListWidget->currentItem()->text());

//        /*long long */int k = (dtstr.left(2).toInt() << 24)
//                    | (dtstr.mid(3,2).toInt() << 16)
//                    | (dtstr.mid(6,2).toInt() << 8)
//                    | dtstr.mid(9,3).toInt();

        if (outputListWidget->count()+1 > 1)
        {
            //QMessageBox::warning(this,tr("警告"),tr("节点数已达1条上限，不能再添加"),QMessageBox::Ok);
            QMessageBox *box = new QMessageBox(tr("警告"),
                                               tr("节点数已达1条上限，不能再添加!"),
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
            if(qFind(outMode, outMode +outputListWidget->count()+1,k) == (outMode + outputListWidget->count()+1))
            {
                outMode[outputListWidget->count()] =  k;
                outputListWidget->addItem(dtstr);
            }
            else
            {
                //QMessageBox::warning(this,tr("警告"),tr("已添加，请重新选择！"),QMessageBox::Ok);
                QMessageBox *box = new QMessageBox(tr("警告"),
                                                   tr("已添加，请重新选择！"),
                                                   QMessageBox::Warning,
                                                   QMessageBox::Ok,
                                                   QMessageBox::NoButton,
                                                   QMessageBox::NoButton,
                                                   this);
                box->setAttribute(Qt::WA_DeleteOnClose);
                connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
                box->exec();
            }
        }
    }
    else
    {
        //QMessageBox::warning(this,tr("警告"),tr("请在待选模块中选择要添加的设备"),QMessageBox::Ok);
        QMessageBox *box = new QMessageBox(tr("警告"),
                                           tr("请在待选模块中选择要添加的设备!"),
                                           QMessageBox::Warning,
                                           QMessageBox::Ok,
                                           QMessageBox::NoButton,
                                           QMessageBox::NoButton,
                                           this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        box->exec();
    }
}

void BusSetDialog::outTocClicked()
{
    if(outputListWidget->count() > 0)
    {
        int *j;
        QString dtstr = outputListWidget->item(0)->text();

        int k = (dtstr.left(2).toInt() << 24)
                | (dtstr.mid(3,2).toInt() << 16)
                | (dtstr.mid(6,2).toInt() << 8)
                | dtstr.mid(9,3).toInt();

        for (j = qFind(outMode, outMode +outputListWidget->count(),k); j < outMode +outputListWidget->count()-1; j++)
        {
            *j =*(j+1);
        }
        *j = 0;
        outputListWidget->takeItem(0);
    }

//    if (outputListWidget->currentRow() != (-1))
//    {

//        int *j;
//        QString dtstr = outputListWidget->currentItem()->text();

//        int k = (dtstr.left(2).toInt() << 24)
//                | (dtstr.mid(3,2).toInt() << 16)
//                | (dtstr.mid(6,2).toInt() << 8)
//                | dtstr.mid(9,3).toInt();

//        for (j = qFind(outMode, outMode +outputListWidget->count(),k); j < outMode +outputListWidget->count()-1; j++)
//        {
//            *j =*(j+1);
//        }
//        *j = 0;
//        outputListWidget->takeItem(outputListWidget->currentRow());
//    }
//    else
//    {
//        //QMessageBox::warning(this,tr("警告"),tr("请在输出模块中选择要删除的设备"),QMessageBox::Ok);
//        QMessageBox *box = new QMessageBox(tr("警告"),
//                                           tr("请在输出模块中选择要删除的设备!"),
//                                           QMessageBox::Warning,
//                                           QMessageBox::Ok,
//                                           QMessageBox::NoButton,
//                                           QMessageBox::NoButton,
//                                           this);
//        box->setAttribute(Qt::WA_DeleteOnClose);
//        connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
//        box->exec();
//    }

}

void BusSetDialog::okSlot()
{
    if (editEnableFlag)
    {
//        int r = QMessageBox::information(this, tr("提示"), tr("设置将会保存"), QMessageBox::No | QMessageBox::Yes);
//        if (r == QMessageBox::Yes)
//        {
//            this->updateBusset();
//        }
        QMessageBox *box = new QMessageBox(tr("提示"),
                                            tr("设置将会保存!"),
                                            QMessageBox::Information,
                                            QMessageBox::Yes | QMessageBox::Default,
                                            QMessageBox::NoButton,
                                            QMessageBox::No | QMessageBox::Escape,
                                            this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        switch(box->exec())
        {
            case QMessageBox::Yes:
            {
                this->updateBusset();
            }
            break;
            case  QMessageBox::No:
            {
                //this->close();
            }
            break;
        }
    }
    else
    {
        close();
    }
}
void BusSetDialog::updateBusset()
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    QTime time;
    query.clear();
    time.restart();

    query.prepare(QString("UPDATE busset SET "
                          "inMode1=?,"
                          "outMode1=?"
                          "WHERE bus_num = %1 AND bus_node = %2").arg(buspanelComboBox->currentText().toInt()).arg(busnodeSpinBox->value()));
    query.addBindValue(inMode[0]);
    query.addBindValue(outMode[0]);

    bool bSuccess = query.exec();
    if (!bSuccess)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText()
                    <<QString(QObject::tr("update error"));
    }
    QSqlDatabase::database().commit();
    qDebug() << QObject::tr("update take %1 ms").arg(time.elapsed());
}


void BusSetDialog::copyPushButtonClicked()
{
    inputListWidget->clear();
    outputListWidget->clear();
    qFill(inMode, inMode + 15, 0);
    qFill(outMode, outMode + 15, 0);
    char inModenum[20];
    char outModenum[20];

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from busset WHERE bus_num = %1 AND bus_node = %2")
               .arg(copybuspanelComboBox->currentText().toInt()).arg(copybusnodeSpinbox->value()));
    if (query.next())
    {
        for(int i = 0;i < 1; i++)
        {
            int k = query.value(i + 3).toInt();
            //qDebug() << query.value(i + 3).toInt();
            inMode[i] = k;
            if (k)
            {
                 /*找出节点所对应的设备类型,在列表中显示*/
                QSqlDatabase::database().transaction();
                QSqlQuery query1;
                query1.setForwardOnly(true);
                query1.exec(QString("select * from unitset WHERE unitid = %1").arg(k));
                if (query1.next())
                {
                    memset(inModenum, 0x0, 20);
                    sprintf(inModenum, "%02d-%02d-%02d-%03d",(k >> 24) & 0xFF, (k >> 16) & 0xFF,
                                                (k >> 8) & 0xFF, k & 0xFF);
                    QString inModeinfo = QString("%1 %2").arg(inModenum)
                            .arg(Type::char_danyuan((query1.value(7).toInt() >> 8) & 0xFF));
                    inputListWidget->addItem(inModeinfo);
                }
                QSqlDatabase::database().commit();
            }
            else
            {
                break;
            }
        }
        for(int j = 0; j < 1; j++)
        {
            int k = query.value(j + 18).toInt();
            outMode[j] = k;
            if (k)
            {
                QSqlDatabase::database().transaction();
                QSqlQuery query2;
                query2.setForwardOnly(true);
                query2.exec(QString("select * from unitset WHERE unitid = %1").arg(k));
                if (query2.next())
                {
                    memset(outModenum, 0x0, 20);
                    sprintf(outModenum, "%02d-%02d-%02d-%03d",(k >> 24) & 0xFF, (k >> 16) & 0xFF,
                                                (k >> 8) & 0xFF, k & 0xFF);
                    QString outModeinfo = QString("%1 %2").arg(outModenum)
                            .arg(Type::char_danyuan((query2.value(7).toInt() >> 8) & 0xFF));
                    outputListWidget->addItem(outModeinfo);
                }
                QSqlDatabase::database().commit();
            }
            else
            {
                break;
            }
        }
    }
    QSqlDatabase::database().commit();
}
