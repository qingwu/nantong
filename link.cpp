#include <QtSql/QSqlError>
#include <QtDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtAlgorithms>
#include <QtCore/QVariant>
#include <QtDebug>
#include <QtCore/QDateTime>
#include "canframe.h"
#include "link.h"

Link::Link()
{
    qFill(linknum, linknum + 100, 0);//
    qFill(linksetnum, linksetnum + 100, 0);

    readlinknum();
}
Link::~Link()
{
}
void Link::readlinknum()
{
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);

    for(int id =1; id <= 100; id++)
    {
        query.exec(QString("select linknum from linkset WHERE linkid = %1").arg(id));
        while (query.next())
        {
            linknum[id-1] = query.value(0).toInt();
        }
    }
    QSqlDatabase::database().commit();
}

void Link::updatelinknum(int id, int newlinknum)
{
    linknum[id-1] = newlinknum;
    linksetnum[id-1] = 0;
}

void Link::DealwithFire( int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodeState)
{
    long long int hostnum = 1;
    int linenum = 1;

    long long int fireunitid = (hostnum<<32) | (iobrnum<<24) | (loopnum<<16) | (nodenum<<8) | linenum;

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec("select * from linkset WHERE linktype < 4");
    while (query.next())
    {
        if(query.value(1).toInt())
        {
            switch(query.value(3).toInt())
            {
            //联动类型 组组 组点 点组 组点
            //点输入
            case 0:
            case 1:
                for(int i = 0;i < 31; i++)
                {
                    long long int k = query.value(i+5).toLongLong();

                    //点输入可以判0终止，组输入不可
                    if(!k)
                    {
                        break;
                    }
                    else
                    {
                        if(fireunitid == k)
                        {
                            linksetnum[query.value(0).toInt()-1]++;
                            if(linksetnum[query.value(0).toInt()-1] >= linknum[query.value(0).toInt()-1])
                            {
                                linkfilter(query.value(0).toInt());
                            }
                            break;
                        }
                    }
                }
                break;
            case 2:
            case 3:
                for(int i = 0;i < 31; i++)
                {
                    long long int k = query.value(i+5).toLongLong();

                    if(k)
                    {
                        long long int type = (long long int)nodetype;
                        if(((k>>8) & 0xFF) == type)
                        {
                            long long int startnode = (k >> 24);
                            long long int endnode = (((k >> 16)&0xFF) | ((k>>24)&0xFFFFFF00));

                            if(((fireunitid >> 8) >= startnode)&&((fireunitid>>8) <= endnode))
                            {
                                linksetnum[query.value(0).toInt()-1]++;
                                if(linksetnum[query.value(0).toInt()-1] >= linknum[query.value(0).toInt()-1])
                                {
                                    linkfilter(query.value(0).toInt());
                                }
                                break;
                            }
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }
            }
        }
    }
    QSqlDatabase::database().commit();
}


void Link::linkfilter(int linkid)
{
    qDebug()<<tr("link ")<<linkid<<tr("is successful");

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from linkset WHERE linkid = %1").arg(linkid));
    while (query.next())
    {
        switch(query.value(3).toInt())
        {
        case 0:
        case 2:
            for(int i = 0;i < 31; i++)
            {
                long long int k = query.value(i+36).toLongLong();

                //点输入可以判0终止，组输入不可
                if(!k)
                {
                    break;
                }
                else
                {
                    emit  transferLinkNode( (k >> 8) & 0xffffffff);
                }
            }
            break;
        case 1:
        case 3:
            for(int i = 0;i < 31; i++)
            {
                long long int k = query.value(i+36).toLongLong();
                if(k)
                {
                    long long int startnode = (k >> 24);
                    long long int endnode = (((k >> 16)&0xFF) | ((k>>24)&0xFFFFFF00));
                    int type = ((k>>8) & 0xFF) ;
                    QSqlQuery query2;
                    query2.setForwardOnly(true);
                    query2.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg(startnode).arg(endnode));
                    while (query2.next())
                    {
                        if(((query2.value(7).toInt() >> 8) & 0xFF) == type)
                        {
                            emit  transferLinkNode( query2.value(5).toInt());
                            //int node = query2.value(5).toInt();
                        }
                    }
                }
                else
                {
                    continue;
                }
            }
            break;
        }
    }
    QSqlDatabase::database().commit();
}

void Link::Clear()
{
    qFill(linksetnum, linksetnum + 100, 0);
//    qFill(linknum, linknum + 100, 0);
}

