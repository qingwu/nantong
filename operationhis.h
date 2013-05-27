#ifndef OPERATIONHIS_H
#define OPERATIONHIS_H
#include <QObject>
#include <QString>
class OperationHis : public QObject
{
        Q_OBJECT
public:
    OperationHis();
    ~OperationHis();

   static void Update(const QString &);
   private:
};

#endif // OPERATIONHIS_H
