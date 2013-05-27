#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlQuery>

class Database : public QSqlQuery
{
public:
    Database();
    ~Database();
    bool createConnection(const QString &, const QString &);
    bool initHostInfo();
    bool initFireHis();
    bool initFaultHis();
    bool initFeedbackHis();
    bool initOperationHis();
    bool initUnitSet();
        bool initShieldHis();
    bool initBusSet();
    bool initLinkSet();
    bool initIobrInfo();
    bool initPasswordInfo();
    bool deletetable(const QString &);
    bool cleartable(const QString &);
private:
    void writeHostInfo();
    void writeOperationHis();
    void writeShieldHis();
    void writeFireHis();
    void writeFaultHis();
    void writeUnitSet();
    void writeBusSet();
    void writeLinkSet();
    void writeIobrInfo();
    void writePasswordInfo();

    void writeZongxianInfo();
    void writeZhikongInfo();

};

#endif // DATABASE_H
