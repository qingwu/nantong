#include <QComboBox>
#include <QString>
#include <QStandardItemModel>

#include "combodelegate.h"
#include "info.h"
comboDelegate::comboDelegate(QObject *parent)
    :QItemDelegate(parent)
{

}
comboDelegate::comboDelegate(int crtcolumn,QObject *parent )
    :QItemDelegate(parent)
{
    currentcolumn = crtcolumn;
}
QWidget *comboDelegate::createEditor ( QWidget * parent,
                       const QStyleOptionViewItem & /*option*/,
                       const QModelIndex & /*index */) const
{
    QComboBox *editor = new QComboBox(parent);
    switch(currentcolumn)
    {
        //case 1://regist
        case 4:
            editor->addItem(tr("√"));
            editor->addItem(tr("×"));
        break;
        case 2:
        {
            for (int i = 0;i < (int)sizeof(char_liandong) /((int)sizeof(char)*4);i++)
            {
                editor->addItem(tr(char_liandong[i]));
            }
        }
        break;
        case 3:
        {
            for (int i = 0;i < (int)sizeof(char_danyuan) /((int)sizeof(char)*4);i++)
            {
                editor->addItem(tr(char_danyuan[i]));
            }
        }
        break;
        case 1:
        case 7:
            editor->addItem(tr("是"));
            editor->addItem(tr("否"));
        break;

        case 13:
            editor->addItem(tr("火警"));
            editor->addItem(tr("预警"));
        break;
        default:
        break;

    }    
    editor->installEventFilter(const_cast<comboDelegate *>(this));
    return editor;
}

void comboDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
    QString str = index.model()->data(index).toString();

    QComboBox *box = static_cast<QComboBox *>(editor);
    int i = box->findText(str);
    box->setCurrentIndex(i);
}
void comboDelegate::setModelData ( QWidget * editor, QAbstractItemModel * model,
                   const QModelIndex & index ) const
{
    QComboBox *box = static_cast<QComboBox *>(editor);
    QString str = box->currentText();
    model->setData(index,str);
}

void comboDelegate::updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option,
                                          const QModelIndex & /*index*/ ) const
{
    editor->setGeometry(option.rect);
}

comboDelegate::~comboDelegate()
{
    delete(this);
}
