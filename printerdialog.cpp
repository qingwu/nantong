#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QFont>
#include <QDebug>

#include "printerdialog.h"

printerDialog::printerDialog(QWidget *parent) :
    QDialog(parent)
{
    //createItems();
//    setWindowTitle(tr("打印机设置"));
//    this->setFixedSize (QSize(500, 400));
}
printerDialog::printerDialog(int flag, QWidget *parent) :
    QDialog(parent)
{
    printerFlagOfDialog = flag;//default is all !
    createItems();
    setWindowTitle(tr("打印机设置"));
    this->setFixedSize (QSize(500, 400));
}
printerDialog::~printerDialog()
{
}
void  printerDialog::createItems()
{
    //printerFlagOfDialog = flag;//default is all !
    qDebug("printerFlagOfDialog is : %x",printerFlagOfDialog);
    QFont f("unifont",12,QFont::Normal);
    //QFont f1("unifont",10,QFont::Bold);
    //QFrame fm(QFrame::Panel,this);
    tipLabel1 = new QLabel(tr("请选择需要实时打印的事件类型(支持多选)"),this);
    tipLabel2 = new QLabel(tr("点击确认 设置生效！"),this);
    firePushButton = new QPushButton(tr(" 火  警 "),this);
    linkPushButton = new QPushButton(tr(" 联  动 "),this);
    faultPushButton = new QPushButton(tr(" 故  障 "),this);
    allPushButton = new QPushButton(tr(" 全  选 "),this);

    tipLabel1->setGeometry(QRect(80, 40, 340, 24));
    tipLabel2->setGeometry(QRect(160, 340, 170, 24));

    firePushButton->setGeometry(QRect(180,80,60,30));
    linkPushButton->setGeometry(QRect(180,120,60,30));
    faultPushButton->setGeometry(QRect(180,160,60,30));
    allPushButton->setGeometry(QRect(180,200,60,30));
    fireLabel = new QLabel(this);
    faultLabel = new QLabel(this);
    linkLabel = new QLabel(this);
    allLabel = new QLabel(this);

    if((printerFlagOfDialog & 0x1))
        allLabel->setText(tr("是"));
    else
        allLabel->setText(tr("否"));

    if((printerFlagOfDialog >> PRINT_FIRE_FLAG) & 0X01)
        fireLabel->setText(tr("是"));
    else
        fireLabel->setText(tr("否"));

    if( (printerFlagOfDialog >> PRINT_LINK_FLAG) & 0x1)
        linkLabel->setText(tr("是"));
    else
        linkLabel->setText(tr("否"));

    if( (printerFlagOfDialog >> PRINT_FAULT_FLAG) & 0x1)
        faultLabel->setText(tr("是"));
    else
        faultLabel->setText(tr("否"));


    fireLabel->setGeometry(QRect(260,83,24,24));
    linkLabel->setGeometry(QRect(260,123,24,24));
    faultLabel->setGeometry(QRect(260,163,24,24));
    allLabel->setGeometry(QRect(260,203,24,24));
    fireLabel->setFrameShape(QFrame::Panel);
    faultLabel->setFrameShape(QFrame::Panel);
    linkLabel->setFrameShape(QFrame::Panel);
    allLabel->setFrameShape(QFrame::Panel);

    tipLabel1->setFont(f);
    tipLabel2->setFont(f);
    okButton = new QPushButton(tr("确定"), this);
    okButton->setGeometry(QRect(140, 290, 80, 30));
    cancelButton = new QPushButton(tr("取消"), this);
    cancelButton->setGeometry(QRect(270, 290, 80, 30));
    cancelButton->setFocus();//set default button

    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(firePushButton,1);
    buttonGroup->addButton(linkPushButton,2);
    buttonGroup->addButton(faultPushButton,3);
    buttonGroup->addButton(allPushButton,0);

    connect(this,SIGNAL(flagValueChangedSignal()),this,SLOT(flagValueChangedSlot()));
    connect(buttonGroup,SIGNAL(buttonClicked(int)),this,SLOT(buttonClickedSlot(int)));
    connect(okButton,SIGNAL(clicked()),this,SLOT(flagSetSlot()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(this,SIGNAL(printerFlagOfDialogSetSignal(int)),this->parentWidget()->parentWidget(),SLOT(printerFlagOfWidgetSetSlot(int)));
    connect(this ,SIGNAL(printerFlagOfDialogSetSignal(int)),this->parentWidget(),SLOT(getprinterFlag(int)));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
}

void printerDialog::buttonClickedSlot(int id)
{
    qDebug()<<"id is "<<id;
    switch (id)
    {
    case 1:
        if( (printerFlagOfDialog>>PRINT_FIRE_FLAG) & 0x1 )
        {
            printerFlagOfDialog &= 0x00ffffff;
            fireLabel->setText(tr("否"));
            emit  flagValueChangedSignal( );
        }
        else
        {
            printerFlagOfDialog |= (0x01<<PRINT_FIRE_FLAG);
            fireLabel->setText(tr("是"));
            emit  flagValueChangedSignal( );
        }
        //qDebug("fire printerFlagOfDialog is : %x",printerFlagOfDialog);
        break;
    case 2:
        if( (printerFlagOfDialog>>PRINT_LINK_FLAG) & 0x1)
        {
            printerFlagOfDialog &= 0xff00ffff;
            linkLabel->setText(tr("否"));
            emit  flagValueChangedSignal( );
        }
        else
        {
            linkLabel->setText(tr("是"));
            printerFlagOfDialog |= (0x1<<PRINT_LINK_FLAG);
            emit  flagValueChangedSignal( );
        }
       // qDebug("link printerFlagOfDialog is : %x",printerFlagOfDialog);
        break;
    case 3:
        if( (printerFlagOfDialog>>PRINT_FAULT_FLAG) & 0x1)
        {
            printerFlagOfDialog &= 0xffff00ff;
            faultLabel->setText(tr("否"));
            emit  flagValueChangedSignal( );
        }
        else
        {
            printerFlagOfDialog |= (0x01<<PRINT_FAULT_FLAG);
            faultLabel->setText(tr("是"));
            emit  flagValueChangedSignal( );
        }
        //qDebug("fault printerFlagOfDialog is : %x",printerFlagOfDialog);
        break;
    case 0:
        if(printerFlagOfDialog & 0x1)
        {
         //   qDebug()<<"it is all selected!";
            printerFlagOfDialog = 0;
            fireLabel->setText(tr("否"));
            linkLabel->setText(tr("否"));
            faultLabel->setText(tr("否"));
            allLabel->setText(tr("否"));
        }
        else
        {
         //   qDebug()<<"it is none selected!";
            printerFlagOfDialog |= (0x01<<PRINT_FIRE_FLAG);
            printerFlagOfDialog |= (0x01<<PRINT_LINK_FLAG);
            printerFlagOfDialog |= (0x01<<PRINT_FAULT_FLAG);
            printerFlagOfDialog |= 0x01;
            fireLabel->setText(tr("是"));
            linkLabel->setText(tr("是"));
            faultLabel->setText(tr("是"));
            allLabel->setText(tr("是"));
        }
        //qDebug("all printerFlagOfDialog is : %x",printerFlagOfDialog);
        break;
    default:
        break;
    }

#if 0
    //this is the ok version
    qDebug()<<"id is "<<id;
    switch (id)
    {
    case 1:
        if( (printerFlagOfDialog>>PRINT_FIRE_FLAG) & 0x01 )
        {
            printerFlagOfDialog &= 0x07;
            fireLabel->setText(tr("否"));
        }
        else
        {
            printerFlagOfDialog |= (0x01<<PRINT_FIRE_FLAG);
            fireLabel->setText(tr("是"));
        }
        qDebug()<<"fire printerFlagOfDialog is "<<printerFlagOfDialog;
        break;
    case 2:        
        if( (printerFlagOfDialog>>PRINT_LINK_FLAG) & 0x01)
        {
            printerFlagOfDialog &= 0x0b;
            linkLabel->setText(tr("否"));
        }
        else
        {
            linkLabel->setText(tr("是"));
            printerFlagOfDialog |= (0x01<<PRINT_LINK_FLAG);
        }
        qDebug()<<"link printerFlagOfDialog is "<<printerFlagOfDialog;
        break;
    case 3:
        if( (printerFlagOfDialog>>PRINT_FAULT_FLAG) & 0x01)
        {
            printerFlagOfDialog &= 0x0d;
            faultLabel->setText(tr("否"));
        }
        else
        {
            printerFlagOfDialog |= (0x01<<PRINT_FAULT_FLAG);
            faultLabel->setText(tr("是"));
        }
        qDebug()<<"fault printerFlagOfDialog is "<<printerFlagOfDialog;
        break;
    case 0:
        if(printerFlagOfDialog & 0x01)
        {
            qDebug()<<"it is all selected!";
            printerFlagOfDialog = 0;
            fireLabel->setText(tr("否"));
            linkLabel->setText(tr("否"));
            faultLabel->setText(tr("否"));
            allLabel->setText(tr("否"));
        }
        else
        {
            qDebug()<<"it is none selected!";
            printerFlagOfDialog |= (0x01<<PRINT_FIRE_FLAG);
            printerFlagOfDialog |= (0x01<<PRINT_LINK_FLAG);
            printerFlagOfDialog |= (0x01<<PRINT_FAULT_FLAG);
            printerFlagOfDialog |= 0x01;
            fireLabel->setText(tr("是"));
            linkLabel->setText(tr("是"));
            faultLabel->setText(tr("是"));
            allLabel->setText(tr("是"));
        }
        qDebug()<<"printerFlagOfDialog is "<<printerFlagOfDialog;
        break;
    default:
        break;
    }
#endif
}

void printerDialog::flagValueChangedSlot( )
{
    if(((printerFlagOfDialog>>PRINT_FAULT_FLAG) & 0x1) && ((printerFlagOfDialog>>PRINT_FIRE_FLAG) & 0x1) && ((printerFlagOfDialog>>PRINT_LINK_FLAG) & 0x1))
    {
        printerFlagOfDialog |= 0x1;
        allLabel->setText(tr("是"));
    }
    else
    {
        printerFlagOfDialog &= 0xffffff00;
        allLabel->setText(tr("否"));
    }

}

void printerDialog::flagSetSlot()
{
    emit printerFlagOfDialogSetSignal(printerFlagOfDialog);
    this->close();
//    printerFlagOfDialog = 0;
//    if(firePushButton->checkState() == Qt::Checked)
//        printerFlagOfDialog |= (0x01<<PRINT_FIRE_FLAG);
//    if(linkPushButton->checkState() == Qt::Checked)
//        printerFlagOfDialog |= (0x01<<PRINT_LINK_FLAG);
//    if(faultPushButton->checkState() == Qt::Checked)
//        printerFlagOfDialog |= (0x01<<PRINT_FAULT_FLAG);
//    if(allPushButton->checkState() == Qt::Checked)
//        printerFlagOfDialog |= 0x01;


}

