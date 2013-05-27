#include "serialtestdialog.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFont>

serialTestDialog::serialTestDialog(QWidget *parent) :
    QDialog(parent)
{
    createItems();
    setWindowTitle(tr("串口测试"));
    this->setFixedSize(QSize(400,260));
}
void serialTestDialog::createItems()
{
    QFont f("unifont",11,QFont::Light);
    hintContentLabel = new QLabel(this);
    hintContentLabel->setText(tr("将 RXD（2）与 TXD（3）短接 CRT 口"));
    hintContentLabel->setFont(f);
    hintContentLabel->setGeometry(QRect(60,40,300,30));
    sendLabel = new QLabel(tr("发 送  :  0  1  2  3  4  5  6"),this);
    sendLabel->setFont(f);
    sendLabel->setGeometry(QRect(100,80,300,30));
    receiveLabel = new QLabel(tr("接 收  :"),this);
    receiveLabel->setFont(f);
    receiveLabel->setGeometry(QRect(100,120,50,30));
    receiveContentLabel = new QLabel(this);
    receiveContentLabel->setGeometry(QRect(140,120,200,30));

    okButton = new QPushButton(tr("确定"), this);
    okButton->setGeometry(QRect(120, 200, 80, 24));
    cancelButton = new QPushButton(tr("取消"), this);
    cancelButton->setGeometry(QRect(220, 200, 80, 24));
    cancelButton->setFocus();//set default button
    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
}
void serialTestDialog::ok()
{
    close();
}
