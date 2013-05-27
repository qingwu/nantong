#ifndef DZLINKSETWIDGET_H
#define DZLINKSETWIDGET_H

#include <QWidget>
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;
class QTabWidget;
class QFrame;


class dzLinkSetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit dzLinkSetWidget(QWidget *parent = 0);
    dzLinkSetWidget(bool editFlag, QWidget *parent=0);

    ~dzLinkSetWidget();
    void linkDisplay(int id);
    void updateLinkset();
    void clearsecondWidget();
    long long int linkin[31];
    long long int linkout[31];
    int secondlinkid;
    int secondnum;
    int secondshield;
    int secondtype;
    int secondsetup;


signals:
    void databaseChange();
public slots:
    void readUnitSet(int);
    void secondLinkinInsertClicked();
    void secondLinkinOutsertClicked();
    void linknumreturn(QString);
    void shieldreturn(int);
    void secondokButtonClicked();
    void secondcancelButtonClicked();


private:
    bool editEnableFlag;


    QFrame *secondlinkinFrame;
    QFrame *secondlinkoutFrame;


    QLabel *secondlinkinTipLabel;
    QLabel *secondlinkinTipLabel2;
    QLineEdit *secondlinkinNum;
    QComboBox *secondhideComboBox;
//    QCheckBox *secondhideCheckBox;
    QLabel *secondlinkinTipLabel3;
    QLabel *secondlinkinTipLabel4;
    QSpinBox *secondIobrinSpinBox;
    QLabel *secondlinkinTipLabel5;
    QSpinBox *secondLoopinSpinBox;
    QLabel *secondlinkinTipLabel6;
    QLabel  *secondlinkinTipLabel7;
    QSpinBox *secondLineinSpinBox;
    QLabel  *secondlinkinTipLabel8;
    QSpinBox *secondHostinSpinBox;
    QPushButton *secondLinkinInsertButton;
    QPushButton *secondLinkinOutsertButton;
    QListWidget *secondlinkinListWidget;
    QListWidget *secondlinkinListWidget2;

    QPushButton *secondokButton;
    QPushButton * secondcancelButton;

      QGroupBox *secondlinkOutGroup[11];
//    QGroupBox *secondlinkOutGroup[4];
    QTabWidget *linkTabWidgetSecond;
    QLabel *secondlinkOutTipLabel;
    QLabel *secondlinkoutNumLabel;
    QLabel *secondlinkoutHostLabel;
    QLabel *secondlinkoutIobrLabel;
    QLabel *secondlinkoutLoopLabel;
    QLabel *secondlinkoutStartLabel;
    QLabel *secondlinkoutEndLabel;
    QLabel *secondlinkoutDeviceLabel;

    QLabel *secondlinkoutLabel[31];
    QSpinBox *secondlinkoutHostSpinBox[31];
    QSpinBox *secondlinkoutIobrSpinBox[31];
    QSpinBox *secondlinkoutLoopSpinBox[31];
    QLineEdit *secondlinkoutStart[31];
    QLineEdit *secondlinkoutEnd[31];
    QComboBox *secondlinkoutDeviceCombo[31];
signals:
    void transferdzLinknum(int id ,int linknum);
    void changeTypeLabel(int type);



};

#endif // DZLINKSETWIDGET_H
