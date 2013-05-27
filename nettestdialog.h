#ifndef NETTESTDIALOG_H
#define NETTESTDIALOG_H

#include <QDialog>
class QLabel;
class QSpinBox;
class QPushButton;
class QFrame;

class netTestDialog : public QDialog
{
    Q_OBJECT
public:
    explicit netTestDialog(QWidget *parent = 0);
private:
    QLabel *hostNumlabelA;
    QLabel *hoseNumLabelB;
    QLabel *sendLabel;
    QLabel *receiveLabel;
    QSpinBox *hostNumSpinBoxA;
    QSpinBox *hostNumSpinBoxB;
    QPushButton *okPushButton;
    QPushButton *cancelPushButton;
    QFrame *mainFrame;

    void createItems();

signals:

private slots:
    void ok();
};

#endif // NETTESTDIALOG_H
