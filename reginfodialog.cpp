#include <QtGui/QTextEdit>
#include <QtGui/QPushButton>
#include <QtCore/QVariant>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtDebug>

#include "reginfodialog.h"

RegInfoDialog::RegInfoDialog(QWidget *parent) :
    QDialog(parent)
{
    createItems();
    setWindowTitle(tr("登记信息"));
    this->setFixedSize (QSize(420, 450));
}
RegInfoDialog::~RegInfoDialog()
{
}
void RegInfoDialog::createItems()
{
    regInfoText = new QTextEdit(this);
    regInfoText->setGeometry(QRect(0, 5, 420, 410));
    regInfoText->setReadOnly(true);
    readInfo();
    okPushButton = new QPushButton(tr("确定"), this);
    okPushButton->setGeometry(QRect(160, 425, 100, 24));
    connect(okPushButton, SIGNAL(clicked()), this, SLOT(close()));
}
void RegInfoDialog::readInfo()
{
    int host = 1;
    int iobr = 0;
    int loop = 0;
    int node = 1;
     int registerNodenum = 0;
    char iobrinfo[5];
    char registerNodenuminfo[5];
    memset(iobrinfo, 0, 5);
    memset(registerNodenuminfo, 0 ,5);
    QString text;

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec(QString("select * from iobrinfo WHERE iobrregist = 'true'"));
    while (query.next())
    {
        iobr = query.value(0).toInt();
        sprintf(iobrinfo, "%02d", iobr);
        text += tr("%1板:").arg(iobrinfo);
        QSqlQuery query1;
        query1.setForwardOnly(true);
        for (loop=1;loop<=4;loop++)
        {
            registerNodenum = 0;
            query1.exec(QString("select * from unitset WHERE unitid >= %1 AND unitid <= %2").arg((host << 24) | (iobr << 16) | (loop << 8) | node)
                                            .arg((host << 24) | (iobr << 16) | (loop << 8) | 0xFF));
            while(query1.next())
            {
                if (query1.value(1).toBool() == true)
                {
                    registerNodenum++;
                }
            }
            sprintf(registerNodenuminfo, "%03d", registerNodenum);
            text += tr("%2回路%3在线  ").arg(loop).arg(registerNodenuminfo);
        }
        text += "\n";
    }
    QSqlDatabase::database().commit();
    regInfoText->setPlainText(text);
}
