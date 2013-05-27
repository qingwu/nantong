#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>
#include <QtDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include<QtCore/QVariant>

#include "linkdialog.h"
#include "ddlinksetwidget.h"
#include "dzlinksetwidget.h"
#include "zdlinksetwidget.h"
#include "zzlinksetwidget.h"

LinkDialog::LinkDialog(QWidget *parent) :
    QDialog(parent)
{

}

LinkDialog::LinkDialog(bool editFlag, QWidget *parent):
    QDialog(parent)
{
    nowcopynum = 1;
    editEnableFlag = editFlag;
    createItems();
    setWindowTitle(tr("联动设置"));
    this->setFixedSize (QSize(800,550));
    readfirstLink(1);
    //    QObject::connect(numSpinbox,SIGNAL(valueChanged(int)),this,SLOT(readLinktype(int)));
    //    QObject::connect(numSpinbox,SIGNAL(valueChanged(int)),this,SLOT(fuzhilinkid(int)));
    QObject::connect(this,SIGNAL(linknumChange(int)),this,SLOT(readLinktype(int)));
    QObject::connect(this,SIGNAL(linknumChange(int)),this,SLOT(fuzhilinkid(int)));
    QObject::connect(numLineedit,SIGNAL(textChanged(QString)),this,SLOT(dealwithNumTextchange(QString)));
    QObject::connect(linkTabWidget,SIGNAL(currentChanged(int)),this,SLOT(readLinktype2(int)));
    QObject::connect(previousbutton,SIGNAL(clicked()),this,SLOT(previousbuttonClickSlot()));
    QObject::connect(nextbutton,SIGNAL(clicked()),this,SLOT(nextbuttonClickSlot()));
    QObject::connect(addbutton,SIGNAL(clicked()),this,SLOT(addbuttonClickSlot()));
    QObject::connect(deletebutton,SIGNAL(clicked()),this,SLOT(deletebuttonClickSlot()));


    connect(firstLinkSetWidget,SIGNAL(changeTypeLabel(int)),this,SLOT(changetypeLabel(int)));
    connect(secondLinkSetWidget,SIGNAL(changeTypeLabel(int)),this,SLOT(changetypeLabel(int)));
    connect(thirdLinkSetWidget,SIGNAL(changeTypeLabel(int)),this,SLOT(changetypeLabel(int)));
    connect(forthLinkSetWidget,SIGNAL(changeTypeLabel(int)),this,SLOT(changetypeLabel(int)));

    connect(firstLinkSetWidget,SIGNAL(databaseChange()),this,SLOT(updateLinkcount()));
    connect(secondLinkSetWidget,SIGNAL(databaseChange()),this,SLOT(updateLinkcount()));
    connect(thirdLinkSetWidget,SIGNAL(databaseChange()),this,SLOT(updateLinkcount()));
    connect(forthLinkSetWidget,SIGNAL(databaseChange()),this,SLOT(updateLinkcount()));

    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));

}

void LinkDialog::createItems()
{
    linkTabWidget = new QTabWidget;
    firstLinkSetWidget = new ddLinkSetWidget(editEnableFlag,this);
    //    secondLinkSetWidget = new dzLinkSetWidget(this);
    //    thirdLinkSetWidget = new zdLinkSetWidget(this);
    //    forthLinkSetWidget = new zzLinkSetWidget(this);
    linkTabWidget->addTab(firstLinkSetWidget,tr("点输入点输出"));
    connect(firstLinkSetWidget,SIGNAL(transferddLinknum(int,int)),this,SIGNAL(transferLinknum(int,int)));
    //上面是第一个窗口的

    secondLinkSetWidget = new dzLinkSetWidget(editEnableFlag,this);
    linkTabWidget->addTab(secondLinkSetWidget,tr("点输入组输出"));
    connect(secondLinkSetWidget,SIGNAL(transferdzLinknum(int,int)),this,SIGNAL(transferLinknum(int,int)));
    //上面是第二个窗口的

    thirdLinkSetWidget = new zdLinkSetWidget(editEnableFlag,this);
    linkTabWidget->addTab(thirdLinkSetWidget,tr("组输入点输出"));
    connect(thirdLinkSetWidget,SIGNAL(transferzdLinknum(int,int)),this,SIGNAL(transferLinknum(int,int)));
    //上面是第三个窗口的

    forthLinkSetWidget = new zzLinkSetWidget(editEnableFlag,this);
    linkTabWidget->addTab(forthLinkSetWidget,tr("组输入组输出"));
    connect(forthLinkSetWidget,SIGNAL(transferzzLinknum(int,int)),this,SIGNAL(transferLinknum(int,int)));
    //上面是第四个窗口的

    toplayout = new QHBoxLayout();
    toplayout->setMargin(0);
    numLabel = new QLabel("NO");

    //    numSpinbox = new QSpinBox;
    //    numSpinbox->setMaximumSize(60,24);
    typeLabel = new QLabel("未知联动");
    //    typeLabel->setText("点点联动");
    //    typeLabel->setText("点组联动");
    //    typeLabel->setText("组点联动");
    //    typeLabel->setText("组组联动");
    //    typeLabel->setText("未知联动");
    int count = 0;
    int realset_count = 0;
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec("select MAX(linkid) from linkset where linktype < 4 ");
    while (query.next())
    {
        count = query.value(0).toInt();
    }

    QSqlQuery realquery;
    realquery.setForwardOnly(true);
    realquery.exec("select linktype from linkset ");
    while (realquery.next())
    {
        if(realquery.value(0).toInt() < 4)
        {
            realset_count++;
        }
    }
    QSqlDatabase::database().commit();

    //    numSpinbox->setRange(1,count);
    //    numSpinbox->setAlignment(Qt::AlignRight);

    linkcountLabel = new QLabel(this);
    linkcountLabel->setText(QString("共%1条联动").arg(realset_count));

    linkidValidator = new QIntValidator(1,count,this);
    numLineedit = new QLineEdit(this);
    numLineedit->setValidator(linkidValidator);
    numLineedit->setFixedSize (QSize(60,30));
    int num = 1 ;
    numLineedit->setText(QString("%1").arg(num));
    //    firstlinkinNum->setReadOnly(true);
    // QObject::connect(firstlinkinNum,SIGNAL(textChanged(QString)),this,SLOT(linknumreturn(QString)));

    previousbutton = new QPushButton(tr("上一条"));
    nextbutton = new QPushButton(tr("下一条"));

    addbutton = new QPushButton(tr("添加"));
    deletebutton = new QPushButton(tr("删除"));



    copyPushButton = new QPushButton(tr("复制"));
    connect(copyPushButton,SIGNAL(clicked()),this,SLOT(copyPushButtonClicked()));

    pastePushButton = new QPushButton(tr("粘贴"));
    connect(pastePushButton,SIGNAL(clicked()),this,SLOT(pastePushButtonClicked()));

    toplayout->addWidget(linkcountLabel);
    toplayout->addWidget(numLabel);
    //    toplayout->addWidget(numSpinbox);
    toplayout->addWidget(numLineedit);
    toplayout->addWidget(typeLabel);
    toplayout->addWidget(previousbutton);
    toplayout->addWidget(nextbutton);
    toplayout->addStretch(1);
    toplayout->addWidget(addbutton);
    toplayout->addWidget(deletebutton);
    toplayout->addStretch(1);
    toplayout->addWidget(copyPushButton);
    toplayout->addWidget(pastePushButton);

    bottomlayout = new QHBoxLayout();
    bottomlayout->setMargin(0);
    bottomlayout->addWidget(linkTabWidget);
    mainlayout = new QVBoxLayout();
    mainlayout->setMargin(0);
    mainlayout->addLayout(toplayout);
    mainlayout->addLayout(bottomlayout);
    this->setLayout(mainlayout);

    if(!editEnableFlag)
    {
        copyPushButton->setVisible(false);
        pastePushButton->setVisible(false);
    }
}

LinkDialog::~LinkDialog()
{
}

void LinkDialog::readLinktype(int)
{
    //    linkTabWidget->setCurrentIndex(0);
    int num = linkTabWidget->currentIndex();
    //    qDebug()<<"num"<<num;
    //  int id = numSpinbox->value();
    int id = numLineedit->text().toInt();
    if(id == 0)
    {
        id = 1;
    }

    int k=0;



    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);

    query.exec(QString("select linktype from linkset WHERE linkid = %1").arg(id));
    while (query.next())
    {
        k = query.value(0).toInt();
    }
    //printf("id号是%d联动类型码为%d ",id,k);
    switch (k)
    {
    case 0:
        typeLabel->setText("点点联动");
        linkTabWidget->setCurrentIndex(0);
        break;
    case 1:
        typeLabel->setText("点组联动");
        linkTabWidget->setCurrentIndex(1);
        break;
    case 2:
        typeLabel->setText("组点联动");
        linkTabWidget->setCurrentIndex(2);
        break;
    case 3:
        typeLabel->setText("组组联动");
        linkTabWidget->setCurrentIndex(3);
        break;
    case 4:
        typeLabel->setText("未知联动");
        linkTabWidget->setCurrentIndex(0);
        break;
    }

    switch (num)
    {
    case 0:
        firstLinkSetWidget->clearfirstWidget();
        qFill(firstLinkSetWidget->linkin,firstLinkSetWidget->linkin + 31, 0);
        qFill(firstLinkSetWidget->linkout, firstLinkSetWidget->linkout + 31, 0);
        break;
    case 1:
        secondLinkSetWidget->clearsecondWidget();
        qFill(secondLinkSetWidget->linkin,secondLinkSetWidget->linkin + 31, 0);
        qFill(secondLinkSetWidget->linkout, secondLinkSetWidget->linkout + 31, 0);
        break;
    case 2:
        thirdLinkSetWidget->clearthirdWidget();
        qFill(thirdLinkSetWidget->linkin,thirdLinkSetWidget->linkin + 31, 0);
        qFill(thirdLinkSetWidget->linkout, thirdLinkSetWidget->linkout + 31, 0);
        break;
    case 3:
        forthLinkSetWidget->clearforthWidget();
        qFill(forthLinkSetWidget->linkin,forthLinkSetWidget->linkin + 31, 0);
        qFill(forthLinkSetWidget->linkout, forthLinkSetWidget->linkout + 31, 0);
        break;
    default:
        break;
    }

    if(num == k)
    {
        switch (k)
        {
        case 0:
            //printf("点点联动\n ");
            firstLinkSetWidget->linkDisplay(id);
            break;
        case 1:
            //printf("点组联动\n ");
            secondLinkSetWidget->linkDisplay(id);
            break;
        case 2:
            //printf("组点联动\n ");
            thirdLinkSetWidget->linkDisplay(id);
            break;
        case 3:
            //printf("组组联动\n ");
            forthLinkSetWidget->linkDisplay(id);
            break;
        case 4:
            //printf("未知联动\n ");
            break;
        }
    }
    QSqlDatabase::database().commit();
}

void LinkDialog::readLinktype2(int num)
{
    //    qDebug()<<"num"<<num;
    //    int id = numSpinbox->value();
    int id = numLineedit->text().toInt();
    if(id == 0)
    {
        id = 1;
    }
    int k=0;

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);

    query.exec(QString("select linktype from linkset WHERE linkid = %1").arg(id));
    while (query.next())
    {
        k = query.value(0).toInt();
    }

    switch (num)
    {
    case 0:
        firstLinkSetWidget->clearfirstWidget();
        qFill(firstLinkSetWidget->linkin,firstLinkSetWidget->linkin + 31, 0);
        qFill(firstLinkSetWidget->linkout, firstLinkSetWidget->linkout + 31, 0);
        break;
    case 1:
        secondLinkSetWidget->clearsecondWidget();
        qFill(secondLinkSetWidget->linkin,secondLinkSetWidget->linkin + 31, 0);
        qFill(secondLinkSetWidget->linkout, secondLinkSetWidget->linkout + 31, 0);
        break;
    case 2:
        thirdLinkSetWidget->clearthirdWidget();
        qFill(thirdLinkSetWidget->linkin,thirdLinkSetWidget->linkin + 31, 0);
        qFill(thirdLinkSetWidget->linkout, thirdLinkSetWidget->linkout + 31, 0);
        break;
    case 3:
        forthLinkSetWidget->clearforthWidget();
        qFill(forthLinkSetWidget->linkin,forthLinkSetWidget->linkin + 31, 0);
        qFill(forthLinkSetWidget->linkout, forthLinkSetWidget->linkout + 31, 0);
        break;
    default:
        break;
    }

    if(num == k)
    {
        switch (k)
        {
        case 0:
            //   printf("点点联动\n ");
            firstLinkSetWidget->linkDisplay(id);
            break;
        case 1:
            //   printf("点组联动\n ");
            secondLinkSetWidget->linkDisplay(id);
            break;
        case 2:
            //   printf("组点联动\n ");
            thirdLinkSetWidget->linkDisplay(id);
            break;
        case 3:
            //    printf("组组联动\n ");
            forthLinkSetWidget->linkDisplay(id);
            break;
        case 4:
            // printf("未知联动\n ");
            break;
        }
    }
    QSqlDatabase::database().commit();
}


void LinkDialog::fuzhilinkid(int linkid)
{
    firstLinkSetWidget->firstlinkid = linkid;
    secondLinkSetWidget->secondlinkid = linkid;
    thirdLinkSetWidget->thirdlinkid = linkid;
    forthLinkSetWidget->forthlinkid = linkid;
}

void LinkDialog::readfirstLink(int id)
{
    int k=0;
    int num = linkTabWidget->currentIndex();
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);

    query.exec("select linktype from linkset WHERE linkid = 1");
    while (query.next())
    {
        k = query.value(0).toInt();
    }
    //printf("第一条id号是%d联动类型码为%d ",id,k);
    switch (k)
    {
    case 0:
        typeLabel->setText("点点联动");
        linkTabWidget->setCurrentIndex(0);
        break;
    case 1:
        typeLabel->setText("点组联动");
        linkTabWidget->setCurrentIndex(1);
        break;
    case 2:
        typeLabel->setText("组点联动");
        linkTabWidget->setCurrentIndex(2);
        break;
    case 3:
        typeLabel->setText("组组联动");
        linkTabWidget->setCurrentIndex(3);
        break;
    case 4:
        typeLabel->setText("未知联动");
        linkTabWidget->setCurrentIndex(0);
        break;
    }

    switch (num)
    {
    case 0:
        firstLinkSetWidget->clearfirstWidget();
        qFill(firstLinkSetWidget->linkin,firstLinkSetWidget->linkin + 31, 0);
        qFill(firstLinkSetWidget->linkout, firstLinkSetWidget->linkout + 31, 0);
        break;
    case 1:
        secondLinkSetWidget->clearsecondWidget();
        qFill(secondLinkSetWidget->linkin,secondLinkSetWidget->linkin + 31, 0);
        qFill(secondLinkSetWidget->linkout, secondLinkSetWidget->linkout + 31, 0);
        break;
    case 2:
        thirdLinkSetWidget->clearthirdWidget();
        qFill(thirdLinkSetWidget->linkin,thirdLinkSetWidget->linkin + 31, 0);
        qFill(thirdLinkSetWidget->linkout, thirdLinkSetWidget->linkout + 31, 0);
        break;
    case 3:
        forthLinkSetWidget->clearforthWidget();
        qFill(forthLinkSetWidget->linkin,forthLinkSetWidget->linkin + 31, 0);
        qFill(forthLinkSetWidget->linkout, forthLinkSetWidget->linkout + 31, 0);
        break;
    default:
        break;
    }

    if(num == k)
    {
        switch (k)
        {
        case 0:
            //printf("点点联动\n ");
            firstLinkSetWidget->linkDisplay(id);
            break;
        case 1:
            //printf("点组联动\n ");
            secondLinkSetWidget->linkDisplay(id);
            break;
        case 2:
            //printf("组点联动\n ");
            thirdLinkSetWidget->linkDisplay(id);
            break;
        case 3:
            //printf("组组联动\n ");
            forthLinkSetWidget->linkDisplay(id);
            break;
        case 4:
            // printf("未知联动\n ");
            break;
        }
    }
    QSqlDatabase::database().commit();
}
void LinkDialog::changetypeLabel(int type)
{
    switch(type)
    {
    case 0:
        typeLabel->setText("点点联动");
        linkTabWidget->setCurrentIndex(0);
        break;
    case 1:
        typeLabel->setText("点组联动");
        linkTabWidget->setCurrentIndex(1);
        break;
    case 2:
        typeLabel->setText("组点联动");
        linkTabWidget->setCurrentIndex(2);
        break;
    case 3:
        typeLabel->setText("组组联动");
        linkTabWidget->setCurrentIndex(3);
        break;
    case 4:
        typeLabel->setText("未知联动");
        linkTabWidget->setCurrentIndex(0);
        break;
    }
}



void LinkDialog::readUnitset()
{
    firstLinkSetWidget->readUnitSet1(1);
    firstLinkSetWidget->readUnitSet2(1);
}

void LinkDialog::copyPushButtonClicked()
{
    nowcopynum = numLineedit->text().toInt();

}

void LinkDialog::dealwithNumTextchange(QString num)
{
    int nowlinkid  = num.toInt();
    if(nowlinkid == 0)
    {
        nowlinkid = 1;
        numLineedit->setText(QString("%1").arg(nowlinkid));
    }
    emit linknumChange(nowlinkid);
}

void LinkDialog::previousbuttonClickSlot()
{
    if(numLineedit->text().toInt() > 1)
    {
        numLineedit->setText(QString("%1").arg(numLineedit->text().toInt() - 1));
    }
}
void LinkDialog::nextbuttonClickSlot()
{
    int count=0;
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec("select MAX(linkid) from linkset where linktype < 4");
    while (query.next())
    {
        count = query.value(0).toInt();
    }
    QSqlDatabase::database().commit();
    if(numLineedit->text().toInt() <= count)
    {
        numLineedit->setText(QString("%1").arg(numLineedit->text().toInt() + 1));
    }

}

void LinkDialog::updateLinkcount()
{
    int realset_count = 0;
    QSqlDatabase::database().transaction();
    QSqlQuery realquery;
    realquery.setForwardOnly(true);
    realquery.exec("select linktype from linkset ");
    while (realquery.next())
    {
        if(realquery.value(0).toInt() < 4)
        {
            realset_count++;
        }
    }
    QSqlDatabase::database().commit();
    linkcountLabel->setText(QString("共%1条联动").arg(realset_count));

}

void LinkDialog::addbuttonClickSlot()
{
    int count=0;
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.setForwardOnly(true);
    query.exec("select MIN(linkid) from linkset where linktype = 4");
    while (query.next())
    {
        count = query.value(0).toInt();
    }
    QSqlDatabase::database().commit();


    numLineedit->setText(QString("%1").arg(count));

}
void LinkDialog::deletebuttonClickSlot()
{

    int copyid=0;
    QSqlDatabase::database().transaction();
    QSqlQuery deletequery;
    deletequery.setForwardOnly(true);
    deletequery.exec("select MIN(linkid) from linkset where linktype = 4");
    while (deletequery.next())
    {
        copyid = deletequery.value(0).toInt();
    }
    QSqlDatabase::database().commit();

    //    int id = numSpinbox->value();
    int id = numLineedit->text().toInt();
    if(id == 0)
    {
        id = 1;
    }


    if(id != copyid)
    {
        QSqlDatabase::database().transaction();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from linkset WHERE linkid = %1").arg(copyid));

        while (query.next())
        {
            QSqlQuery copyquery;
            copyquery.clear();
            copyquery.prepare(QString("UPDATE linkset SET "
                                      "linkshield=?, linksetup=?, linktype=?, linknum=?, "
                                      "linkINInfo1=?, linkINInfo2=?, linkINInfo3=?, linkINInfo4=?, linkINInfo5=?, linkINInfo6=?, linkINInfo7=?, linkINInfo8=?, linkINInfo9=?, linkINInfo10=?, "
                                      "linkINInfo11=?, linkINInfo12=?, linkINInfo13=?, linkINInfo14=?, linkINInfo15=?, linkINInfo16=?, linkINInfo17=?, linkINInfo18=?, linkINInfo19=?, "
                                      "linkINInfo20=?, linkINInfo21=?, linkINInfo22=?, linkINInfo23=?, linkINInfo24=?, linkINInfo25=?, linkINInfo26=?, linkINInfo27=?, linkINInfo28=?, "
                                      "linkINInfo29=?, linkINInfo30=?, linkINInfo31=?, linkOUTInfo1=?, linkOUTInfo2=?, linkOUTInfo3=?, linkOUTInfo4=?, linkOUTInfo5=?, linkOUTInfo6=?, "
                                      "linkOUTInfo7=?, linkOUTInfo8=?, linkOUTInfo9=?, linkOUTInfo10=?, linkOUTInfo11=?, linkOUTInfo12=?, linkOUTInfo13=?, linkOUTInfo14=?, "
                                      "linkOUTInfo15=?, linkOUTInfo16=?, linkOUTInfo17=?, linkOUTInfo18=?, linkOUTInfo19=?, linkOUTInfo20=?, linkOUTInfo21=?, linkOUTInfo22=?, "
                                      "linkOUTInfo23=?, linkOUTInfo24=?, linkOUTInfo25=?, linkOUTInfo26=?, linkOUTInfo27=?, linkOUTInfo28=?, linkOUTInfo29=?, linkOUTInfo30=?, linkOUTInfo31=? "
                                      "WHERE linkid =%1").arg(id));

            //   query.prepare(QString("UPDATE linkset SET linknum=?,linksetup=?, linkshield=?, linktype=?  WHERE linkid = %1").arg(firstlinkid));

            copyquery.bindValue(0,query.value(1).toInt());
            // qDebug()<<"shujukuzhong"<<firstshield;
            //      printf("firstshield%d",firstshield);
            copyquery.bindValue(1,query.value(2).toInt());
            copyquery.bindValue(2,query.value(3).toInt());
            copyquery.bindValue(3,query.value(4).toInt());

            for (int j=0; j<31; j++)
            {
                copyquery.bindValue(j+4,query.value(j+5).toLongLong());
                //       qDebug() <<j+5 ;
            }
            for (int j=0; j<31; j++)
            {
                copyquery.bindValue(j+35,query.value(j+36).toLongLong());
            }
            bool bSuccess = copyquery.exec();
            if (!bSuccess)
            {
                QSqlError lastError = copyquery.lastError();
                qDebug() << lastError.driverText()
                         <<QString(QObject::tr("copy update error"));
            }
        }
        this->readLinktype(id);
        this->updateLinkcount();
        //        this->readfirstLink(copyid); //if want to use this idea,you can find the type of this link in the database and then set the index
    }


}

void LinkDialog::pastePushButtonClicked()
{
    int copyid = nowcopynum;
    //    int id = numSpinbox->value();
    int id = numLineedit->text().toInt();
    if(id == 0)
    {
        id = 1;
    }


    if(id != copyid)
    {
        QSqlDatabase::database().transaction();
        QSqlQuery query;
        query.setForwardOnly(true);
        query.exec(QString("select * from linkset WHERE linkid = %1").arg(copyid));

        while (query.next())
        {
            QSqlQuery copyquery;
            copyquery.clear();
            copyquery.prepare(QString("UPDATE linkset SET "
                                      "linkshield=?, linksetup=?, linktype=?, linknum=?, "
                                      "linkINInfo1=?, linkINInfo2=?, linkINInfo3=?, linkINInfo4=?, linkINInfo5=?, linkINInfo6=?, linkINInfo7=?, linkINInfo8=?, linkINInfo9=?, linkINInfo10=?, "
                                      "linkINInfo11=?, linkINInfo12=?, linkINInfo13=?, linkINInfo14=?, linkINInfo15=?, linkINInfo16=?, linkINInfo17=?, linkINInfo18=?, linkINInfo19=?, "
                                      "linkINInfo20=?, linkINInfo21=?, linkINInfo22=?, linkINInfo23=?, linkINInfo24=?, linkINInfo25=?, linkINInfo26=?, linkINInfo27=?, linkINInfo28=?, "
                                      "linkINInfo29=?, linkINInfo30=?, linkINInfo31=?, linkOUTInfo1=?, linkOUTInfo2=?, linkOUTInfo3=?, linkOUTInfo4=?, linkOUTInfo5=?, linkOUTInfo6=?, "
                                      "linkOUTInfo7=?, linkOUTInfo8=?, linkOUTInfo9=?, linkOUTInfo10=?, linkOUTInfo11=?, linkOUTInfo12=?, linkOUTInfo13=?, linkOUTInfo14=?, "
                                      "linkOUTInfo15=?, linkOUTInfo16=?, linkOUTInfo17=?, linkOUTInfo18=?, linkOUTInfo19=?, linkOUTInfo20=?, linkOUTInfo21=?, linkOUTInfo22=?, "
                                      "linkOUTInfo23=?, linkOUTInfo24=?, linkOUTInfo25=?, linkOUTInfo26=?, linkOUTInfo27=?, linkOUTInfo28=?, linkOUTInfo29=?, linkOUTInfo30=?, linkOUTInfo31=? "
                                      "WHERE linkid =%1").arg(id));

            //   query.prepare(QString("UPDATE linkset SET linknum=?,linksetup=?, linkshield=?, linktype=?  WHERE linkid = %1").arg(firstlinkid));

            copyquery.bindValue(0,query.value(1).toInt());
            // qDebug()<<"shujukuzhong"<<firstshield;
            //      printf("firstshield%d",firstshield);
            copyquery.bindValue(1,query.value(2).toInt());
            copyquery.bindValue(2,query.value(3).toInt());
            copyquery.bindValue(3,query.value(4).toInt());

            for (int j=0; j<31; j++)
            {
                copyquery.bindValue(j+4,query.value(j+5).toLongLong());
                //       qDebug() <<j+5 ;
            }
            for (int j=0; j<31; j++)
            {
                copyquery.bindValue(j+35,query.value(j+36).toLongLong());
            }
            bool bSuccess = copyquery.exec();
            if (!bSuccess)
            {
                QSqlError lastError = copyquery.lastError();
                qDebug() << lastError.driverText()
                         <<QString(QObject::tr("copy update error"));
            }
        }
        this->readLinktype(id);
        this->updateLinkcount();
        //        this->readfirstLink(copyid); //if want to use this idea,you can find the type of this link in the database and then set the index
    }
}





