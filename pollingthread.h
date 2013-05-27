#ifndef POLLINGTHREAD_H
#define POLLINGTHREAD_H

#include <QThread>
#include <QVector>
#include <QQueue>
#include <QDateTime>

#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <fcntl.h>

#include "canframe.h"
#include "defMacro.h"
#include "nodeid.h"


class QTimer;

class pollingThread : public QThread
{
    Q_OBJECT

public:
    //int nwrite;

    pollingThread(QObject *parent = 0);
    ~pollingThread();


private:

      int can_fd;
      fd_set rfds;
      struct timeval tv;
      QVector<int > boardRegNum;
      int counter[BOARDMAXINUM];//board not reply time,if >=6,board is offline
      int countMax[BOARDMAXINUM];//store the max times that board i not reply
      QTimer *timer100ms;
      QTimer *timer20ms;
      struct can_frame sendFrame,recvFrame,sendPollingFrame,sendBusLedFrame,sendLinkFrame;
      int boardnum;
      bool hasLinkToSend;
      int  isResetFlag;

      QQueue<nodeID> busledQueue;
      QQueue<nodeID> linkQueue;
      QQueue<nodeID> louxianQueue;
      int pollingSendTime;
      int currentBusledSendTime;
      int currentLinkSendTime;
      int currentLouxianSendTime;

      void getBoardState();//read from datebase,get board state,online or offline
      void sendAck();
      void sendPolling();
      void sendStartWork();
      void sendLink();
      void sendBusLed();
      void sendDisplay();

      void dealInfoFrame();
      void dealFire();
      void dealBreakDown();
      void dealBreakDownRecover();
      void dealFeedBack();
      void dealFeedBackRecover();
      void dealLinkageStart();
      void dealLinkageStop();
      void dealActionFailed();
      void dealKeyInfReport();//总线盘按键按下
      bool isRegistAndUnShield(int node);
      bool isRegisted(int node);
      void readCanFrame();
protected:
    void run();

private slots:
    void timer100msOutSlot();
    void timer20msOutSlot();
    void linkFlagSetSlot();
signals:    
    void autoLinkStartSignal(int iobrnum,int loopnum,int nodenum);
    void eventComes(int ti, int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodeState, QDateTime datetime);
    void KeyInfComes(int iobrnum,int buspanlnum,int busnodenum, unsigned char nodeState, QDateTime datetime);
public slots:
    void nodeEnqueueSlot(char flag,nodeID node);
    void stopPolling();

};

#endif // POLLINGTHREAD_H
