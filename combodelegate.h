#ifndef COMBODELEGATE_H
#define COMBODELEGATE_H

#include <QItemDelegate>

class comboDelegate:public QItemDelegate
{
    Q_OBJECT
public:
    int currentcolumn;
    explicit comboDelegate(QObject *parent = 0);
    comboDelegate(int ,QObject *parent = 0);

    QWidget *createEditor ( QWidget * parent, const QStyleOptionViewItem & option,
                           const QModelIndex & index ) const;
    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData ( QWidget * editor, QAbstractItemModel * model,
                       const QModelIndex & index ) const;
    void updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option,
                                              const QModelIndex & index ) const;

    ~comboDelegate();

};

#endif // COMBODELEGATE_H
