#ifndef NONOGRAMTABLEDELEGATE_H
#define NONOGRAMTABLEDELEGATE_H

#include <QItemDelegate>

class NonogramTableDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    NonogramTableDelegate(QObject* parent=0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

#endif // NONOGRAMTABLEDELEGATE_H
