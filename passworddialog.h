#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>

class QPushButton;
class QLineEdit;
class QLabel;
class PasswordDialog : public QDialog
{
    Q_OBJECT
public:
    int type;
    explicit PasswordDialog(QWidget *parent = 0);
    ~PasswordDialog();
    PasswordDialog(int index, QWidget *parent = 0);
    bool matchFlag;
signals:

public slots:
    void inToSlot();

private:
    void createItems();
    QPushButton* intoPushButton;
    QLineEdit* passwordLineEdit;
    QLabel* infoLabel;
};

#endif // PASSWORDDIALOG_H
