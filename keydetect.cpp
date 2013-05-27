/*
    2012-8-16 16:34:51
    by qingwu
    run()：检测按键，发送键值，在mainWidget.cpp中接收键值并处理。
    ledControl()：在polling线程中接收火警故障等不同信息，点亮不同的led灯作为指示。
    key value range is :0x40~0x7f
*/
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>

#include <QMessageBox>
#include <QLabel>
#include <QtDebug>
#include <QTimer>
#include <QDateTime>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include<QtCore/QVariant>

#include "keydetect.h"
#include "canframe.h"
#include "ch452.h"
#include "pic16f676.h"

KeyDetect::KeyDetect(QObject *parent)
    :QThread(parent)
{
    initFlags();

    QSqlDatabase::database().transaction();
    int lockkeytime = 0;
    QSqlQuery lockkeyquery;
    lockkeyquery.setForwardOnly(true);
    lockkeyquery.exec("select lockkeytime from hostset");
    while (lockkeyquery.next())
    {
        lockkeytime =(lockkeyquery.value(0).toInt()+1)*60*1000;
        //      lockkeytime =(lockkeyquery.value(0).toInt()+1)*60*100;
    }

    QSqlDatabase::database().commit();

    lockKeyTimer = new QTimer(this);
    lockKeyTimer->setInterval(lockkeytime);
    lockKeyTimer->setSingleShot(true);
    connect(lockKeyTimer,SIGNAL(timeout()),this,SLOT(dealwithLockkeyEvent()));
    lockKeyTimer->start();

    //open key and led dev
    buttons_fd = open("/dev/ch452", O_RDWR);
    if(buttons_fd < 0)
    {
        qDebug()<<"KeyDetect:can't open  button_dev...buttons_fd = "<< buttons_fd;
        //return;//do not return,go on here.
    }
    else
        qDebug()<<"KeyDetect:open button_dev successfully..bingbing ,buttons_fd = "<< buttons_fd;

    //open speaker dev
    //pic16f676_fd = open("/dev/pic16f676",O_RDWR);
    pic16f676_fd = open("/dev/iodevice",O_RDWR);
    if(pic16f676_fd < 0)
    {
        qDebug()<<"IN keyDetect thread:can't open pic16f676_dev..pic16f676_fd = "<<pic16f676_fd;
        //return;//do not return,go on here.
    }
    else
        qDebug()<<"open pic16f676_dev successfully..bingbing ,pic16f676_fd = "<<pic16f676_fd;


    oneSecondTimer = new QTimer(this);
    oneSecondTimer->setInterval(1000);//1s
    connect(oneSecondTimer,SIGNAL(timeout()),this,SLOT(checkPowerSlot()));    
    oneSecondTimer->start();

    fireSoundTimer = new QTimer(this);
    fireSoundTimer->setInterval(2000);//1s
    fireSoundTimer->setSingleShot(true);


    linkSoundTimer= new QTimer(this);
    linkSoundTimer->setInterval(2000);
    linkSoundTimer->setSingleShot(true);

    faultSoundTimer= new QTimer(this);
    faultSoundTimer->setInterval(2000);
    faultSoundTimer->setSingleShot(true);


    connect(fireSoundTimer,SIGNAL(timeout()),this,SLOT(fireSoundOffSlot()));
    connect(fireSoundTimer,SIGNAL(timeout()),linkSoundTimer,SLOT(start()));
    connect(fireSoundTimer,SIGNAL(timeout()),this,SLOT(linkSoundOnSlot()));

    connect(linkSoundTimer,SIGNAL(timeout()),this,SLOT(linkSoundOffSlot()));
    connect(linkSoundTimer,SIGNAL(timeout()),faultSoundTimer,SLOT(start()));
    connect(linkSoundTimer,SIGNAL(timeout()),this,SLOT(faultSoundOnSlot()));

    connect(faultSoundTimer,SIGNAL(timeout()),this,SLOT(faultSoundOffSlot()));

}

void KeyDetect::timerStart()
{
    oneSecondTimer->start();
}
KeyDetect::~KeyDetect()
{
}

void KeyDetect::run()
{
    qDebug()<<"KeyDetectThreadId_____run___________:"<<QThread::currentThreadId();
    if( buttons_fd < 0)
    {
        qDebug()<<"KeyDetect::run():buttons_fd = 0,return here.";
        return;
    }

    unsigned char  key_value = 0x00;
    while(1)
    {
        int nread = read(buttons_fd,&key_value,sizeof(key_value));
        if(nread != sizeof(key_value))
            continue;
        emit keyPressedSignal(key_value);
        //        qDebug()<<"You pressed buttons "<<key_value<<endl;
        key_value = 0;
    }
}

void KeyDetect::ledStateChangeSlot(char ledtype, char ledstate)
{
    if( (pic16f676_fd < 0) || (buttons_fd < 0) )
    {
        qDebug()<<"KeyDetect::ledStateChangeSlot():there is error of fd.pic16f676_fd="<<pic16f676_fd<<"buttons_fd="<<buttons_fd;
    }
    else
    {
        light_map[ledtype] = ledstate;
        if(ledstate == LED_PUT_ON)
        {
            ioctl(buttons_fd, CH452_IOCTL_SETLED, ledtype);
            if( ledtype == LED_OF_FIRE)
            {
                ioctl(pic16f676_fd,SOUND_ON,ALARM);
                ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_SOUNDEARSE);
                light_map[LED_OF_SOUNDEARSE] = LED_PUT_OFF;
            }
            else if ( ledtype == LED_OF_LINK_START)
            {
                ioctl(pic16f676_fd,SOUND_ON,LINKAGE);
                ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_SOUNDEARSE);
                light_map[LED_OF_SOUNDEARSE] = LED_PUT_OFF;
            }
            else if ( ledtype == LED_OF_LINK_FEEDBACK)
            {
                ioctl(pic16f676_fd,SOUND_ON,LINKAGE);
                ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_SOUNDEARSE);
                light_map[LED_OF_SOUNDEARSE] = LED_PUT_OFF;
            }
            else if ( ledtype == LED_OF_FAULT)
            {
                ioctl(pic16f676_fd,SOUND_ON,FAULT);
                ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_SOUNDEARSE);
                light_map[LED_OF_SOUNDEARSE] = LED_PUT_OFF;
            }
            else if (ledtype == LED_OF_SOUND_AND_ALARM_DEVICE_START)
            {
                ioctl(pic16f676_fd, SOUND_LIGHT_ALARM_ON, 0);//start the device , the sound of the device will start
                ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_SOUNDEARSE);
                light_map[LED_OF_SOUNDEARSE] = LED_PUT_OFF;
            }
            else
            {
                //qDebug()<<"in KeyDetect::ledStateChangeSlot():NO_SOUND_ON";
            }
        }
        else if(ledstate == LED_PUT_OFF)
        {
            ioctl(buttons_fd, CH452_IOCTL_PUTLED, ledtype);
            if( ledtype == LED_OF_FAULT)
            {
                ioctl(pic16f676_fd,SOUND_OFF,FAULT);
            }
            else if (ledtype == LED_OF_SOUND_AND_ALARM_DEVICE_START)
            {
                ioctl(pic16f676_fd, SOUND_LIGHT_ALARM_OFF, 0);//stop the device , the sound of the device will stop
            }
        }
        else if(ledstate == LED_PUT_FLICK)
        {
            ioctl(buttons_fd, CH452_IOCTL_SETFLK, LED_OF_LINK_START+1);//(LED_OF_LINK_START>>3)+1);//0x08是第二排第一个灯;8>>3+1 = 2是第二排
            ioctl(pic16f676_fd,SOUND_ON,LINKAGE);
            ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_SOUNDEARSE);
            light_map[LED_OF_SOUNDEARSE] = LED_PUT_OFF;
        }
        else
        {
            qDebug()<<"in KeyDetect::ledStateChangeSlot():LED_TYPE UNKNOWN";
        }
    }


    /* ledtype

        #define LED_OF_LINK_START    0x00
        #define LED_OF_FIRE      0x08
        #define LED_OF_LINK_FEEDBACK     0x20
        #define LED_OF_FAULT     0x21

        #define LED_OF_POWER_WORK    0x09
        #define LED_OF_POWER_FAULT   0x0A
        #define LED_OF_RESERVE_POWER_WORK  0x11
        #define LED_OF_RESERVE_POWER_FAULT  0x12


        #define LED_OF_SUPERVISE 0x10
        #define LED_OF_MANUAL 0x0B
        #define LED_OF_AUTOMATIC 0x13
        #define LED_OF_SYSTEMFAULT 0x22

        #define LED_OF_SELFCHECK 0x0C
        #define LED_OF_SHIELD 0x14
        #define LED_OF_OUTPUT_DELAY 0x23
        #define LED_OF_SOUNDEARSE 0x0D
        #define LED_OF_LOCK_KEY 0x15
        #define LED_OF_FIRE_DELAY 0x24
        #define LED_OF_ZHIKONG_HAND_ALLOW 0x25

        #define LED_OF_SOUND_AND_ALARM_DEVICE_START 0x18
        #define LED_OF_SOUND_AND_ALARM_DEVICE_FAULT 0x19
        #define LED_OF_SOUND_AND_ALARM_DEVICE_SHIELD 0x1A

        #define LED_OF_GAS_FIRE_CONTROL_REQUEST 0x1B
        #define LED_OF_GAS_FIRE_CONTROL_ALLOW 0x1C
        #define LED_OF_GAS_FIRE_CONTROL_GAS_ALLOW 0x1D



    switch(ledstate)
    {
    case LED_PUT_ON:
    {
        switch(ledtype)
        {
        case LED_OF_FIRE:
            ioctl(buttons_fd, CH452_IOCTL_SETLED, LED_OF_FIRE);
            ioctl(pic16f676_fd,SOUND_ON,ALARM);
            break;
        case LED_OF_LINK_START:
            ioctl(buttons_fd, CH452_IOCTL_SETLED, LED_OF_LINK_START);
            ioctl(pic16f676_fd,SOUND_ON,LINKAGE);
            break;
        case LED_OF_LINK_FEEDBACK:
            ioctl(buttons_fd, CH452_IOCTL_SETLED, LED_OF_LINK_FEEDBACK);
            ioctl(pic16f676_fd,SOUND_ON,LINKAGE);
            break;

        case LED_OF_FAULT:
            ioctl(buttons_fd, CH452_IOCTL_SETLED, LED_OF_FAULT);
            ioctl(pic16f676_fd,SOUND_ON,FAULT);
            break;

        case LED_OF_POWER_WORK:
            ioctl(buttons_fd, CH452_IOCTL_SETLED, LED_OF_POWER_WORK);
            break;
        case LED_OF_POWER_FAULT:
            ioctl(buttons_fd, CH452_IOCTL_SETLED, LED_OF_POWER_FAULT);
            break;
        case LED_OF_RESERVE_POWER_WORK:
            ioctl(buttons_fd, CH452_IOCTL_SETLED, LED_OF_RESERVE_POWER_WORK);
            break;
        case LED_OF_RESERVE_POWER_FAULT:
            ioctl(buttons_fd, CH452_IOCTL_SETLED, LED_OF_RESERVE_POWER_FAULT);
            break;

        }
    }
    break;
    case LED_PUT_OFF:
    {
        switch(ledtype)
        {
//        case LED_OF_FIRE:
//            ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_FIRE);
//            ioctl(pic16f676_fd,SOUND_OFF,ALARM);
//            break;
//        case LED_OF_LINK_START:
//            ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_LINK_START);
//            ioctl(pic16f676_fd,SOUND_OFF,LINKAGE);
//            break;
        case LED_OF_FAULT:
            ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_FAULT);
            ioctl(pic16f676_fd,SOUND_OFF,FAULT);
            break;
        case LED_OF_LINK_FEEDBACK:
            ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_LINK_FEEDBACK);
            //ioctl(pic16f676_fd,SOUND_OFF,LINKAGE);
            break;

        case LED_OF_POWER_WORK:
            ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_POWER_WORK);
            break;
        case LED_OF_POWER_FAULT:
            ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_POWER_FAULT);
            break;
        case LED_OF_RESERVE_POWER_WORK:
            ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_RESERVE_POWER_WORK);
            break;
        case LED_OF_RESERVE_POWER_FAULT:
            ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_RESERVE_POWER_FAULT);
            break;
        }
    }
    break;
    case LED_PUT_FLICK:
        ioctl(buttons_fd, CH452_IOCTL_SETFLK, LED_OF_LINK_START+1);//(LED_OF_LINK_START>>3)+1);//0x08是第二排第一个灯;8>>3+1 = 2是第二排
        ioctl(pic16f676_fd,SOUND_ON,LINKAGE);
        break;
    default:
        qDebug()<<"in KeyDetect::ledStateChangeSlot():LED_TYPE UNKNOWN";
        break;
    }
    */
}
void KeyDetect::fireSoundOffSlot()
{
    ioctl(pic16f676_fd,SOUND_OFF,ALARM);
}
void KeyDetect::fireSoundOnSlot()
{
    ioctl(pic16f676_fd,SOUND_ON,ALARM);
}
void KeyDetect::linkSoundOffSlot()
{
    ioctl(pic16f676_fd,SOUND_OFF,LINKAGE);
}
void KeyDetect::linkSoundOnSlot()
{
    ioctl(pic16f676_fd,SOUND_ON,LINKAGE);
}
void KeyDetect::faultSoundOffSlot()
{
    ioctl(pic16f676_fd,SOUND_OFF,FAULT);
}
void KeyDetect::faultSoundOnSlot()
{
    ioctl(pic16f676_fd,SOUND_ON,FAULT);
}
void KeyDetect::allSoundOffSlot()
{
    ioctl(pic16f676_fd,SOUND_OFF,ALL);
}

void KeyDetect::soundErasureSlot()
{
    /*消音指报警器的报警声 声光启停有控制声光的按键*/
    ioctl(pic16f676_fd,SOUND_OFF,ALL);//all sound off!
    //ioctl(pic16f676_fd, SOUND_LIGHT_ALARM_OFF, 0);//声光警报器 停止发声
    ioctl(buttons_fd, CH452_IOCTL_SETLED, LED_OF_SOUNDEARSE);
    qDebug()<<"in KeyDetect::soundErasureSlot: sound OFF";
}

void KeyDetect::soundLedResetSlot()
{
    if( (pic16f676_fd < 0) || (buttons_fd < 0) )
    {
        qDebug()<<"KeyDetect::soundLedResetSlot():there is error of fd.pic16f676_fd="<<pic16f676_fd<<"buttons_fd="<<buttons_fd;
    }

    ioctl(pic16f676_fd, SOUND_OFF, RESET);//sound reset
    ioctl(pic16f676_fd, SOUND_LIGHT_ALARM_OFF, 0);//声光警报器 停止发声
    ioctl(buttons_fd, CH452_IOCTL_PUTALL, 0);
    qDebug()<<"in KeyDetect::soundLedResetSlot: sound OFF-----led OFF ";


    //    ioctl(pic16f676_fd,SOUND_OFF,ALL);//all sound off!
    //    ioctl(pic16f676_fd, SOUND_LIGHT_ALARM_OFF, 0);//声光警报器 停止发声
    //    ioctl(buttons_fd, CH452_IOCTL_PUTLED, LED_OF_LINK_START);//set all led off!
    /*
    for(int i = 0; i < 6; i++)
    {
        ioctl(buttons_fd, CH452_IOCTL_PUTLED, 0x08+i);//set all led off!
        ioctl(buttons_fd, CH452_IOCTL_PUTLED, 0x10+i);//set all led off!
        ioctl(buttons_fd, CH452_IOCTL_PUTLED, 0x18+i);//set all led off!
        ioctl(buttons_fd, CH452_IOCTL_PUTLED, 0x20+i);//set all led off!
    }
   */
}

void KeyDetect::checkPowerSlot()
{

    //qDebug()<<"KeyDetectThreadId__checkPowerSlot_________:"<<QThread::currentThreadId();
    /*	read the GPK6, GPK7(LOCK), GPP8,GPP9,GPP10,GPP11(the state of power)
         GPK6->0,GPK7->1, GPQ4 ->2,  GPP8->4, GPP9->5, GPP10->6, GPP11->7	bit is start with 0	*/
    if( pic16f676_fd < 0 )
    {
        qDebug()<<"KeyDetect::checkPowerSlot(): pic16f676_fd="<<pic16f676_fd<<"return here.";
        return;
    }

    unsigned char  power_state = 0x00;
    int nread = read(pic16f676_fd,&power_state,sizeof(power_state));
    if(nread != sizeof(power_state))
    {
        qDebug()<<"KeyDetect::checkPowerSlot(): read error has occurred. "<<endl;
        return;
    }
    else
    {
        if(pre_power_state == power_state)
        {
            /*power_state is not changed.*/
            return;
        }
        pre_power_state = power_state;
        qDebug()<<"KeyDetect::checkPowerSlot():power_state changed to:"<<power_state<<endl;
        /*  read(pic16f676_fd,&power_state,sizeof(power_state));
             bit7=1 reserve work; bit7=0 power work
          // bit 6 = 0 reserve fault
          // bit 4 = 0 power fault  */

        if(!((power_state>>7) & 0x01))
        {
            this->ledStateChangeSlot(LED_OF_POWER_WORK,LED_PUT_ON);
            this->ledStateChangeSlot(LED_OF_RESERVE_POWER_WORK,LED_PUT_OFF);
        }
        else
        {
            this->ledStateChangeSlot(LED_OF_RESERVE_POWER_WORK,LED_PUT_ON);
            this->ledStateChangeSlot(LED_OF_POWER_WORK,LED_PUT_OFF);
        }


        if( emitLocalFaultFlag )//flag = 1, ok to emit; flag=0, no emit
        {
            /* when polling is started */
            /*  case 0xd5:  return 0xd1;  break;//0D1H:主电停止; 0D5H:主电启动
                case 0xd6:  return 0xd2;  break;//0D2H:备电停止; 0D6H:备电启动  */
            if(!((power_state>>6) & 0x01))
            {
                /*备电故障  0x50: 控制器;0xD2:备电停止*/                
                if( !reservePowerFaultFlag )
                {
                    this->ledStateChangeSlot(LED_OF_RESERVE_POWER_FAULT,LED_PUT_ON);
                    reservePowerFaultFlag = 1;
                    emit localFaultSignal(TI_BreakDown, 0, 0, 0, 0x50, 0xD2, QDateTime::currentDateTime());
                    qDebug()<<"reserve_Power BreakDown:";
                }
            }
            else
            {
                /*备电恢复 0x50: 控制器;0xD2:备电启动*/                
                if( reservePowerFaultFlag )//reservePowerFaultFlag = 1 ,备电故障
                {
                    this->ledStateChangeSlot(LED_OF_RESERVE_POWER_FAULT,LED_PUT_OFF);
                    reservePowerFaultFlag = 0;
                    emit localFaultSignal(TI_BreakDownRecover, 0, 0, 0, 0x50, 0xD6, QDateTime::currentDateTime());
                    qDebug()<<"reserve_Power BreakDownRecover:";
                }
            }

            if(!((power_state>>4) & 0x01))
            {
                /*主电故障 0x50: 控制器;0xD1:主电停止*/                
                if( !hostPowerFaultFlag )
                {
                    this->ledStateChangeSlot(LED_OF_POWER_FAULT,LED_PUT_ON);
                    hostPowerFaultFlag = 1;
                    emit localFaultSignal(TI_BreakDown, 0, 0, 0, 0x50, 0xD1, QDateTime::currentDateTime());
                    qDebug()<<"host_Power BreakDown :";
                }
            }
            else
            {
                /*主电恢复  0x50: 控制器;0xD2:主电启动*/                
                if( hostPowerFaultFlag )
                {
                    this->ledStateChangeSlot(LED_OF_POWER_FAULT,LED_PUT_OFF);
                    hostPowerFaultFlag = 0;
                    emit localFaultSignal(TI_BreakDownRecover, 0, 0, 0, 0x50, 0xD5, QDateTime::currentDateTime());
                    qDebug()<<"host_Power BreakDownRecover :";
                }
            }

            /*  GPQ3：声光警报器输出，为高，开启，为低，关闭。
            例如：  ioctl(fd, SOUND_LIGHT_ALARM_ON, 0);即可使它发声。
                    ioctl(fd, SOUND_LIGHT_ALARM_OFF, 0);即停止发声。
            GPQ4：声光警报器输入，为高，正常，为低，故障。
            用read函数读出后判断其第二位是否为1即可。 //GPQ4 ->2
            例如：nread = read(fd, &buf, sizeof(char));
            if(buf & 0x04)
                    正常
            else
                    故障    */
            if( power_state & 0x04)
            {
                if( sgFaultFlag )
                {
                    //danyuan_ma:0x3A-->声光警报;shijian_ma:0E2H-->声光恢复                    
                    sgFaultFlag = 0;
                    //this->ledStateChangeSlot(LED_OF_SOUND_AND_ALARM_DEVICE_FAULT,LED_PUT_OFF);
                    //emit localFaultSignal(TI_BreakDownRecover, 0, 0, 0, 0x3A, 0xE2, QDateTime::currentDateTime());
                    emit sgFlagChangeSignal(sgFaultFlag);
                    qDebug()<<"sheng_guang BreakDownRecover sgFaultFlag="<<(sgFaultFlag?1:0);
                }
            }
            else
            {
                if( !sgFaultFlag )
                {
                    //danyuan_ma:0x3A-->声光警报;shijian_ma:0E2H-->声光故障
                    sgFaultFlag = 1;
                    emit sgFlagChangeSignal(sgFaultFlag);
                    qDebug()<<"sheng_guang BreakDown :sgFaultFlag="<<(sgFaultFlag?1:0);
                }
            }
        }        
    }
}


void KeyDetect::put_on_all_light()
{

    qDebug()<<"put_on_all_lights";
    ioctl(buttons_fd, CH452_IOCTL_SETALL, 0);
}
void KeyDetect::put_off_all_light()
{
    qDebug()<<"put_off_all_light";
    ioctl(buttons_fd, CH452_IOCTL_PUTALL, 0);

}

void KeyDetect::put_on_the_lights_before_selfcheck()
{
    qDebug()<<"put_on_the_lights_before_selfcheck";
    QMapIterator<char,char> i(light_map);
    while (i.hasNext())
    {
        qDebug()<<"i.key()"<< i.key();
        if(i.value() == LED_PUT_ON)
        {
            ioctl(buttons_fd, CH452_IOCTL_SETLED, i.key());
        }
        else if(i.value() == LED_PUT_FLICK)
        {
            ioctl(buttons_fd, CH452_IOCTL_SETFLK, i.key()+1);
        }
        else
        {
            ioctl(buttons_fd, CH452_IOCTL_PUTLED, i.key());
        }
        i.next();
    }



}

void KeyDetect::sound_take_turn_on()
{
    qDebug()<<"sound_take_turn_on";



    fireSoundTimer->start();
    this->fireSoundOnSlot();



}

void KeyDetect::put_off_selfchecklight()
{
    qDebug()<<"put_off_selfchecklight";
    ioctl(buttons_fd, CH452_IOCTL_PUTLED,LED_OF_SELFCHECK );
}

void KeyDetect::put_on_selfchecklight()
{
    qDebug()<<"put_on_selfchecklight";
    ioctl(buttons_fd, CH452_IOCTL_SETLED, LED_OF_SELFCHECK);//selfcheck light is still on

}

void KeyDetect::Delay(int time)
{
    clock_t now = clock();
    while(clock() - now < time);
}


void KeyDetect::pollingStartSlot()
{
    emitLocalFaultFlag = 1;
    hostPowerFaultFlag = 0;//主电故障Flag
    reservePowerFaultFlag = 0;//备电故障Flag
    sgFaultFlag = 0;//声光警报器故障Flag    
    pre_power_state = 0;
    qDebug()<<"in KeyDetect():pollingStartSlot():power state start!";
}
void KeyDetect::initFlags()
{
    pre_power_state = 0;
    emitLocalFaultFlag = 0;
    hostPowerFaultFlag = 0;//主电故障Flag
    reservePowerFaultFlag = 0;//备电故障Flag
    sgFaultFlag = 0;//声光警报器故障Flag
}


void KeyDetect::dealwithLockkeyEvent()
{
    this->ledStateChangeSlot(LED_OF_LOCK_KEY,LED_PUT_ON);
    emit setLockkeyFlagTrue();
}
void KeyDetect::dealwithUnlockkeyEvent()
{
    this->ledStateChangeSlot(LED_OF_LOCK_KEY,LED_PUT_OFF);
    emit setLockkeyFlagFalse();
}

void KeyDetect::resetLockKeyTimer(int lockkeytime)
{
    lockKeyTimer->stop();
    lockKeyTimer->setInterval((lockkeytime+1)*60*1000);
    //lockKeyTimer->setInterval((lockkeytime+1)*60*100);
    lockKeyTimer->start();
}
