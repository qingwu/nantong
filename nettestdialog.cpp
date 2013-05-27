#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QSpinBox>
#include <QFrame>
#include "nettestdialog.h"
#define dialogLength 400
#define dialogWidth 280
netTestDialog::netTestDialog(QWidget *parent) :
    QDialog(parent)
{
    createItems();
    setWindowTitle(tr("联网测试"));
    this->setFixedSize(QSize(dialogLength,dialogWidth));
}

void netTestDialog::createItems()
{
    mainFrame = new QFrame(this);
    mainFrame->setGeometry(QRect(80,60,240,120));
    mainFrame->setFrameShape(QFrame::Box);
    hostNumlabelA = new QLabel(tr("机号"),mainFrame);
    hoseNumLabelB = new QLabel(tr("机号"),mainFrame);
    sendLabel = new QLabel(mainFrame);
    receiveLabel = new QLabel(mainFrame);
    hostNumSpinBoxA = new QSpinBox(mainFrame);
    hostNumSpinBoxB = new QSpinBox(mainFrame);
    hostNumSpinBoxA->setRange(1,20);
    hostNumSpinBoxB->setRange(1,20);
    hostNumlabelA->setGeometry(QRect(30, 20, 30, 30));
    hoseNumLabelB->setGeometry(QRect(170, 20, 30, 30));
    hostNumSpinBoxA->setGeometry(QRect(25, 70, 50, 30));
    hostNumSpinBoxB->setGeometry(QRect(165, 70, 50, 30));
    sendLabel->setGeometry(QRect(80,20,80,30));
    receiveLabel->setGeometry(QRect(80,70,80,30));
    sendLabel->setText(tr("< < < < < < "));
    receiveLabel->setText(tr("> > > > > > "));

    okPushButton = new QPushButton(tr("确定"), this);
    okPushButton->setGeometry(QRect(110, 230, 80, 24));
    cancelPushButton = new QPushButton(tr("取消"), this);
    cancelPushButton->setGeometry(QRect(210, 230, 80, 24));
    cancelPushButton->setFocus();//set default button
    connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));

}
void netTestDialog::ok()
{
    close();
}
