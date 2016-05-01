#ifndef NONOGRAMTABLEDELEGATE_H
#define NONOGRAMTABLEDELEGATE_H

#include <QItemDelegate>

class NonogramTableDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    NonogramTableDelegate(QObject* parent=0);
    ~NonogramTableDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    inline QSize cellSize() const              {return _cellSize;}
    inline int borderWidth() const             {return _borderWidth;}
    inline Qt::GlobalColor borderColor() const {return _borderColor;}
    inline int groupSize() const               {return _groupSize;}
    inline bool borderVisible() const          {return _borderVisible;}

    inline void setCellSize(QSize cellSize)                 {_cellSize=cellSize;}
    inline void setBorderWidth(int borderWidth)             {_borderWidth=borderWidth;}
    inline void setBorderColor(Qt::GlobalColor borderColor) {_borderColor=borderColor;}
    inline void setGroupSize(int groupSize)                 {_groupSize=groupSize;}
    inline void setBorderVisible(bool borderVisible)        {_borderVisible=borderVisible;}


private:
    Qt::Edges edgesForIndex(const QModelIndex &index) const;

    QSize _cellSize;
    int _borderWidth;
    Qt::GlobalColor _borderColor;
    int _groupSize;
    bool _borderVisible;
};

#endif // NONOGRAMTABLEDELEGATE_H
