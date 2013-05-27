#ifndef LINKDIALOG_H
#define LINKDIALOG_H

#include <QDialog>
class QLabel;
class QTabWidget;
class QHBoxLayout;
class QVBoxLayout;
class QSpinBox;
class QPushButton;
class QLineEdit;
class QIntValidator;

class ddLinkSetWidget;
class dzLinkSetWidget;
class zdLinkSetWidget;
class zzLinkSetWidget;

class LinkDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LinkDialog(QWidget *parent = 0);
    LinkDialog(bool editFlag, QWidget *parent=0);

    ~LinkDialog();
//    QSpinBox *numSpinbox;
    QLabel *typeLabel;
    QLineEdit *numLineedit;

    void readUnitset();
     int nowcopynum;

signals:

public slots:
    void readfirstLink(int);
    void readLinktype(int );
    void readLinktype2(int );
    void fuzhilinkid(int);
    void changetypeLabel(int);
    void copyPushButtonClicked();
    void pastePushButtonClicked();
private:
    void createItems();

    QLabel *linkcountLabel;
//    QSpinBox *copySpinbox;
    QPushButton *copyPushButton;
    QPushButton *pastePushButton;
//    QLabel *copyLabel;
//    QLabel *copyLabel2;

    QPushButton *previousbutton;
    QPushButton *nextbutton;

    QPushButton *addbutton;
    QPushButton *deletebutton;

    QIntValidator *linkidValidator;


    bool editEnableFlag;


    QTabWidget *linkTabWidget;
    ddLinkSetWidget *firstLinkSetWidget;
    dzLinkSetWidget *secondLinkSetWidget;
    zdLinkSetWidget *thirdLinkSetWidget;
    zzLinkSetWidget *forthLinkSetWidget;

    //    ddLinkSetWidget *secondLinkSetWidget;
    //    ddLinkSetWidget *thirdLinkSetWidget;
    //    ddLinkSetWidget *forthLinkSetWidget;

    QHBoxLayout *toplayout;
    QLabel *numLabel;

//    QSpinBox *numSpinbox;
    QHBoxLayout *bottomlayout;
    QVBoxLayout *mainlayout;
signals:
    void transferLinknum(int id ,int linknum);
    void linknumChange(int);
public slots:
    void dealwithNumTextchange(QString);
    void previousbuttonClickSlot();
    void nextbuttonClickSlot();
    void addbuttonClickSlot();
    void deletebuttonClickSlot();
    void updateLinkcount();

};

#endif // LINKDIALOG_H
