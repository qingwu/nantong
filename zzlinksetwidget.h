#ifndef ZZLINKSETWIDGET_H
#define ZZLINKSETWIDGET_H

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

class zzLinkSetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit zzLinkSetWidget(QWidget *parent = 0);
    zzLinkSetWidget(bool editFlag, QWidget *parent=0);

    ~zzLinkSetWidget();
    void linkDisplay(int id);
    void updateLinkset();
    void clearforthWidget();
    long long int linkin[31];
    long long int linkout[31];
    int forthlinkid;
    int forthnum;
    int forthshield;
    int forthtype;
    int forthsetup;

signals:
   void databaseChange();

public slots:
    void linknumreturn(QString);
    void shieldreturn(int);
    void forthokButtonClicked();
    void forthcancelButtonClicked();

private:

    bool editEnableFlag;

    QFrame *forthlinkinFrame;
    QFrame *forthlinkoutFrame;


//    QGroupBox *forthlinkInGroup[4];
    QGroupBox *forthlinkInGroup[11];
    QTabWidget *linkTabWidgetforth1;

    QLabel *forthlinkInTipLabel;
    QLabel *forthlinkinTipLabel2;
    QLineEdit *forthlinkinNum;
//    QCheckBox *forthhideCheckBox;
    QComboBox *forthhideComboBox;
    QLabel *forthlinkinNumLabel;
    QLabel *forthlinkinHostLabel;
    QLabel *forthlinkinIobrLabel;
    QLabel *forthlinkinLoopLabel;
    QLabel *forthlinkinStartLabel;
    QLabel *forthlinkinEndLabel;
    QLabel *forthlinkinDeviceLabel;
    QLabel *forthlinkinLabel[31];
    QSpinBox *forthlinkinHostSpinBox[31];
    QSpinBox *forthlinkinIobrSpinBox[31];
    QSpinBox *forthlinkinLoopSpinBox[31];
    QLineEdit *forthlinkinStart[31];
    QLineEdit *forthlinkinEnd[31];
    QComboBox *forthlinkinDeviceCombo[31];

    QPushButton *forthokButton;
    QPushButton * forthcancelButton;

   // QGroupBox *forthlinkOutGroup[4];
    QGroupBox *forthlinkOutGroup[11];
    QTabWidget *linkTabWidgetforth2;
    QLabel *forthlinkOutTipLabel;
    QLabel *forthlinkoutNumLabel;
    QLabel *forthlinkoutHostLabel;
    QLabel *forthlinkoutIobrLabel;
    QLabel *forthlinkoutLoopLabel;
    QLabel *forthlinkoutStartLabel;
    QLabel *forthlinkoutEndLabel;
    QLabel *forthlinkoutDeviceLabel;
    QLabel *forthlinkoutLabel[31];
    QSpinBox *forthlinkoutHostSpinBox[31];
    QSpinBox *forthlinkoutIobrSpinBox[31];
    QSpinBox *forthlinkoutLoopSpinBox[31];
    QLineEdit *forthlinkoutStart[31];
    QLineEdit *forthlinkoutEnd[31];
    QComboBox *forthlinkoutDeviceCombo[31];
signals:
    void transferzzLinknum(int id ,int linknum);
    void changeTypeLabel(int type);

};

#endif // ZZLINKSETWIDGET_H
