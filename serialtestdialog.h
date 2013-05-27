#ifndef SERIALTESTDIALOG_H
#define SERIALTESTDIALOG_H

#include <QtGui/QDialog>
class QLabel;
class QPushButton;

class serialTestDialog : public QDialog
{
    Q_OBJECT
public:
    explicit serialTestDialog(QWidget *parent = 0);
private:

    QLabel *hintContentLabel;
    QLabel *sendLabel;
    QLabel *receiveLabel;
    QLabel *receiveContentLabel;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void createItems();

signals:

private slots:
    void ok();
};

#endif // SERIALTESTDIALOG_H
