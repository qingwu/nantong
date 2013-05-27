#ifndef SHIELDWIDGET_H
#define SHIELDWIDGET_H

#include <QtGui/QDialog>

class QLabel;
class QPushButton;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QFrame;
class QTableWidget;

class ShieldWidget : public QDialog
{
    Q_OBJECT
public:
        int messageid;
    explicit ShieldWidget(QWidget *parent = 0);
    ~ShieldWidget();
//    ShieldWidget(int index, QWidget *parent = 0);
private:



    void createItems();
    QFrame *shieldwidgetFrame;
    QLabel *hostLabel;
    QLabel *iobrLabel;
    QLabel *loopLabel;
    QLabel *startnodeLabel;
    QLabel *endnodeLabel;
    QLabel *equipLabel;
        QLabel *buttonLabel;

    QSpinBox *hostSpinBox;
    QSpinBox *iobrSpinBox;
    QSpinBox *loopSpinBox;
    QSpinBox *startnodeSpinBox;
    QSpinBox *endnodeSpinBox;
    QComboBox *equipComboBox;

    QPushButton *shieldPushButton;
    QPushButton *unshieldPushButton;
    QPushButton *clearPushButton;
    QTableWidget *shieldTableWidget;

    void insertintoShieldHis(int unitid,int type);
    void deletefromShieldHis(int unitid);


signals:
        void shieldhisChange(int);

public slots:
    void shieldPushButtonClicked();
    void unshieldPushButtonClicked();
    void clearPushButtonClicked();

};

#endif // SHIELDWIDGET_H
