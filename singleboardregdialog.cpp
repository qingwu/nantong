#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QFont>
#include <QMessageBox>
#include <QDebug>

#include "singleboardregdialog.h"

singleBoardRegDialog::singleBoardRegDialog(QWidget *parent) :
    QDialog(parent)
{    
    this->setFixedSize(QSize(480, 240));
    setWindowTitle(tr("接口板登记设置"));
    QFont f("unifont", 12, QFont::Light);
    singleRegInfoLabel = new QLabel(this);
    singleRegInfoLabel->setText(tr("►  单接口板登记 : 请输入要登记的接口板号，并点击确定."));
    singleRegInfoLabel->setAlignment(Qt::AlignCenter);
    singleRegInfoLabel->setFont(f);
    singleRegInfoLabel->setGeometry(QRect(40,20,420,40));

    singleBoardNumLabel = new QLabel(this);
    singleBoardNumLabel->setText(tr("接口板号 :"));
    singleBoardNumLabel->setGeometry(QRect(110,70,70,24));

    boardNumLineEdit = new QLineEdit(this);
    boardNumLineEdit->setGeometry(QRect(195,68,60,30));
    boardNumLineEdit->setAlignment(Qt::AlignCenter);

    singleOkPushButton = new QPushButton(tr("确定"),this);
    singleOkPushButton->setGeometry(QRect(330,70,85,27));

    allRegInfoLabel = new QLabel(this);
    allRegInfoLabel->setText(tr("►  全体接口板登记，请点击下方按钮:"));
    allRegInfoLabel->setGeometry(QRect(40,120,270,40));
    allRegInfoLabel->setFont(f);

    allRegPushButton = new QPushButton(tr("全体接口板登记"),this);
    //allRegPushButton->setText(tr("全体接口板登记"));
    allRegPushButton->setGeometry(QRect(130,170,140,27));

    cancelPushButton = new QPushButton(tr("取消"),this);
    cancelPushButton->setGeometry(QRect(330,170,85,27));


    connect(singleOkPushButton,SIGNAL(clicked()),this,SLOT(emitSingleBoardNum()));
    connect(allRegPushButton,SIGNAL(clicked()),this,SIGNAL(allRegPushButtonClicked()));
    connect(cancelPushButton,SIGNAL(clicked()),this,SIGNAL(close()));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));

}

void singleBoardRegDialog::emitSingleBoardNum()
{
    QString numstr = boardNumLineEdit->text();
    qDebug()<<"signel board to reg is: "<<numstr;
    int num = numstr.toInt();
    if( (num < 35) && (num > 0))
    {
        emit singleBoardNumSignal(num);
    }
    else
    {
        QMessageBox *box = new QMessageBox(tr("接口板登记"),
                                           tr("请输入正确的接口板号[0,34]"),
                                           QMessageBox::Information,
                                           QMessageBox::Ok,
                                           QMessageBox::NoButton,
                                           QMessageBox::NoButton,
                                           this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        connect(this->parentWidget(), SIGNAL(closedialog()), box, SLOT(close()));
        box->exec();
    }
}
singleBoardRegDialog::~singleBoardRegDialog()
{
    delete(singleRegInfoLabel);
    delete(allRegInfoLabel);
    delete(singleBoardNumLabel);
    delete(boardNumLineEdit);
    delete(singleOkPushButton);
    delete(allRegPushButton);
    delete(cancelPushButton);
}


