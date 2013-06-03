#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QVariant>
#include <QtCore/QTimer>
#include <QtCore/QString>
#include <QDebug>
#include <fcntl.h>

#include "boardregthread.h"

boardRegThread::boardRegThread(QObject *parent) :
    QThread(parent)
{

    isSingleBoardRegFlag = false;
    singleBoardNum = 0;
    /*  200s count down for board register over */
    //isReRegisterFlag = false;
    timer200s = new QTimer(this);
    timer200s->setInterval(200000);//200s
    timer200s->setSingleShot(false);
    isTimeOut200s = false;
    connect(timer200s,SIGNAL(timeout()),this,SLOT(timeOutSlot1()));

    /* 3s count down for board ack */
    timer3s = new QTimer(this);
    timer3s->setInterval(3000);//3s
    timer3s->setSingleShot(false);
    isTimeOut3s = false;
    connect(timer3s,SIGNAL(timeout()),this,SLOT(timeOutSlot2()));

}

void boardRegThread::setSysResetFlag()
{
    //isReRegisterFlag = true;
}

void boardRegThread::stopBoardRegThread()
{
    if (timer200s->isActive())
    {
        timeOutSlot1();
    }
    if(timer3s->isActive())
    {
        timeOutSlot2();
        //timer3s->stop();
    }
    i = BOARDMAXINUM+1;//to exit the for loop in run()
    qDebug()<<"in board register thread: timer stopped!!!";
}

void boardRegThread::run()
{
    can_fd = open("/dev/can", O_RDWR);
    if(can_fd < 0)
    {
        qDebug()<<"in boardreg thread: can't open  can_dev ,can_fd = "<< can_fd <<endl;
        emit regFault();
        return;
    }
    else
        qDebug() <<"In boardRegThread:in boardreg thread: open  can_dev successfully!,can_fd = "<< can_fd <<endl;

    emit regStart();//start ,1 means nothing...forget it .just for the following board info show.
    //it is signal and slot.see regStartMessage()in boardRegDialog
    qDebug() << "board registing start..." << endl;

    if(isSingleBoardRegFlag)
    {
        signalBoardReg(singleBoardNum);
        updateBoardRegFlagToDB(singleBoardNum);
        close(can_fd);
        emit singleRegOver(singleBoardNum);
        qDebug() <<"In boardRegThread:emit singleRegOver signal--- board-flag is :"<< boardRegFlag[singleBoardNum -1];
        //after single board regover,isSingleBoardRegFlag=false
        isSingleBoardRegFlag = false;
    }
    else
    {
        i = 1;//boardnum ,use as global variable
        boardRegSum = 0;
        memset(&boardRegFlag,0,sizeof(boardRegFlag));

        sendBroadcast();

        /****   single board request registration information command frame */
        for( i = 1; i <= BOARDMAXINUM ; i++ )//i = 1; i <= BOARDMAXINUM ; i++
        {
            signalBoardReg(i);
        }

        //   if( !isReRegisterFlag )
        //   {
        saveBoardRegFlagToDB(boardRegFlag);
        close(can_fd);
        emit(regAllOver(boardRegSum));
        qDebug() <<"In boardRegThread:emit regAllOver signal-----total registerd board number is :"<< boardRegSum;

        //    }
        //    else
        //    {
        //        close(can_fd);
        //        qDebug()<<"IN boardRegThread::run():response system reset !";
        //    }
        //     qDebug()<<"IN boardRegThread::run(): run() !";
    }

}

void boardRegThread::signalBoardReg(int currentRegNum)
{
    int num,nread;
    fd_set rfds;
    struct timeval tv;

    /*****  send single board  register command frame to board i *******/
    memset (&nodeType, 0x0, sizeof(nodeType));
    memset (&loopRegNum, 0x0, sizeof(loopRegNum));
    memset (&louxianReg,0,sizeof(louxianReg));
    memset (&zhikongzongxianReg,0,sizeof(zhikongzongxianReg));
    //memset (&zhikongReg,0,sizeof(zhikongReg));
    //memset (&zongxianReg,0,sizeof(zongxianReg));
    memset (&sendFrame, 0x0, sizeof(struct can_frame));
    memset (&recvFrame, 0x0, sizeof(struct can_frame));
    isTimeOut3s = false;
    isTimeOut200s = false;

    sendSingle(currentRegNum);//send single frame to board
    emit currentRegBoardSgl(currentRegNum);//info for boardregDialog ,to tell people which board is registing...
    timer3s->start();
    qDebug()<<"3s-timer start, 1st do-while loop";
    do
    {
        if(isTimeOut3s)
        {
            /*no ack of current board in 3s, do something here ,then continue to  next board*/
            qDebug() << "by timeOut3s: no receive in 3 seconds,board "<< i <<" is offline"<<endl;
            break;
        }

        FD_ZERO(&rfds);//清空串口接收端口集
        FD_SET(can_fd, &rfds);//设置串口接收端口集
        //tv.tv_sec = 3;
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        num = select (can_fd + 1, &rfds, NULL, NULL, &tv);
        if (num == 0)//no receive in 3 seconds
        {
            qDebug() << "by select:no receive in 3 seconds,board "<< i <<" is offline"<<endl;
            isTimeOut3s = true;//no receive in 3s
            break;
        }
        else
        {
            nread = read(can_fd, &recvFrame, sizeof(struct can_frame));
            if(recvFrame.ti == TI_Ack && recvFrame.src_addr == sendFrame.dest_addr && recvFrame.priority == Priority4 )
            {
                timer3s->stop();//3s count down STOP !
                timer200s->start();//200s counting down
                qDebug() << "200s counting down...";
                qDebug() <<"In boardRegThread:In boardRegThread: receive ack from board "<<recvFrame.src_addr;
                isTimeOut3s = false;
            }
            else
            {
                qDebug() << "receive wrong ack....go to next do-while-1 loop...";
                continue;//go to next do-while loop
            }
        }
    }while((recvFrame.ti != TI_Ack) || (recvFrame.src_addr != sendFrame.dest_addr) || (recvFrame.priority != Priority4) );

    if(timer3s->isActive())
        timer3s->stop();

    if(isTimeOut3s)//no ack in 3s,go to next board
    {
        return;
        //continue;
    }
    qDebug()<<"2nd do-while loop";
    do
    {
        if(isTimeOut200s)
        {
            /**** board fault, no register over frame received in 200s ******/
            qDebug() <<"In boardRegThread:In boardRegThread:no registerOver frame received in 200 seconds,board fault"<<endl;
            break;
        }
        /***  deal with the info reported of board i  ***/
        FD_ZERO(&rfds);//清空串口接收端口集
        FD_SET(can_fd, &rfds);//设置串口接收端口集
        tv.tv_sec = 3;
        tv.tv_usec = 0;

        num = select (can_fd + 1, &rfds, NULL, NULL, &tv);
        if (num == 0)
        {
            qDebug() << "no receive in 3 seconds"<<endl;
            continue;//in 200s,every 3s if no info report,go to next do-while loop
        }
        nread = read(can_fd, &recvFrame, sizeof(struct can_frame));
        if(nread != sizeof(struct can_frame))
        {
            qDebug()<<"in boardregthread::run():read recvFrame error.";
            continue;
        }
        if( (recvFrame.ti == TI_RegisterInfReport) && (recvFrame.src_addr == sendFrame.dest_addr) && (recvFrame.priority == Priority5) )
        {
            dealInfo(recvFrame.src_addr);
            //qDebug() <<"In boardRegThread:In boardRegThread:receive info from board "<<recvFrame.src_addr;
        }
        else if((recvFrame.ti == TI_RegisterOver) && (recvFrame.src_addr == sendFrame.dest_addr) && (recvFrame.priority == Priority5) )
        {    /*  END of board i register, save info to file for test */
            /*  header of 8 bytes
                    @int boardstate; //4 byte
                    @unsigned char loopRegNum[4];//4 byte
                */
            qDebug() <<"In boardRegThread:In boardRegThread:receive overFrame from board "<<recvFrame.src_addr;
            timer200s->stop();
            qDebug() <<"In boardRegThread:In boardRegThread:200s-timer stop";
            sendAck();
            if(currentRegNum < LOUXIANNUM)
            {
                for(int j = 0; j<LOOPMAXINUM; j++)
                {
                    for(int k = 0; k<NODEMAXINUM; k++)
                        if (nodeType[j][k] != 0xFF && nodeType[j][k] != 0x00)
                            loopRegNum[j]+=1;
                }
                saveNodeToDB(currentRegNum, nodeType);//save to unitset table
            }
            else if( currentRegNum == LOUXIANNUM)
            {
                louxianRegNum = 0;
                for(int j = 0; j< LOUXIANMAXNUM; j++)
                    if(louxianReg[j] == LOUXIAN)
                        louxianRegNum += 1;
                saveLouxianToDB();
            }
            else if( currentRegNum == ZHIKONGZONGXIAN)
            {
                zhikongRegNum = 0;
                for(int j = 0; j< 20; j++)
                    if(zhikongzongxianReg[j] == ZHIKONGPAN)
                        zhikongRegNum += 1;
                for(int j = 21; j< ZHIKONGZONGXIANNUM ; j++)
                    if(zhikongzongxianReg[j] == ZONGXIANPAN)
                        zongxianRegNum += 1;
            }
            boardRegFlag[currentRegNum - 1] = TRUE;
            boardRegSum += 1;
        }
    }while(recvFrame.ti != TI_RegisterOver );//&& recvFrame.priority == Priority5 );//while((recvFrame.pri_ti & 0x1F) != TI_RegisterOver);

    if(recvFrame.ti != TI_RegisterOver)
    {
        boardRegFlag[currentRegNum - 1] = FALSE;
    }
    if(timer200s->isActive())
        timer200s->stop();
    if(timer3s->isActive())
        timer3s->stop();
}

void boardRegThread::singleBoardRegSlot(int num)
{

    isSingleBoardRegFlag = true;
    singleBoardNum = num;
    //this->start();


    //    can_fd = open("/dev/can", O_RDWR);
    //    if(can_fd < 0)
    //    {
    //        qDebug()<<"in boardreg thread: can't open  can_dev ,can_fd = "<< can_fd <<endl;
    //        emit regFault();
    //        return;
    //    }
    //    else
    //        qDebug() <<"In signalBoardRegSlot: open can_dev successfully!,can_fd = "<< can_fd <<endl;

    //    emit regStart();//start ,1 means nothing...forget it .just for the following board info show.
    //    qDebug() << "SINGLE board registing start..." << endl;

    //    signalBoardReg(num);
    //    updateBoardRegFlagToDB(num);
    //    close(can_fd);
    //    emit singleRegOver(num);
    //    qDebug() <<"In boardRegThread:emit regAllOver signal-----total registerd board number is :"<< boardRegSum;

}

void boardRegThread::updateBoardRegFlagToDB(int boardnum)
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.clear();

    query.prepare(QString("UPDATE iobrinfo SET "
                          "iobrregist=?"
                          "WHERE id = %1").arg(boardnum));
    qDebug()<<"UPDATE boardnum"<<boardnum<<"flag is "<<boardRegFlag[boardnum -1];
    query.addBindValue(boardRegFlag[boardnum - 1]);
    bool bSuccess = query.exec();
    if (!bSuccess)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText()
                 << QString(QObject::tr("saveBoardRegFlagToDB update error"));
    }
    QSqlDatabase::database().commit();
}

void boardRegThread::sendBroadcast()
{
    /******  send broadcast frame ************/
    memset (&sendFrame, 0x0, sizeof(struct can_frame));
    sendFrame.priority = Priority2;
    sendFrame.ti = TI_BroadcastRegister;
    sendFrame.src_addr = CONTROLLADDR;
    sendFrame.dest_addr = 0x00;
    sendFrame.can_dlc = 0x00;
    qDebug() <<"In boardRegThread:send broadcast frame ...";
    nwrite = write(can_fd, &sendFrame, sizeof(struct can_frame));
    //    qDebug() <<"In boardRegThread:nwrite is "<<nwrite;
    qDebug() <<"In boardRegThread:send broadcast frame over...";
}

void boardRegThread::sendAck()
{
    /* send ACK  ****/
    memset (&sendFrame, 0x0, sizeof(struct can_frame));
    sendFrame.priority = Priority4;
    sendFrame.ti = TI_Ack;
    sendFrame.src_addr = CONTROLLADDR;
    sendFrame.dest_addr = recvFrame.src_addr;
    sendFrame.can_dlc = 0x00;
    qDebug() <<"In boardRegThread: END of Register of current board...send ack to board "<<recvFrame.src_addr;
    nwrite = write(can_fd, &sendFrame, sizeof(struct can_frame));
    //qDebug() <<"In boardRegThread:nwrite is "<<nwrite;
    qDebug() <<"In boardRegThread: send ack to board "<<recvFrame.src_addr<<" over...";
}

void boardRegThread::sendSingle(int i)
{
    sendFrame.priority = Priority5;
    if(!isSingleBoardRegFlag)
        sendFrame.ti = TI_SignalBoardReqRegisterInf;
    else
        sendFrame.ti = TI_SignalBoardRegister;//   = 0x14
    sendFrame.src_addr = CONTROLLADDR;
    sendFrame.dest_addr = i;
    sendFrame.can_dlc = 0x00;
    qDebug() <<"In boardRegThread:send single frame to board "<<i;
    nwrite = write(can_fd, &sendFrame, sizeof(struct can_frame));
    if(nwrite != sizeof(struct can_frame))
    {
        qDebug() <<"In boardRegThread:nwrite wrong.return. ";
        return;
    }

    qDebug() <<"In boardRegThread:send single frame over...ti is"<<sendFrame.ti;

}

void boardRegThread::timeOutSlot1()
{
    isTimeOut200s = true;
}
void boardRegThread::timeOutSlot2()
{
    isTimeOut3s = true;
}


void  boardRegThread::dealInfo(int i)
{
    int loopid = 0, nodeid = 0;
    if(i < LOUXIANNUM)// board 1~32
    {
        /* current board num is i, loop num is recvFrame.data[0]*/
        loopid = recvFrame.data[0];        
        nodeid = recvFrame.data[1];
        if( (loopid > 4) || (nodeid-1 > LOUXIANMAXNUM) )
        {
            qDebug()<<"in boardRegThread::dealInfo():loopid="<<loopid<<"nodeid="<<nodeid<<",return here.";
            return;
        }
        memcpy( &nodeType[loopid-1][nodeid-1], &recvFrame.data[2], 5*sizeof(unsigned char));
    }
    else if( i == LOUXIANNUM)//louxian board 33
    {        
        nodeid = recvFrame.data[0];
        if( nodeid-1 > LOUXIANMAXNUM )
        {
            qDebug()<<"in boardRegThread::dealInfo():louxian--nodeid="<<nodeid<<",return here.";
            return;
        }
        memcpy( &louxianReg[nodeid-1], &recvFrame.data[1], 6*sizeof(unsigned char));
    }
    else if( i == ZHIKONGZONGXIAN)//zhikong board 34
    {
        nodeid = recvFrame.data[0];
        if( nodeid-1 > ZHIKONGZONGXIANNUM )
        {
            qDebug()<<"in boardRegThread::dealInfo():louxian--nodeid="<<nodeid<<",return here.";
            return;
        }
        memcpy( &zhikongzongxianReg[nodeid-1], &recvFrame.data[1], 6*sizeof(unsigned char));
    }
}

void boardRegThread::saveNodeToDB(int boardnum, unsigned char nodeType[][256])
{
    int host = 1, loop = 1,node = 0;
    QSqlDatabase::database().transaction();
    QSqlQuery query1;
    bool bSuccess = query1.exec(QString("DELETE FROM unitset WHERE unitid >= %1 AND unitid <= %2")
                                .arg((host << 24) | (boardnum << 16) | (0 << 8) | 0).arg((host << 24) | (boardnum << 16) | 0XFFFF));

    if (!bSuccess)
    {
        QSqlError lastError = query1.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("delete unitset error"));
    }

    //add by linwotian,if delete the device from the unitset,then delete the same device from the shieldhis
    QSqlQuery queryshield;
    bool bSuccess_shield = queryshield.exec(QString("DELETE FROM shieldhis WHERE unitid >= %1 AND unitid <= %2")
                                .arg((host << 24) | (boardnum << 16) | (0 << 8) | 0).arg((host << 24) | (boardnum << 16) | 0XFFFF));

    if (!bSuccess_shield)
    {
        QSqlError lastError = queryshield.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("delete shieldhis error"));
    }


    //add by qingwu :2013-3-30
    //增加接口板号从1到32，回路号节点号都为0的板级节点，为了后面处理接口板的通信故障。
    /*  1-1-0-0
        1-1-1-0
        1-1-2-0
        1-1-3-0
        1-1-4-0 */
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.clear();
    query.prepare("INSERT INTO unitset(id, regist, shield, restated, level, unitid, geo_addr, type, delay, str)"
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    for ( loop = 0;loop <= 4;loop++)
    {
        /*为每一个在线接口板增加接口板号 回路号不变 节点号为0的节点,node start from 0*/
        node = 0;
        query.addBindValue((boardnum-1)*4*255+(loop-1)*255+ node);
        query.addBindValue(true);//registed
        query.addBindValue(false);//unshield
        query.addBindValue(false);
        query.addBindValue(false);
        query.addBindValue((host<<24) | (boardnum<<16) | (loop<<8) | node);
        query.addBindValue(0x00000000);
        query.addBindValue(0x00FFFF00);
        query.addBindValue(0x00000000);
        if(loop != 0)
            query.addBindValue(QString("%1主机%2接口%3回路").arg(host).arg(boardnum).arg(loop));
        else
            query.addBindValue(QString("%1主机%2接口").arg(host).arg(boardnum));
        //change by qingwu:node info str only nodenum to 01主机01接口01回路001单元
        //            char hostnumChar[4],iobrnumChar[4], loopnumChar[4];
        //            sprintf(hostnumChar,"%02d",host);
        //            sprintf(iobrnumChar,"%02d",boardnum);
        //            sprintf(loopnumChar,"%01d",loop);
        //            //sprintf(nodenumChar,"%03d",node);

        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("insert unitset 0 node error"));
        }
    }


    /*Before the opening of each transaction，adding this code to improve the speed*/
    query.clear();
    query.prepare("INSERT INTO unitset(id, regist, shield, restated, level, unitid, geo_addr, type, delay, str)"
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    for ( loop = 1;loop <= 4;loop++)
    {
        for ( node = 1;node <= 255;node++)
        {
            query.addBindValue((boardnum-1)*4*255+(loop-1)*255+node);
            if (((int)nodeType[loop - 1][node - 1] == 0xFF) || ((int)nodeType[loop - 1][node - 1] == 0x00))
            {
                query.addBindValue(false);
            }
            else
            {
                query.addBindValue(true);
            }
            query.addBindValue(false);
            query.addBindValue(false);
            query.addBindValue(false);
            query.addBindValue((host<<24) | (boardnum<<16) | (loop<<8) | node);
            query.addBindValue(0x00000000);
            query.addBindValue(0x00FF0000 | ((int)nodeType[loop - 1][node - 1] << 8));
            query.addBindValue(0x00000000);

            //change by qingwu:node info str only nodenum to 01主机01接口01回路001单元
            char hostnumChar[4],iobrnumChar[4], loopnumChar[4], nodenumChar[4];
            sprintf(hostnumChar,"%02d",host);
            sprintf(iobrnumChar,"%02d",boardnum);
            sprintf(loopnumChar,"%01d",loop);
            sprintf(nodenumChar,"%03d",node);
            query.addBindValue(QString(tr("%1主机%2接口%3回路%4单元")
                                       .arg(host).arg(boardnum).arg(loop).arg(node)));
            //nodestr.sprintf("%02d主机%02d接口%02d回路%03d单元",host,boardnum,loop,node);
            //query.addBindValue(nodestr);
            bool bSuccess = query.exec();
            if (!bSuccess)
            {
                QSqlError lastError = query.lastError();
                qDebug() << lastError.driverText()
                         <<QString(QObject::tr("insert unitset error"));
            }
        }
    }
    /*Close the transaction*/
    QSqlDatabase::database().commit();
    qDebug()<<"saveNodeToDB():boardnum is "<<boardnum;
}
void boardRegThread::saveLouxianToDB()
{
    int host = 1,iobr = 33,loop = 1,node = 0;
    QSqlDatabase::database().transaction();

    QSqlQuery query1;
    bool bSuccess1 = query1.exec(QString("DELETE FROM unitset WHERE unitid >= %1 AND unitid <= %2")
                                 .arg((host<<24) | (iobr<<16) | (loop<<8) | 1)
                                 .arg((host<<24) | (iobr<<16) | (loop<<8) | 0xff));
    if (!bSuccess1)
    {
        QSqlError lastError = query1.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("delete unitset-louxian error"));
    }

    //add by linwotian,if delete the device from the unitset,then delete the same device from the shieldhis
    QSqlQuery queryshield;
    bool bSuccess_shield = queryshield.exec(QString("DELETE FROM shieldhis WHERE unitid >= %1 AND unitid <= %2")
                                            .arg((host<<24) | (iobr<<16) | (loop<<8) | 1)
                                            .arg((host<<24) | (iobr<<16) | (loop<<8) | 0xff));

    if (!bSuccess_shield)
    {
        QSqlError lastError = queryshield.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("delete shieldhis error"));
    }

    QSqlQuery query;
    query.clear();
    query.prepare("INSERT INTO unitset(id, regist, shield, restated, level, unitid, geo_addr, type, delay, str)"
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    for (node = 1;node <= 255;node++)
    {
        query.addBindValue((iobr-1)*4*255+(loop-1)*8+node);
        (louxianReg[node-1]==LOUXIAN)?query.addBindValue(true):query.addBindValue(false);
        //(node == 0) ? query.addBindValue(true) : ((louxianReg[node-1]==LOUXIAN) ? query.addBindValue(true) : query.addBindValue(false));
        query.addBindValue(false);//initialized unshield
        query.addBindValue(false);
        query.addBindValue(false);
        query.addBindValue((host<<24) | (iobr<<16) | (loop<<8) | node);
        query.addBindValue(0x00000000);
        query.addBindValue(0x00FF0000|((int)(LOUXIAN)<<8));
        query.addBindValue(0x00000000);
        query.addBindValue(QString("%1号楼显").arg(node));

        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("update louxian error"));
        }
    }
}

void boardRegThread::saveBoardRegFlagToDB(bool boardRegFlag[BOARDMAXINUM])
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    for (int iobr = 1;iobr <= BOARDMAXINUM ;iobr++)
    {
        query.clear();
        query.prepare(QString("UPDATE iobrinfo SET "
                              "iobrregist=?"
                              "WHERE id = %1").arg(iobr));
        //qDebug()<<"boardRegFlag["<<(iobr - 1)<<"] is :"<<boardRegFlag[iobr - 1];
        query.addBindValue(boardRegFlag[iobr - 1]);
        bool bSuccess = query.exec();
        if (!bSuccess)
        {
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText()
                     << QString(QObject::tr("saveBoardRegFlagToDB update error"));
        }
    }
    QSqlDatabase::database().commit();

    /* add by qingwu ,if zongxianzhikong board (34) is online,save the info to database... */
    if(boardRegFlag[BOARDMAXINUM - 1] == TRUE)//zongxianzhikong is online
    {
        QSqlDatabase::database().transaction();
        QSqlQuery query;
        query.setForwardOnly(true);
        for (int zongxnum = 1; zongxnum <= 64 ;zongxnum++)
        {
            query.clear();
            query.prepare(QString("UPDATE zongxianinfo SET "
                                  "zongxianregist=?"
                                  "WHERE id = %1").arg(zongxnum));
            query.addBindValue((zhikongzongxianReg[zongxnum - 1 + 20]==ZONGXIANPAN)?true:false);
            bool bSuccess = query.exec();
            if (!bSuccess)
            {
                QSqlError lastError = query.lastError();
                qDebug() << lastError.driverText()
                         << QString(QObject::tr("save zongxianRegFlagToDB update error"));
            }

            /*add by qingwu:update zongxianpan in unit */
            query.clear();
            query.prepare(QString("UPDATE unitset SET "
                                  "regist=?"
                                  "WHERE id = %1").arg((ZHIKONGZONGXIAN-1)*4*255+(zongxnum-1+20)*64 + 0));
            query.addBindValue((zhikongzongxianReg[zongxnum - 1 + 20]==ZONGXIANPAN)?true:false);
            bSuccess = query.exec();
            if (!bSuccess)
            {
                QSqlError lastError = query.lastError();
                qDebug() << lastError.driverText()
                         << QString(QObject::tr("update zongxianpan in unit error"));
            }
        }

        for (int zhikongnum = 1; zhikongnum <= 20 ;zhikongnum++)
        {
            query.clear();
            query.prepare(QString("UPDATE zhikonginfo SET "
                                  "zhikongregist=?"
                                  "WHERE id = %1").arg(zhikongnum));
            query.addBindValue((zhikongzongxianReg[zhikongnum - 1]==ZHIKONGPAN)?true:false);
            bool bSuccess = query.exec();
            if (!bSuccess)
            {
                QSqlError lastError = query.lastError();
                qDebug() << lastError.driverText()
                         << QString(QObject::tr("save zhikongRegFlagToDB update error"));
            }

            /*add by qingwu:update zhikongpan in unit */
            query.clear();
            query.prepare(QString("UPDATE unitset SET "
                                  "regist=?"
                                  "WHERE id = %1").arg((ZHIKONGZONGXIAN-1)*4*255+(zhikongnum-1)*8 + 0));
            query.addBindValue((zhikongzongxianReg[zhikongnum - 1 + 20]==ZHIKONGPAN)?true:false);
            bSuccess = query.exec();
            if (!bSuccess)
            {
                QSqlError lastError = query.lastError();
                qDebug() << lastError.driverText()
                         << QString(QObject::tr("update zhikongpan in unit error"));
            }
        }

        QSqlDatabase::database().commit();
    }

}

boardRegThread::~boardRegThread()
{
    delete(timer200s);
    delete(timer3s);
    deleteLater();
}
