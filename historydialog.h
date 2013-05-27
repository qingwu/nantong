#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QtGui/QDialog>
class QLabel;
class QLineEdit;
class QPushButton;
class QStringList;
class QTableWidget;
class HistoryDialog : public QDialog
{
    Q_OBJECT
public:
    int type;
    explicit HistoryDialog(QWidget *parent = 0);
    ~HistoryDialog();
    HistoryDialog(int index, QWidget *parent = 0);
signals:

public slots:
    void doPrint();
    void selectedCellChangedSlot(int);
private:
    void createItems();
    void readHistory();
    void rangeFaultMessage();
    void printerFaultMessage();

    QTableWidget *table;
    QLabel *from;
    QLabel *to;
    QLineEdit *fromLineEdit;
    QLineEdit *toLineEdit;
    QPushButton *printButton;
    QPushButton *exitButton;
};

#endif // HISTORYDIALOG_H
