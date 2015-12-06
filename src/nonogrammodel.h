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
//    int columnCount(const QModelIndex &parent) const;
//    int rowCount(const QModelIndex &parent) const;
//    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
signals:

public slots:
    void onItemChanged();

private:
    bool _editing;
    Nonogram* _nonogram;
    int _maxColumnInfo;
    int _maxRowInfo;
    std::shared_ptr<QStandardItem> infoItemPrototype;
    std::shared_ptr<QStandardItem> addItemPrototype;
    std::shared_ptr<QStandardItem> dataItemPrototype;
    std::shared_ptr<QStandardItem> emptyItemPrototype;

    void refreshInfo();
    void refreshRow(int row);
    void refreshColumn(int column);
};

#endif // NOMOGRAMMODEL_H
