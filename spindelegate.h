#ifndef SPINDELEGATE_H
#define SPINDELEGATE_H

#include <QItemDelegate>

class spinDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    int currentcolumn;
    explicit spinDelegate(QObject *parent = 0);
    spinDelegate(int ,QObject *parent = 0);

    QWidget *createEditor ( QWidget * parent, const QStyleOptionViewItem & option,
                           const QModelIndex & index ) const;
    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData ( QWidget * editor, QAbstractItemModel * model,
                       const QModelIndex & index ) const;
    void updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option,
                                              const QModelIndex & index ) const;

    ~spinDelegate();
signals:

public slots:

};

#endif // SPINDELEGATE_H
