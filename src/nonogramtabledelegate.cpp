#include "nonogramtabledelegate.h"
#include "nonogrammodel.h"
#include "numberlineedit.h"
#include <QDebug>
#include <QPainter>

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

QSize NonogramTableDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const{
    QSize size(_cellSize);
    Qt::Edges edges=edgesForIndex(index);
    if (edges&Qt::TopEdge)
        size.setHeight(size.height()+_borderWidth);
    if (edges&Qt::BottomEdge)
        size.setHeight(size.height()+_borderWidth);
    if (edges&Qt::LeftEdge)
        size.setWidth(size.width()+_borderWidth);
    if (edges&Qt::RightEdge)
        size.setWidth(size.width()+_borderWidth);
    return size;
}

void NonogramTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    QStyleOptionViewItem newOption(option);
    QRect r(option.rect);
    Qt::Edges edges=edgesForIndex(index);
    if (edges&Qt::TopEdge){
        newOption.rect.setY(newOption.rect.y()+_borderWidth);
        painter->fillRect(r.x(), r.y(), r.width(), _borderWidth, _borderColor);
    }
    if (edges&Qt::BottomEdge){
        newOption.rect.setHeight(newOption.rect.height()-_borderWidth);
        painter->fillRect(r.x(), r.bottom()-_borderWidth+1, r.width(), _borderWidth, _borderColor);
    }
    if (edges&Qt::LeftEdge){
        newOption.rect.setX(newOption.rect.x()+_borderWidth);
        painter->fillRect(r.x(), r.y(), _borderWidth, r.height(), _borderColor);
    }
    if (edges&Qt::RightEdge){
        newOption.rect.setWidth(newOption.rect.width()-_borderWidth);
        painter->fillRect(r.right()-_borderWidth+1, r.y(), _borderWidth, r.height(), _borderColor);
    }

    QItemDelegate::paint(painter, newOption, index);
}

Qt::Edges NonogramTableDelegate::edgesForIndex(const QModelIndex &index) const{
    Qt::Edges edges;
    NonogramModel* model=(NonogramModel*)index.model();
    int dataRow=index.row()-model->dataBlockRow();
    int dataColumn=index.column()-model->dataBlockColumn();
    if ((dataRow>=0 && dataRow%_groupSize==0) || index.row()==0)
        edges|=Qt::TopEdge;
    if (index.row()==model->rowCount()-1)
        edges|=Qt::BottomEdge;
    if ((dataColumn>=0 && dataColumn%_groupSize==0) || index.column()==0)
        edges|=Qt::LeftEdge;
    if (index.column()==model->columnCount()-1)
        edges|=Qt::RightEdge;
    return edges;
}
