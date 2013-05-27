#ifndef __CH452_H__
#define __CH452_H__

#define CH452_IOCTL_LED1	 0x0701
#define CH452_IOCTL_LED2	 0x0702

#define CH452_IOCTL_SETLED	0x0703
#define CH452_IOCTL_PUTLED	0x0704
#define CH452_IOCTL_SETFLK      0x0705

#define CH452_IOCTL_SETALL 0x0706
#define CH452_IOCTL_PUTALL 0x0707   //put all lights off


//#define KEY_RESET 64
//#define KEY_SHIFT 65
//#define KEY_CONTROL 66
//#define KEY_TAB 67
//#define KEY_BACKTAB 68
//#define KEY_RETURN 69
//#define KEY_DELETE 70
//#define KEY_SOUND_ERASURE 71

//#define KEY_LEFT 72
//#define KEY_UP 81
//#define KEY_RIGHT 88
//#define KEY_DOWN 80
//#define KEY_ENTER 73
//#define KEY_ESCAPE 89
//#define KEY_NUMLOCK 90

//#define KEY_0 74
//#define KEY_1 75
//#define KEY_2 76
//#define KEY_3 77
//#define KEY_4 78
//#define KEY_5 79
//#define KEY_6 82
//#define KEY_7 83
//#define KEY_8 84
//#define KEY_9 85


#define KEY_RESET 0x40
#define KEY_PROGRAMME 0x48
#define KEY_PAGEDOWN 0x50
#define KEY_LEFT 0x58
#define KEY_SELFCHECK 0x60
#define KEY_SHIELD 0x68
#define KEY_UP 0x70
#define KEY_DOWN 0x78
#define KEY_SOUND_ERASURE 0x41
//#define KEY_DELETE 0x49
#define KEY_REFER 0x49
#define KEY_PAGEUP 0x51
#define KEY_RIGHT 0x59

#define KEY_1 0x61
#define KEY_4 0x69
#define KEY_7 0x71
#define KEY_ESCAPE 0x79
#define KEY_2 0x42
#define KEY_5 0x4a
#define KEY_8 0x52
#define KEY_0 0x5a
#define KEY_3 0x62
#define KEY_6 0x6a
#define KEY_9 0x72
#define KEY_ENTER 0x7a

#define KEY_HANDAUTO 0x43
#define KEY_HOST 0x4b
#define KEY_LOOP 0x53
#define KEY_CAAVALARM 0x5b //combined aural and visual alarm 声光报警器
#define KEY_STOP 0x63
#define KEY_IOBR 0x6b
#define KEY_UNIT 0x73
#define KEY_START 0x7b

#define KEY_Q 0x44
#define KEY_A 0x4c
#define KEY_CHANGE 0x54
#define KEY_W 0x5c
#define KEY_S 0x64
#define KEY_Z 0x6c

#define KEY_E 0x74
#define KEY_D 0x7c
#define KEY_X 0x45
#define KEY_R 0x4d
#define KEY_F 0x55
#define KEY_C 0x5d

#define KEY_T 0x65
#define KEY_G 0x6d
#define KEY_V 0x75
#define KEY_Y 0x7d
#define KEY_H 0x46
#define KEY_B 0x4e

#define KEY_U 0x56
#define KEY_J 0x5e
#define KEY_N 0x66
#define KEY_I 0x6e
#define KEY_K 0x76
#define KEY_M 0x7e

#define KEY_O 0x47
#define KEY_L 0x4f
#define KEY_ASTERISK 0x57
#define KEY_P 0x5f
#define KEY_SLASH 0x67
#define KEY_PLUS 0x6f






/*按照新的灯板定义*/

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


/*
     ioctl(fd, CH452_IOCTL_SETLED, 0x0B);
     printf("手动\n");
     sleep(2);
     ioctl(fd, CH452_IOCTL_SETLED, 0x13);
     printf("自动\n");
     sleep(2);
     ioctl(fd, CH452_IOCTL_SETLED, 0x22);
     printf("系统故障\n");
     sleep(2);
     ioctl(fd, CH452_IOCTL_SETLED, 0x0C);
     printf("自检\n");
     sleep(2);
     ioctl(fd, CH452_IOCTL_SETLED, 0x14);
     printf("屏蔽\n");
     sleep(2);
     ioctl(fd, CH452_IOCTL_SETLED, 0x23);
     printf("输出延时\n");
     sleep(2);
     ioctl(fd, CH452_IOCTL_SETLED, 0x0D);
     printf("消音\n");
     sleep(2);
     ioctl(fd, CH452_IOCTL_SETLED, 0x15);
     printf("锁键\n");
     sleep(2);
     ioctl(fd, CH452_IOCTL_SETLED, 0x24);
     printf("直接控制\n");
*/



#define LED_PUT_ON  0x00
#define LED_PUT_OFF 0x01
#define LED_PUT_FLICK   0x02

#endif

