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

    QLabel *fireLabel;
    QLabel *linkLabel;
    QLabel *superviseLabel;
    QLabel *faultLabel;
    QLabel *delayLabel;
    QLabel *shieldLabel;

    QLabel *firenumLabel;
    QLabel *linknumLabel;
    QLabel *supervisenumLabel;
    QLabel *faultnumLabel;
    QLabel *delaynumLabel;
    QLabel *shieldnumLabel;

    void changeEventNum(int , int , int ,int  ,int);

protected:
    void timerEvent (QTimerEvent *event);
private:
    //QTimer *timer3s;
    void createItems();
    void readEvent(QString, QList<QString> eventList );

    /*QAction*/
    QAction *hideAction;
    QAction *secondhideAction;

signals:

        void hideWindowSignal();
        void secondhideWindowSignal();

public slots:
        void oneEventShowSLot(QString, QList<QString>&, char );
        void updateOneEventSlot(QString);
        void mySetWindowTitle(char flag);
        void clearEventDialogSlot();

        void hideWindow();
        void secondhideWindow();


};

#endif // EVENTDIALOG_H
