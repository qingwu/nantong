#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QtGui/QDialog>
#include "printerdialog.h"

/*Does not include the header files but forword declaration,
  *so that the compiler speed will be higher
*/
class QMenu;
class QMenuBar;
class QLabel;
class QStatusBar;
class QFrame;
class QPushButton;
class QTextCodec;
class LinkDialog;
class Link;


class MenuWindow : public QDialog
{
    Q_OBJECT

public:
    MenuWindow(QWidget *parent = 0);
    ~MenuWindow();

    Link *nowLink;
signals:
    void regButtonClicked();
    void closedialog();
    void hidesetshowview();
    void transferEvent(/*int ti,*/ int iobrnum,int loopnum,int nodenum,unsigned char nodetype, unsigned char nodeState);
    //void linkComeSignal(int boardnumber,int loopnumber,int nodenumber,int start_delay,int pulse_delay);
    void linkNodeSignal(int nodeid);
    void unitSetDialogClose(int);

public  slots:
     void getprinterFlag(int getflag);//yang ju

private slots:
    void hideWindow();
    void secondhideWindow();


    void nowLinkClear();
    void unitSet();
    void busSet();
    void linkSet();
    void printerSet();
    void networkSet();
    void hostSet();
    void timeSet();
    void shieldSet();

    void singleTest();
    void soundTest();
    void serialTest();
    void networkTest();
    void logicTest();

protected:
    void timerEvent (QTimerEvent *event);
private:
    void createActions();
    void createMenus();
    void createStatusBar();
    void addGroupButtons();

    int printerFlagOfWindow;//yang  ju

    QMenuBar *menuBar;
    /*Here Will be modified*/
    QMenu *systemSetMenu;
    QMenu *systemDebugMenu;
    /*QAction*/
    QAction *hideAction;
    QAction *secondhideAction;

    QAction *unitSetAction;
    QAction *busSetAction;
    QAction *linkSetAction;
    QAction *printerSetAction;
    QAction *networkSetAction;
    QAction *hostSetAction;
    QAction *timeSetAction;
    QAction *registerSetAction;
    QAction *shieldSetAction;

    QAction *singleTestAction;
    QAction *soundTestAction;
    QAction *serialTestAction;
    QAction *networkTestAction;
    QAction *logicTestAction;

    QStatusBar *statusBar;
    QLabel *remindLabel;
    QLabel *timeLabel;

    QFrame *buttonFrame;
    /*QPushButton*/
    QPushButton *unitSetButton;
    QPushButton *busSetButton;
    QPushButton *linkSetButton;
    QPushButton *printerSetButton;
    QPushButton *networkSetButton;
    QPushButton *hostSetButton;
    QPushButton *timeSetButton;
    QPushButton *registerSetButton;
    QPushButton *shieldSetButton;

    QPushButton *singleTestButton;
    QPushButton *soundTestButton;
    QPushButton *serialTestButton;
    QPushButton *networkTestButton;
    QPushButton *logicTestButton;

    LinkDialog *linkwin;
};

#endif // MENUWINDOW_H
