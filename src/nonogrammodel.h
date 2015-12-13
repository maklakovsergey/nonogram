#ifndef NONOGRAMMODEL_H
#define NONOGRAMMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QStandardItemModel>
#include "nonogram.h"
#include <memory>

class NonogramModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit NonogramModel(QObject *parent = 0);
    ~NonogramModel();
    void setNonogram(Nonogram* nonogram);
    bool setData(const QModelIndex &index, const QVariant &value, int role);

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
private:
    bool _editing;
    bool _headerVisible;
    Nonogram* _nonogram;
    int _maxColumnInfo;
    int _maxRowInfo;
    int _dataBlockColumn;
    int _dataBlockRow;

    QStandardItem* itemAt(int row, int column);
    QStandardItem* setupInfoItem(QStandardItem* item, const QString& value);
    QStandardItem* setupDataItem(QStandardItem* item, Nonogram::CellStatus status);
    QStandardItem* setupAddItem(QStandardItem *item);
    void refreshRowInfoSize();
    void refreshColumnInfoSize();
};

#endif // NONOGRAMMODEL_H
