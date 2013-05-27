#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <unistd.h>     /*Unix标准函数定义*/
#include <sys/types.h>  /**/
#include <sys/stat.h>   /**/
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <sys/select.h>
#include <QString>


//#define      TRUE   1
#define      SERIAL_FALSE  0

int OpenDev(char *Dev);
int set_Opt(int fd ,int nSpeed ,int nBits,int nStop,int nEven );
#endif // SERIAL_H
