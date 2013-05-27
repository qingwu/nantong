#ifndef MYQSPLASHSCREEN_H
#define MYQSPLASHSCREEN_H

#include <QSplashScreen>

class myQSplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    //explicit myQSplashScreen(QObject *parent = 0);

signals:

public slots:
    void updateSplashMessage();

};

#endif // MYQSPLASHSCREEN_H
