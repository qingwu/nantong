#ifndef HOSTSETDIALOG_H
#define HOSTSETDIALOG_H

#include <QtGui/QDialog>
class QFrame;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class HostSetDialog : public QDialog
{
    Q_OBJECT
public:
    explicit HostSetDialog(QWidget *parent = 0);
    ~HostSetDialog();
signals:
    void linkFeedBackSetSaveSignal(int );
    void handTypeChange(int);
    void lockkeyTimeChange(int);
    void sound_alarm_statechange(int);
    void gasfirecontrolstatechange(int);
public slots:
    void linkFeedBackChangeSlot(int );
    void okSaveSlot();
private:
    void createItems();

    int linkFeedBackFlag;
    QFrame *hostFrame;

    QLabel *hostSetLabel;
    QLabel *hostNumLabel;
    QLabel *passwordResetLabel;
    QLabel *passwordOkResetLabel;
    QLabel *passwordSetLabel;
    QLabel *passwordOkSetLabel;
    QLabel *linkFeedBackLabel;
    QLabel *linkStyleLabel;
    QLabel *lockKeyLabel;
    QLabel *soundAndAlarmDeviceLabel;
    QLabel *gasFireControlLabel;

    QComboBox *hostNumCombox;
    QComboBox *linkFeedBackCombox;
    QComboBox *linkStyleCombox;
    QComboBox *lockKeyCombox;
    QComboBox *soundAndAlarmDeviceCombox;
    QComboBox *gasFireControlCombox;

    QLineEdit *resetLineEdit;
    QLineEdit *resetOkLineEdit;
    QLineEdit *setLineEdit;
    QLineEdit *setOkLineEdit;

    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // HOSTSETDIALOG_H
