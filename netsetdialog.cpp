#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QTableWidget>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QHeaderView>

#include "netsetdialog.h"
#define HOST_TABLE_ROWS 64
#define HOST_TABLE_CLOS 2
NetSetDialog::NetSetDialog(QWidget *parent) :
    QDialog(parent)
{
}
NetSetDialog::NetSetDialog(bool editEnableFlag,QWidget *parent) :
    QDialog(parent)
{
    createItems(editEnableFlag);
    setWindowTitle(tr("联网设置"));
    this->setFixedSize (QSize(600, 400));
}
NetSetDialog::~NetSetDialog()
{
}
void NetSetDialog::createItems(bool editEnableFlag)
{
    localHostNum = new QLabel(tr("主机号:"), this);
    localHostNum->setGeometry(QRect(200, 10, 60, 24));
    localHostBox = new QSpinBox(this);
    localHostBox->setGeometry(QRect(300, 10, 60, 24));
    localHostBox->setRange(1,64);
    localHostBox->setValue(1);
    localHostBox->setSingleStep(1);

    netSetTable = new  QTableWidget(HOST_TABLE_ROWS, HOST_TABLE_CLOS, this);
    QStringList label;
    label<<tr("主机号")<<tr("登记");
    netSetTable->setHorizontalHeaderLabels(label);
    netSetTable->horizontalHeader()->setStretchLastSection(true);
    netSetTable->verticalHeader()->setDefaultSectionSize(24);//set the verticalHeader size is 24
    netSetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);//set the table read-only
    //netSetTable->setTextAlignment(Qt::AlignHCenter);
    netSetTable->setGeometry(QRect(0, 40,  600, 320));
    for (int i = 0; i < HOST_TABLE_ROWS; i++)
    {

        QTableWidgetItem *item = new QTableWidgetItem(tr("%1").arg(i+1));
        item->setTextAlignment(Qt::AlignHCenter);
        netSetTable->setItem(i, 0, item);
#if 0
        QComboBox *comBox = new QComboBox(netSetTable);
        comBox->addItem(tr("√"));
        comBox->addItem(tr("×"));
        comBox->setCurrentIndex(1);
        //comBox->setTextAlignment(Qt::AlignHCenter);
        comBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        netSetTable->setCellWidget(i , 1, comBox);
#endif
        QCheckBox *checkbox = new QCheckBox(tr("登记"), netSetTable);
        checkbox->setChecked(false);
        checkbox->setCheckable(editEnableFlag);
        netSetTable->setCellWidget(i , 1, checkbox);
    }

    okButton = new QPushButton(tr("确定"), this);
    okButton->setGeometry(QRect(160, 370, 80, 24));
    cancelButton = new QPushButton(tr("取消"), this);
    cancelButton->setGeometry(QRect(380, 370, 80, 24));
    cancelButton->setFocus();//set default button
    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));

}
void NetSetDialog::ok()
{

}
