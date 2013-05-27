#ifndef EVENTYPE_H
#define EVENTYPE_H

class nodeID
{
public:

    nodeID( unsigned char hostnum, unsigned char iobrnum,
              unsigned char loopnum, unsigned char nodenum);

    nodeID( unsigned char hostnum, unsigned char iobrnum,
              unsigned char loopnum, unsigned char nodenum,unsigned char actiontype);

    nodeID( unsigned char hostnum, unsigned char iobrnum,
              unsigned char buspanelnum, unsigned char busnodenum,
                unsigned char actiontype,unsigned char ledtype);
    /*for 33 louxian:
    nodeID( unsigned char louxiannum, unsigned char nodeiobrnum,
              unsigned char nodeloopnum, unsigned char nodenodenum,
                 unsigned char nodefiretype,unsigned char nodefirestate);
                //最后两个：报警点类型：温头/烟头 报警状态：火警/预警
    */

    //nodeID(const nodeID &);

    //    nodeID::~nodeID();

#if 1
    unsigned char gethostnum() const;
    unsigned char getiobrnum() const;
    unsigned char getloopnum() const;
    unsigned char getnodenum() const;
    unsigned char getactiontype() const;
    unsigned char getledtype() const;
#endif

  private:
    unsigned char myhostnum, myiobrnum, myloopnum, mynodenum;
    unsigned char myactiontype;
    unsigned char myledtype;
};

#endif // EVENTYPE_H
