#ifndef SINGLETESTDIALOG_H
#define SINGLETESTDIALOG_H

#include <QDialog>
class QLabel;
class QSpinBox;
class QPushButton;
class singleTestDialog : public QDialog
{
    Q_OBJECT
public:
    explicit singleTestDialog(QWidget *parent = 0);
private:
    QLabel *warningLabel;
    QLabel *iobrLabel;
    QLabel *loopLabel;
    QLabel *addrLabel;
    QSpinBox *iobrSpinBox;
    QSpinBox *loopSpinBox;
    QSpinBox *addrSpinBox;
    QPushButton *okPushButton;
    QPushButton *cancelPushButton;

    void createItems();

signals:

private slots:
    void ok();

};

#endif // SINGLETESTDIALOG_H
