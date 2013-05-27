#include <QtGui/QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
/*下面这个头文件很重要，用到这个类QVariant时要添加头文件*/
#include<QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtDebug>
#include "database.h"

Database::Database()
{
}

Database::~Database()
{
}

bool Database::createConnection(const QString &DatabaseName, const QString &DatabaseFileName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase(DatabaseName);
    /*if setDatabaseName is ":memory:",the database will be created in the memory
        So we set the databaseName is *.db,and the database will be created in the ROM
      */
    db.setDatabaseName(DatabaseFileName);
    if (!db.open())
    {
        QMessageBox::critical(0, QObject::tr("数据库错误"), QObject::tr("退出请按Cancel键"),
                              QMessageBox::Cancel, QMessageBox::NoButton);
        return false;
    }
    return true;
}

bool Database::initFireHis()
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    /*List all the tables in the database*/
    QStringList tableList = db.tables();
    /*if this table does not exist we will create*/
    if (!tableList.contains("firehis"))
    {
        bool success = query.exec("create table firehis (id int primary key, "
                                  "type int,"
                                  "infoid int,"
                                  "geo_addr int,"
                                  "time datetime,"
                                  "str varchar(48))");
        if (success)
        {
            qDebug() <<QObject::tr("firehis successfully created!") ;
            //writeFireHis();
        }
        else
        {
            qDebug() <<QObject::tr("firehis unsuccessfully created!");
            return false;
        }
    }
    return true;
}

void Database::writeFireHis()
{
    int host = 1;
    int iobr = 1;
    int loop = 3;
    int node = 16;
    int infoid = (host<<24) | (iobr<<16) | (loop<<8) | node;
    int geo_addr = (host<<24) | (iobr<<16) | (loop<<8) | node;
    QDateTime dt = QDateTime::currentDateTime();

    /*Before the opening of each transaction，adding this code to improve the speed*/
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO firehis(id, type, infoid, geo_addr, time, str)"
                  "VALUES (?, ?, ?, ?, ?, ?)");
    for (int i = 0;i<100; i++)
    {
        query.addBindValue(i);
        query.addBindValue(0x000B4000);

        query.addBindValue(infoid);
        query.addBindValue(geo_addr);
        query.addBindValue(dt);
        query.addBindValue(QString("华中科技大学南一楼%1室").arg(i));
        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("insert firehis error"));
        }
    }
    /*Close the transaction*/
    QSqlDatabase::database().commit();
}

bool Database::initHostInfo()
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    /*List all the tables in the database*/
    QStringList tableList = db.tables();
    /*if this table does not exist we will create*/
    if (!tableList.contains("hostset"))
    {
        bool success = query.exec("create table hostset (id int primary key, "
                                  "hostnum int,"
                                  "systemtype int,"
                                  "lockkeytime int,"
                                  "sound_alarm_state int,"
                                  "gasfirecontrolstate int)");
        if (success)
        {
            qDebug() <<QObject::tr("hostset successfully created!") ;
            writeHostInfo();
        }
        else
        {
            qDebug() <<QObject::tr("hostset unsuccessfully created!");
            return false;
        }
    }
    return true;
}

void Database::writeHostInfo()
{
    /*Before the opening of each transaction，adding this code to improve the speed*/
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO hostset(id,hostnum, systemtype, lockkeytime,sound_alarm_state,gasfirecontrolstate)"
                  "VALUES (?, ?, ?,?,?,?)");

    query.addBindValue(1);
    query.addBindValue(1);
    query.addBindValue(0);
    query.addBindValue(0);
    query.addBindValue(0);
    query.addBindValue(0);
    bool bSuccess = query.exec();
    if (!bSuccess)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("insert hostset error"));
    }
    /*Close the transaction*/
    QSqlDatabase::database().commit();
}


bool Database::initFaultHis()
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    /*List all the tables in the database*/
    QStringList tableList = db.tables();
    /*if this table does not exist we will create*/
    if (!tableList.contains("faulthis"))
    {
        bool success = query.exec("create table faulthis (id int primary key, "
                                  "type int,"
                                  "infoid int,"
                                  "geo_addr int,"
                                  "time datetime,"
                                  "str varchar(48))");
        if (success)
        {
            qDebug() <<QObject::tr("faulthis successfully created!") ;
            //writeFaultHis();
        }
        else
        {
            qDebug() <<QObject::tr("faulthis unsuccessfully created!");
            return false;
        }
    }
    return true;
}

bool Database::initFeedbackHis()
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    /*List all the tables in the database*/
    QStringList tableList = db.tables();
    /*if this table does not exist we will create*/
    if (!tableList.contains("linkhis"))
    {
        bool success = query.exec("create table linkhis (id int primary key, "
                                  "type int,"
                                  "infoid int,"
                                  "geo_addr int,"
                                  "time datetime,"
                                  "str varchar(48))");
        if (success)
        {
            qDebug() <<QObject::tr("linkhis successfully created!") ;
            //writeFeedbackHis();
        }
        else
        {
            qDebug() <<QObject::tr("linkhis unsuccessfully created!");
            return false;
        }
    }
    return true;
}


bool Database::initOperationHis()
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    /*List all the tables in the database*/
    QStringList tableList = db.tables();
    /*if this table does not exist we will create*/
    if (!tableList.contains("operationhis"))
    {
        bool success = query.exec("create table operationhis (id int primary key, "
                                  "time datetime,"
                                  "str varchar(48))");
        if (success)
        {
            qDebug() <<QObject::tr("operationhis successfully created!") ;
            //writeOperationHis();
        }
        else
        {
            qDebug() <<QObject::tr("operationhis unsuccessfully created!");
            return false;
        }
    }
    return true;
}

void Database::writeOperationHis()
{

    QDateTime dt = QDateTime::currentDateTime();

    /*Before the opening of each transaction，adding this code to improve the speed*/
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO operationhis(id, time, str)"
                  "VALUES (?, ?, ?)");
    for (int i = 0;i<100; i++)
    {
        query.addBindValue(i);

        query.addBindValue(dt);
        query.addBindValue(QString("操作%1").arg(i));
        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("insert operationhis error"));
        }
    }
    /*Close the transaction*/
    QSqlDatabase::database().commit();
}


bool Database::initShieldHis()
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    /*List all the tables in the database*/
    QStringList tableList = db.tables();
    /*if this table does not exist we will create*/
    if (!tableList.contains("shieldhis"))
    {
        bool success = query.exec("create table shieldhis (time datetime , "
                                  "unitid int primary key,"
                                  "type int)");
        if (success)
        {
            qDebug() <<QObject::tr("shieldhis successfully created!") ;
            writeShieldHis();
        }
        else
        {
            qDebug() <<QObject::tr("shieldhis unsuccessfully created!");
            return false;
        }
    }
    return true;
}

void Database::writeShieldHis()
{

    QSqlDatabase::database().transaction();
    QDateTime datetime = QDateTime::currentDateTime();
    QSqlQuery query;
    query.setForwardOnly(true);

    query.prepare("INSERT INTO shieldhis( time,unitid,type)"
                  "VALUES ( ?, ?,?)");

    query.addBindValue(datetime);
    query.addBindValue(0x01000000);
    query.addBindValue(0x00FFFF00);

    bool bSuccess = query.exec();
    if (!bSuccess)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("in datebase : insert soundandalarmdevice into the shieldhis error"));
    }

//    QDateTime dt = QDateTime::currentDateTime();

//    /*Before the opening of each transaction，adding this code to improve the speed*/
//    QSqlDatabase::database().transaction();
//    QSqlQuery query;
//    query.prepare("INSERT INTO shieldhis( time,unitid,type)"
//                  "VALUES ( ?, ?,?)");

//    int host = 1;
//    int iobr = 1;
//    int loop = 1;


//    for (int i = 0;i<100; i++)
//    {
//        query.addBindValue(dt);
//        query.addBindValue((host<<24) | (iobr<<16) | (loop<<8) | (i+1));
//        query.addBindValue(0x000000FF);
//        bool bSuccess = query.exec();
//        if (!bSuccess)
//        {
//            QSqlError lastError = query.lastError();
//            qDebug() << lastError.driverText()
//                     <<QString(QObject::tr("insert shieldhis error"));
//        }
//    }
    /*Close the transaction*/
    QSqlDatabase::database().commit();
}



void Database::writeFaultHis()
{
    int host = 1;
    int iobr = 1;
    int loop = 3;
    int node = 16;
    int infoid = (host<<24) | (iobr<<16) | (loop<<8) | node;
    int geo_addr = (host<<24) | (iobr<<16) | (loop<<8) | node;
    QDateTime dt = QDateTime::currentDateTime();

    /*Before the opening of each transaction，adding this code to improve the speed*/
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO firehis(id, type, infoid, geo_addr, time, str)"
                  "VALUES (?, ?, ?, ?, ?, ?)");
    for (int i = 0;i<100; i++)
    {
        query.addBindValue(i);
        query.addBindValue(0x000B4000);

        query.addBindValue(infoid);
        query.addBindValue(geo_addr);
        query.addBindValue(dt);
        query.addBindValue(QString("华中科技大学南一楼%1室").arg(i));
        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("insert firehis error"));
        }
    }
    /*Close the transaction*/
    QSqlDatabase::database().commit();
}

bool Database::initUnitSet()
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    /*delete the table of the db*/
    //query.exec("DROP TABLE unitset");
    /*List all the tables in the database*/
    QStringList tableList = db.tables();
    /*if this table does not exist we will create*/
    if (!tableList.contains("unitset"))
    {
        bool success = query.exec("create table unitset (id int, "
                                  "regist bool,"
                                  "shield bool,"
                                  "restated bool,"
                                  "level bool,"
                                  "unitid int primary key,"
                                  "geo_addr int,"
                                  "type int,"
                                  "delay int,"
                                  "str varcchar(44))");
        if (success)
        {
            qDebug() <<QObject::tr("unitset successfully created!") ;
            writeUnitSet();
        }
        else
        {
            qDebug() <<QObject::tr("unitset  created wrong !");
            return false;
        }
    }    
    return true;
}

void Database::writeUnitSet()
{
#if 0
    /*Before the opening of each transaction，adding this code to improve the speed*/
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO unitset(id, regist, shield, restated, level, unitid, geo_addr, type, delay, str)"
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    for (int iobr = 1;iobr <= 32;iobr++)
    {
        for (int loop = 1;loop <= 4;loop++)
        {
            for (int node = 1;node <= 255;node++)
            {
                //if((iobr-1)*4*255+(loop-1)*255+node != rowCount++)
                //qDebug() << QObject::tr("false");
                query.addBindValue((iobr-1)*4*255+(loop-1)*255+node);
                query.addBindValue(false);
                query.addBindValue(false);
                query.addBindValue(false);
                query.addBindValue(false);
                query.addBindValue((host<<24) | (iobr<<16) | (loop<<8) | node);
                query.addBindValue(0x00000000);
                query.addBindValue(0x00FFFF00);
                query.addBindValue(0x00000000);
                query.addBindValue(QString("华中科技大学南一楼%1室").arg(node));
                bool bSuccess = query.exec();
                if (!bSuccess)
                {
                    QSqlError lastError = query.lastError();
                    qDebug() << lastError.driverText()
                             <<QString(QObject::tr("insert unitset error"));
                }
            }
        }
    }
#endif

    int host = 1;
    //QDateTime dt = QDateTime::currentDateTime();

    //add by qingwu :2013-4-2
    /*增加节点号依次为： 01-00-00-000 的设备，
    作为本机非接口板设备的报警所用，比如 声光警报器，系统主电，系统备电 等。。*/
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.clear();
    query.prepare("INSERT INTO unitset(id, regist, shield, restated, level, unitid, geo_addr, type, delay, str)"
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(0);
    query.addBindValue(true);//registed
    query.addBindValue(false);//unshield
    query.addBindValue(false);
    query.addBindValue(false);
    query.addBindValue((host<<24) | 0);//1-0-0-0
    query.addBindValue(0x00000000);
    query.addBindValue(0x00FFFF00);
    query.addBindValue(0x00000000);
    query.addBindValue(QString("本机设备"));
    bool bSuccess = query.exec();
    if (!bSuccess)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("insert unitset 1-0-0-0 node error"));
    }

    //add by linwotian :为了33线路通信故障！为33接口板增加虚拟的回路号，并初始为1
    query.clear();
    query.prepare("INSERT INTO unitset(id, regist, shield, restated, level, unitid, geo_addr, type, delay, str)"
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    for (int iobr = 33,loop = 0,node = 0;iobr < 35; iobr++)//for board 33 and board 34,33-0-0-0,34-0-0-0
    {
        //if((iobr-1)*4*255+(loop-1)*255+node != rowCount++)
        //qDebug() << QObject::tr("false");
        query.addBindValue((iobr-1)*4*255);
        query.addBindValue(true);//initialized unregisted
        query.addBindValue(false);//initialized unshield
        query.addBindValue(false);
        query.addBindValue(false);
        query.addBindValue((host<<24) | (iobr<<16) | (loop<<8) | node);
        query.addBindValue(0x00000000);
        if(iobr == 33)
            query.addBindValue(0x00FF0000|(0x41<<8));//LOUXIAN 0x41
        else
            query.addBindValue(0x00FFFF00);
        query.addBindValue(0x00000000);
        if(iobr == 33)
            query.addBindValue(QString("1号主机33号接口板"));
        else
            query.addBindValue(QString("1号主机34号接口板"));
        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("insert louxian error"));
        }
    }


    //add by linwotian ,init the zhikong information
    query.clear();
    query.prepare("INSERT INTO unitset(id, regist, shield, restated, level, unitid, geo_addr, type, delay, str)"
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    for (int iobr = 34, loop = 1;loop <= 20;loop++)
    {
        for (int node = 0;node <= 8;node++)
        {
            //if((iobr-1)*4*255+(loop-1)*255+node != rowCount++)
            //qDebug() << QObject::tr("false");
            query.addBindValue((iobr-1)*4*255+(loop-1)*8+node);
            query.addBindValue(false);//initialized unregisted
            query.addBindValue(false);//initialized unshield
            query.addBindValue(false);
            query.addBindValue(false);
            query.addBindValue((host<<24) | (iobr<<16) | (loop<<8) | node);
            query.addBindValue(0x00000000);
            query.addBindValue(0x00FFFF00);
            query.addBindValue(0x00000000);
            query.addBindValue(QString("%1号直控盘%2号直控点").arg(loop).arg(node));
            bool bSuccess = query.exec();
            if (!bSuccess)
            {
                QSqlError lastError = query.lastError();
                qDebug() << lastError.driverText()
                         <<QString(QObject::tr("insert zhikong error"));
            }
        }
    }

    //add by qingwu :为了34线路通信故障！为34接口板增加接口板号 回路号不变 节点号为0的节点
    query.clear();
    query.prepare("INSERT INTO unitset(id, regist, shield, restated, level, unitid, geo_addr, type, delay, str)"
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    for (int iobr = 34,loop = 21;loop <= 84;loop++)
    {
        query.addBindValue((iobr-1)*4*255+(loop-1)*64 + 0);
        query.addBindValue(false);//initialized unregisted
        query.addBindValue(false);//initialized shield
        query.addBindValue(false);
        query.addBindValue(false);
        query.addBindValue((host<<24) | (iobr<<16) | (loop<<8) | 0);
        query.addBindValue(0x00000000);
        query.addBindValue(0x00FFFF00);
        query.addBindValue(0x00000000);
        query.addBindValue(QString("%1号总线盘%2号总线点").arg(iobr).arg(loop));
        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("insert 0 node error"));
        }
    }

    /*Close the transaction*/
    QSqlDatabase::database().commit();
}

bool Database::initBusSet()
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    /*delete the table of the db*/
    //query.exec("DROP TABLE busset");
    /*List all the tables in the database*/
    QStringList tableList = db.tables();
    /*if this table does not exist we will create*/
    if (!tableList.contains("busset"))
    {
        bool success = query.exec("create table busset (id int primary key, "
                                  "bus_num int,"
                                  "bus_node int,"
                                  "inMode1 int, inMode2 int, inMode3 int, inMode4 int, inMode5 int,"
                                  "inMode6 int, inMode7 int, inMode8 int, inMode9 int, inMode10 int,"
                                  "inMode11 int, inMode12 int, inMode13 int, inMode14 int, inMode15 int,"
                                  "outMode1 int, outMode2 int, outMode3 int, outMode4 int, outMode5 int,"
                                  "outMode6 int, outMode7 int, outMode8 int, outMode9 int, outMode10 int,"
                                  "outMode11 int, outMode12 int, outMode13 int, outMode14 int, outMode15 int)");
        if (success)
        {
            qDebug() <<QObject::tr("busset successfully created!") ;
            writeBusSet();
        }
        else
        {
            qDebug() <<QObject::tr("busset unsuccessfully created!");
            return false;
        }
    }
    return true;
}

void Database::writeBusSet()
{
    /*Before the opening of each transaction，adding this code to improve the speed*/
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO busset(id,"
                  "bus_num, bus_node,"
                  "inMode1, inMode2, inMode3, inMode4, inMode5,"
                  "inMode6, inMode7, inMode8, inMode9, inMode10,"
                  "inMode11, inMode12, inMode13, inMode14, inMode15,"
                  "outMode1, outMode2, outMode3, outMode4, outMode5,"
                  "outMode6, outMode7, outMode8, outMode9, outMode10,"
                  "outMode11, outMode12, outMode13, outMode14, outMode15)"
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    for (int bus_num = 1;bus_num <= 64;bus_num++)
    {
        for (int bus_node = 1;bus_node <= 64;bus_node++)
        {
            //            if((bus_num-1)*128 + bus_node != rowCount++)
            //                qDebug() << QObject::tr("false");
            query.bindValue(0, (bus_num-1)*64 + bus_node);
            query.bindValue(1, bus_num);
            query.bindValue(2, bus_node);
            for (int i = 0; i < 15; i++)
            {
                query.bindValue(i+3, 0);
            }
            for (int j = 0; j < 15; j++)
            {
                query.bindValue(j+18, 0);
            }
            bool bSuccess = query.exec();
            if (!bSuccess)
            {
                QSqlError lastError = query.lastError();
                qDebug() << lastError.driverText()
                         <<QString(QObject::tr("insert busset error"));
            }
        }
    }
}

bool Database::initLinkSet()
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    //query.exec("DROP TABLE linkset");
    /*List all the tables in the database*/
    QStringList tableList = db.tables();
    /*if this table does not exist we will create*/
    if (!tableList.contains("linkset"))
    {

        bool success = query.exec("create table linkset (linkid int primary key, "
                                  "linkshield int,"
                                  "linksetup int,"
                                  "linktype int,"
                                  "linknum int,"

                                  "linkINInfo1 LongLong,"
                                  "linkINInfo2 LongLong,"
                                  "linkINInfo3 LongLong,"
                                  "linkINInfo4 LongLong,"
                                  "linkINInfo5 LongLong,"
                                  "linkINInfo6 LongLong,"
                                  "linkINInfo7 LongLong,"
                                  "linkINInfo8 LongLong,"
                                  "linkINInfo9 LongLong,"
                                  "linkINInfo10 LongLong,"
                                  "linkINInfo11 LongLong,"
                                  "linkINInfo12 LongLong,"
                                  "linkINInfo13 LongLong,"
                                  "linkINInfo14 LongLong,"
                                  "linkINInfo15 LongLong,"
                                  "linkINInfo16 LongLong,"
                                  "linkINInfo17 LongLong,"
                                  "linkINInfo18 LongLong,"
                                  "linkINInfo19 LongLong,"
                                  "linkINInfo20 LongLong,"
                                  "linkINInfo21 LongLong,"
                                  "linkINInfo22 LongLong,"
                                  "linkINInfo23 LongLong,"
                                  "linkINInfo24 LongLong,"
                                  "linkINInfo25 LongLong,"
                                  "linkINInfo26 LongLong,"
                                  "linkINInfo27 LongLong,"
                                  "linkINInfo28 LongLong,"
                                  "linkINInfo29 LongLong,"
                                  "linkINInfo30 LongLong,"
                                  "linkINInfo31 LongLong,"

                                  "linkOUTInfo1 LongLong,"
                                  "linkOUTInfo2 LongLong,"
                                  "linkOUTInfo3 LongLong,"
                                  "linkOUTInfo4 LongLong,"
                                  "linkOUTInfo5 LongLong,"
                                  "linkOUTInfo6 LongLong,"
                                  "linkOUTInfo7 LongLong,"
                                  "linkOUTInfo8 LongLong,"
                                  "linkOUTInfo9 LongLong,"
                                  "linkOUTInfo10 LongLong,"
                                  "linkOUTInfo11 LongLong,"
                                  "linkOUTInfo12 LongLong,"
                                  "linkOUTInfo13 LongLong,"
                                  "linkOUTInfo14 LongLong,"
                                  "linkOUTInfo15 LongLong,"
                                  "linkOUTInfo16 LongLong,"
                                  "linkOUTInfo17 LongLong,"
                                  "linkOUTInfo18 LongLong,"
                                  "linkOUTInfo19 LongLong,"
                                  "linkOUTInfo20 LongLong,"
                                  "linkOUTInfo21 LongLong,"
                                  "linkOUTInfo22 LongLong,"
                                  "linkOUTInfo23 LongLong,"
                                  "linkOUTInfo24 LongLong,"
                                  "linkOUTInfo25 LongLong,"
                                  "linkOUTInfo26 LongLong,"
                                  "linkOUTInfo27 LongLong,"
                                  "linkOUTInfo28 LongLong,"
                                  "linkOUTInfo29 LongLong,"
                                  "linkOUTInfo30 LongLong,"
                                  "linkOUTInfo31 LongLong)");

        if (success)
        {
            qDebug() <<QObject::tr("linkset successfully created!") ;
            writeLinkSet();
        }
        else
        {
            qDebug() <<QObject::tr("linkset  unsuccessfully created!");
            return false;
        }
    }
    return true;
}

void Database::writeLinkSet()
{
    /*Before the opening of each transaction，adding this code to improve the speed*/
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO linkset(linkid, linkshield, linksetup, linktype, linknum, "
                  "linkINInfo1, linkINInfo2, linkINInfo3, linkINInfo4, linkINInfo5, linkINInfo6, linkINInfo7, linkINInfo8, linkINInfo9, linkINInfo10,"
                  "linkINInfo11, linkINInfo12, linkINInfo13, linkINInfo14, linkINInfo15, linkINInfo16, linkINInfo17, linkINInfo18, linkINInfo19,"
                  "linkINInfo20, linkINInfo21, linkINInfo22, linkINInfo23, linkINInfo24, linkINInfo25, linkINInfo26, linkINInfo27, linkINInfo28, "
                  "linkINInfo29, linkINInfo30, linkINInfo31, linkOUTInfo1, linkOUTInfo2, linkOUTInfo3, linkOUTInfo4, linkOUTInfo5, linkOUTInfo6,"
                  "linkOUTInfo7, linkOUTInfo8, linkOUTInfo9, linkOUTInfo10, linkOUTInfo11, linkOUTInfo12, linkOUTInfo13, linkOUTInfo14,"
                  "linkOUTInfo15, linkOUTInfo16, linkOUTInfo17, linkOUTInfo18, linkOUTInfo19, linkOUTInfo20, linkOUTInfo21, linkOUTInfo22, "
                  "linkOUTInfo23, linkOUTInfo24, linkOUTInfo25, linkOUTInfo26, linkOUTInfo27, linkOUTInfo28, linkOUTInfo29, linkOUTInfo30, linkOUTInfo31)"
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    for (int i = 0;i<30; i++)
    {
        long long int host = 0;
        long long int iobr = 0;
        int loop = 0;
        int dnode = 0;
        int dline = 0;

        long  long   int linkINInfo1 = (host<<32) | (iobr<<24) | (loop<<16) | (dnode<<8) | (dline);
        int znodestar = 0;
        int znodeend = 0;
        int zequiptype = 0;
        int zsetupnum = 0;

        long  long  int linkOUTInfo1 =  ((long  long  int)host<<48)
                | ((long  long  int)iobr<<40)
                | ((long  long  int)loop<<32)
                | (znodestar<<24)
                | (znodeend<<16)
                | (zequiptype<<8)
                | zsetupnum;

        query.bindValue(0,i+1);
        query.bindValue(1,0);
        query.bindValue(2,0);
        query.bindValue(3,4);
        query.bindValue(4,1);

        for (int j=0; j<31; j++)
        {
            query.bindValue(j+5,linkINInfo1);
        }
        for (int j=0; j<31; j++)
        {
            query.bindValue(j+36,linkOUTInfo1);
        }

        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("insert 联动error"));
        }
    }
    /*Close the transaction*/
    QSqlDatabase::database().commit();
}
bool Database::initIobrInfo()
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    /*List all the tables in the database*/
    QStringList tableList = db.tables();
    /*if this table does not exist we will create*/
    if (!tableList.contains("iobrinfo"))
    {
        bool success = query.exec("create table iobrinfo (id int primary key, "
                                  "iobrregist bool)");
        if (success)
        {
            qDebug() <<QObject::tr("iobrinfo successfully created!") ;
            writeIobrInfo();
        }
        else
        {
            qDebug() <<QObject::tr("iobrinfo unsuccessfully created!");
        }

        /*add by linwot: create zhikong and zongxian table in  database .*/
        if (!tableList.contains("zhikonginfo"))
        {
            bool success1 = query.exec("create table zhikonginfo (id int primary key, "
                                       "zhikongregist bool)");
            if (success1)
            {
                qDebug() <<QObject::tr("zhikonginfo successfully created!") ;
                writeZhikongInfo();
            }
            else
            {
                qDebug() <<QObject::tr("zhikonginfo unsuccessfully created!");
            }
        }

        if (!tableList.contains("zongxianinfo"))
        {
            bool success1 = query.exec("create table zongxianinfo (id int primary key, "
                                       "zongxianregist bool)");
            if (success1)
            {
                qDebug() <<QObject::tr("zongxianinfo successfully created!") ;
                writeZongxianInfo();
            }
            else
            {
                qDebug() <<QObject::tr("zongxianinfo unsuccessfully created!");
            }
        }
    }
    return true;
}
void Database::writeIobrInfo()
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO iobrinfo(id, iobrregist)"
                  "VALUES (?, ?)");
    for (int iobr = 1;iobr <= 34;iobr++)
    {
        query.addBindValue(iobr);
        query.addBindValue(false);

        //        if ( iobr == 1 )
        //        {
        //            query.addBindValue(true);
        //        }
        //        else
        //        {
        //            query.addBindValue(false);
        //        }

        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("iobrinfo insert error"));
        }
    }
    QSqlDatabase::database().commit();
}


void Database::writeZongxianInfo()
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO zongxianinfo(id, zongxianregist)"
                  "VALUES (?, ?)");
    for (int zongxian = 1;zongxian <= 64;zongxian++)
    {
        query.addBindValue(zongxian);        
        query.addBindValue(false);//none  regist when initialize..

        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("zongxianinfo insert error"));
        }
    }
    QSqlDatabase::database().commit();

}

void Database::writeZhikongInfo()
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO zhikonginfo(id, zhikongregist)"
                  "VALUES (?, ?)");
    for (int zhikong = 1;zhikong <= 20;zhikong++)
    {
        query.addBindValue(zhikong);
        query.addBindValue(false);

        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("zhikonginfo insert error"));
        }
    }
    QSqlDatabase::database().commit();

}



bool Database::initPasswordInfo()
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    /*List all the tables in the database*/
    QStringList tableList = db.tables();
    /*if this table does not exist we will create*/
    if (!tableList.contains("password"))
    {
        bool success = query.exec("create table password (id int primary key, "
                                  "passwordinfo varchar(20))");
        if (success)
        {
            qDebug() <<QObject::tr("password successfully created!") ;
            writePasswordInfo();
        }
        else
        {
            qDebug() <<QObject::tr("password unsuccessfully created!");
        }
    }
    return true;
}
void Database::writePasswordInfo()
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO password(id, passwordinfo)"
                  "VALUES (?, ?)");
    for (int i = 1;i <= 2;i++)
    {
        query.addBindValue(i);
        query.addBindValue("000000");
        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("password insert error"));
        }
    }
    QSqlDatabase::database().commit();
}
/*完全删除数据库表*/
bool Database::deletetable(const QString &tablename)
{
    /*To obtain the default object of QSqlDatabase*/
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    bool bSuccess = query.exec(QString("DROP TABLE %1").arg(tablename));
    if (bSuccess)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*清除数据库表内容*/
bool Database::cleartable(const QString &tablename)
{
    /*To obtain the default object of QSqlDatabase*/
    bool bSuccess = false;
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if (tablename == "linkset")
    {
        bSuccess= query.exec(QString("DELETE FROM linkset WHERE linkid>=0"));
    }
    else
    {
        bSuccess = query.exec(QString("DELETE FROM %1 WHERE id>=0").arg(tablename));
    }
    if (bSuccess)
    {
        return true;
    }
    else
    {
        return false;
    }
}


