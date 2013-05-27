#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QList>


class QLabel;
class QLineEdit;
class QPushButton;
class QStringList;
class QTableWidget;
class HandWidget;
class QTimer;

class eventDialog : public QDialog
{
    Q_OBJECT
public:
    int type;
    int fireexitnumber;
    int exitnumber;
    explicit eventDialog(QWidget *parent = 0);
    ~eventDialog();
    eventDialog(int index, int exitnum, int fireexitnum, QWidget *parent = 0);
    eventDialog(QString, QList<QString> ,int index, int exitnum, int fireexitnum, QWidget *parent = 0);

    QLabel *timeLabel;
    QLabel *firstfireTipLabel;
    QTableWidget *firstFireTableWidget;
    QTableWidget *eventTableWidget;
    HandWidget *handwidget;
    QTableWidget *shieldTableWidget;

protected:
    void timerEvent (QTimerEvent *event);
private:
    //QTimer *timer3s;
    void createItems();
    void readEvent(QString, QList<QString> eventList );

signals:

public slots:
        void oneEventShowSLot(QString, QList<QString>&, char );
        void updateOneEventSlot(QString);
        void mySetWindowTitle(char flag);
        void clearEventDialogSlot();


};

#endif // EVENTDIALOG_H
