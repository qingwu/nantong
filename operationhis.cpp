#include <QtSql/QSqlError>
#include <QtDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtAlgorithms>
#include <QtCore/QVariant>
#include <QtDebug>
#include <QtCore/QDateTime>
#include "operationhis.h"

OperationHis::OperationHis()
{
}
OperationHis::~OperationHis()
{
}
void OperationHis::Update(const QString &operation_string)
{
    int eventMinId = 0;
    int enentMaxId = 0;

    QSqlDatabase::database().transaction();

    QSqlQuery queryMin;
    queryMin.setForwardOnly(true);

    queryMin.exec("select MIN(id) from operationhis");
    while (queryMin.next())
    {
        eventMinId = queryMin.value(0).toInt();
    }

    QSqlQuery queryMax;
    queryMax.setForwardOnly(true);

    queryMax.exec("select MAX(id) from operationhis");
    while (queryMax.next())
    {
        enentMaxId = queryMax.value(0).toInt();
    }
    QDateTime datetime = QDateTime::currentDateTime();

    QSqlQuery query;
    query.prepare("INSERT INTO operationhis(id,  time, str)"
                  "VALUES (?, ?, ?)");

    query.addBindValue(enentMaxId+1);
    query.addBindValue(datetime);
    query.addBindValue(operation_string);

    bool bSuccess = query.exec();
    if (!bSuccess)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText()
                 <<QString(QObject::tr("insert operationhis error"));
    }

    if (enentMaxId - eventMinId >9999)
    {

        QSqlQuery queryDel;
        queryDel.exec(QString("delete  from operationhis where id = %1").arg(eventMinId));
        bool bSuccess = queryDel.exec();
        if (!bSuccess)
        {
            QSqlError lastError = queryDel.lastError();
            qDebug() << lastError.driverText()
                     <<QString(QObject::tr("delete operationhis error"));
        }

    }
    QSqlDatabase::database().commit();

}
