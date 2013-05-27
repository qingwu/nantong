#ifndef UNITSET_H
#define UNITSET_H

#include <QtGui/QDialog>

class QLabel;
//class QVBoxLayout;
//class QHBoxLayout;
class QStandardItemModel;
class QTableView;
//class QSpinBox;
class QComboBox;
class QPushButton;
class QStandardItem;
class QStandardItemModel;
class QTableView;
class comboDelegate;
class spinDelegate;
class QModelIndex;
class GroupsetDialog;

#define rowNumber 255
#define columnNumber 15

class UnitSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UnitSetDialog(QWidget *parent = 0);
    ~UnitSetDialog();
    UnitSetDialog(bool editFlag,QWidget *parent = 0);

private:
    bool editEnableFlag;
    int dataChangeFlag;
    bool comboChangeFlag;

//    GroupsetDialog *groupsetwin;

    QStandardItemModel  *model;
    QTableView *tableview;

    QPushButton *okPushButton;
    QPushButton *cancelPushButton;
    QPushButton *groupsetPushButton;

    QLabel *boardLabel;
    QLabel *loopLabel;
    QLabel *zhikongLabel;
    QLabel *zongxianLabel;
    QLabel *registerNodeLabel;
    QLabel *registerNodenumLabel;
    QLabel *danyuanCountnumLabel;
    QLabel *countnumLabel;

    QComboBox *boardComboBox;
    QComboBox *loopComboBox;
    QComboBox *floorshowloopComboBox;
    QComboBox *zhikongComboBox;
    QComboBox *zongxianComboBox;
    QComboBox *typeComboBox;
    QComboBox *danyuanComboBox;
    void createItems();
    void initModel();
    //void saveFile();
    void readUnitSet();
    void insertintoShieldHis(int unitid,int type);
    void deletefromShieldHis(int unitid);
private slots:   
    void okToContinue();
    void dataChangedSlot(const QModelIndex &index);
    void boardreadUnitSet();
    void loopreadUnitSet();
    void danyuanreadUnitSet();
    void typeChange(int);
    void zhikongreadUnitSet();
    void groupsetPushButtonClick();
};

#endif // UNITSET_H
