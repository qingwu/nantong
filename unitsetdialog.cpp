/*
@date:2012.5.25
@qingwu
*/
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>
#include <QtGui/QStandardItemModel>
#include <QtGui/QTableView>
#include <QtGui/QHeaderView>
#include <QtGui/QMessageBox>
#include <QtCore/QModelIndex>
#include <QtCore/QTextStream>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtDebug>
#include <QString>

#include "unitsetdialog.h"
#include "combodelegate.h"
#include "spindelegate.h"
#include "info.h"
#include "type.h"
#include "groupsetdialog.h"

UnitSetDialog::UnitSetDialog(QWidget *parent)
    : QDialog(parent)
{
}
UnitSetDialog::UnitSetDialog(bool editFlag, QWidget *parent)
    : QDialog(parent)
{
    dataChangeFlag = 0;
    comboChangeFlag = false;
    //boardComboChangeFlag = false;
    editEnableFlag = editFlag;
    createItems();
    initModel();
    connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(dataChangedSlot(QModelIndex)));
    this->setWindowTitle(tr("单元设置"));
    this->resize(QSize(800, 480));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
}

UnitSetDialog::~UnitSetDialog()
{
}

void UnitSetDialog::createItems()
{
    dataChangeFlag = 0;
    boardLabel = new QLabel(tr("接口板号"), this);
    boardLabel->setGeometry(QRect(160,12,60,20));
    loopLabel = new QLabel(tr("回路号"), this);
    loopLabel->setGeometry(QRect(400,12,50,20));
    loopLabel->setVisible(false);
    zhikongLabel = new QLabel(tr("直控盘号"), this);
    zhikongLabel->setGeometry(QRect(400,12,50,20));
    zhikongLabel->setVisible(false);
    zongxianLabel = new QLabel(tr("总线盘号"), this);
    zongxianLabel->setGeometry(QRect(400,12,50,20));
    zongxianLabel->setVisible(false);

    registerNodeLabel = new QLabel(tr("登记节点数:"), this);
    registerNodeLabel->setGeometry(QRect(20, 12, 80,20));
    registerNodenumLabel = new QLabel(tr("000"),this);
    registerNodenumLabel->setGeometry(QRect(100 , 12, 30, 20));
    danyuanCountnumLabel = new QLabel(tr("000"), this);
    danyuanCountnumLabel->setGeometry(QRect(660 , 12, 30, 20));
    countnumLabel = new QLabel(tr("个"), this);

    groupsetPushButton = new QPushButton(tr("批设置"), this);
    groupsetPushButton->setGeometry(QRect(725, 12, 55, 20));
    groupsetPushButton->setVisible(false);
    connect(groupsetPushButton, SIGNAL(clicked()), this, SLOT(groupsetPushButtonClick()));


    countnumLabel->setGeometry(QRect(690, 12, 20, 20));
    countnumLabel->setFont(QFont("Times", 10, QFont::Bold));

    boardComboBox = new QComboBox(this);
    boardComboBox->setGeometry(QRect(230,8,70,27));
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from iobrinfo WHERE iobrregist = 'true'"));
    while (query.next())
    {
        boardComboBox->addItem(tr("%1").arg(query.value(0).toInt()));
    }
    //    QSqlDatabase::database().commit();

    typeComboBox = new QComboBox(this);
    typeComboBox->setGeometry(QRect(305, 8, 80, 27));
    typeComboBox->addItem(tr("直控盘"));
    typeComboBox->addItem(tr("总线盘"));
    typeComboBox->setVisible(false);


    loopComboBox = new QComboBox(this);
    loopComboBox->setGeometry(QRect(460, 8, 55, 27));
    for (int i = 1;i <= 4;i++)
    {
        loopComboBox->addItem(tr("%1").arg(i));
    }
    loopComboBox->setVisible(false);

    floorshowloopComboBox = new QComboBox(this);
    floorshowloopComboBox->setGeometry(QRect(460, 8, 55, 27));
    for (int i = 1;i <= 1;i++)
    {
        floorshowloopComboBox->addItem(tr("%1").arg(i));
    }
    floorshowloopComboBox->setVisible(false);

    zhikongComboBox = new QComboBox(this);
    zhikongComboBox->setGeometry(QRect(460, 8, 55, 27));
    query.clear();
    query.exec(QString("select * from zhikonginfo WHERE zhikongregist = 'true'"));
    while (query.next())
    {
        zhikongComboBox->addItem(tr("%1").arg(query.value(0).toInt()));
    }
    zhikongComboBox->setVisible(false);

    zongxianComboBox = new QComboBox(this);
    zongxianComboBox->setGeometry(QRect(460, 8, 55, 27));
    query.clear();
    query.exec(QString("select * from zongxianinfo WHERE zongxianregist = 'true'"));
    while (query.next())
    {
        zongxianComboBox->addItem(tr("%1").arg(query.value(0).toInt()));
    }
    zongxianComboBox->setVisible(false);
    QSqlDatabase::database().commit();

    danyuanComboBox = new QComboBox(this);
    danyuanComboBox->setGeometry(QRect(550, 8, 100, 27));
    for (int i = 0;i < (int)sizeof(char_danyuan) /((int)sizeof(char)*4);i++)
    {
        danyuanComboBox->addItem(tr(char_danyuan[i]));
    }

    if (boardComboBox->currentText().toInt() < 33)
    {
        loopLabel->setVisible(true);
        loopComboBox->setVisible(true);
        groupsetPushButton->setVisible(true);
        danyuanComboBox->setVisible(true);
        danyuanCountnumLabel->setVisible(true);
        countnumLabel->setVisible(true);
    }
    else if(boardComboBox->currentText().toInt() == 33)
    {
        loopLabel->setVisible(false);//loopLabel->setVisible(true);
        floorshowloopComboBox->setVisible(true);
        groupsetPushButton->setVisible(true);
        danyuanComboBox->setVisible(false);
        danyuanCountnumLabel->setVisible(false);
        countnumLabel->setVisible(false);
    }
    else if(boardComboBox->currentText().toInt() == 34)
    {
        typeComboBox->setVisible(true);
        zhikongLabel->setVisible(true);
        zhikongComboBox->setVisible(true);
        danyuanComboBox->setVisible(false);
        danyuanCountnumLabel->setVisible(false);
        countnumLabel->setVisible(false);
    }


    connect(boardComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(boardreadUnitSet()));
    connect(loopComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(loopreadUnitSet()));
    connect(danyuanComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(danyuanreadUnitSet()));
    connect(typeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(typeChange(int)));
    connect(zhikongComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(zhikongreadUnitSet()));

    okPushButton = new QPushButton(tr("确定"), this);
    okPushButton->setGeometry(QRect(275, 450, 85, 27));
    connect(okPushButton, SIGNAL(clicked()), this, SLOT(okToContinue()));
    cancelPushButton = new QPushButton(tr("取消"), this);
    cancelPushButton->setGeometry(QRect(435, 450, 85, 27));
    connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));

    model = new QStandardItemModel (rowNumber, columnNumber, this);
    tableview = new QTableView(this);
    tableview->setModel(model);
    tableview->setGeometry(QRect(10, 40, 775, 395));
    if(!editEnableFlag)
    {
        tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
        groupsetPushButton->setEnabled(false);
    }
}

void UnitSetDialog::initModel()
{
    int host = 1;
    int iobr = boardComboBox->currentText().toInt();
    //int loop = loopComboBox->currentText().toInt();

    //add bu linwot ,if we only have the zhikong regist in when init the system ,the loop maybe different
    int loop = 0;
    if(iobr == 0)
    {
       loop = 0;
       loopComboBox->setEnabled(false);
    }
    else if(iobr <= 32)
    {
       loop = loopComboBox->currentText().toInt();
    }
    else if(iobr == 33)
    {
       loop = floorshowloopComboBox->currentText().toInt();
    }
    else if (iobr == 34)
    {
        loop = zhikongComboBox->currentText().toInt();
        for(int row = 0 ; row <= 7 ; row++)
        {
            tableview->showRow(row);
        }
        for(int row = 8 ; row <= 254 ; row++)
        {
            tableview->hideRow(row);
        }
    }

    int node = 1;
    int registerNodenum = 0;
    int danyuanCountnum = 0;

    model->setHeaderData(0, Qt::Horizontal, tr("地址号"));
    model->setHeaderData(1, Qt::Horizontal, tr("登记"));
    model->setHeaderData(2, Qt::Horizontal, tr("设备类型"));
    model->setHeaderData(3, Qt::Horizontal, tr("生产类型"));
    model->setHeaderData(4, Qt::Horizontal, tr("屏蔽"));
    model->setHeaderData(5, Qt::Horizontal, tr("报启延时"));
    model->setHeaderData(6, Qt::Horizontal, tr("脉冲延时"));
    model->setHeaderData(7, Qt::Horizontal, tr("重报探头"));
    model->setHeaderData(8, Qt::Horizontal, tr("所属楼显"));
    model->setHeaderData(9, Qt::Horizontal, tr("栋号"));
    model->setHeaderData(10, Qt::Horizontal, tr("层号"));
    model->setHeaderData(11, Qt::Horizontal, tr("区号"));
    model->setHeaderData(12, Qt::Horizontal, tr("房号"));
    model->setHeaderData(13, Qt::Horizontal, tr("报警级别"));
    model->setHeaderData(14, Qt::Horizontal, tr("位置描述"));

    spinDelegate *nodeId = new spinDelegate(0);
    tableview->setItemDelegateForColumn(0, nodeId);
    comboDelegate *regisFlag = new comboDelegate(1);
    tableview->setItemDelegateForColumn(1, regisFlag);
    comboDelegate *equipType = new comboDelegate(2);
    tableview->setItemDelegateForColumn(2, equipType);
    //    comboDelegate *proType = new comboDelegate(3);
    //    tableview->setItemDelegateForColumn(3, proType);
    comboDelegate *maskFlag = new comboDelegate(4);
    tableview->setItemDelegateForColumn(4, maskFlag);
    comboDelegate *reDetector = new comboDelegate(7);
    tableview->setItemDelegateForColumn(7, reDetector);

    spinDelegate *flrIndicator = new spinDelegate(8);
    tableview->setItemDelegateForColumn(8, flrIndicator);
    spinDelegate *buildingNumber = new spinDelegate(9);
    tableview->setItemDelegateForColumn(9, buildingNumber);
    spinDelegate *floorNumber = new spinDelegate(10);
    tableview->setItemDelegateForColumn(10, floorNumber);
    spinDelegate *zoneNumber = new spinDelegate(11);
    tableview->setItemDelegateForColumn(11, zoneNumber);
    spinDelegate *roomNumber = new spinDelegate(12);
    tableview->setItemDelegateForColumn(12, roomNumber);
    comboDelegate *alarmColumn = new comboDelegate(13);
    tableview->setItemDelegateForColumn(13, alarmColumn);
    if(loop == 0)
    {
        //        for(int row = 0; row <=11; row++)
        //        {
        //            for(int column = 0;column <=14;column++)
        //            {
        //                model->setData(model->index(row, column, QModelIndex()), QVariant(" "));
        //            }
        //        }
        tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);

    }
    else
    {
        QSqlDatabase::database().transaction();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg((host << 24) | (iobr << 16) | (loop << 8) | node)
                   .arg((host << 24) | (iobr << 16) | (loop << 8) | 0xFF));
        while (query.next())
        {
            model->setData(model->index(query.at(), 0, QModelIndex()), query.value(5).toInt() & 0xFF);
            model->setData(model->index(query.at(), 1, QModelIndex()), query.value(1).toBool() == true ? QVariant("是") : QVariant("否"));
            model->setData(model->index(query.at(), 2, QModelIndex()), Type::char_liandong((query.value(7).toInt() >> 16) & 0xFF));
            model->setData(model->index(query.at(), 3, QModelIndex()), Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF));
            model->setData(model->index(query.at(), 4, QModelIndex()), query.value(2).toBool() == true ? QVariant("√") : QVariant("×"));
            model->setData(model->index(query.at(), 5, QModelIndex()), query.value(8).toInt() & 0xFF);
            model->setData(model->index(query.at(), 6, QModelIndex()), (query.value(8).toInt() >> 8)  & 0xFF);
            model->setData(model->index(query.at(), 7, QModelIndex()), query.value(3).toBool() == true ? QVariant("是") : QVariant("否"));
            model->setData(model->index(query.at(), 8, QModelIndex()), query.value(7).toInt() & 0xFF);
            model->setData(model->index(query.at(), 9, QModelIndex()), (query.value(6).toInt() >> 24) & 0xFF);
            model->setData(model->index(query.at(), 10, QModelIndex()), (query.value(6).toInt() >> 16) & 0xFF);
            model->setData(model->index(query.at(), 11, QModelIndex()), (query.value(6).toInt() >> 8) & 0xFF);
            model->setData(model->index(query.at(), 12, QModelIndex()), query.value(6).toInt() & 0xFF);
            model->setData(model->index(query.at(), 13, QModelIndex()), query.value(4).toBool() == true ? QVariant("预警") : QVariant("火警"));
            model->setData(model->index(query.at(), 14, QModelIndex()), query.value(9).toString());
            if (query.value(1).toBool() == true)
            {
                registerNodenum++;
//                if (Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF) == danyuanComboBox->currentText())
//                {
//                    danyuanCountnum++;
//                }
                if(danyuanComboBox->currentText() == QString("所有类型"))
                {
                    danyuanCountnum = registerNodenum;
                }
                else if(Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF) == danyuanComboBox->currentText())
                {
                    danyuanCountnum++;
                 }



            }
        }
        registerNodenumLabel->setText(tr("%1").arg(registerNodenum));
        danyuanCountnumLabel->setText(tr("%1").arg(danyuanCountnum));
        QSqlDatabase::database().commit();
    }
}

#if 1
void UnitSetDialog::okToContinue()
{
    if (editEnableFlag)
    {
        if(dataChangeFlag == 0)
        {
            QMessageBox *box = new QMessageBox(tr("保存"),
                                               tr("设置已保存!"),
                                               QMessageBox::Information,
                                               QMessageBox::Ok,
                                               QMessageBox::NoButton,
                                               QMessageBox::NoButton,
                                               this);
            box->setAttribute(Qt::WA_DeleteOnClose);
            connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
            box->exec();
            //close();//单元设置保存后，不要自动退出设置界面
        }
        else
        {
            //            int r = QMessageBox::information(this, tr("Save"),
            //                                             tr("there is %1 changes in Board %2 \n save the changes?").arg(dataChangeFlag).arg(boardComboBox->currentText().toInt()),
            //                                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            //            if(r == QMessageBox::Yes)
            //            {
            //                //saveFile();
            //                dataChangeFlag = 0;
            //            }
            QMessageBox *box = new QMessageBox(tr("Save"),
                                               tr("there is %1 changes in Board %2 \n save the changes?")
                                               .arg(dataChangeFlag).arg(boardComboBox->currentText().toInt()),
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
                dataChangeFlag = 0;
            }
            break;
            case  QMessageBox::No:
            {
                //this->close();
            }
            break;
            }
        }
    }
    else
    {
        close();
    }
}

#endif
void UnitSetDialog::dataChangedSlot(const QModelIndex &index)
{
    if (!comboChangeFlag)
    {
        dataChangeFlag = 0;
        int host = 1;
        QModelIndex currentIndex[15];

        QTime time;
        //QModelIndex index = model->index(1,2,QModelIndex());
        //qDebug() << index.row() << index.column() << index.model()->data(index).toString();
        time.restart();
        /*There is a space behind SET,do not forget*/
        QSqlDatabase::database().transaction();
        for (int j = 0;j <= 14;j++)
        {
            currentIndex[j] = model->index(index.row(), j, QModelIndex());
        }

        int loop;
        if(boardComboBox->currentText().toInt() <= 32)
        {
            loop = loopComboBox->currentText().toInt();
        }
        else if (boardComboBox->currentText().toInt() == 33)
        {
            loop = floorshowloopComboBox->currentText().toInt();
        }
        else if ((boardComboBox->currentText().toInt() == 34)&&(typeComboBox->currentIndex() ==0))
        {
            loop = zhikongComboBox->currentText().toInt();
        }
        else
        {
            loop = 0;
        }

        if(loop == 0)
        {

        }
        else
        {
            QSqlQuery query;
            query.clear();
            //        query.prepare(QString("UPDATE unitset SET "
            //                              "regist=?, shield=?, restated=?, level=?,"
            //                              "geo_addr=?, type=?, delay=?, str=?"
            //                              "WHERE unitid = %1").arg((host<<24) | (boardComboBox->currentText().toInt() <<16) | (loopComboBox->currentText().toInt() <<8) | (index.row() + 1)));


            query.prepare(QString("UPDATE unitset SET "
                                  "regist=?, shield=?, restated=?, level=?,"
                                  "geo_addr=?, type=?, delay=?, str=?"
                                  "WHERE unitid = %1").arg((host<<24) | (boardComboBox->currentText().toInt() <<16) | (loop <<8) | (index.row() + 1)));

            query.addBindValue(currentIndex[1].model()->data(currentIndex[1]).toString() == QVariant("是")?true : false);
            query.addBindValue(currentIndex[4].model()->data(currentIndex[4]).toString() == QVariant("√")?true : false);
            query.addBindValue(currentIndex[7].model()->data(currentIndex[7]).toString() == QVariant("是")?true : false);
            query.addBindValue(currentIndex[13].model()->data(currentIndex[13]).toString() == QVariant("预警")?true : false);
            query.addBindValue((currentIndex[9].model()->data(currentIndex[9]).toInt() << 24)
                               | (currentIndex[10].model()->data(currentIndex[10]).toInt() << 16)
                               | (currentIndex[11].model()->data(currentIndex[11]).toInt() << 8)
                               | currentIndex[12].model()->data(currentIndex[12]).toInt());
            query.addBindValue((Type::int_liandongId(currentIndex[2].model()->data(currentIndex[2]).toString()) << 16)
                               | (Type::int_danyuanId(currentIndex[3].model()->data(currentIndex[3]).toString()) << 8)
                               | currentIndex[8].model()->data(currentIndex[8]).toInt());
            query.addBindValue((currentIndex[6].model()->data(currentIndex[6]).toInt() << 8)
                               | currentIndex[5].model()->data(currentIndex[5]).toInt());
            query.addBindValue(currentIndex[14].model()->data(currentIndex[14]).toString());

            bool bSuccess = query.exec();
            if (!bSuccess)
            {
                QSqlError lastError = query.lastError();
                qDebug() << lastError.driverText()
                         << QString(QObject::tr("update error"));
            }
            QSqlDatabase::database().commit();
            qDebug() << QObject::tr("update耗时：%1 ms").arg(time.elapsed());
            if(currentIndex[1].model()->data(currentIndex[1]).toString() == QVariant("是")?true : false)
            {
                if(currentIndex[4].model()->data(currentIndex[4]).toString() == QVariant("√")?true : false)
                {
                    //shiled the node,maybe add into the shieldhis
                    this->insertintoShieldHis(((host<<24) | (boardComboBox->currentText().toInt() <<16) | (loop <<8) | (index.row() + 1)),Type::int_danyuanId(currentIndex[3].model()->data(currentIndex[3]).toString()));


                }
                else
                {
                    //unshiled the node,maybe delete from the shieldhis
                    this->deletefromShieldHis(((host<<24) | (boardComboBox->currentText().toInt() <<16) | (loop <<8) | (index.row() + 1)));
                }
            }


        }
    }
}

void UnitSetDialog::boardreadUnitSet()
{

    int boardnum = boardComboBox->currentText().toInt();
    if(boardnum  <= 32)
    {
        groupsetPushButton->setVisible(true);
        loopLabel->setVisible(true);
        zhikongLabel->setVisible(false);
        zongxianLabel->setVisible(false);
        typeComboBox->setVisible(false);
        loopComboBox->setVisible(true);
        zongxianComboBox->setVisible(false);
        zhikongComboBox->setVisible(false);
        floorshowloopComboBox->setVisible(false);
        danyuanComboBox->setVisible(true);
        danyuanCountnumLabel->setVisible(true);
        countnumLabel->setVisible(true);
        readUnitSet();
    }
    else  if(boardnum  == 33)
    {
        groupsetPushButton->setVisible(false);
        loopLabel->setVisible(false);//loopLabel->setVisible(true);
        zhikongLabel->setVisible(false);
        zongxianLabel->setVisible(false);
        typeComboBox->setVisible(false);
        loopComboBox->setVisible(false);
        zongxianComboBox->setVisible(false);
        zhikongComboBox->setVisible(false);
        floorshowloopComboBox->setVisible(true);
        danyuanComboBox->setVisible(false);
        danyuanCountnumLabel->setVisible(false);
        countnumLabel->setVisible(false);
        readUnitSet();
    }
    else if((boardnum == 34) &&(zhikongComboBox->currentText().toInt() != 0))
    {
        groupsetPushButton->setVisible(false);
        loopLabel->setVisible(false);
        zhikongLabel->setVisible(true);
        zongxianLabel->setVisible(false);
        typeComboBox->setVisible(true);
        loopComboBox->setVisible(false);
        zongxianComboBox->setVisible(false);
        zhikongComboBox->setVisible(true);
        floorshowloopComboBox->setVisible(false);
        danyuanComboBox->setVisible(false);
        danyuanCountnumLabel->setVisible(false);
        countnumLabel->setVisible(false);
        typeComboBox->setCurrentIndex(0);
        readUnitSet();
    }
    else  if((boardnum == 34) &&(zhikongComboBox->currentText().toInt() == 0))
    {
        groupsetPushButton->setVisible(false);
        loopLabel->setVisible(false);
        zhikongLabel->setVisible(true);
        zongxianLabel->setVisible(false);
        typeComboBox->setVisible(true);
        loopComboBox->setVisible(false);
        zongxianComboBox->setVisible(false);
        zhikongComboBox->setVisible(true);
        floorshowloopComboBox->setVisible(false);
        danyuanComboBox->setVisible(false);
        danyuanCountnumLabel->setVisible(false);
        countnumLabel->setVisible(false);
        typeComboBox->setCurrentIndex(0);
        for(int row = 0; row < 8; row++)
        {
            for(int column = 0;column <=14;column++)
            {
                model->setData(model->index(row, column, QModelIndex()), QVariant("-"));
            }
        }
        for(int row = 8 ; row <= 254 ; row++)
        {
            tableview->hideRow(row);
        }
        tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

}

void UnitSetDialog::loopreadUnitSet()
{
    readUnitSet();
}
void UnitSetDialog::danyuanreadUnitSet()
{
    readUnitSet();
}
void UnitSetDialog::zhikongreadUnitSet()
{
    readUnitSet();
}

void UnitSetDialog::typeChange(int index)
{
    if(index == 0 && zhikongComboBox->currentText().toInt() != 0)
    {
        zhikongLabel->setVisible(true);
        zongxianLabel->setVisible(false);
        zongxianComboBox->setVisible(false);
        zhikongComboBox->setVisible(true);
        readUnitSet();
    }
    else if(index == 0 && zhikongComboBox->currentText().toInt() == 0)
    {
        zhikongLabel->setVisible(true);
        zongxianLabel->setVisible(false);
        zongxianComboBox->setVisible(false);
        zhikongComboBox->setVisible(true);
        for(int row = 0 ; row <= 7 ; row++)
        {
            tableview->showRow(row);
        }
        for(int row = 8 ; row <= 254 ; row++)
        {
            tableview->hideRow(row);
        }
        for(int row = 0; row <=7; row++)
        {
            for(int column = 0;column <=14;column++)
            {
                model->setData(model->index(row, column, QModelIndex()), QVariant(tr("-")));
            }
        }
        tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    else if((index == 1 && zongxianComboBox->currentText().toInt() != 0))
    {
        zhikongLabel->setVisible(false);
        zongxianLabel->setVisible(true);
        zongxianComboBox->setVisible(true);
        zhikongComboBox->setVisible(false);
        readUnitSet();

    }
    else if((index == 1 && zongxianComboBox->currentText().toInt() == 0))
    {
        zhikongLabel->setVisible(false);
        zongxianLabel->setVisible(true);
        zongxianComboBox->setVisible(true);
        zhikongComboBox->setVisible(false);
        for(int row = 0 ; row <= 63 ; row++)
        {
            tableview->showRow(row);
        }
        for(int row = 64 ; row <= 254 ; row++)
        {
            tableview->hideRow(row);
        }
        for(int row = 0; row <=63; row++)
        {
            for(int column = 0;column <=14;column++)
            {
                model->setData(model->index(row, column, QModelIndex()), QVariant("-"));
            }
        }
        tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }


}

void UnitSetDialog::readUnitSet()
{
    /*切换接口板和回路号以及单元类型时，此标志设置为true*/
    comboChangeFlag = true;
    int host = 1;
    int iobr = boardComboBox->currentText().toInt();
    //    int loop = loopComboBox->currentText().toInt();
    //add bu linwot ,if we only have the zhikong regist in when init the system ,the loop maybe different
    int loop = 0;
    if(iobr <= 32)
    {
        loop = loopComboBox->currentText().toInt();
        for(int row = 0 ; row <= 254 ; row++)
        {
            tableview->showRow(row);
        }
    }
    else if(iobr == 33)
    {
        loop = floorshowloopComboBox->currentText().toInt();
        for(int row = 0 ; row <= 254 ; row++)
        {
            tableview->showRow(row);
        }
    }
    else if ((iobr == 34)&&(typeComboBox->currentIndex() == 0))
    {
        loop = zhikongComboBox->currentText().toInt();
        for(int row = 0 ; row <= 7 ; row++)
        {
            tableview->showRow(row);
        }
        for(int row = 8 ; row <= 254 ; row++)
        {
            tableview->hideRow(row);
        }
    }
    else if ((iobr == 34)&&(typeComboBox->currentIndex() == 1))
    {
        loop = zongxianComboBox->currentText().toInt();
        for(int row = 0 ; row <= 63 ; row++)
        {
            tableview->showRow(row);
        }
        for(int row = 64 ; row <= 254 ; row++)
        {
            tableview->hideRow(row);
        }
    }

    int node = 1;
    int registerNodenum = 0;
    int danyuanCountnum = 0;
    //int querysize = 0;

    if ((iobr == 34)&&(typeComboBox->currentIndex() == 1))
    {
        QTime time;
        time.restart();
        for(int row = 0; row <=63; row++)
        {
            model->setData(model->index(row, 0, QModelIndex()), row+1);
            model->setData(model->index(row, 1, QModelIndex()), QVariant("是"));
            model->setData(model->index(row, 2, QModelIndex()), Type::char_liandong(0xFF));
            model->setData(model->index(row, 3, QModelIndex()), Type::char_danyuan(0xFF));
            model->setData(model->index(row, 4, QModelIndex()), QVariant("×"));
            model->setData(model->index(row, 5, QModelIndex()), QVariant(" "));
            model->setData(model->index(row, 6, QModelIndex()), QVariant(" "));
            model->setData(model->index(row, 7, QModelIndex()), QVariant("是"));
            model->setData(model->index(row, 8, QModelIndex()), QVariant(" "));
            model->setData(model->index(row, 9, QModelIndex()), QVariant(" "));
            model->setData(model->index(row, 10, QModelIndex()), QVariant(" "));
            model->setData(model->index(row, 11, QModelIndex()), QVariant(" "));
            model->setData(model->index(row, 12, QModelIndex()), QVariant(" "));
            model->setData(model->index(row, 13, QModelIndex()), QVariant(" "));
            model->setData(model->index(row, 14, QModelIndex()), QVariant(" "));
        }
        tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);

        registerNodenum = 64;
        registerNodenumLabel->setText(tr("%1").arg(registerNodenum));
        qDebug() << QObject::tr("select take time：%1 ms").arg(time.elapsed());
        /*切换接口板和回路号以及单元类型结束后，此值变为false*/
        comboChangeFlag = false;
        dataChangeFlag = 0;

    }
    else if((iobr == 34)&&(typeComboBox->currentIndex() == 0))
    {
        registerNodenum = 8;
        registerNodenumLabel->setText(tr("%1").arg(registerNodenum));
    }
    else
    {
        QTime time;
        time.restart();
        QSqlDatabase::database().transaction();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg((host << 24) | (iobr << 16) | (loop << 8) | node)
                   .arg((host << 24) | (iobr << 16) | (loop << 8) | 0xFF));

        while (query.next())
        {
            model->setData(model->index(query.at(), 0, QModelIndex()), query.value(5).toInt() & 0xFF);
            model->setData(model->index(query.at(), 1, QModelIndex()), query.value(1).toBool() == true ? QVariant("是") : QVariant("否"));
            model->setData(model->index(query.at(), 2, QModelIndex()), Type::char_liandong((query.value(7).toInt() >> 16) & 0xFF));
            model->setData(model->index(query.at(), 3, QModelIndex()), Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF));
            model->setData(model->index(query.at(), 4, QModelIndex()), query.value(2).toBool() == true ? QVariant("√") : QVariant("×"));
            model->setData(model->index(query.at(), 5, QModelIndex()), query.value(8).toInt() & 0xFF);
            model->setData(model->index(query.at(), 6, QModelIndex()), (query.value(8).toInt() >> 8) & 0xFF);
            model->setData(model->index(query.at(), 7, QModelIndex()), query.value(3).toBool() == true ? QVariant("是") : QVariant("否"));
            model->setData(model->index(query.at(), 8, QModelIndex()), query.value(7).toInt() & 0xFF);
            model->setData(model->index(query.at(), 9, QModelIndex()), (query.value(6).toInt() >> 24) & 0xFF);
            model->setData(model->index(query.at(), 10, QModelIndex()), (query.value(6).toInt() >> 16) & 0xFF);
            model->setData(model->index(query.at(), 11, QModelIndex()), (query.value(6).toInt() >> 8) & 0xFF);
            model->setData(model->index(query.at(), 12, QModelIndex()), query.value(6).toInt() & 0xFF);
            model->setData(model->index(query.at(), 13, QModelIndex()), query.value(4).toBool() == true ? QVariant("预警") : QVariant("火警"));
            model->setData(model->index(query.at(), 14, QModelIndex()), query.value(9).toString());
            if (query.value(1).toBool() == true)
            {
                registerNodenum++;
//                if (Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF) == danyuanComboBox->currentText())
//                {
//                    danyuanCountnum++;
//                }
                if(danyuanComboBox->currentText() == QString("所有类型"))
                {
                    danyuanCountnum = registerNodenum;
                }
                else if(Type::char_danyuan((query.value(7).toInt() >> 8) & 0xFF) == danyuanComboBox->currentText())
                {
                    danyuanCountnum++;
                 }
            }
        }

        if(editEnableFlag)
        {
            tableview->setEditTriggers(QAbstractItemView:: AllEditTriggers);
        }
        registerNodenumLabel->setText(tr("%1").arg(registerNodenum));
        danyuanCountnumLabel->setText(tr("%1").arg(danyuanCountnum));
        QSqlDatabase::database().commit();
        qDebug() << QObject::tr("select耗时：%1 ms").arg(time.elapsed());
        /*切换接口板和回路号以及单元类型结束后，此值变为false*/
        comboChangeFlag = false;
        dataChangeFlag = 0;
    }
}

void UnitSetDialog::groupsetPushButtonClick()
{
    GroupsetDialog groupsetwin(boardComboBox->currentText().toInt(), loopComboBox->currentText().toInt(), this);
    groupsetwin.exec();
    readUnitSet();

//    groupsetwin = new GroupsetDialog(boardComboBox->currentText().toInt(), loopComboBox->currentText().toInt(), this);
//    groupsetwin->exec();
}


void UnitSetDialog::insertintoShieldHis(int unitid, int type)
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
                     <<QString(QObject::tr("in UnitSetDialog : insert shieldhis error"));
        }
    }

        QSqlDatabase::database().commit();

}
void UnitSetDialog::deletefromShieldHis(int unitid)
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
                 <<QString(QObject::tr("in UnitSetDialog :delete shieldhis error"));
    }
    QSqlDatabase::database().commit();
    }

}
