#ifndef HANDWIDGET_H
#define HANDWIDGET_H

#define NODE_START 0x01
#define NODE_STOP 0x02
//#define LINK_ACTION_START 0x01
//#define LINK_ACTION_STOP 0x02

#include <QWidget>

class QLabel;
class QPushButton;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QFrame;
class QTableWidget;

class HandWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HandWidget(QWidget *parent = 0);
    ~HandWidget();
    HandWidget(int index, QWidget *parent = 0);
    void openFunction();
    void closeFunction();
private:
    void createItems();
    QFrame *handwidgetFrame;
        QFrame *allwidgetFrame;
    QLabel *hostLabel;
    QLabel *iobrLabel;
    QLabel *loopLabel;
    QLabel *startnodeLabel;
    QLabel *endnodeLabel;
    QLabel *equipLabel;
        QLabel *nohandwidgetLabel;

    QSpinBox *hostSpinBox;
    QSpinBox *iobrSpinBox;
    QSpinBox *loopSpinBox;
    QSpinBox *startnodeSpinBox;
    QSpinBox *endnodeSpinBox;
    QComboBox *equipComboBox;

    QPushButton *startPushButton;
    QPushButton *stopPushButton;
    QPushButton *findPushButton;
    QTableWidget *handTableWidget;
signals:
//    void handstartNode(int nodeid);
//    void handstopNode(int nodeid);
    void handNode(char action,int nodeid);//action is start or stop
    //#define NODE_START 0x01
    //#define NODE_STOP 0x02
public slots:
    void startPushButtonClicked();
    void stopPushButtonClicked();
    void findPushButtonClicked();

};

#endif // HANDWIDGET_H
