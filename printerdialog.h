#ifndef PRINTERDIALOG_H
#define PRINTERDIALOG_H

//#define PRINT_FIRE_FLAG 3
//#define PRINT_LINK_FLAG 2
//#define PRINT_FAULT_FLAG 1
#define PRINT_FIRE_FLAG 24
#define PRINT_LINK_FLAG 16
#define PRINT_FAULT_FLAG 8

#include <QDialog>

class QLabel;
class QPushButton;
class QButtonGroup;

class printerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit printerDialog(QWidget *parent = 0);
    ~printerDialog();
    printerDialog(int printerFlagOfDialog, QWidget *parent = 0);

private:
    int printerFlagOfDialog;// firePushButton<<3 |linkPushButton <<2 | faultPushButton << 1 | allPushButton<<0;
    void createItems();

    QLabel *tipLabel1;
    QLabel *tipLabel2;
    QPushButton *okButton;
    QPushButton *cancelButton;

    QPushButton *firePushButton;
    QPushButton *faultPushButton;
    QPushButton *linkPushButton;
    QPushButton *allPushButton;

    QLabel *fireLabel;
    QLabel *faultLabel;
    QLabel *linkLabel;
    QLabel *allLabel;

    QButtonGroup *buttonGroup;

signals:
    void printerFlagOfDialogSetSignal(int printerFlagOfDialog);
    void flagValueChangedSignal();
public slots:
    void flagSetSlot();
    void buttonClickedSlot(int );
    void flagValueChangedSlot();


};

#endif // PRINTERDIALOG_H
