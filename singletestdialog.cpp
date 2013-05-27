#include <QLabel>
#include <QSpinBox>
#include <QPushButton>

#include "singletestdialog.h"
#define dialogLength 400
#define dialogWidth 280
singleTestDialog::singleTestDialog(QWidget *parent) :
    QDialog(parent)
{
    createItems();
    setWindowTitle(tr("单步测试"));
    this->setFixedSize(QSize(dialogLength,dialogWidth));
}
void singleTestDialog::createItems()
{
    QFont f("unifont",11,QFont::Light);
    warningLabel = new QLabel(this);
    warningLabel->setText(tr("警告： 单步测试状态下，系统将失去报警功能"));
    warningLabel->setAlignment(Qt::AlignCenter);
    warningLabel->setFont(f);
    warningLabel->setGeometry(QRect(60,40,320,30));
    iobrLabel = new QLabel(tr("接口板号 :"),this);
    iobrLabel->setGeometry(QRect(130,80,80,30));
    loopLabel = new QLabel(tr("回路号 :"),this);
    loopLabel->setGeometry(QRect(130,120,80,30));
    addrLabel = new QLabel(tr("地址号 :"),this);
    addrLabel->setGeometry(QRect(130,160,80,30));
    iobrSpinBox = new QSpinBox(this);
    loopSpinBox = new QSpinBox(this);
    addrSpinBox = new QSpinBox(this);/*
    iobrSpinBox->setValue(1);
    loopSpinBox->setValue(1);
    addrSpinBox->setValue(1);*/
    iobrSpinBox->setRange(1,32);
    loopSpinBox->setRange(1,4);
    addrSpinBox->setRange(1,256);
    iobrSpinBox ->setGeometry(QRect(230,80,60,30));
    loopSpinBox ->setGeometry(QRect(230,120,60,30));
    addrSpinBox ->setGeometry(QRect(230,160,60,30));


    okPushButton = new QPushButton(tr("确定"), this);
    okPushButton->setGeometry(QRect(110, 230, 80, 24));
    cancelPushButton = new QPushButton(tr("取消"), this);
    cancelPushButton->setGeometry(QRect(210, 230, 80, 24));
    cancelPushButton->setFocus();//set default button
    connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));

}
void singleTestDialog::ok()
{
    close();
}
