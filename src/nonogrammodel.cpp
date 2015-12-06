#include "nonogrammodel.h"
#include <QBrush>
#include <QDebug>

NonogramModel::NonogramModel(QObject *parent) : QStandardItemModel(parent)
{
    _nonogram=NULL;
    _editing=true;
    emptyItemPrototype=std::shared_ptr<QStandardItem>(new QStandardItem());
    emptyItemPrototype->setDragEnabled(false);
    emptyItemPrototype->setDropEnabled(false);
    emptyItemPrototype->setSelectable(false);
    emptyItemPrototype->setEditable(false);
    emptyItemPrototype->setCheckable(false);
    emptyItemPrototype->setTextAlignment(Qt::AlignCenter);
    addItemPrototype=std::shared_ptr<QStandardItem>(emptyItemPrototype->clone());
    addItemPrototype->setText("+");
    addItemPrototype->setBackground(QBrush(Qt::lightGray));
    addItemPrototype->setEditable(true);
    infoItemPrototype=std::shared_ptr<QStandardItem>(emptyItemPrototype->clone());
    infoItemPrototype->setBackground(QBrush(Qt::gray));
    infoItemPrototype->setEditable(true);
    dataItemPrototype=std::shared_ptr<QStandardItem>(emptyItemPrototype->clone());
}

NonogramModel::~NonogramModel(){
}

void NonogramModel::onItemChanged(){

}

bool NonogramModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if (role==Qt::EditRole){
        int row=index.row();
        int column=index.column();
        int dataBlockColumn=_maxRowInfo+(_editing?1:0);
        int dataBlockRow=_maxColumnInfo+(_editing?1:0);
        //qDebug()<<row<<column<<value;
        bool isInt;
        int intValue=value.toString().toInt(&isInt);
        if (row>=dataBlockRow&&column<_maxRowInfo){
            //edit row info block
            auto info=&_nonogram->rowInfo()[row-dataBlockRow];
            int listIndex=info->count()-_maxRowInfo+column;
            if (listIndex<0){
                if (isInt)
                    info->insert(0, intValue);
            }
            else{
                if (isInt)
                    info->replace(listIndex, intValue);
                else
                    info->removeAt(listIndex);
            }
            refreshRow(row-dataBlockRow);
        }
        else if (row>=dataBlockRow && _editing && column==_maxRowInfo){
            //row + button pressed
        }
        else if (column>=dataBlockColumn&&row<_maxColumnInfo){
            //edit column info block
            auto info=&_nonogram->columnInfo()[column-dataBlockColumn];
            int listIndex=info->count()-_maxColumnInfo+row;
            if (listIndex<0){
                if (isInt)
                    info->insert(0, intValue);
            }
            else{
                if (isInt)
                    info->replace(listIndex, intValue);
                else
                    info->removeAt(listIndex);
            }
            refreshColumn(column-dataBlockColumn);
        }
        else if (column>=dataBlockColumn && _editing && row==_maxColumnInfo){
            //column + button pressed
        }
        return true;
    }
    else
        return QStandardItemModel::setData(index, value, role);
}

void NonogramModel::setNonogram(Nonogram* nonogram){
    _nonogram=nonogram;
    refreshInfo();
}

void NonogramModel::refreshInfo(){
    int columnCount=_nonogram->width();
    auto columnInfo=_nonogram->columnInfo();
    _maxColumnInfo=0;
    for(int i=0; i<columnCount; i++)
        if (columnInfo[i].count()>_maxColumnInfo)
            _maxColumnInfo=columnInfo[i].count();

    int rowCount=_nonogram->height();
    auto rowInfo=_nonogram->rowInfo();
    _maxRowInfo=0;
    for(int i=0; i<rowCount; i++)
        if (rowInfo[i].count()>_maxRowInfo)
            _maxRowInfo=rowInfo[i].count();

    int dataBlockColumn=_maxRowInfo+(_editing?1:0);
    int dataBlockRow=_maxColumnInfo+(_editing?1:0);

    setColumnCount(dataBlockColumn+columnCount);
    setRowCount(dataBlockRow+rowCount);
    disconnect(this, itemChanged, this, onItemChanged);

    for(int c=0; c<dataBlockColumn; c++)
        for(int r=0; r<dataBlockRow; r++)
            setItem(r, c, emptyItemPrototype->clone());
    for(int r=0; r<rowCount; r++)
        refreshRow(r);
    for(int c=0; c<columnCount; c++)
        refreshColumn(c);

    if (_editing){
        for(int r=0; r<rowCount; r++)
            setItem(r+dataBlockRow, _maxRowInfo, addItemPrototype->clone());
        for(int c=0; c<columnCount; c++)
            setItem(_maxColumnInfo, c+dataBlockColumn, addItemPrototype->clone());
    }
    for(int c=0; c<columnCount; c++)
        for(int r=0; r<rowCount; r++){
            QStandardItem* item=dataItemPrototype->clone();
            switch(_nonogram->data()[c*rowCount+r]){
            case Nonogram::Full:    item->setBackground(QBrush(Qt::black)); break;
            case Nonogram::Free:    item->setBackground(QBrush(Qt::lightGray)); break;
            case Nonogram::Unknown: item->setBackground(QBrush(Qt::white)); break;
            default: break;
            }
            setItem(r+dataBlockRow, c+dataBlockColumn, item);
        }
    connect(this, itemChanged, this, onItemChanged);
}

void NonogramModel::refreshRow(int row){
    auto info=_nonogram->rowInfo()[row];
    int dataBlockRow=_maxColumnInfo+(_editing?1:0);

    if (info.count()>_maxRowInfo)
    {
        _maxRowInfo=info.count();
        QList<QStandardItem*> newColumn;
        for(int i=0; i<dataBlockRow; i++)
            newColumn.append(emptyItemPrototype->clone());
        for(int i=0; i<_nonogram->height(); i++)
            newColumn.append(infoItemPrototype->clone());
        insertColumn(0, newColumn);
    }
    else
        for(int c=0; c<_maxRowInfo; c++){
            QStandardItem* item=this->item(row+dataBlockRow, _maxRowInfo-c-1)?:infoItemPrototype->clone();
            if (c<info.count())
                item->setText(QString::number(info[info.count()-1-c]));
            else
                item->setText("");
            setItem(row+dataBlockRow, _maxRowInfo-c-1, item);
        }
}

void NonogramModel::refreshColumn(int column){
    auto info=_nonogram->columnInfo()[column];
    int dataBlockColumn=_maxRowInfo+(_editing?1:0);

    if (info.count()>_maxColumnInfo){
        _maxColumnInfo=info.count();
        QList<QStandardItem*> newRow;
        for(int i=0; i<dataBlockColumn; i++)
            newRow.append(emptyItemPrototype->clone());
        for(int i=0; i<_nonogram->width(); i++)
            newRow.append(infoItemPrototype->clone());
        insertRow(0, newRow);
    }
    else
        for(int r=0; r<_maxColumnInfo; r++){
            QStandardItem* item=this->item(_maxColumnInfo-r-1, column+dataBlockColumn)?:infoItemPrototype->clone();
            if (r<info.count())
                item->setText(QString::number(info[info.count()-r-1]));
            else
                item->setText("");
            setItem(_maxColumnInfo-r-1, column+dataBlockColumn, item);
        }
}
