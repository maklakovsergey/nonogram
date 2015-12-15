#include "nonogrammodel.h"
#include <QBrush>
#include <QDebug>

NonogramModel::NonogramModel(QObject *parent) : QStandardItemModel(parent)
{
    _nonogram=NULL;
    _editing=true;
    _headerVisible=true;
    QStandardItem* emptyItemPrototype=new QStandardItem();
    emptyItemPrototype->setFlags(Qt::ItemIsEnabled);
    emptyItemPrototype->setTextAlignment(Qt::AlignCenter);
    emptyItemPrototype->setBackground(QBrush(Qt::white));
    setItemPrototype(emptyItemPrototype);

    //connect(this, &NonogramModel::itemChanged, this, &NonogramModel::onItemChanged);
}

NonogramModel::~NonogramModel(){
}

void NonogramModel::onItemChanged(QStandardItem * item){
    QModelIndex index(item->index());
    int row=index.row();
    int column=index.column();
    const char* text;
    if (row>=_dataBlockRow && column>=_dataBlockColumn)
        text="data";
    else if (row<_dataBlockRow && column>=_dataBlockColumn)
        text="row info";
    else if (row>=_dataBlockRow && column<_dataBlockColumn)
        text="column info";
    else
        text="empty";
    qDebug()<<row<<column<<text;
}

QStandardItem*  NonogramModel::setupAddItem(){
    QStandardItem* item=itemPrototype()->clone();
    item->setEditable(true);
    item->setTextAlignment(Qt::AlignCenter);
    item->setBackground(QBrush(Qt::lightGray));
    item->setText("+");
    return item;
}

QStandardItem*  NonogramModel::setupDataItem(Nonogram::CellStatus status){
    QStandardItem* item=itemPrototype()->clone();
    switch(status){
    case Nonogram::Full:    item->setBackground(QBrush(Qt::darkGray)); break;
    case Nonogram::Free:    item->setBackground(QBrush(Qt::white)); break;
    case Nonogram::Unknown: item->setBackground(QBrush(Qt::lightGray)); break;
    default: break;
    }
    return item;
}

QStandardItem* NonogramModel::setupInfoItem(const QString& value, const Nonogram::LineStatus status){
    Qt::GlobalColor bgColor;
    switch (status) {
    case Nonogram::WillSolve: bgColor=Qt::gray;break;
    case Nonogram::Solving: bgColor=Qt::darkGray;break;
    case Nonogram::Solved: bgColor=Qt::lightGray;break;
    default:bgColor=Qt::gray;
        break;
    }
    QStandardItem* item=itemPrototype()->clone();
    item->setEditable(true);
    item->setTextAlignment(Qt::AlignCenter);
    item->setBackground(QBrush(bgColor));
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
    //connect(_nonogram, &Nonogram::columnStatusChanged, this, &NonogramModel::refreshColumn, Qt::BlockingQueuedConnection);
    //connect(_nonogram, &Nonogram::rowStatusChanged, this, &NonogramModel::refreshRow, Qt::BlockingQueuedConnection);
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
    int rowCount=_nonogram->height();
    _maxColumnInfo=0;
    _maxRowInfo=0;
    if (_headerVisible){
        for(int i=0; i<columnCount; i++)
            if (_nonogram->columnInfo(i).count()>_maxColumnInfo)
                _maxColumnInfo=_nonogram->columnInfo(i).count();

        for(int i=0; i<rowCount; i++)
            if (_nonogram->rowInfo(i).count()>_maxRowInfo)
                _maxRowInfo=_nonogram->rowInfo(i).count();
    }
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
            setItem(r+_dataBlockRow, _maxRowInfo, setupAddItem());
        for(int c=0; c<columnCount; c++)
            setItem(_maxColumnInfo, c+_dataBlockColumn, setupAddItem());
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
            newColumn.append(setupInfoItem(""));
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
            newRow.append(setupInfoItem(""));
        insertRow(0, newRow);
    }
    while(_maxColumnInfo>newMaxColumnInfo){
        _maxColumnInfo--;
        removeRow(0);
    }
    _dataBlockRow=_maxColumnInfo+(_editing?1:0);
}

void NonogramModel::refreshRow(int row){
    if (!_headerVisible)
        return;
    refreshRowInfoSize();

    InfoListType info=_nonogram->rowInfo(row);
    for(int c=0; c<_maxRowInfo; c++){
        QString text;
        if (c<info.count())
            text=QString::number(info[info.count()-1-c]);
        setItem(row+_dataBlockRow, _maxRowInfo-c-1, setupInfoItem(text, _nonogram->rowStatus(row)));
    }
}

void NonogramModel::refreshColumn(int column){
    if (!_headerVisible)
        return;
    refreshColumnInfoSize();

    InfoListType info=_nonogram->columnInfo(column);
    for(int r=0; r<_maxColumnInfo; r++){
        QString text;
        if (r<info.count())
            text=QString::number(info[info.count()-r-1]);
        setItem(_maxColumnInfo-r-1, column+_dataBlockColumn, setupInfoItem(text, _nonogram->columnStatus(column)));
    }
}

void NonogramModel::refreshData(int row, int column){
    setItem(row+_dataBlockRow, column+_dataBlockColumn, setupDataItem(_nonogram->data(row, column)));
}

void NonogramModel::rowInserted(int row){
    QList<QStandardItem*> newRow;
    InfoListType info=_nonogram->rowInfo(row);
    for(int c=0; c<_maxRowInfo; c++){
        QString text;
        if (c<info.count())
            text=QString::number(info[info.count()-c-1]);
        newRow.append(setupInfoItem(text));
    }
    if (_editing)
        newRow.append(setupAddItem());
    for(int c=0; c<_nonogram->width(); c++)
        newRow.append(setupDataItem(_nonogram->data(row, c)));
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
        newColumn.append(setupInfoItem(text));
    }
    if (_editing)
        newColumn.append(setupAddItem());
    for(int r=0; r<_nonogram->height(); r++)
        newColumn.append(setupDataItem(_nonogram->data(r, column)));
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
