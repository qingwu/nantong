#ifndef DDLINKSETWIDGET_H
#define DDLINKSETWIDGET_H

#include <QWidget>
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;
class QFrame;

class ddLinkSetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ddLinkSetWidget(QWidget *parent = 0);
    ddLinkSetWidget(bool editFlag, QWidget *parent=0);

    ~ ddLinkSetWidget();
    void linkDisplay(int id);
    void clearfirstWidget();
    void updateLinkset();
    long long int linkin[31];
    long long int linkout[31];
    int firstlinkid;
    int firstnum;
    int firstshield;
    int firsttype;
    int firstsetup;

signals:
    void databaseChange();
public slots:
    void readUnitSet1(int);
    void readUnitSet2(int);
    void firstLinkinInsertClicked();
    void firstLinkinOutsertClicked();
    void firstLinkoutInsertClicked();
    void firstLinkoutOutsertClicked();
    void linknumreturn(QString);
    void shieldreturn(int);
    void firstokButtonClicked();
    void firstcancelButtonClicked();

private:

    bool editEnableFlag;

    QFrame *firstlinkinFrame;
    QFrame *firstlinkoutFrame;
    QLabel *firstlinkinTipLabel;
    QLabel *firstlinkinTipLabel2;
    QLineEdit *firstlinkinNum;
    QComboBox *firsthideComboBox;
//    QCheckBox *firsthideCheckBox;
    QLabel *firstlinkinTipLabel3;
    QLabel *firstlinkinTipLabel4;
    QSpinBox *firstIobrinSpinBox;
    QLabel *firstlinkinTipLabel5;
    QSpinBox *firstLoopinSpinBox;
    QLabel *firstlinkinTipLabel6;
    QLabel  *firstlinkinTipLabel7;
    QSpinBox *firstLineinSpinBox;
    QLabel  *firstlinkinTipLabel8;
    QSpinBox *firstHostinSpinBox;
    QPushButton *firstLinkinInsertButton;
    QPushButton *firstLinkinOutsertButton;
    QListWidget *firstlinkinListWidget;
    QListWidget *firstlinkinListWidget2;

    QPushButton *firstokButton;
    QPushButton *firstcancelButton;

    QLabel *firstlinkOutTipLabel;
    QLabel *firstlinkoutTipLabel3;
    QLabel *firstlinkoutTipLabel4;
    QSpinBox *firstIobroutSpinBox;
    QLabel *firstlinkoutTipLabel5;
    QSpinBox *firstLoopoutSpinBox;
    QLabel *firstlinkoutTipLabel6;
    QLabel  *firstlinkoutTipLabel7;
    QSpinBox *firstLineoutSpinBox;
    QLabel  *firstlinkoutTipLabel8;
    QSpinBox *firstHostoutSpinBox;
    QPushButton *firstLinkoutInsertButton;
    QPushButton *firstLinkoutOutsertButton;
    QListWidget *firstlinkoutListWidget;
    QListWidget *firstlinkoutListWidget2;
signals:
    void transferddLinknum(int id ,int linknum);
    void changeTypeLabel(int type);

};

#endif // DDLINKSETWIDGET_H
