#ifndef DEFMACRO_H
#define DEFMACRO_H

#define LOOPMAXINUM 4
#define BOARDMAXINUM 34
#define LOUXIANNUM 33
#define ZHIKONGZONGXIAN 34
#define NODEMAXINUM  256
#define LOUXIANMAXNUM 255
#define ZHIKONGZONGXIANNUM 84

#define OFFLINE 10
#define ONLINE 1
#define OTHER 2

#define LOUXIAN  0x41
#define ZHIKONGPAN 0x52
#define ZONGXIANPAN 0x53
//40直控点      41楼显        42总线点
//50控制器      51回路        52直控盘      53总线盘      54转接模块

//add by qingwu:2012.11.05
//8, 7,hand 6,shield 5,delay  4,fault 3,supervise 2,link 1,fire
#define FIRE_PUSHBUTTON  0x01
#define LINK_PUSHBUTTON  0x02
#define SUPERVISE_PUSHBUTTON 0x04
#define FAULT_PUSHBUTTON  0x08
#define DELAY_PUSHBUTTON  0x10
#define SHIELD_PUSHBUTTON  0x20
#define HANDAOTO_PUSHBUTTON  0x40

#define LINK_SHIJIAN_MA 0xEF
#define NO_STATE 0x0
#define LINK_START_NO_FEEDBACK_STATE 0x01
#define LINK_START_YES_FEEDBACK_STATE 0x02
#define LINK_FEEDBACK_STATE 0x03

#define LINK_NODE_ENQUEUE 1
#define BUS_NODE_ENQUEUE 2
#define LOUXIAN_NODE_ENQUEUE 3

/*启动灯：0x01  反馈灯：0x02
  点灯＝0x00、熄灯＝0x01、闪灯=0x02*/
#define BUS_LED_OF_START   0x01
#define BUS_LED_OF_FEEDBACK    0x02
#define LED_ACTION_PUT_ON   0x00
#define LED_ACTION_PUT_OUT  0x01
#define LED_ACTION_FLICK    0x02

#define SIXTY_SECONDS 60
#define TWENTY_NINE_MINS 180 //180 is for test.the right value is 29*60

//add by qingwu 2012.11.12 linkageAciton
//0x01-启动（或双输出第一路启动）
//0x02停止（或双输出第一路停止）
//0x11-双输出第二路启动
//0x12-双输出第二路停止
#define LINK_ACTION_NULL 0x0
#define LINK_ACTION_START 0x01
#define LINK_ACTION_STOP 0x02
#define LINK_ACTION_SECOND_START 0x11
#define LINK_ACTION_SECOND_STOP 0x12


#endif // DEFMACRO_H
