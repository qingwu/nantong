#ifndef LINK_H
#define LINK_H
#include <QObject>
#include <QDateTime>
class Link : public QObject
{
    Q_OBJECT
public:
    Link();
    ~Link();
     void Clear();
private:
    int linknum[100];
    int linksetnum[100];

    void linkfilter(int linkid);


private slots:
public slots:

    void readlinknum();
    void updatelinknum(int id, int newlinknum);
    void DealwithFire( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodestate);

signals:
    void transferLinkNode(int nodeid);

};

#endif // LINK_H
