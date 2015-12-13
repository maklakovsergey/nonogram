#include "nonogrammodel.h"
#include <QBrush>
#include <QDebug>

NonogramModel::NonogramModel(QObject *parent) : QStandardItemModel(parent)
{
    _nonogram=NULL;
    _editing=true;
    _headerVisible=true;
    QStandardItem* prototype=new QStandardItem();
    prototype->setDragEnabled(false);
    prototype->setDropEnabled(false);
    prototype->setSelectable(false);
    prototype->setEditable(false);
    prototype->setCheckable(false);
    prototype->setTextAlignment(Qt::AlignCenter);
    prototype->setBackground(QBrush(Qt::white));
    setItemPrototype(prototype);
}

NonogramModel::~NonogramModel(){
}

QStandardItem* NonogramModel::itemAt(int row, int column){
    QStandardItem* itm=item(row, column);
    if (!itm){
        itm=itemPrototype()->clone();
        setItem(row, column, itm);
    }
    return itm;
}

QStandardItem*  NonogramModel::setupAddItem(QStandardItem* item){
    item->setText("+");
    item->setBackground(QBrush(Qt::lightGray));
    item->setEditable(true);
    return item;
}

QStandardItem*  NonogramModel::setupDataItem(QStandardItem* item, Nonogram::CellStatus status){
    switch(status){
    case Nonogram::Full:    item->setBackground(QBrush(Qt::darkGray)); break;
    case Nonogram::Free:    item->setBackground(QBrush(Qt::white)); break;
    case Nonogram::Unknown: item->setBackground(QBrush(Qt::lightGray)); break;
    default: break;
    }
    return item;
}

QStandardItem*  NonogramModel::setupInfoItem(QStandardItem* item, const QString& value){
    item->setBackground(QBrush(Qt::gray));
    item->setEditable(true);
    item->setText(value);
    return item;
}

bool NonogramModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if (role==Qt::EditRole){
        int row=index.row()-_dataBlockRow;
        int column=index.column()-_dataBlockColumn;
        bool isInt;
        int intValue=value.toString().toInt(&isInt);
        if (intValue==0)
            isInt=false;
        if (row>=0&&column<0){
            //edit row info block
            InfoListType info(_nonogram->rowInfo(row));
            int listIndex=info.count()+column+(_editing?1:0);
            if (listIndex<0){
                if (isInt)
                    info.insert(0, intValue);
            }
            else if (listIndex==info.count()){
                if (isInt)
                    info.append(intValue);
            }
            else{
                if (isInt)
                    info.replace(listIndex, intValue);
                else
                    info.removeAt(listIndex);
            }
            _nonogram->setRowInfo(row, info);
        }
        else if (column>=0&&row<0){
            //edit column info block
            InfoListType info(_nonogram->columnInfo(column));
            int listIndex=info.count()+row+(_editing?1:0);
            if (listIndex<0){
                if (isInt)
                    info.insert(0, intValue);
            }
            else if (listIndex==info.count()){
                if (isInt)
                    info.append(intValue);
            }
            else{
                if (isInt)
                    info.replace(listIndex, intValue);
                else
                    info.removeAt(listIndex);
            }
            _nonogram->setColumnInfo(column, info);
        }
        return true;
    }
    else
        return QStandardItemModel::setData(index, value, role);
}

void NonogramModel::setNonogram(Nonogram* nonogram){
    _nonogram=nonogram;
    connect(_nonogram, &Nonogram::columnInfoChanged, this, &NonogramModel::refreshColumn);
    connect(_nonogram, &Nonogram::rowInfoChanged, this, &NonogramModel::refreshRow);
    connect(_nonogram, &Nonogram::dataChanged, this, &NonogramModel::refreshData, Qt::BlockingQueuedConnection);
    connect(_nonogram, &Nonogram::rowInserted, this, &NonogramModel::rowInserted);
    connect(_nonogram, &Nonogram::rowRemoved, this, &NonogramModel::rowRemoved);
    connect(_nonogram, &Nonogram::columnInserted, this, &NonogramModel::columnInserted);
    connect(_nonogram, &Nonogram::columnRemoved, this, &NonogramModel::columnRemoved);
    refreshInfo();
}

void NonogramModel::refreshInfo(){
    if (!_nonogram)
        return;
    int columnCount=_nonogram->width();
    _maxColumnInfo=0;
    for(int i=0; i<columnCount; i++)
        if (_nonogram->columnInfo(i).count()>_maxColumnInfo)
            _maxColumnInfo=_nonogram->columnInfo(i).count();

    int rowCount=_nonogram->height();
    _maxRowInfo=0;
    for(int i=0; i<rowCount; i++)
        if (_nonogram->rowInfo(i).count()>_maxRowInfo)
            _maxRowInfo=_nonogram->rowInfo(i).count();

    _dataBlockColumn=_maxRowInfo+(_editing?1:0);
    _dataBlockRow=_maxColumnInfo+(_editing?1:0);

    beginResetModel();
    clear();

    setColumnCount(_dataBlockColumn+columnCount);
    setRowCount(_dataBlockRow+rowCount);

    for(int c=0; c<_dataBlockColumn; c++)
        for(int r=0; r<_dataBlockRow; r++)
            setItem(r, c, itemPrototype()->clone());

    for(int r=0; r<rowCount; r++)
        refreshRow(r);
    for(int c=0; c<columnCount; c++)
        refreshColumn(c);

    if (_editing){
        for(int r=0; r<rowCount; r++)
            setupAddItem(itemAt(r+_dataBlockRow, _maxRowInfo));
        for(int c=0; c<columnCount; c++)
            setupAddItem(itemAt(_maxColumnInfo, c+_dataBlockColumn));
    }
    for(int r=0; r<rowCount; r++)
        for(int c=0; c<columnCount; c++)
            refreshData(r, c);

    endResetModel();
}

void NonogramModel::refreshRowInfoSize(){
    int rowCount=_nonogram->height();
    int newMaxRowInfo=0;
    for(int i=0; i<rowCount; i++)
        if (_nonogram->rowInfo(i).count()>newMaxRowInfo)
            newMaxRowInfo=_nonogram->rowInfo(i).count();

    while (_maxRowInfo<newMaxRowInfo){
        _maxRowInfo++;
        QList<QStandardItem*> newColumn;
        for(int i=0; i<_dataBlockRow; i++)
            newColumn.append(itemPrototype()->clone());
        for(int i=0; i<rowCount; i++)
            newColumn.append(setupInfoItem(itemPrototype()->clone(), ""));
        insertColumn(0, newColumn);
    }
    while(_maxRowInfo>newMaxRowInfo){
        _maxRowInfo--;
        removeColumn(0);
    }
    _dataBlockColumn=_maxRowInfo+(_editing?1:0);
}

void NonogramModel::refreshColumnInfoSize(){
    int columnCount=_nonogram->width();
    int newMaxColumnInfo=0;
    for(int i=0; i<columnCount; i++)
        if (_nonogram->columnInfo(i).count()>newMaxColumnInfo)
            newMaxColumnInfo=_nonogram->columnInfo(i).count();

    while (_maxColumnInfo<newMaxColumnInfo){
        _maxColumnInfo++;
        QList<QStandardItem*> newRow;
        for(int i=0; i<_dataBlockColumn; i++)
            newRow.append(itemPrototype()->clone());
        for(int i=0; i<columnCount; i++)
            newRow.append(setupInfoItem(itemPrototype()->clone(), ""));
        insertRow(0, newRow);
    }
    while(_maxColumnInfo>newMaxColumnInfo){
        _maxColumnInfo--;
        removeRow(0);
    }
    _dataBlockRow=_maxColumnInfo+(_editing?1:0);
}

void NonogramModel::refreshRow(int row){
    refreshRowInfoSize();

    InfoListType info=_nonogram->rowInfo(row);
    for(int c=0; c<_maxRowInfo; c++){
        QString text;
        if (c<info.count())
            text=QString::number(info[info.count()-1-c]);
        setupInfoItem(itemAt(row+_dataBlockRow, _maxRowInfo-c-1), text);
    }
}

void NonogramModel::refreshColumn(int column){
    refreshColumnInfoSize();

    InfoListType info=_nonogram->columnInfo(column);
    for(int r=0; r<_maxColumnInfo; r++){
        QString text;
        if (r<info.count())
            text=QString::number(info[info.count()-r-1]);
        setupInfoItem(itemAt(_maxColumnInfo-r-1, column+_dataBlockColumn), text);
    }
}

void NonogramModel::refreshData(int row, int column){
    setupDataItem(itemAt(row+_dataBlockRow, column+_dataBlockColumn), _nonogram->data(row, column));
}

void NonogramModel::rowInserted(int row){
    QList<QStandardItem*> newRow;
    InfoListType info=_nonogram->rowInfo(row);
    for(int c=0; c<_maxRowInfo; c++){
        QString text;
        if (c<info.count())
            text=QString::number(info[info.count()-c-1]);
        newRow.append(setupInfoItem(itemPrototype()->clone(), text));
    }
    if (_editing)
        newRow.append(setupAddItem(itemPrototype()->clone()));
    for(int c=0; c<_nonogram->width(); c++)
        newRow.append(setupDataItem(itemPrototype()->clone(), _nonogram->data(row, c)));
    insertRow(_dataBlockRow+row, newRow);
    refreshRowInfoSize();
}

void NonogramModel::columnInserted(int column){
    QList<QStandardItem*> newColumn;
    InfoListType info=_nonogram->columnInfo(column);
    for(int r=0; r<_maxColumnInfo; r++){
        QString text;
        if (r<info.count())
            text=QString::number(info[info.count()-r-1]);
        newColumn.append(setupInfoItem(itemPrototype()->clone(), text));
    }
    if (_editing)
        newColumn.append(setupAddItem(itemPrototype()->clone()));
    for(int r=0; r<_nonogram->height(); r++)
        newColumn.append(setupDataItem(itemPrototype()->clone(), _nonogram->data(r, column)));
    insertColumn(_dataBlockColumn+column, newColumn);
    refreshColumnInfoSize();
}

void NonogramModel::rowRemoved(int row){
    removeRow(_dataBlockRow+row);
    refreshRowInfoSize();
}

void NonogramModel::columnRemoved(int column){
    removeColumn(_dataBlockColumn+column);
    refreshColumnInfoSize();
}
