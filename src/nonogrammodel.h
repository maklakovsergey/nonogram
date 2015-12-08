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
signals:

private slots:
    void onItemChanged();
    void refreshRow(int row);
    void refreshColumn(int column);
private:
    bool _editing;
    Nonogram* _nonogram;
    int _maxColumnInfo;
    int _maxRowInfo;
    int _dataBlockColumn;
    int _dataBlockRow;

    void refreshInfo();

    QStandardItem* itemAt(int row, int column);
    QStandardItem* setupInfoItem(QStandardItem* item, const QString& value);
    QStandardItem* setupDataItem(QStandardItem* item, Nonogram::CellStatus status);
    QStandardItem* setupAddItem(QStandardItem *item);
};

#endif // NONOGRAMMODEL_H
