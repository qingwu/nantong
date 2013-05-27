#ifndef GROUPSETDIALOG_H
#define GROUPSETDIALOG_H

#include <QDialog>

class QLabel;
class QPushButton;
class QLineEdit;
class QComboBox;
class QSpinBox;

class GroupsetDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GroupsetDialog(QWidget *parent = 0);
    ~ GroupsetDialog();
    GroupsetDialog(int iobr,int loop, QWidget *parent = 0);

signals:

public slots:
    void okButtonClicked();

private:
    int iobrnum;
    int loopnum;
    void createItems();

    QLabel *tipLabel;
    QLabel *nodeLabel1;
    QLabel *nodeLabel2;
    QLabel *equipLabel;
    QLabel *maskLabel;
    QLabel *startDelayLabel;
    QLabel *ImpulseDelayLabel;
    QLabel *reDetectorLabel;
    QLabel *flrIndicatorLabel;
    QLabel *buildingLabel;
    QLabel *floorLabel;
    QLabel *zoneLabel;
    QLabel *roomLabel;
    QLabel *alarmLabel;
    QLabel *positionLabel;

//    QLineEdit *startNode;
//    QLineEdit *endNode;

    QSpinBox *startNode;
    QSpinBox *endNode;
    QComboBox *equipComboBox;
    QComboBox *maskComboBox;
    QSpinBox *startDelaySpinBox;
    QSpinBox *ImpulseDelaySpinBox;
    QComboBox *reDetectorComboBox;
    QSpinBox *flrIndicatorSpinBox;
    QSpinBox *buildingSpinBox;
    QSpinBox *floorSpinBox;
    QSpinBox *zoneSpinBox;
    QSpinBox *roomSpinBox;
    QComboBox *alarmComboBox;
    QLineEdit *posititionLineEdit;

    QPushButton *okButton;
    QPushButton *cancelButton;


    void insertintoShieldHis(int unitid,int type);
    void deletefromShieldHis(int unitid);



};

#endif // GROUPSETDIALOG_H
