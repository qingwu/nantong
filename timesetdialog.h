#ifndef TIMESETDIALOG_H
#define TIMESETDIALOG_H

#include <QtGui/QDialog>
#include <QDateTime>
class QLabel;
class QSpinBox;
class QPushButton;

class TimeSetDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TimeSetDialog(QWidget *parent = 0);
    ~TimeSetDialog();

private slots:
    void timeResetSlot();
signals:
    void setSuccessful();
private:
    void createItems();
    QDateTime nDateTime;

    QLabel *date;
    QLabel *time;
    QLabel *year;
    QLabel *month;
    QLabel *day;
    QLabel *timemark1;
    QLabel *timemark2;

    QSpinBox *yearSpinBox;
    QSpinBox *monthSpinBox;
    QSpinBox *daySpinBox;
    QSpinBox *hourSpinBox;
    QSpinBox *minuteSpinBox;
    QSpinBox *secondSpinBox;

    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // TIMESETDIALOG_H
