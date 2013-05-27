#ifndef SINGLEBOARDREGDIALOG_H
#define SINGLEBOARDREGDIALOG_H

#include <QDialog>
class QLabel;
class QLineEdit;
class QPushButton;

class singleBoardRegDialog : public QDialog
{
    Q_OBJECT
public:
    explicit singleBoardRegDialog(QWidget *parent = 0);
    ~singleBoardRegDialog();

    QLabel *singleRegInfoLabel;
    QLabel *allRegInfoLabel;
    QLabel *singleBoardNumLabel;
    QLineEdit *boardNumLineEdit;

    QPushButton *singleOkPushButton;
    QPushButton *allRegPushButton;
    QPushButton *cancelPushButton;

signals:
    void singleBoardNumSignal(int i);
    void allRegPushButtonClicked();
public slots:
    void emitSingleBoardNum();

};

#endif // SINGLEBOARDREGDIALOG_H
