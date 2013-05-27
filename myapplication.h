#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

#include <QApplication>

class MyApplication : public QApplication
{
    Q_OBJECT
public:
    bool eventFilter(QObject *, QEvent *);
    MyApplication(int argc, char *argv[]);

signals:
    void restartMainTimerSignal();

public slots:

};

#endif // MYAPPLICATION_H
