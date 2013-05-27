/*
 * canframe.h
 * Define the type and the structure of CAN frame
 * Date	: 2011/09/17
 * By	: Staufen Yang

 * Date	: 2012/6/25
 * updated By: qingwu
*/

#ifndef CANFRAME_H
#define CANFRAME_H
class QSemaphore;

//#include <linux/types.h>

#define U32 unsigned int
#define U16 unsigned short
#define S32 int
#define S16 short int
#define U8  unsigned char
#define S8  char

#define UINT16 U16
#define UINT32 U32
#define UINT8  U8

//#define TRUE 	1
//#define FALSE 0

#define BOOL   int
#define VOID   void

#define BYTE   U8
#define WORD   S16
#define DWORD  S32

/**
 * struct can_frame - basic CAN frame structure
 * @pri_ti:    priority and TI code
 * @reserve:   Reserved
 * @src_addr:  the source address of this can frame
 * @dest_addr: the destination address of this can frame
 * @can_dlc:   the data length field of the CAN frame
 * @data:      the CAN frame payload.
 */
#define CAN_FRAME_MAX_DATA_LEN 8

struct can_frame {
 //   BYTE    pri_ti;
//    BYTE    reserve;
    BYTE    priority;
    BYTE    ti;
    BYTE    src_addr;
    BYTE    dest_addr;
    BYTE    can_dlc; /* data length code: 0 .. 8 */
    BYTE	    data[8] __attribute__((aligned(8)));

};
#define CONTROLLADDR 100
#define Priority0 0x00
#define Priority1 0x01
#define Priority2 0x02
#define Priority3 0x03
#define Priority4 0x04
#define Priority5 0x05

#define TI_BroadcastRegister   0x00
#define TI_Rst                 0x01
#define TI_SelfCheck           0x02
#define TI_Work                0x03

#define TI_Ack                 0x04
#define TI_NormalAck           0x05

#define TI_Inspection           0x06

#define TI_BreakDown           0x07
#define TI_BreakDownRecover    0x08
#define TI_ActionFailed          0x09

#define TI_SignalBoardReqRegisterInf 0x11
#define TI_RegisterInfReport        0x12
#define TI_RegisterOver             0x13
#define TI_SignalBoardRegister      0x14
#define TI_KeyInfReport             0x15
#define TI_SetLED                   0x16
#define TI_FireDisplay         0x17
#define TI_FireAlarm           0x0a

#define TI_FeedBack            0x0b
#define TI_FeedBackRecover     0x0c
#define TI_LinkageStart        0x0d
#define TI_LinkageStop         0x0e

#define TI_LinkageAction       0x0f



#endif /* CANFRAME_H */
