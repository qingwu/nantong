#ifndef VIEWMENUWINDOW_H
#define VIEWMENUWINDOW_H

#include <QtGui/QDialog>
class QMenu;
class QMenuBar;
class QLabel;
class QStatusBar;
class QFrame;
class QPushButton;
class LinkDialog;

class ViewMenuWindow : public QDialog
{
    Q_OBJECT
public:
    /*explicit*/ ViewMenuWindow(QWidget *parent = 0);
    ~ViewMenuWindow();
signals:
    void closedialog();
    void hideviewshowset();
public slots:
    void hideWindow();
    void secondhideWindow();

    void unitInfo();
    void busInfo();
    void linkInfo();
    void locationInfo();
    void networkInfo();

    void allHis();
    void fireHis();
    void faultHis();
    void linkHis();
    void feedbackHis();
    void operateHis();

    void systemAbout();
    void runAbout();

protected:
    void timerEvent (QTimerEvent *event);
private:
    void createActions();
    void createMenus();
    void createStatusBar();
    void addGroupButtons();

    QMenuBar *menuBar;
    /*Here Will be modified*/
    QMenu *systemInfoMenu;
    QMenu *historySeekMenu;
    QMenu *aboutMenu;

    /*QAction*/
    QAction *hideAction;
    QAction *secondhideAction;

    QAction *unitInfoAction;
    QAction *busInfoAction;
    QAction *linkInfoAction;
    QAction *locationInfoAction;
    QAction *networkInfoAction;

    QAction *allHisAction;
    QAction *fireHisAction;
    QAction *faultHisAction;
    QAction *linkHisAction;
    QAction *feedbackHisAction;
    QAction *operateHisAction;

    QAction *systemAboutAction;
    QAction *runAboutAction;

    QStatusBar *statusBar;
    QLabel *remindLabel;
    QLabel *timeLabel;

    QFrame *buttonFrame;

    /*QPushButton*/
    QPushButton *unitInfoButton;
    QPushButton *busInfoButton;
    QPushButton *linkInfoButton;
    QPushButton *locationInfoButton;
    QPushButton *networkInfoButton;

    QPushButton *allHisButton;
    QPushButton *fireHisButton;
    QPushButton *faultHisButton;
    QPushButton *linkHisButton;
    QPushButton *feedbackHisButton;
    QPushButton *operateHisButton;

    QPushButton *systemAboutButton;
    QPushButton *runAboutButton;

    LinkDialog *linkwin2;
};

#endif // VIEWMENUWINDOW_H
