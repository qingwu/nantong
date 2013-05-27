#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QMessageBox>
#include <QtGui/QFont>
#include <QtCore/QStringList>
#include <QtCore/QProcess>
#include <QtDebug>

#include "timesetdialog.h"

TimeSetDialog::TimeSetDialog(QWidget *parent) :
    QDialog(parent)
{
    nDateTime = QDateTime::currentDateTime();
    createItems();
    setWindowTitle(tr("时间设置"));
    this->setFixedSize (QSize(400, 280));

    connect(okButton, SIGNAL(clicked()), this, SLOT(timeResetSlot()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(this, SIGNAL(setSuccessful()), this, SLOT(close()));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
}
TimeSetDialog::~TimeSetDialog()
{

}
void TimeSetDialog::createItems()
{
    date = new QLabel(tr("日期:"), this);
    time = new QLabel(tr("时间:"), this);
    year = new QLabel(tr("年"), this);
    month = new QLabel(tr("月"), this);
    day = new QLabel(tr("日"), this);
    timemark1 = new QLabel(tr(":"), this);
    timemark2 = new QLabel(tr(":"), this);

    QFont f("unifont",10,QFont::Bold);

    yearSpinBox=new QSpinBox(this);
    yearSpinBox->setFont(f);
    yearSpinBox->setRange(1990,2050);
    yearSpinBox->setSingleStep(1);
    yearSpinBox->setValue(nDateTime.date().year());

    monthSpinBox=new QSpinBox(this);
    monthSpinBox->setFont(f);
    monthSpinBox->setRange(1,12);
    monthSpinBox->setSingleStep(1);
    monthSpinBox->setValue(nDateTime.date().month());

    daySpinBox=new QSpinBox(this);
    daySpinBox->setFont(f);
    daySpinBox->setRange(1,31);
    daySpinBox->setSingleStep(1);
    daySpinBox->setValue(nDateTime.date().day());

    hourSpinBox=new QSpinBox(this);
    hourSpinBox->setFont(f);
    hourSpinBox->setRange(0,23);
    hourSpinBox->setSingleStep(1);
    hourSpinBox->setValue(nDateTime.time().hour());

    minuteSpinBox=new QSpinBox(this);
    minuteSpinBox->setFont(f);
    minuteSpinBox->setRange(0,59);
    minuteSpinBox->setSingleStep(1);
    minuteSpinBox->setValue(nDateTime.time().minute());

    secondSpinBox=new QSpinBox(this);
    secondSpinBox->setFont(f);
    secondSpinBox->setRange(0,59);
    secondSpinBox->setSingleStep(1);
    secondSpinBox->setValue(nDateTime.time().second());

    /*setGeometry*/
    date->setGeometry(QRect(50, 50, 30, 24));
    yearSpinBox->setGeometry(QRect(90, 50, 60, 24));
    year->setGeometry(QRect(160, 50, 20, 24));
    monthSpinBox->setGeometry(QRect(190, 50, 60, 24));
    month->setGeometry(QRect(260, 50, 20, 24));
    daySpinBox->setGeometry(QRect(290, 50, 60, 24));
    day->setGeometry(QRect(360, 50, 20, 24));

    time->setGeometry(QRect(50, 100, 30, 24));
    hourSpinBox->setGeometry(QRect(90, 100, 60, 24));
    timemark1->setGeometry(QRect(160, 100, 20, 24));
    minuteSpinBox->setGeometry(QRect(190, 100, 60, 24));
    timemark2->setGeometry(QRect(260, 100, 20, 24));
    secondSpinBox->setGeometry(QRect(290, 100, 60, 24));

    okButton = new QPushButton(tr("确定"), this);
    okButton->setGeometry(QRect(80, 200, 80, 24));
    cancelButton = new QPushButton(tr("取消"), this);
    cancelButton->setGeometry(QRect(240, 200, 80, 24));
    cancelButton->setFocus();//set default button
}

void TimeSetDialog::timeResetSlot()
{
    qDebug()<<"in 1 time reset slot";
    QDate tmpDate;
    QTime tmpTime;
    QProcess *dateProcess = new QProcess(this);
    QStringList hwClockSetArgs;
    tmpDate.setDate(yearSpinBox->value(),monthSpinBox->value(),daySpinBox->value());
    tmpTime.setHMS(hourSpinBox->value(),minuteSpinBox->value(),secondSpinBox->value());
    nDateTime.setDate(tmpDate);
    nDateTime.setTime(tmpTime);
    qDebug()<<"in 2 time reset ";
    hwClockSetArgs <<"-s";
    hwClockSetArgs << nDateTime.toString("MMddhhmmyyyy.ss");
    dateProcess->start("date",hwClockSetArgs);
    dateProcess->waitForFinished(3);
    qDebug()<<"in 3 time reset ";
    hwClockSetArgs.clear();
    hwClockSetArgs<<"-w";
    dateProcess->start("hwclock",hwClockSetArgs);
    dateProcess->waitForFinished(3);

    qDebug()<<"time reset successfully!";
    QMessageBox *box = new QMessageBox(tr("time reset"),
                                        tr("Great ,time reset successfully O(∩_∩)O~!"),
                                        QMessageBox::Information,
                                        QMessageBox::Ok,
                                        QMessageBox::NoButton,
                                        QMessageBox::NoButton,
                                        this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
    box->exec();

    yearSpinBox->setValue(nDateTime.date().year());
    monthSpinBox->setValue(nDateTime.date().month());
    daySpinBox->setValue(nDateTime.date().day());
    hourSpinBox->setValue(nDateTime.time().hour());
    minuteSpinBox->setValue(nDateTime.time().minute());
    secondSpinBox->setValue(nDateTime.time().second());

    delete(dateProcess);
    emit setSuccessful();
}
