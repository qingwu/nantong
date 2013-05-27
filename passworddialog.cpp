#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include<QtCore/QVariant>
#include "passworddialog.h"

PasswordDialog::PasswordDialog(QWidget *parent) :
    QDialog(parent)
{
}
PasswordDialog::PasswordDialog(int index, QWidget *parent) :
    QDialog(parent)
{
    type = index;
    matchFlag = false;
    createItems();
    this->setFixedSize (QSize(200, 120));
}
PasswordDialog::~PasswordDialog()
{
}
void PasswordDialog::createItems()
{
    infoLabel = new QLabel(this);//infoLabel = new QLabel(tr("请输入密码:"), this);
    infoLabel->setGeometry(QRect(10, 20, 180, 24));
    infoLabel->setAlignment(Qt::AlignLeft);//Qt::AlignCenter
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setGeometry(QRect(10, 60, 100, 24));
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    intoPushButton = new QPushButton(tr("确定"), this);
    intoPushButton->setGeometry(QRect(130, 60, 40, 24));
    switch(type)
    {

    case 1:
    {
        //        setWindowTitle(tr("输入复位密码"));
        //        infoLabel = new QLabel(tr("请输入密码:"), this);
        setWindowTitle(tr("系统复位"));
        infoLabel->setText(tr("请输入复位密码:"));
        connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
    }
    break;
    case 2:
    {
        //setWindowTitle(tr("输入设置密码"));
        setWindowTitle(tr("系统设置"));
        infoLabel->setText(tr("请输入设置密码:"));//infoLabel = new QLabel(tr("请输入设置密码:"), this);
        connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
    }
    break;
    case 3:
    {
        setWindowTitle(tr("输入手动启动密码"));
    }
    break;
    case 4:
    {
        setWindowTitle(tr("输入屏蔽设置密码"));
    }
    break;
    case 5:
    {
         setWindowTitle(tr("输入锁键解锁密码"));
    }
    break;
    }
    connect(intoPushButton, SIGNAL(clicked()), this, SLOT(inToSlot()));

}
void PasswordDialog::inToSlot()
{
    int id = 0;
    if((type == 1) || (type == 3) || (type == 4) || (type ==  5))
      {
          id = 1;
      }
      else
      {
          id = 2;
      }
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
 //   query.exec(QString("select * from password WHERE id=%1").arg(type));
        query.exec(QString("select * from password WHERE id=%1").arg(id));
    if (query.next())
    {
        if (query.value(1).toString() == passwordLineEdit->text())
        {
            this->close();
            matchFlag = true;
        }
        else
        {
            matchFlag = false;
            QMessageBox *box = new QMessageBox(tr("警告"),
                                               tr("密码错误，请重新输入!"),
                                               QMessageBox::Warning,
                                               QMessageBox::Ok,
                                               QMessageBox::NoButton,
                                               QMessageBox::NoButton,
                                               this);
            box->setAttribute(Qt::WA_DeleteOnClose);
            connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
            box->exec();
        }
    }
    QSqlDatabase::database().commit();
}
