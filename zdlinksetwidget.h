#ifndef ZDLINKSETWIDGET_H
#define ZDLINKSETWIDGET_H

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

class zdLinkSetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit zdLinkSetWidget(QWidget *parent = 0);
    zdLinkSetWidget(bool editFlag, QWidget *parent=0);

    ~zdLinkSetWidget();
    void linkDisplay(int id);
    void updateLinkset();
    void clearthirdWidget();
    long long int linkin[31];
    long long int linkout[31];
    int thirdlinkid;
    int thirdnum;
    int thirdshield;
    int thirdtype;
    int thirdsetup;


signals:
    void databaseChange();

public slots:
    void readUnitSet(int);
    void thirdLinkoutInsertClicked();
    void thirdLinkoutOutsertClicked();
    void linknumreturn(QString);
    void shieldreturn(int);
    void thirdokButtonClicked();
    void thirdcancelButtonClicked();

private:

    QFrame *thirdlinkinFrame;
    QFrame *thirdlinkoutFrame;

    bool editEnableFlag;
  //    QGroupBox *thirdlinkInGroup[4];
    QGroupBox *thirdlinkInGroup[11];

    QTabWidget *linkTabWidgetThird;

    QLabel *thirdlinkInTipLabel;
    QLabel *thirdlinkinTipLabel2;
    QLineEdit *thirdlinkinNum;
    QComboBox *thirdhideComboBox;
//    QCheckBox *thirdhideCheckBox;

    QLabel *thirdlinkinNumLabel;
    QLabel *thirdlinkinHostLabel;
    QLabel *thirdlinkinIobrLabel;
    QLabel *thirdlinkinLoopLabel;
    QLabel *thirdlinkinStartLabel;
    QLabel *thirdlinkinEndLabel;
    QLabel *thirdlinkinDeviceLabel;

    QLabel *thirdlinkinLabel[31];
    QSpinBox *thirdlinkinHostSpinBox[31];
    QSpinBox *thirdlinkinIobrSpinBox[31];
    QSpinBox *thirdlinkinLoopSpinBox[31];
    QLineEdit *thirdlinkinStart[31];
    QLineEdit *thirdlinkinEnd[31];
    QComboBox *thirdlinkinDeviceCombo[31];

    QPushButton *thirdokButton;
    QPushButton * thirdcancelButton;

    QLabel *thirdlinkOutTipLabel;
    QLabel *thirdlinkoutTipLabel3;
    QLabel *thirdlinkoutTipLabel4;
    QSpinBox *thirdIobroutSpinBox;
    QLabel *thirdlinkoutTipLabel5;
    QSpinBox *thirdLoopoutSpinBox;
    QLabel *thirdlinkoutTipLabel6;
    QLabel  *thirdlinkoutTipLabel7;
    QSpinBox *thirdLineoutSpinBox;
    QLabel  *thirdlinkoutTipLabel8;
    QSpinBox *thirdHostoutSpinBox;

    QPushButton *thirdLinkoutInsertButton;
    QPushButton *thirdLinkoutOutsertButton;
    QListWidget *thirdlinkoutListWidget;
    QListWidget *thirdlinkoutListWidget2;
signals:
    void transferzdLinknum(int id ,int linknum);
    void changeTypeLabel(int type);


};

#endif // ZDLINKSETWIDGET_H
