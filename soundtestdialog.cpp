#include "soundtestdialog.h"
#include <QPushButton>
#include <QFrame>

soundTestDialog::soundTestDialog(QWidget *parent) :
    QDialog(parent)
{
    createItems();
    this->setFixedSize(400,340);
    setWindowTitle(tr("声光测试"));
}
void soundTestDialog::createItems()
{
    mainFrame = new QFrame(this);
    mainFrame->setGeometry(QRect(50,50,300,200));
    mainFrame->setFrameShape(QFrame::Box);

    lightAllOnPushButton  = new QPushButton(tr("指示灯全亮"),mainFrame);
    lightAllOffPushButton  = new QPushButton(tr("指示灯全灭"),mainFrame);
    fireAlarmSoundPushButton  = new QPushButton(tr("火警声"),mainFrame);
    watchSoundPushButton  = new QPushButton(tr("监管声"),mainFrame);
    linkSoundPushButton  = new QPushButton(tr("联动声"),mainFrame);
    inicatorSoundPushButton  = new QPushButton(tr("提示声"),mainFrame);
    faultSoundPushButton  = new QPushButton(tr("故障声"),mainFrame);
    printerPushButton  = new QPushButton(tr("打印机"),mainFrame);
    cancelPushButton  = new QPushButton(tr("取消"),this);
    lightAllOnPushButton->setGeometry(QRect(50,10,90,35));
    lightAllOffPushButton->setGeometry(QRect(180,10,90,35));
    fireAlarmSoundPushButton->setGeometry(QRect(50,55,90,35));
    watchSoundPushButton->setGeometry(QRect(180,55,90,35));
    linkSoundPushButton->setGeometry(QRect(50,100,90,35));
    inicatorSoundPushButton->setGeometry(QRect(180,100,90,35));
    faultSoundPushButton->setGeometry(QRect(50,145,90,35));
    printerPushButton->setGeometry(QRect(180,145,90,35));
    cancelPushButton->setGeometry(QRect(160,290,80,35));

    connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
}
