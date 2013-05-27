#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    createItems();
    setWindowTitle(tr("关于系统"));
    this->setFixedSize (QSize(400, 280));
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::createItems()
{
//    aboutSystem = new QLabel (tr("TB-ARM火灾报警联动系统\n"
//                                               "版本信息：UI.0 Build090226\n"
//                                                "华中科技大学电子与信息工程系 版权所有 2012-2020\n"
//                                                "\n地址华中科技大学南一楼\n"
//                                                "联系电话：027-87434521"), this);
    /*版本号：1.0
公司：江苏河马自动化设备有限公司
地址：江苏海安开发区鑫来路64号
电话：0513-88917718 88912800（传）
网址：www.hippo.com.cn
email:habjy@public.nt.js.cn
*/
    aboutSystem = new QLabel (tr("版本号：1.0\n"
                                               "公司：江苏河马自动化设备有限公司\n"
                                                "地址：江苏海安开发区鑫来路64号\n"
                                                "\n电话：0513-88917718 88912800（传）\n"
                                                "email:habjy@public.nt.js.cn\n"
                                                "网址：www.hippo.com.cn"), this);

    aboutSystem->setGeometry(QRect(20, 20, 350, 180));
    aboutSystem->setAlignment(Qt::AlignCenter);

    okButton = new QPushButton (tr("确定"), this);
    okButton->setGeometry(QRect(160, 220, 100, 24));
    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(this->parentWidget(), SIGNAL(closedialog()), this, SLOT(close()));
}
