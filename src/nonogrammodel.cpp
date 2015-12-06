#include "nonogrammodel.h"
#include <QBrush>
#include <QDebug>

NonogramModel::NonogramModel(QObject *parent) : QStandardItemModel(parent)
{
    _nonogram=NULL;
    _editing=true;
    QStandardItem* prototype=new QStandardItem();
    prototype->setDragEnabled(false);
    prototype->setDropEnabled(false);
    prototype->setSelectable(false);
    prototype->setEditable(false);
    prototype->setCheckable(false);
    prototype->setTextAlignment(Qt::AlignCenter);
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
    case Nonogram::Full:    item->setBackground(QBrush(Qt::black)); break;
    case Nonogram::Free:    item->setBackground(QBrush(Qt::lightGray)); break;
    case Nonogram::Unknown: item->setBackground(QBrush(Qt::white)); break;
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

void NonogramModel::onItemChanged(){
}

bool NonogramModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if (role==Qt::EditRole){
        int dataBlockColumn=_maxRowInfo+(_editing?1:0);
        int dataBlockRow=_maxColumnInfo+(_editing?1:0);
        int row=index.row()-dataBlockRow;
        int column=index.column()-dataBlockColumn;
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
    connect(_nonogram, SIGNAL(columnInfoChanged(int)), this, SLOT(refreshColumn(int)));
    connect(_nonogram, SIGNAL(rowInfoChanged(int)), this, SLOT(refreshRow(int)));
    refreshInfo();
}

void NonogramModel::refreshInfo(){
    clear();
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

    int dataBlockColumn=_maxRowInfo+(_editing?1:0);
    int dataBlockRow=_maxColumnInfo+(_editing?1:0);

    setColumnCount(dataBlockColumn+columnCount);
    setRowCount(dataBlockRow+rowCount);
    disconnect(this, itemChanged, this, onItemChanged);

    for(int c=0; c<dataBlockColumn; c++)
        for(int r=0; r<dataBlockRow; r++)
            setItem(r, c, itemPrototype()->clone());
    for(int r=0; r<rowCount; r++)
        refreshRow(r);
    for(int c=0; c<columnCount; c++)
        refreshColumn(c);

    if (_editing){
        for(int r=0; r<rowCount; r++)
            setupAddItem(itemAt(r+dataBlockRow, _maxRowInfo));
        for(int c=0; c<columnCount; c++)
            setupAddItem(itemAt(_maxColumnInfo, c+dataBlockColumn));
    }
    for(int c=0; c<columnCount; c++)
        for(int r=0; r<rowCount; r++){
            setupDataItem(itemAt(r+dataBlockRow, c+dataBlockColumn), _nonogram->data()[c*rowCount+r]);
        }
    connect(this, itemChanged, this, onItemChanged);
}

void NonogramModel::refreshRow(int row){
    int rowCount=_nonogram->height();
    int newMaxRowInfo=0;
    for(int i=0; i<rowCount; i++)
        if (_nonogram->rowInfo(i).count()>newMaxRowInfo)
            newMaxRowInfo=_nonogram->rowInfo(i).count();
    int dataBlockRow=_maxColumnInfo+(_editing?1:0);

    while (_maxRowInfo<newMaxRowInfo){
        _maxRowInfo++;
        QList<QStandardItem*> newColumn;
        for(int i=0; i<dataBlockRow; i++)
            newColumn.append(itemPrototype()->clone());
        for(int i=0; i<rowCount; i++)
            newColumn.append(setupInfoItem(itemPrototype()->clone(), ""));
        insertColumn(0, newColumn);
    }
    while(_maxRowInfo>newMaxRowInfo){
        _maxRowInfo--;
        removeColumn(0);
    }

    InfoListType info=_nonogram->rowInfo(row);
    for(int c=0; c<_maxRowInfo; c++){
        QString text;
        if (c<info.count())
            text=QString::number(info[info.count()-1-c]);
        setupInfoItem(itemAt(row+dataBlockRow, _maxRowInfo-c-1), text);
    }
}

void NonogramModel::refreshColumn(int column){
    int columnCount=_nonogram->width();
    int newMaxColumnInfo=0;
    for(int i=0; i<columnCount; i++)
        if (_nonogram->columnInfo(i).count()>newMaxColumnInfo)
            newMaxColumnInfo=_nonogram->columnInfo(i).count();
    int dataBlockColumn=_maxRowInfo+(_editing?1:0);

    while (_maxColumnInfo<newMaxColumnInfo){
        _maxColumnInfo++;
        QList<QStandardItem*> newRow;
        for(int i=0; i<dataBlockColumn; i++)
            newRow.append(itemPrototype()->clone());
        for(int i=0; i<columnCount; i++)
            newRow.append(setupInfoItem(itemPrototype()->clone(), ""));
        insertRow(0, newRow);
    }
    while(_maxColumnInfo>newMaxColumnInfo){
        _maxColumnInfo--;
        removeRow(0);
    }
    InfoListType info=_nonogram->columnInfo(column);

    for(int r=0; r<_maxColumnInfo; r++){
        QString text;
        if (r<info.count())
            text=QString::number(info[info.count()-r-1]);
        setupInfoItem(itemAt(_maxColumnInfo-r-1, column+dataBlockColumn), text);
    }
}
