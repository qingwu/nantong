#ifndef SOUNDTESTDIALOG_H
#define SOUNDTESTDIALOG_H

#include <QDialog>
class QPushButton;
class QFrame;

class soundTestDialog : public QDialog
{
    Q_OBJECT
public:
    explicit soundTestDialog(QWidget *parent = 0);
private:
    QPushButton *lightAllOnPushButton;
    QPushButton *lightAllOffPushButton;
    QPushButton *fireAlarmSoundPushButton;
    QPushButton *watchSoundPushButton;
    QPushButton *linkSoundPushButton;
    QPushButton *inicatorSoundPushButton;
    QPushButton *faultSoundPushButton;
    QPushButton *printerPushButton;
    QPushButton *cancelPushButton;
    QFrame *mainFrame;

    void createItems();
signals:

public slots:


};

#endif // SOUNDTESTDIALOG_H
