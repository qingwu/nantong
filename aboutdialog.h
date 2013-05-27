#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtGui/QDialog>

class QLabel;
class QPushButton;

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private:
    void createItems();

    QLabel *aboutSystem;
    QPushButton *okButton;

};

#endif // ABOUTDIALOG_H
