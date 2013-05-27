#ifndef TYPE_H
#define TYPE_H

//#include <QObject>
#include <QString>
class Type : public QString
{
    //Q_OBJECT
public:
    explicit Type(/*QString *parent = 0*/);
    ~Type();
    static QString char_shijian(int shijian_index, int danyuan_index);
    static QString char_danyuan(int danyuan_index);
    static QString char_liandong(int liandong_index);

    static int int_liandongId(const QString &);
    static int int_danyuanId(const QString &);
//signals:

//public slots:

};

#endif // TYPE_H
