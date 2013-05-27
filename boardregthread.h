#ifndef BOARDREGTHREAD_H
#define BOARDREGTHREAD_H

#include "canframe.h"
#include "defMacro.h"
#include <QThread>
class QTimer;

class boardRegThread : public QThread
{
    Q_OBJECT
public:
    explicit boardRegThread(QObject *parent = 0);
    ~boardRegThread();
    void stopBoardRegThread();
    void setSysResetFlag();

private:
    int singleBoardNum;
    int i;//current board number
    int can_fd;
    int nwrite;
    bool boardRegFlag[BOARDMAXINUM];//reg,1;not reg,0
    int boardRegSum;//total number of registed board
    int loopRegNum[4];//4 Bytes ,registerd node number of 4 loops of  a board
    int louxianRegNum;//registed louxian number
    int zhikongRegNum;//registed zhikongpan number
    int zongxianRegNum;//registed zongxianpan number
    unsigned char nodeType[LOOPMAXINUM][NODEMAXINUM];//[4][256]
    unsigned char louxianReg[LOUXIANMAXNUM];//[255],state:online or offline
    unsigned char zhikongzongxianReg[ZHIKONGZONGXIANNUM];//20 ZHIKONG + 64 ZONGXIAN
    //unsigned char zhikongReg[ZHIKONGMAXNUM];
    //unsigned char zongxianReg[ZONGXIANMAXNUM];
    struct can_frame sendFrame,recvFrame;
    QTimer *timer200s;
    QTimer *timer3s;

    bool isTimeOut200s;
    bool isTimeOut3s;
    bool isSingleBoardRegFlag;
    //bool isReRegisterFlag;

    void sendBroadcast();
    void sendSingle(int i);
    void sendAck();
    void dealInfo(int );

    void saveNodeToDB(int boardnum, unsigned char nodeType[][NODEMAXINUM]);
    void saveLouxianToDB();
    void saveBoardRegFlagToDB(bool boardRegFlag[BOARDMAXINUM]);//
    void updateBoardRegFlagToDB(int boardnum);

protected:
    void run();
    void signalBoardReg(int i);
private slots:
    void timeOutSlot1();
    void timeOutSlot2();
signals:
    void regStart();
    void regFault();
    void regAllOver(int );
    void currentRegBoardSgl(int );
    void singleRegOver(int );
public slots:
    void singleBoardRegSlot(int num);

};

#endif // BOARDREGTHREAD_H
