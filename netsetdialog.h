#ifndef NETSETDIALOG_H
#define NETSETDIALOG_H

#include <QtGui/QDialog>

class QLabel;
class QSpinBox;
class QPushButton;
class QTableWidget;
class NetSetDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NetSetDialog(QWidget *parent = 0);
    ~NetSetDialog();
    NetSetDialog(bool editEnableFlag,QWidget *parent=0);

signals:

public slots:
private slots:
    void ok();
private:
    void createItems(bool editEnableFlag);

    QTableWidget *netSetTable;
    QLabel *localHostNum;
    QSpinBox *localHostBox;

    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // NETSETDIALOG_H
