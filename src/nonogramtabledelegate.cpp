#include "nonogramtabledelegate.h"
#include "nonogrammodel.h"
#include "numberlineedit.h"

NonogramTableDelegate::NonogramTableDelegate(QObject* parent):QItemDelegate(parent){
}

QWidget *NonogramTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const{
    NumberLineEdit *editor = new NumberLineEdit(parent);
    return editor;
}

void NonogramTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const{
    QString data=index.model()->data(index, Qt::EditRole).toString();
    if (data!="+")
        ((QLineEdit*)editor)->setText(data);
}

void NonogramTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const{
    model->setData(index, ((QLineEdit*)editor)->text());
}
