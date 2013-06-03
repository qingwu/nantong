#include "spindelegate.h"
#include <QSpinBox>
#include <QStandardItemModel>

spinDelegate::spinDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}
spinDelegate::spinDelegate(int crtcolumn,QObject *parent )
    :QItemDelegate(parent)
{
    currentcolumn = crtcolumn;
}
QWidget *spinDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    QSpinBox *editor = new QSpinBox(parent);
    switch(currentcolumn)
    {
        case 0:
            editor->setRange(1, 255);
        break;
        case 5:
            editor->setRange(1, 120);
        break;
        case 8:
        case 9:
        case 10:
        case 11:
            editor->setRange(0, 100);
            break;            
        case 12:
            editor->setRange(0, 9999);
            break;

        /*case 9:
            editor->setRange(1,100);
            break;
        case 10:
            editor->setRange(1,100);
            break;
        case 11:
            editor->setRange(1,100);
            break;
        case 12:
            editor->setRange(1,100);
            break;    */

        default:
            break;

    }
    //editor->setValue(1);
    editor->installEventFilter(const_cast<spinDelegate*>(this));
    return editor;
}

void spinDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    int value = index.model()->data(index).toInt();
    QSpinBox *spin = static_cast<QSpinBox*>(editor);
    spin->setValue(value);
}

void spinDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QSpinBox *spin = static_cast<QSpinBox*>(editor);
    int value = spin->value();

    model->setData(index, value);
}

void spinDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
spinDelegate::~spinDelegate()
{
    delete(this);
}
