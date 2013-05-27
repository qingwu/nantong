#ifndef REGINFODIALOG_H
#define REGINFODIALOG_H

#include <QDialog>
class QTextEdit;
class QPushButton;
class RegInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RegInfoDialog(QWidget *parent = 0);
    ~RegInfoDialog();
public slots:
    void readInfo();
private:
    void createItems();

    QTextEdit *regInfoText;
    QPushButton *okPushButton;
};

#endif // REGINFODIALOG_H
