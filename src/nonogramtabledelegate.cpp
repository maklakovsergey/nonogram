#include "nonogramtabledelegate.h"
#include <QLineEdit>
#include <QIntValidator>
#include "nonogrammodel.h"

NonogramTableDelegate::NonogramTableDelegate(QObject* parent):QItemDelegate(parent){
}

QWidget *NonogramTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setValidator(new QIntValidator(1, 99, parent));
    return editor;
}

void NonogramTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const{
    ((QLineEdit*)editor)->setText(index.model()->data(index, Qt::EditRole).toString());
}

void NonogramTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const{
    model->setData(index, ((QLineEdit*)editor)->text());
}
