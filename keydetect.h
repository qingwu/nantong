#ifndef TEA_H
#define TEA_H

#include <QThread>
#include <QMap>
#include <QDateTime>
#include "canframe.h"

#define HOST_POWER_FAULT 1
#define RESERVE_POWER_FAULT 2
#define SOUND_LIGHT_FAULT 3

class QTimer;
class KeyDetect : public QThread
{
    Q_OBJECT
public:
    KeyDetect(QObject *parent);
    ~KeyDetect();
    QTimer *oneSecondTimer;
    QTimer *fireSoundTimer ;
    QTimer *linkSoundTimer;
    QTimer *faultSoundTimer;
    QTimer *lockKeyTimer;

    void Delay(int time);
    void initFlags();

signals:
    void keyPressedSignal(int key_value);

protected:
    void run();

private:
    QMap<char,char> light_map;
    int buttons_fd;
    int pic16f676_fd;
    unsigned char pre_power_state;

    //为1表示故障，为0表示正常
    char emitLocalFaultFlag;
    char hostPowerFaultFlag;//主电故障
    char reservePowerFaultFlag;//备电故障
    char sgFaultFlag;//声光警报器故障
    char faultType;//1:hostPowerFault  2:reservePowerFault  3:sgFault

signals:
    void localFaultSignal(int ti, int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodeState, QDateTime datetime);
    void setLockkeyFlagTrue();
    void setLockkeyFlagFalse();
    void sgFlagChangeSignal( char sgflag);

public slots:
    //    void ledSetSlot(int );
    //    void ledPutSlot(int );
    void allSoundOffSlot();
    void linkSoundOffSlot();
    void linkSoundOnSlot();
    void faultSoundOffSlot();
    void faultSoundOnSlot();
    void fireSoundOffSlot();
    void fireSoundOnSlot();
    void ledStateChangeSlot(char ledtype,char ledstate);
    void soundLedResetSlot();
    void soundErasureSlot();
    void checkPowerSlot();

    void timerStart();

    void put_on_selfchecklight();
    void put_off_selfchecklight();

    void put_on_all_light();
    void put_off_all_light();
    void put_on_the_lights_before_selfcheck();

    void sound_take_turn_on();

    void pollingStartSlot();//to set flags,and to emit local_fault


    void dealwithLockkeyEvent();
    void dealwithUnlockkeyEvent();
    void resetLockKeyTimer(int);

};


#endif // TEA_H
