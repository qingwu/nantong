//for QSqlite
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QVariant>
#include <QCoreApplication>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QTime>
#include <QVector>
#include <QDateTime>

#include "pollingthread.h"


pollingThread::pollingThread(QObject *parent)
    : QThread(parent)
{
    hasLinkToSend = false;

    memset(counter,0,sizeof(counter));
    memset(countMax,0,sizeof(countMax));
    timer100ms = new QTimer(this);
    timer100ms->setInterval(100);//100ms to test
    timer20ms = new QTimer(this);
    timer20ms->setInterval(20);//20ms

    connect(timer100ms,SIGNAL(timeout()),this,SLOT(timer100msOutSlot()));
    connect(timer20ms,SIGNAL(timeout()),this,SLOT(timer20msOutSlot()));
}
void pollingThread::run()
{
    qDebug()<<"pollingThread___currentThreadId______:"<<QThread::currentThreadId();
    pollingSendTime = 0;
    currentLinkSendTime = 0;
    currentBusledSendTime = 0;
    currentLouxianSendTime = 0;
    isResetFlag = 0;//it is very mportant to start  polling !!!
    boardnum = 0;// variable for  Class pollingThread

    can_fd = open("/dev/can", O_RDWR);
    if(can_fd < 0)
    {
        qDebug()<<"in polling thread: can't open  can_dev ,can_fd = "<< can_fd <<endl;
        return;
    }
    else
        qDebug()<<"in polling thread: open  can_dev successfully!"<<endl;

    for(int sendi = 0; sendi < 3; sendi++ )
    {
        sendStartWork();
    }

    qDebug()<<"in pollingThread: send 3 times startwork frame...";
    //sleep(3);
    QTime t;
    t.start();
    while(t.elapsed()<3000)//3s
        QCoreApplication::processEvents();

    getBoardState();
    if(boardRegNum.isEmpty())
    {
        //if there is no board registed,dont start polling. return.
        qDebug()<<"in pollingThread:no board register.return. ";
        return;
    }

    qDebug()<<"after getBoardState ";
    for(int i = 0; i<boardRegNum.count(); i++)
    {
        qDebug()<<"boardRegNum "<<i<<" is "<<boardRegNum.at(i);
    }

    timer100ms->start();
    qDebug()<<"in pollingThread::run():100mstimer start ";
    timer20ms->start();
    qDebug()<<"in pollingThread::run():20mstimer start ";

    readCanFrame();
}

/* get board state ----- online or offline -----from iobrinfo table in database */
void pollingThread::getBoardState()
{
    boardRegNum.clear();
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from iobrinfo"));

    while (query.next())
    {
        if(query.value(1).toBool())
        {
            boardRegNum.append(query.value(0).toInt());  //query.value(0).toInt()
            counter[query.value(0).toInt() - 1] = 0;
        }
        else
            counter[query.value(0).toInt() - 1] = OFFLINE;

        //qDebug()<<"counter"<< query.value(0).toInt() - 1<<"is:" <<counter[query.value(0).toInt() - 1];
    }
    QSqlDatabase::database().commit();

}
/*
    火灾控制器的巡检流程。控制器向接口板发巡检命令，接口板回复正常或者火警等信息。以下是注意事项。
    1，每100ms巡检一个接口板，即发送巡检帧给当前接口板。若接口板正常，则上报正常信息。如果100ms内
        没有收到回复，再发送一次巡检命令给当前接口板。若一次巡检过程中连续两次发巡检命令都没有收到
        接口板回复的任何信息，则巡检下一接口板。等下一轮巡检到该接口板时，也是一样的处理。如果发送
        2*3次巡检命令之后，仍然没有上报信息，那么控制器报这个接口板通讯故障。以后每次巡检，只对这个
        接口板发送一次巡检命令。如果任何一次收到了接口板上报的任何信息，就故障恢复。
    2，控制器发巡检命令，如果上报的是火警等信息，那么控制器每收到一帧info信息，就要给接口板回复确认，
        即：巡检-上报信息-回复确认，直到接收到接口板上报的正常信息为止，进入下一接口板的巡检。
    3，接口板总数目：BOARDMAXINUM
         当前处理的接口板号：boardRegNum.at(boardnum) ，boardnum从0开始
         发送及接收的can帧数据结构：struct can_frame，can_frame.ti == TI_NormalAck 表示接口板上报的是正常信息。
         给当前接口板发送巡检命令函数： sendPolling()
         发送确认信息：sendAck()
    */
void pollingThread::stopPolling()
{
    if(timer100ms->isActive())
        timer100ms->stop();
    qDebug()<<"in polling thread: timer100ms stopped!!! Polling Stopped!";

    if(timer20ms->isActive())
        timer20ms->stop();
    qDebug()<<"in polling thread: timer20ms stopped!!! Polling Stopped!";
    isResetFlag = 1;
    close(can_fd);//3.21
}

void pollingThread::timer100msOutSlot()
{
    sendPolling();
    //if(linkComeSgnl)//is there any link frame to send
    //qDebug()<<"pollingThread___timer100msOutSlot______:"<<QThread::currentThreadId();

//    /* we just polling online board,the online-board num is stored in QVector<int>boardRegNum */
//    if(counter[boardRegNum.at(boardnum) - 1] >= 6)//if board has  communication fault
//    {
//        sendPolling();
//        //pollingSendTime += 1;
//        //qDebug()<<"counter["<<boardRegNum.at(boardnum) - 1<<"] = "<<counter[boardRegNum.at(boardnum) - 1] <<" >= 6,sendpolling to board : "<<sendFrame.dest_addr;
//    }
//    else if ( counter[boardRegNum.at(boardnum) - 1] < 6 )
//    {
//        sendPolling();//send polling frame to board boardnum
//        counter[boardRegNum.at(boardnum) - 1] += 1;
//        if(counter[boardRegNum.at(boardnum) - 1] > countMax[boardRegNum.at(boardnum) - 1] )
//            countMax[boardRegNum.at(boardnum) - 1] = counter[boardRegNum.at(boardnum) - 1];

//        // qDebug()<<"no receive in 100ms, board "<< boardRegNum.at(boardnum) <<" maybe offline..."<<"counter["<<boardRegNum.at(boardnum) - 1<<"] = "<<counter[boardRegNum.at(boardnum) - 1];
//        //pollingSendTime += 1;
//        //qDebug()<<"counter["<<boardRegNum.at(boardnum) - 1<<"] = "<<counter[boardRegNum.at(boardnum) - 1] <<" < 6,sendpolling to board : "<<sendFrame.dest_addr;

//    }
//    return;
}

void pollingThread::nodeEnqueueSlot(char flag,nodeID tempnode)
{
    //qDebug()<<"in nodeEnqueueSlot.";
    switch(flag)
    {
    case LINK_NODE_ENQUEUE:
        linkQueue.enqueue(tempnode);
        //qDebug()<<"in nodeEnqueueSlot():link node"<< "enqueue";
//        qDebug()<<"in nodeEnqueueSlot():link node"
//                << tempnode.getiobrnum()
//                <<tempnode.getloopnum()
//                <<tempnode.getnodenum()
//                << tempnode.getactiontype()
//                <<tempnode.getledtype()
//                <<"enqueue";
        break;
    case BUS_NODE_ENQUEUE:
        busledQueue.enqueue(tempnode);
//        qDebug()<<"in nodeEnqueueSlot():bus node"
//                << tempnode.getiobrnum()
//                <<tempnode.getloopnum()
//                <<tempnode.getnodenum()
//                << tempnode.getactiontype()
//                <<tempnode.getledtype()
//                <<"enqueue";
        break;
    case LOUXIAN_NODE_ENQUEUE:
        louxianQueue.enqueue(tempnode);
        //qDebug()<<"in nodeEnqueueSlot():louxian node enqueue";
        break;
    default:
        break;
    }
}

void pollingThread::timer20msOutSlot()
{
    if(!linkQueue.isEmpty())
    {
        if( currentLinkSendTime < 5 )
        {
            currentLinkSendTime += 1;
            sendLink();
        }
        else
        {
            qDebug() <<"in pollingThread::timer20msOutSlot():sendLinkTime>5,current node dequeue.";
            linkQueue.dequeue();
            currentLinkSendTime = 0;
            if(!linkQueue.isEmpty())
            {
                currentLinkSendTime += 1;
                sendLink();
            }
            else
                qDebug() <<"in pollingThread::timer20msOutSlot():linkQueue is Empty";
        }
    }


    if(!busledQueue.isEmpty())
    {
        if( currentBusledSendTime < 5 )
        {
            currentBusledSendTime += 1;
            sendBusLed();
        }
        else
        {
            qDebug() <<"in pollingThread::timer20msOutSlot():BusledSendTime>5,current node dequeue.";
            busledQueue.dequeue();
            currentBusledSendTime = 0;
            if(!busledQueue.isEmpty())
            {
                currentBusledSendTime += 1;
                sendBusLed();
            }
            else
                qDebug() <<"in pollingThread::timer20msOutSlot():busledQueue is Empty";
        }
    }

    if(!louxianQueue.isEmpty())
    {
        if( currentLouxianSendTime < 5 )
        {
            currentLouxianSendTime += 1;
            sendDisplay();
        }
        else
        {
            qDebug() <<"in pollingThread::timer20msOutSlot():sendLinkTime>5,current node dequeue.";
            louxianQueue.dequeue();
            currentLouxianSendTime = 0;
            if(!louxianQueue.isEmpty())
            {
                currentLouxianSendTime += 1;
                sendDisplay();
            }
            else
                qDebug() <<"in pollingThread::timer20msOutSlot():linkQueue is Empty";
        }
    }

}


void pollingThread::readCanFrame()
{
    //qDebug()<<"pollingThread___readCanFrame______:"<<QThread::currentThreadId();
    while(!isResetFlag)
    {
        FD_ZERO(&rfds);
        FD_SET(can_fd, &rfds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000;//20000;
        int num = 0,nread = 0;
        memset (&recvFrame, 0x0, sizeof(struct can_frame));
        num = select(can_fd + 1, &rfds, NULL, NULL, &tv);
        if( num == 0)
        {
            if (counter[boardRegNum.at(boardnum) - 1] < 6)
            {
                if(counter[boardRegNum.at(boardnum) - 1] %2 != 0 )
                {
                    //qDebug()<<"-------num = 0 ,boardnum ="<<boardRegNum.at(boardnum)<<QDateTime::currentDateTime().toString("mm:ss:zzz");;
                    continue;//go on polling the same board
                }
                else
                {
                    //go to polling the next board
                    if(boardnum < ( boardRegNum.count() - 1) )
                        boardnum += 1;
                    else
                        boardnum = 0;
                    //qDebug()<<"------num = 0000 ,boardnum ="<<boardRegNum.at(boardnum)<<QDateTime::currentDateTime().toString("mm:ss:zzz");
                    continue;
                }
            }
            else
            {
                if(boardnum < ( boardRegNum.count() - 1) )
                    boardnum += 1;
                else
                    boardnum = 0;
                //qDebug()<<"--------counter[boardRegNum.at(boardnum) - 1] >= 6 ,boardnum ="<<boardnum;
                continue;
            }
        }

        nread = read(can_fd, &recvFrame, sizeof(struct can_frame));
        if(nread != sizeof(struct can_frame))
        {
            qDebug()<<"in pollingThread::readCanFrame():nread != sizeof(struct can_frame).";
            continue;
        }

        //如果接口板没有登记，那么该接口板上报的信息应该忽略。
        if( !boardRegNum.contains(recvFrame.src_addr) )
        {
            continue;
//            if(counter[boardRegNum.at(boardnum) - 1] %2 != 0 )
//            {
//                sendPolling();
//                continue;//go on polling the same board
//            }
//            else
//            {
//                //go to polling the next board
//                if(boardnum < ( boardRegNum.count() - 1) )
//                    boardnum += 1;
//                else
//                    boardnum = 0;
//                sendPolling();
//                continue;
//            }
        }

        if (counter[recvFrame.src_addr - 1] >= 6)
        {
            QDateTime datetime = QDateTime::currentDateTime();
            emit eventComes(TI_BreakDownRecover, recvFrame.src_addr, 0, 0, 0, 0x0f, datetime);//0x0f is:板通信故障
//            qDebug()<<"ACTIVE BREAK_DOWN_Recover from board: "<< recvFrame.src_addr
//                    <<"-"<<0
//                    <<"-"<<0
//                    <<"-"<<0
//                    <<"-"<< 0x0f;
        }
        counter[recvFrame.src_addr - 1] = 0;

        //收到的信息有哪几种类型，分别处理
        if((recvFrame.ti == TI_NormalAck) && (recvFrame.priority == Priority4))
        {
            if ( (recvFrame.src_addr != boardRegNum.at(boardnum)) /*&& (counter[boardRegNum.at(boardnum) - 1] < 6)*/ )
            {
                continue;
//                if(counter[boardRegNum.at(boardnum) - 1] %2 != 0 )
//                {
//                    sendPolling();
//                    continue;//go on polling the same board
//                }
//                else
//                {
//                    //go to polling the next board
//                    if(boardnum < ( boardRegNum.count() - 1) )
//                        boardnum += 1;
//                    else
//                        boardnum = 0;
//                    sendPolling();
//                    continue;
//                }
            }
            else
            {
                if(boardnum < ( boardRegNum.count() - 1) )
                    boardnum += 1;
                else
                    boardnum = 0;
                //qDebug()<<"------boardnum+="<<boardRegNum.at(boardnum)<<QDateTime::currentDateTime().toString("mm:ss:zzz");;
                continue;
            }
//            /*receive normal info,go to  next online-board */
//            qDebug()<<"receive normal ack from board..."<<recvFrame.src_addr;
//            //pollingSendTime = 0;
//            if(boardnum < ( boardRegNum.count() - 1))
//                boardnum += 1;
//            else
//                boardnum = 0;
//            continue;

        }
        else if( (recvFrame.ti == TI_Ack) && (recvFrame.priority == Priority4) )
        {
            // recvFrame.data[0]----0x01：联动确认 0x02：点灯确认
            if(!linkQueue.isEmpty() )
            {
                nodeID linktempnode = linkQueue.head();
                if( recvFrame.src_addr == linktempnode.getiobrnum())
                {
                    //qDebug()<<"receive linkAck frame from board "<<recvFrame.src_addr;
                    if(currentLinkSendTime > 0)
                    {
                        linkQueue.dequeue();
                        currentLinkSendTime = 0;
                    }
                    continue;
                }
            }
            if(!busledQueue.isEmpty())
            {
                nodeID busledtempnode = busledQueue.head();
                if (recvFrame.src_addr == busledtempnode.getiobrnum())
                {
                    //qDebug()<<"receive busLedAck frame from board "<<recvFrame.src_addr;
                    if(currentBusledSendTime > 0)
                    {
                        busledQueue.dequeue();
                        currentBusledSendTime = 0;
                    }
                    continue;
                }
            }

            if(!louxianQueue.isEmpty())
            {
                nodeID busledtempnode = louxianQueue.head();
                if (recvFrame.src_addr == busledtempnode.getiobrnum())
                {
                    //qDebug()<<"receive busLedAck frame from board "<<recvFrame.src_addr;
                    if(currentLinkSendTime > 0)
                    {
                        louxianQueue.dequeue();
                        currentLouxianSendTime = 0;
                    }
                    continue;
                }
            }
        }
        else
        {
            qDebug()<<"__________________________receive info from board"<<recvFrame.src_addr;
            sendAck();
            dealInfoFrame();
//            if( recvFrame.src_addr != boardRegNum.at(boardnum) )
//            {
//                if(boardnum < ( boardRegNum.count() - 1) )
//                    boardnum += 1;
//                else
//                    boardnum = 0;
//            }
            continue;
        }
    }
}


void pollingThread::linkFlagSetSlot()
{
    hasLinkToSend = true;
}

void pollingThread::sendPolling()
{
   // qDebug()<<"pollingThread___sendPolling______:"<<QThread::currentThreadId();
    memset (&sendPollingFrame, 0x0, sizeof(struct can_frame));
    sendPollingFrame.priority = Priority3;
    sendPollingFrame.ti = TI_Inspection;
    sendPollingFrame.src_addr = CONTROLLADDR;
    sendPollingFrame.dest_addr = boardRegNum.at(boardnum);
    sendPollingFrame.can_dlc = 0x00;

    //qDebug()<<"sendpolling to board : "<<sendPollingFrame.dest_addr;
    int nwrite = write(can_fd, &sendPollingFrame, sizeof(struct can_frame));
    if(nwrite != sizeof(struct can_frame))
    {
        qDebug() <<"in pollingThread::sendPolling():Error. nwrite is"<<nwrite<<"!= sizeof(can_frame)"<<sizeof(struct can_frame);
        return;
    }

    counter[boardRegNum.at(boardnum) - 1] += 1;
    if( counter[boardRegNum.at(boardnum) - 1] > 6 )
    {
        counter[boardRegNum.at(boardnum) - 1] = 6;
        QDateTime datetime = QDateTime::currentDateTime();
        emit eventComes(TI_BreakDown, boardRegNum.at(boardnum), 0, 0, 0, 0x0f, datetime);//0x0f is:板通信故障
//        qDebug()<<"ACTIVE BREAK_DOWN from board: "<<boardRegNum.at(boardnum)
//                <<"-"<<0
//                <<"-"<<0
//                <<"-"<<0
//                <<"-"<< 0x0f;
    }

}

void pollingThread::sendAck()
{
    memset (&sendFrame, 0x0, sizeof(struct can_frame));
    sendFrame.priority = Priority4;
    sendFrame.ti = TI_Ack;
    sendFrame.src_addr = CONTROLLADDR;
    sendFrame.dest_addr = recvFrame.src_addr;// boardnum
    sendFrame.can_dlc = 0x00;

    int nwrite = write(can_fd, &sendFrame, sizeof(struct can_frame));
    if(nwrite != sizeof(struct can_frame))
    {
        qDebug() <<"in pollingThread::sendAck():Error:send ack frame...";
        return;
    }
    //else
        //qDebug()<<"send ack to board ..." << recvFrame.src_addr;
}

void pollingThread::sendStartWork()
{
    /******  send start work frame ************/

    memset (&sendFrame, 0x0, sizeof(struct can_frame));
    sendFrame.priority = Priority2;
    sendFrame.ti = TI_Work;
    sendFrame.src_addr = CONTROLLADDR;
    sendFrame.dest_addr = 0x00;
    sendFrame.can_dlc = 0x00;
    int nwrite = write(can_fd, &sendFrame, sizeof(struct can_frame));
    if(nwrite != sizeof(struct can_frame))
        qDebug() <<"in pollingThread::sendStartWork():Error:send start work frame...";

}

void pollingThread::sendLink()
{
    //	联动动作（控制器——>接口板）
    /* Byte 5（T/RXBnD0）  回路编号
       Byte 6（T/RXBnD1）  编址单元地址
       Byte 7（T/RXBnD2）  动作类型
                           0x01-启动（或双输出第一路启动）
                           0x02-停止（或双输出第一路停止）
                           0x11-双输出第二路启动
                           0x12-双输出第二路停止
       Byte 8（T/RXBnD3）  校验码	*/

    nodeID tempnode = linkQueue.head();
    int boardnumber = tempnode.getiobrnum();
    int loopnumber = tempnode.getloopnum();
    int nodenumber = tempnode.getnodenum();
    unsigned char actiontype = tempnode.getactiontype();
    struct can_frame tmpSendFrame;//,tmpRecvFrame;
    int nwrite = 0;//, nread = 0;

    memset (&tmpSendFrame, 0x0, sizeof(struct can_frame));
    tmpSendFrame.priority = Priority1;
    tmpSendFrame.ti = TI_LinkageAction;
    tmpSendFrame.src_addr = CONTROLLADDR;
    tmpSendFrame.dest_addr = boardnumber;
    tmpSendFrame.can_dlc = 0x04;
    tmpSendFrame.data[0] = loopnumber;
    tmpSendFrame.data[1] = nodenumber;
    tmpSendFrame.data[2] = actiontype;
    nwrite = write(can_fd, &tmpSendFrame, sizeof(struct can_frame));
    if(nwrite != sizeof(struct can_frame))
    {
        qDebug() <<"in pollingThread::timer20msOutSlot():Error:send link frame...";
        return;
    }
    qDebug() <<"in pollingThread::timer20msOutSlot():send link to"<<boardnumber<<loopnumber<<nodenumber<<actiontype<<"sendtime is"<<currentLinkSendTime;

    /*if send start to node,default show the node already started.*/
    if(actiontype == LINK_ACTION_START)
    {
        QDateTime datetime = QDateTime::currentDateTime();
        emit autoLinkStartSignal(boardnumber, loopnumber, nodenumber);

        qDebug()<<"AUTO LinkageStart Frame from board: "<<recvFrame.src_addr
                <<"-"<<recvFrame.data[0]
                <<"-"<<recvFrame.data[1]
                <<"-"<<recvFrame.data[2]
                <<"-"<< recvFrame.data[3];
    }

}

void pollingThread::sendBusLed()
{
    /*  busLink led set  2012.12.14 */
    //（控制器——>直控/总线接口板）TI_SetLED
    /* Byte 4（T/RXBnDLC）  数据长度	    0x05
       Byte 5（T/RXBnD0）   回路编号	    21~84（总线盘编号）
       Byte 6（T/RXBnD1）   灯的类型	    启动灯：0x01  反馈灯：0x02
       Byte 7（T/RXBnD2）   编址单元地址    1~64
       Byte 8（T/RXBnD3）   编址单元状态    点灯＝0x00、熄灯＝0x01、闪灯=0x02*/

    nodeID buslednode = busledQueue.head();
    int boardnumber = buslednode.getiobrnum();
    int buspanelnumber = buslednode.getloopnum();
    int busnodenumber = buslednode.getnodenum();
    unsigned char actiontype = buslednode.getactiontype();
    unsigned char ledtype = buslednode.getledtype();
    struct can_frame tmpSendFrame;
    int nwrite = 0;

    memset (&tmpSendFrame, 0x0, sizeof(struct can_frame));
    tmpSendFrame.priority = Priority1;
    tmpSendFrame.ti = TI_SetLED;
    tmpSendFrame.src_addr = CONTROLLADDR;
    tmpSendFrame.dest_addr = boardnumber;
    tmpSendFrame.can_dlc = 0x05;
    tmpSendFrame.data[0] = buspanelnumber+20;
    tmpSendFrame.data[1] = ledtype;
    tmpSendFrame.data[2] = busnodenumber;
    tmpSendFrame.data[3] = actiontype;

    nwrite = write(can_fd, &tmpSendFrame, sizeof(struct can_frame));
    if(nwrite != sizeof(struct can_frame))
    {
        qDebug() <<"in pollingThread::timer20msOutSlot():Error:send busled frame...";
        return;
    }
    qDebug()<<"in pollingThread::sendBusLedSlot():send to bus34 "<<buspanelnumber+20<<busnodenumber<<actiontype<<ledtype<<"sendtime is"<<currentBusledSendTime;

}

void pollingThread::sendDisplay()
{
 /* Byte 0（T/RXBnSIDH）	优先级+TI码	Priority1 + TI_FireDisplay
    Byte 1（T/RXBnSIDL）	Reserve	0x08
    Byte 2（T/RXBnEID8）	源地址	控制器地址
    Byte 3（T/RXBnEID0）	目的地址	楼显接口板地址（33）
    Byte 4（T/RXBnDLC）	数据长度	0x07
    Byte 5（T/RXBnD0）	回路编号	楼显编号（1~255）
    Byte 6（T/RXBnD1）	报警点所在接口板号	1-32
    Byte 7（T/RXBnD2）	报警点所在回路号	1-4
    Byte 8（T/RXBnD3）	报警点编址单元地址	1-255
    Byte 9（T/RXBnD4）	报警点类型	烟头/温头……
    Byte 10（T/RXBnD5）	报警状态	火警/预警……
    Byte 11（T/RXBnD6）	校验码	     */

    nodeID tempnode = louxianQueue.head();
    int louxiannum =tempnode.gethostnum();
    int iobrnum = tempnode.getiobrnum();
    int loopnum = tempnode.getloopnum();
    int nodenum = tempnode.getnodenum();
    unsigned char nodetype = tempnode.getactiontype();
    unsigned char nodestate = tempnode.getledtype();
    struct can_frame tmpSendFrame;
    int nwrite = 0;

    memset (&tmpSendFrame, 0x0, sizeof(struct can_frame));
    tmpSendFrame.priority = Priority1;
    tmpSendFrame.ti = TI_FireDisplay;
    tmpSendFrame.src_addr = CONTROLLADDR;
    tmpSendFrame.dest_addr = LOUXIANNUM;//33
    tmpSendFrame.can_dlc = 0x07;
    tmpSendFrame.data[0] = louxiannum;
    tmpSendFrame.data[1] = iobrnum;
    tmpSendFrame.data[2] = loopnum;
    tmpSendFrame.data[3] = nodenum;
    tmpSendFrame.data[4] = nodetype;
    tmpSendFrame.data[5] = nodestate;

    nwrite = write(can_fd, &tmpSendFrame, sizeof(struct can_frame));
    if(nwrite != sizeof(struct can_frame))
    {
        qDebug() <<"in pollingThread::timer20msOutSlot():Error:send display frame...";
        return;
    }
    qDebug() <<"in pollingThread::sendDisplay():send display frame!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~";
}

void pollingThread::dealInfoFrame()
{
    //判断节点是否登记 是否设置了屏蔽
    /*query.addBindValue((host<<24) | (iobrnum<<16) | (loopnum<<8) | nodenum);    */
    int hostnum = 1;
    int reportnode = (hostnum<<24) | (recvFrame.src_addr<<16) | (recvFrame.data[0]<<8) | (recvFrame.data[1]);
    if( recvFrame.src_addr<34 && (!isRegistAndUnShield(reportnode)) )//0 means 登记且未屏蔽
    {
        qDebug()<<"in pollingThread::dealInfoFrame():node"<<recvFrame.src_addr<<recvFrame.data[0]<<recvFrame.data[1]<<"is shield or unregisted.so ignore it.";
        return;
    }
    else if(recvFrame.src_addr == 34 && (!isRegisted(recvFrame.data[0])))
    {
        qDebug()<<"in pollingThread:the node"<<recvFrame.src_addr<<recvFrame.data[0]<<recvFrame.data[1]<<"is not registed.ignored.";
        return;
    }
    //else ,the node is registed and is not shield
    switch(recvFrame.ti)
    {
        case TI_FireAlarm :
            dealFire();
            break;

        case TI_BreakDown :
            dealBreakDown();
            break;

        case  TI_BreakDownRecover:
            dealBreakDownRecover();
            break;

        case TI_FeedBack :
            dealFeedBack();
            break;

        case  TI_FeedBackRecover:
            dealFeedBackRecover();
            break;

        case  TI_LinkageStart :
            dealLinkageStart();
            break;

        case  TI_LinkageStop:
            dealLinkageStop();
            break;
        case TI_ActionFailed:
            dealActionFailed();
            break;
        case  TI_KeyInfReport:
            dealKeyInfReport();
              break;

        default:
            qDebug()<<"in pollingThread::dealInfoFrame():unknow frame type,recvFrame.ti is:"<<recvFrame.ti;
        break;
    }
    return;
}


bool pollingThread::isRegisted(int node)
{
    /*for zhikongpan*/
    if(node < 21)
    {
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from zhikonginfo WHERE id = %1 ").arg(node));
        if (query.next())
        {
            if (query.value(1).toBool())
                return true;
            else
                return false;
        }
        else
        {
            qDebug()<<"in pollingThread::isRegisted():node is not in zhikonginfo.return false";
            return false;
        }
    }
    else
    {
        /*//loop num > 21, for zongxianpan*/
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from zongxianinfo WHERE id = %1 ").arg(node-20));
        if (query.next())
        {
            if (query.value(1).toBool())
                return true;
            else
                return false;
        }
        else
        {
            qDebug()<<"in pollingThread::isRegisted():node is not in zhikonginfo.return false";
            return false;
        }
    }
}

bool pollingThread::isRegistAndUnShield(int node)
{
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from unitset WHERE unitid = %1 ").arg(node));
    if (query.next())
    {
        /* query.value(1):"regist bool"   query.value(2):"shield bool" */
        if (query.value(1).toBool() && (!query.value(2).toBool()))
            return true;
        else
            return false;
    }
    else
    {
        qDebug()<<"in pollingThread::isShield():node"<<((node>>24)&0xff)<<((node>>16)&0xff)<<((node>>8)&0xff)<<(node&0xff)<<"ti"<<recvFrame.ti<<"is not in the unitset database.";
        return false;
    }
}


void pollingThread::dealFire()
{
    QDateTime datetime = QDateTime::currentDateTime();
    emit eventComes(TI_FireAlarm, recvFrame.src_addr,recvFrame.data[0],recvFrame.data[1],recvFrame.data[2],recvFrame.data[3], datetime);
    qDebug()<<"receive FIRE from board: "<<recvFrame.src_addr
            <<"-"<<recvFrame.data[0]
            <<"-"<<recvFrame.data[1]
            <<"-"<<recvFrame.data[2]
            <<"-"<< recvFrame.data[3];
}

void pollingThread::dealBreakDown()
{
    QDateTime datetime = QDateTime::currentDateTime();
    emit eventComes(TI_BreakDown, recvFrame.src_addr,recvFrame.data[0],recvFrame.data[1],recvFrame.data[2],recvFrame.data[3], datetime);
    qDebug()<<"receive BREAK_DOWN from board: "<<recvFrame.src_addr
            <<"-"<<recvFrame.data[0]
            <<"-"<<recvFrame.data[1]
            <<"-"<<recvFrame.data[2]
            <<"-"<< recvFrame.data[3];
    /*
            <<"loop: "<<recvFrame.data[0]
            <<"nodeID: "<<recvFrame.data[1]
            <<"nodeType: "<<recvFrame.data[2]
            <<"nodeState: "<< recvFrame.data[3];*/
}

void pollingThread::dealBreakDownRecover()
{
    QDateTime datetime = QDateTime::currentDateTime();
    emit eventComes(TI_BreakDownRecover, recvFrame.src_addr,recvFrame.data[0],recvFrame.data[1],recvFrame.data[2],recvFrame.data[3], datetime);

    qDebug()<<"receive BREAK_DOWN_Recover Frame from board: "<<recvFrame.src_addr
            <<"-"<<recvFrame.data[0]
            <<"-"<<recvFrame.data[1]
            <<"-"<<recvFrame.data[2]
            <<"-"<< recvFrame.data[3];
}

void pollingThread::dealFeedBack()
{
    QDateTime datetime = QDateTime::currentDateTime();
    emit eventComes(TI_FeedBack, recvFrame.src_addr,recvFrame.data[0],recvFrame.data[1],recvFrame.data[2],recvFrame.data[3], datetime);

    qDebug()<<"receive FEED_BACK Frame from board: "<<recvFrame.src_addr
            <<"-"<<recvFrame.data[0]
            <<"-"<<recvFrame.data[1]
            <<"-"<<recvFrame.data[2]
            <<"-"<< recvFrame.data[3];
}
void pollingThread::dealFeedBackRecover()
{
    QDateTime datetime = QDateTime::currentDateTime();
    emit eventComes(TI_FeedBackRecover, recvFrame.src_addr,recvFrame.data[0],recvFrame.data[1],recvFrame.data[2],recvFrame.data[3], datetime);

    qDebug()<<"receive FEED_BACK_Recover Frame from board: "<<recvFrame.src_addr
            <<"-"<<recvFrame.data[0]
            <<"-"<<recvFrame.data[1]
            <<"-"<<recvFrame.data[2]
            <<"-"<< recvFrame.data[3];
}

void pollingThread::dealLinkageStart()
{
    QDateTime datetime = QDateTime::currentDateTime();
    emit eventComes(TI_LinkageStart, recvFrame.src_addr,recvFrame.data[0],recvFrame.data[1],recvFrame.data[2],recvFrame.data[3], datetime);

    qDebug()<<"receive LinkageStart Frame from board: "<<recvFrame.src_addr
            <<"-"<<recvFrame.data[0]
            <<"-"<<recvFrame.data[1]
            <<"-"<<recvFrame.data[2]
            <<"-"<< recvFrame.data[3];
}
void pollingThread::dealLinkageStop()
{
    QDateTime datetime = QDateTime::currentDateTime();
    emit eventComes(TI_LinkageStop, recvFrame.src_addr,recvFrame.data[0],recvFrame.data[1],recvFrame.data[2],recvFrame.data[3], datetime);

    qDebug()<<"receive LinkageStop Frame from board: "<<recvFrame.src_addr
            <<"-"<<recvFrame.data[0]
            <<"-"<<recvFrame.data[1]
            <<"-"<<recvFrame.data[2]
            <<"-"<< recvFrame.data[3];
}

void pollingThread::dealActionFailed()
{
    QDateTime datetime = QDateTime::currentDateTime();
    emit eventComes(TI_ActionFailed, recvFrame.src_addr,recvFrame.data[0],recvFrame.data[1],recvFrame.data[2],recvFrame.data[3], datetime);

    qDebug()<<"receive LinkageStop Frame from board: "<<recvFrame.src_addr
            <<"-"<<recvFrame.data[0]
            <<"-"<<recvFrame.data[1]
            <<"-"<<recvFrame.data[2]
            <<"-"<< recvFrame.data[3];
}

/*按键信息上报（直控/总线接口板——>控制器）*/
void pollingThread::dealKeyInfReport()
{
/*
    Byte 0（T/RXBnSIDH）	优先级+TI码	Priority1 + TI_KeyInfReport
    Byte 1（T/RXBnSIDL）	Reserve         0x08
    Byte 2（T/RXBnEID8）	源地址          接口板地址
    Byte 3（T/RXBnEID0）	目的地址	控制器地址
    Byte 4（T/RXBnDLC）	数据长度	0x05
    Byte 5（T/RXBnD0）	回路盘编号	总线盘(21-84)
    Byte 6（T/RXBnD1）	编址单元地址	按键编号1~64
    Byte 7（T/RXBnD2）	编址单元类型	Reserved (未用)0x00
    Byte 8（T/RXBnD3）	编址单元状态	启动＝0、停止＝1
    Byte 9（T/RXBnD4）	校验码
    */
    QDateTime datetime = QDateTime::currentDateTime();
    emit KeyInfComes(recvFrame.src_addr,recvFrame.data[0]-20,recvFrame.data[1],recvFrame.data[3], datetime);

    qDebug()<<"receive key-info Frame from board: "<<recvFrame.src_addr
            <<"loop: "<<recvFrame.data[0]-20
            <<"nodeID: "<<recvFrame.data[1]
            <<"nodeState: "<<recvFrame.data[3];

}

pollingThread::~pollingThread()
{
    close(can_fd);
}
