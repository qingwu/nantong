#include "nodeid.h"


nodeID::nodeID( unsigned char hostnum, unsigned char iobrnum,
          unsigned char loopnum, unsigned char nodenum)
    :myhostnum(hostnum),myiobrnum(iobrnum),
      myloopnum(loopnum),mynodenum(nodenum)
{}
nodeID::nodeID( unsigned char hostnum, unsigned char iobrnum,
          unsigned char loopnum, unsigned char nodenum,unsigned char actiontype)
    :myhostnum(hostnum),myiobrnum(iobrnum),
      myloopnum(loopnum),mynodenum(nodenum),myactiontype(actiontype)
{}

nodeID::nodeID( unsigned char hostnum, unsigned char iobrnum,
          unsigned char loopnum, unsigned char nodenum,
               unsigned char actiontype,unsigned char ledtype)
    :myhostnum(hostnum),myiobrnum(iobrnum),
      myloopnum(loopnum),mynodenum(nodenum),
      myactiontype(actiontype),myledtype(ledtype)
{}

//nodeID::nodeID(const nodeID &node)
//{
//    myhostnum = node.myhostnum;
//    myiobrnum = node.myiobrnum;
//    myloopnum = node.myloopnum;
//    mynodenum = node.mynodenum;
//    myactiontype = node.myactiontype;
//    myledtype = node.myledtype;

//}

//nodeID::~nodeID()
//{

//}
#if 1
unsigned char nodeID::gethostnum() const
{
    return myhostnum;
}
unsigned char nodeID::getiobrnum() const
{
    return myiobrnum;
}
unsigned char nodeID::getloopnum() const
{
    return myloopnum;
}
unsigned char nodeID::getnodenum() const
{
     return mynodenum;
}
unsigned char nodeID::getactiontype() const
{
     return myactiontype;
}
unsigned char nodeID::getledtype() const
{
     return myledtype;
}
#endif
