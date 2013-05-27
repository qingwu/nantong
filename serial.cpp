#include <QString>
#include <QTextCodec>
#include <QByteArray>
#include "serial.h"

int speed_arr[] = {B115200,B38400,B9600,B4800,B2400};
int speed_name[] = {115200,38400,9600,4800,2400};

int OpenDev(char *Dev)
{
    int fd = open( Dev , O_RDWR);
    if (  fd < 0)
    {
        printf("can't open Serial Port\n");
        //exit(0);
        return -1;
    }
    else
        return fd;
}

int set_Opt(int fd ,int nSpeed ,int nBits,int nStop,int nEven )
{
    int i;
    struct termios oldtio,newtio;
    //store existing option
    if(tcgetattr( fd , &oldtio ) != 0){
        perror("get old termios error\n");
        return -1;
    }
    // bzero(&newtio,sizeof(struct termios));
    newtio = oldtio;//can't figure out what's the point here..

    /*SHIELDING OTHER FLAGS */
    cfmakeraw(&newtio);
    newtio.c_cflag &= ~CSIZE;

    /*SET SPEED */
    for(i = 0 ; i < int(sizeof(speed_name)/sizeof(int)); i++)
    {
        if ( nSpeed == speed_name[i] )
        {
            cfsetispeed(&newtio,speed_arr[i]);
            cfsetospeed(&newtio,speed_arr[i]);
            printf("baud is set to %d\n",speed_name[i]);
        }
    }

    /*SET DATABITS*/
    switch( nBits )
    {
    case 8:
        newtio.c_cflag |= CS8;  break;
    case 7:
        newtio.c_cflag |= CS7;  break;
    }

    /*SET EVENBITS*/
    switch ( nEven )
    {
    case 'n':
    case 'N':
        newtio.c_cflag &= ~PARENB;
        // newtio.c_cflag &= ~INPCK;
        break;
    case 'o':
    case 'O':
        newtio.c_cflag |= (PARODD | PARENB);  /* 设置为奇效验*/
        newtio.c_iflag |= INPCK;             /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        newtio.c_cflag |= PARENB;     /* Enable parity */
        newtio.c_cflag &= ~PARODD;   /* 转换为偶效验*/
        newtio.c_iflag |= INPCK;       /* Disnable parity checking */
        break;
    case 'S':
    case 's':  /*as no parity*/
        newtio.c_cflag &= ~PARENB;
        newtio.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr,"Unsupported parity\n");
        return (SERIAL_FALSE);
    }

    /*SET STOPBITS*/
    switch ( nStop )
    {
    case 1 :
        newtio.c_cflag &= ~CSTOPB;
        break;
    case 2:
        newtio.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr,"Unsupported stop bits\n");
        return (SERIAL_FALSE);
    }

    newtio.c_cflag &= ~CRTSCTS;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd,TCIOFLUSH);
    if( (tcsetattr(fd, TCSANOW , &newtio) ) != 0)
    {
        perror("set error");
        return -1;
    }
    printf("set done!\n");
    return 0;

}

//void doPrint( QString str1, QString str2, QString str3)
//{
//    //printer the event selected
//    char ToHanzi[2];
//    char p = 10;
//    char zouzhi[3] = { 27, 74, 20};
//    ToHanzi[0] = 28; ToHanzi[1] = 38;
//    write(fd_printer,ToHanzi,sizeof(ToHanzi)/sizeof(char));//to hanzi

//    QTextCodec *gbk = QTextCodec::codecForName("gb18030");
//    QByteArray encodedString1 = gbk->fromUnicode(str1.toLocal8Bit());
//    QByteArray encodedString2 = gbk->fromUnicode(str2.toLocal8Bit());
//    QByteArray encodedString3 = gbk->fromUnicode(str3.toLocal8Bit());
//    char *pstr1 = encodedString1.data();
//    char *pstr2 = encodedString2.data();
//    char *pstr3 = encodedString3.data();
//    write(fd_printer, pstr1, encodedString1.count());//string to print
//    write(fd_printer,&p,1);//print current row and line feed
//    write(fd_printer, pstr2, encodedString2.count());//string to print
//    write(fd_printer,&p,1);//print current row and line feed
//    write(fd_printer, pstr3, encodedString3.count());//string to print
//    write(fd_printer,&p,1);//print current row and line feed

//    write(fd_printer,zouzhi, sizeof(zouzhi)/sizeof(char));
//    qDebug()<<"print data over!";
//}

#if 0
/***@serial_qingwu 	main() for test....*/
int main(int argc, char **argv)
{
    int fd;
    int nread,num;
    fd_set rfds;
    struct timeval tv;
    char buff[512];
    char *dev ="/dev/s3c_serial1";
    char w[3];
    char p = 10;
    fd = OpenDev(dev);
    if (fd>0)
        printf(" s3c_serial1 open \n");
    else
    {
        printf("Can't Open Serial Port!\n");
        exit(0);
    }
    set_Opt( fd , 9600 , 8 , 1 , 'n' );
    w[0] = 27;
    w[1] = 49;
    w[2] = 3;
    printf("write data here.\n");

    //write(fd,w,3);

    write(fd,"hello hust1\nhello hust2\n",25);
    write(fd,&p,1);
    printf("write data over.use str+0A\n");

    while(1)
    {
        FD_ZERO(&rfds);
        FD_SET(fd,&rfds);
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        num = select(fd+1,&rfds,NULL,NULL,&tv);
        if(num == 0)
            printf(" no receive ,delay here\n");

        else
        {
            printf(" start to read ");
            nread = read(fd,buff,512);
            printf("Len %d ",nread);
            buff[nread+1]='\0';
            printf(" the data read is : %s \n",buff);
        }
    }
    close(fd);
}


#endif

