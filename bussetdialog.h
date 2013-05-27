#ifndef BUSSETDIALOG_H
#define BUSSETDIALOG_H

#include <QDialog>
class QLabel;
class QSpinBox;
class QComboBox;
class QPushButton;
class QListWidget;
class QFont;
class QFrame;

class BusSetDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BusSetDialog(QWidget *parent = 0);
    BusSetDialog(bool editFlag, QWidget *parent=0);
private:
    bool editEnableFlag;
    QFrame *candidateFrame;
    QFrame *mainFrame;

    QFont *labelFont;
    QLabel *buspanelLabel;
    QLabel *busnodeLabel;
    QLabel *inputLabel;
    QLabel *outputLabel;
    QLabel *candidateLabel;
    QLabel *hostLabel;
    QLabel *iobrLabel;
    QLabel *loopLabel;

    QComboBox *buspanelComboBox;
    QSpinBox *busnodeSpinBox;
    QSpinBox *hostSpinBox;
    QSpinBox *iobrSpinBox;
    QSpinBox *loopSpinBox;

    QListWidget *inputListWidget;
    QListWidget *outputListWidget;
    QListWidget *candidateListWidget;

    QPushButton *okPushButton;
    QPushButton *cancelPushButton;
    QPushButton *inTocPushButton;
    QPushButton *cToinPushButton;
    QPushButton *outTocPushButton;
    QPushButton *cTooutPushButton;
    /*long long */int inMode[15];
    /*long long */int outMode[15];

    QComboBox *copybuspanelComboBox;
    QSpinBox *copybusnodeSpinbox;
    QPushButton *copyPushButton;
    QLabel *copyLabel;
    QLabel *copyLabel2;
    QLabel *copyLabel3;

    void createWidget();
    void updateBusset();

signals:

private slots:
    void okSlot();
    void readUnitSet(int);
    void readBusSet(int);
    void inTocClicked();
    void cToinClicked();
    void cTooutClicked();
    void outTocClicked();
    void copyPushButtonClicked();
};

#endif // BUSSETDIALOG_H
