#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QDialog>
#include <QDateTime>
#include <QMultiHash>
#include <QList>
#include <QModelIndex>
#include <QTimer>

class QLabel;
class QLineEdit;
class QTableWidget;
class QStatusBar;
class MenuWindow;
class ViewMenuWindow;
class pollingThread;
class boardRegThread;
class BoardRegDialog;
class QMessageBox;
//class nodeID;
class KeyDetect;
class QPushButton;
class eventDialog;
class QTabWidget;
class HandWidget;
#include "nodeid.h"

class MainWidget : public QDialog
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

    QAction *hideAction;
    QAction *secondhideAction;
    QAction *escapeAction;
    QAction *upKeyAction;
    QAction *downKeyAction;
    QAction *shieldAction;

    QAction *fireAction;
    QAction *linkAction;
    QAction *fire_linkAction;
    QAction *superviseAction;
    QAction *faultAction;
    QAction *delayAction;

    QAction *hand_automatic;

    //QPushButton *fireButton;
    //QPushButton *linkButton;
    //QPushButton *fire_linkButton;
    QPushButton *superviseButton;
    QPushButton *faultButton;
    QPushButton *delayButton;
    QPushButton *shieldButton;
    QPushButton *hand_automaticButton;

    QLabel *backgroundLabel;
    //    QLabel *fireLabel;
    //    QLabel *linkLabel;
    //    QLabel *fire_linkLabel;
    QLabel *superviseLabel;
    QLabel *faultLabel;
    QLabel *delayLabel;
    QLabel *shieldLabel;
    //QLabel *hand_automaticLabel;


protected:
    void timerEvent (QTimerEvent *event);

private:
    QTimer *oneSecondTimer;
    QTimer *timer3s;//for update the fire in fireTableWidget
    QTimer *timer10s;//to stop auto-update the widget
    QTimer *timer4s;//to close the eventdialog when there is no event to show
    QTimer *changeWidgettimer30s;
    static const int IS_IO_FLAG = 1;
    static const int IS_NOT_IO_FLAG = 0;
    static const int SoundAndAlarmDeviceID = 0x01000000;

    int ifire;// to update the fire in fireTableWidget
    int ilink;
    int fireMaxId;
    int linkMaxId;
    int superviseMaxId;
    int faultMaxId;
    int menutype;

    int fd_printer;
    char sysResetFlag;//add by qingwu:for printer reset "start" or "restart" 2013.5.6
    int printerFlagOfWidget;//add by qingwu
    int linkFeedBackFlag;
    bool lockkeyFlag; //add by linwotian , if lockkeyStateFlag = true, lock all  keys except the soundearse key
    char oneEventShowFlag;//2012.10.31  8, 7,hand 6,pingbi 5,delay  4,fault 3,supervise 2,link 1,fire
    bool selfcheckFlag;//add by linwotian ,if selfcheckFlag = true,the system is selfchecking .
    bool sound_alarm_fault_Flag;//add by qingwu.2013.4.19
    bool sound_alarm_shield_Flag;//add by linwotian ,if sound_alarm_shield_Flag = true , the soundAndAlarmDevice is shield.
    bool sound_alarm_startOrStop_Flag;//add by linwotian ,if sound_alarm_startOrStop_Flag = true ,the soundAndAlarmDevice is working.
    bool enterPasswordFlag;
    bool changeWindowFlag;

    MenuWindow *w;
    ViewMenuWindow *v;
    eventDialog *eventdialog;

    QLabel *fireTipLabel;
    QLabel *linkTipLabel;
    //QLabel *superviseTipLabel;

    QLabel *firenumLabel;
    QLabel *linknumLabel;
    //QLabel *supervisenumLabel;

    QTableWidget *firstFireTableWidget;
    QTableWidget *fireTableWidget;
    QTableWidget *linkTableWidget;

    QStatusBar *statusBar;
    QLabel *remindLabel;
    QLabel *timeLabel;

    //add by linwotian
    QTableWidget *superviseTableWidget;
    QTableWidget *faultTableWidget;
    QTableWidget *delayTableWidget;
    QTableWidget *shieldTableWidget;
    //    QTableWidget *handTableWidget;
    HandWidget *handWidget;

    int superviseIndex;
    int faultIndex;
    int delayIndex;
    int shieldIndex;
    int handIndex;
    QTabWidget *eventTabWidget;

    int fireAllNumber;
    int faultAllNumber;
    int linkAllNumber;

    /*add by qingwu*/
    pollingThread *polling;
    BoardRegDialog *boardDialog;
    boardRegThread *boardThread;
    KeyDetect *keyThread;


    QMultiHash<nodeID, int> hashFire;
    QMultiHash<nodeID, int> hashFault;
    QMultiHash<nodeID, int> hashLink;
    //QMultiHash<nodeID, int> hashDelay;
    QMultiHash<nodeID, int> hashFireDelay;
    QMultiHash<nodeID, int> hashLinkStartDelay;
    QMultiHash<nodeID, int> hashLinkPulseDelay;
    QHash<nodeID, int> hashRestart60s;//for 重报节点
    QHash<nodeID, int> hashRestart29min;//for 重报节点

    //联动命令表:用来判断联动信息是不是重报 <nodeID, 上一条状态改变时的命令<<8|最新的一条命令>
    QHash<nodeID, int> hashLinkageAction;
    //总线盘
    //QHash<nodeID, int> hashBusOut;//nodeID =  hostnum<<24 | iobrnum<<16 | loopnum<<8 | nodenum,
    //      nodeState << 16 |buspanleNum << 8 | busnodeNum
    //QHash<nodeID, int> hashBusIn;// int  = nodestate <<16 |buspanelNum << 8 | busnodeNum

    //add by qingwu 2012.10.28
    QList<QString> fireList;
    QList<QString> linkList;
    QList<QString> faultList;
    QList<QString> superviseList;
    QList<QString> delayList;
    QList<QString> shieldList;
    QList<QString> handautoList;

    //QList<QString> linkStateList;

    void createThread();
    void initMaxId();//init all the max id in the database
    void createActions();
    void createItems();
    //void createStatusBar();//by qingwu
    //void getHashBusFromBusset();//hashBusOut and hashBusIn initialize
    void initSystem();
    void initSystemLight();
    void systemSelfcheck();
    void printerInit();
    void sendResetFrame(int can_fd);
    void sendSelfCheckFrame(int can_fd);

    void saveFireToDB( int nodestate, int nodetype,int iobrnum,int loopnum,int nodenum,
                      int bldnum ,int flrnum,int zonenum ,int roomnum,QDateTime datetime, const QString &str);
    void saveFaultToDB( int nodestate, int nodetype,int iobrnum,int loopnum,int nodenum,
                       int bldnum ,int flrnum,int zonenum ,int roomnum,QDateTime datetime, const QString &str);
//    void saveLinkToDB( int nodestate, int nodetype,int iobrnum,int loopnum,int nodenum,
//                      int bldnum ,int flrnum,int zonenum ,int roomnum,QDateTime datetime, const QString &str);
    void saveLinkToDB( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate, QDateTime datetime);

    void resetSystem();
    unsigned char getFaultCode(unsigned char );
    int  isReReport(int ti, nodeID tempnode,unsigned char shijian);//,QMultiHash<nodeID, int> hashTable

    void fireComeSlot(int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate, QDateTime datetime);
    void faultComeSlot(int iobrnum,int loopnum,int nodenum,unsigned char nodetype,unsigned char nodestate, QDateTime datetime);
    void faultRecoverSlot(int iobrnum,int loopnum,int nodenum,unsigned char nodetype,unsigned char nodestate, QDateTime datetime);
    void feedBackSlot( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodeState, QDateTime datetime);
    void feedBackRecoverSlot( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodeState, QDateTime datetime);
    void linkageStartSlot( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodeState, QDateTime datetime);
    void linkageStopSlot( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodeState, QDateTime datetime);
    void actionFailedSlot( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodeState, QDateTime datetime);

    void hashTest(QMultiHash<nodeID, int>);//for test

    QString getStingItemFromDB(int hostnum,int iobrnum,int loopnum,int nodenum,char shijianma,char danyuanma,char nodetype,QDateTime datetime);
    void removeLinkItem(int hostnum,int iobrnum,int loopnum,int nodenum,char shijianma,char danyuanma,QDateTime datetime);
    void removeFireItem(nodeID node, int rmRowNumber);
    void linkShow();
    void dealLinkLed();
    void dealLinkLedStop();
    //int  getNewState(char preState, char order, unsigned char message);  

    void updateHashRowNumber(QMultiHash<nodeID, int> &hashTable,int rmRowNumber);
    void fireShow(int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate, QDateTime datetime);
    void busLinkAction(int isIOFlag,int hostnum,int iobrnum,int loopnum,int nodenum);
    void doPrint(QString str1, QString str2, QString str3);//printer the event selected
    void eventPrint(int hostnum,int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate, QDateTime datetime);

    void backgroundLabelDispaly();
    void testnowwidget(QWidget *);


public slots:

    void currentFireChangedSlot();

    void fireEventDialog();
    void linkEventDialog();
    void fire_linkEventDialog();
    void superviseEventDialog();
    void faultEventDialog();
    void delayEventDialog();
    void shieldEventDialog();
    void hand_automaticEventDialog();


    void upKeySlot();
    void downKeySlot();

    void escWindow();
    void hideWindow();
    void secondhideWindow();
    void thirdhideWindow();
    void creatShieldWidget();


    void printerFlagOfWidgetSetSlot(int flag);//add by qingwu 2012.10.25
    void linkFeedBackSetSlot(int index);
    void dealDelayHash();
    void updateHandwidgetType(int);
    void updateSoundAndAlarmDeviceState(int);
    void sgFlagChangeSlot(char sgFlag);//add by qingwu
    void updateGasFireControlState(int);

private slots:
    void timer3sStart();
    void updateFireWidgetSlot();
    void updateLinkWidgetSlot();

    void regAllOverSlot();
    void cancelPushButtonClickedSlot();
    void okPushButtonClickedSlot();

    void keyPressedSlot(int key_value);
    void eventComeSlot(int ti,int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate, QDateTime datetime);
    void autoLinkStartSlot(int iobrnum,int loopnum,int nodenum);
    void KeyInfSlot(int, int, int, unsigned char, QDateTime);//总线盘按键按下
    //void updateHashBusSlot(int, int ,int inMode[],int outMode[]);//总线设置修改
    void eventDialogRejectedSlot();
    //void dealLinkDelaySlot(int iobrnumber,int loopnumber,int nodenumber,int start_delay,int pulse_delay );
    void dealLinkNodeSlot(int nodeid);
    void dealHandNodeSlot(char action,int nodeid);
    void updateTabWidget(int index);
    void eventNumberChangedSlot();
    void singleBoardNumSlot(int );
    void setSelfcheckFlagFalseSlot();//add by linwotian


    void setLockkeyFlagTrueSlot();
    void setLockkeyFlagFalseSlot();

    void sendSelfcheckFrameOnce();
    void ChangeWidgettimer30sTimeoutSlot();
    void restartChangeWidgettimer30sSlot();


signals:
    //    void LedSetSignal(int);
    //    void LedPutSignal(int);
    void linkSoundOffSignal();
    void ledStateChangeSignal(char ledtype, char ledstate);
    void soundLedResetSignal();
    void closeWVSignal();
    void eventTrueComes(int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate);

    //void sendLinkSignal(int ti, int boardnumber,int loopnumber,int nodenumber,int actiontype);
    //    void sendLinkSignal( nodeID node);
    //    void busCommandSignal( nodeID node);
    void nodeEnqueueSignal(char flag,nodeID node);

    void sysResetKeyPressed();
    void soundErasureKeyPressed();
    void hidemainshowset();
    void hidemainshowview();
    void oneEventShowSignal(QString, QList<QString> &, char );
    void nullEventShowSignal(char);
    void updateOneEventSignal(QString );
    void clearEventDialogSignal();
    void eventNumberChangedSignal();

    void updateLockkeyTime(int);
};

#endif // MAINWIDGET_H
