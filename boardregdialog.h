#ifndef BOARDREGDIALOG_H
#define BOARDREGDIALOG_H

#include <QtGui/QDialog>
class QLabel;
class QPushButton;
class QLineEdit;
class QTimer;

class BoardRegDialog : public QDialog
{
    Q_OBJECT

public:
    BoardRegDialog(QWidget *parent = 0);
    ~BoardRegDialog();

    QLabel *timeLabel;
    QLabel *infoLabel;
    QLabel *backgroundLabel;
//    QPushButton *okPushButton;
//    QPushButton *cancelPushButton;
    QPushButton *okToPollingButton;


    QLabel *singleRegInfoLabel;
    QLabel *allRegInfoLabel;
    QLabel *singleBoardNumLabel;
    QLineEdit *singleBoardNumLineEdit;

    QPushButton *singleOkPushButton;
    QPushButton *allRegPushButton;
    QPushButton *regCancelPushButton;

    void statrTimer15s();

private:
    QTimer *pollingStartTimer15s;
    QTimer *timer1s;
    int timeLeft;
    void createEscapeAction();
    QAction *escapeAction;
    bool isSingleRegFlag;
    void createSingleBoardDialog();
    void setSignalVisibleFalse();
    void setSignalVisibleTrue();

public slots:    
    void regStartMessage();
    void regFaultMessage();
    void currentRegBoardSlot(int );
    void regOverSlot(int);
    void refreshDialog();
    void emitSingleBoardNum();
    void singleRegOverSlot(int );
private slots:
    void stopTimerSlot();
    void updateTimeLabelSlot();
signals:
    void okPushButtonClicked();
    void cancelPushButtonClicked();
    void regAllOverSignal();
    void singleBoardNumSignal(int);
    void allRegButtonClicked();

};

#endif // BOARDREGISTER_H
