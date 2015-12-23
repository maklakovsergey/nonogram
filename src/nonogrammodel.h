#ifndef NONOGRAMMODEL_H
#define NONOGRAMMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QStandardItemModel>
#include <qvector.h>
#include "nonogram.h"

class NonogramModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit NonogramModel(QObject *parent = 0);
    ~NonogramModel();
    void setNonogram(Nonogram* nonogram);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
	Qt::ItemFlags flags(const QModelIndex & index) const;

    inline int dataBlockRow()const {return _dataBlockRow;}
    inline int dataBlockColumn()const {return _dataBlockColumn;}

    inline bool editing()       const          {return _editing;}
    inline bool headerVisible() const          {return _headerVisible;}
    inline void setEditing(bool editing)       {_editing=editing;}
    inline void setHeaderVisible(bool visible) {_headerVisible=visible;}
    void refreshInfo();
signals:

private slots:
    void refreshData(int row, int column);
    void refreshRow(int row);
    void refreshColumn(int column);
    void rowInserted(int row);
    void columnInserted(int column);
    void rowRemoved(int row);
    void columnRemoved(int column);

    void onItemChanged(QStandardItem * item);
private:
    bool _editing;
    bool _headerVisible;
    Nonogram* _nonogram;
    int _maxColumnInfo;
    int _maxRowInfo;
    int _dataBlockColumn;
    int _dataBlockRow;

    QStandardItem* setupInfoItem(const QModelIndex& index, const QString& value, const Nonogram::LineStatus status=Nonogram::Normal);
    QStandardItem* setupDataItem(const QModelIndex& index, Nonogram::CellStatus status);
    QStandardItem* setupAddItem(const QModelIndex& index);
    void refreshRowInfoSize();
    void refreshColumnInfoSize();

	QVector<int> redrawCount;
	void printRedrawCount() const;
};

#endif // NONOGRAMMODEL_H
