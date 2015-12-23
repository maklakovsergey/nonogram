#include "nonogrammodel.h"
#include <QBrush>
#include <QDebug>

enum CellType {
	CellTypeEmpty,
	CellTypeAdd,
	CellTypeInfo,
	CellTypeData
};

const int CellTypeRole=Qt::UserRole+1;

NonogramModel::NonogramModel(QObject *parent) : QStandardItemModel(parent)
{
    _nonogram=NULL;
    _editing=true;
    _headerVisible=true;
    QStandardItem* emptyItemPrototype=new QStandardItem();
    emptyItemPrototype->setFlags(Qt::ItemIsEnabled);
    emptyItemPrototype->setTextAlignment(Qt::AlignCenter);
    emptyItemPrototype->setBackground(QBrush(Qt::white));
	emptyItemPrototype->setData(CellTypeEmpty, CellTypeRole);
    setItemPrototype(emptyItemPrototype);

    //connect(this, &NonogramModel::itemChanged, this, &NonogramModel::onItemChanged);
}

NonogramModel::~NonogramModel(){
	//printRedrawCount();
}

void NonogramModel::printRedrawCount() const{
	int rowCnt = rowCount();
	int columnCnt = columnCount();
	qDebug() << "redrawCount";
	for (int r = 0; r < rowCnt; r++) {
		QString counts;
		for (int c = 0; c < columnCnt; c++)
			counts += QString::number(redrawCount[r*columnCnt + c]) + " ";
		qDebug() << counts;
	}
}

void NonogramModel::onItemChanged(QStandardItem * item){
    QModelIndex index(item->index());
	redrawCount[index.row()*columnCount() + index.column()]++;
}

QStandardItem*  NonogramModel::setupAddItem(const QModelIndex& index){
	QStandardItem* item = itemFromIndex(index);
	if (!item || item->data(CellTypeRole).toInt()!=CellTypeAdd) {
		item = itemPrototype()->clone();
		item->setEditable(true);
		item->setTextAlignment(Qt::AlignCenter);
		item->setBackground(QBrush(Qt::lightGray));
		item->setText("+");
		item->setData(CellTypeAdd, CellTypeRole);
		setItem(index.row(), index.column(), item);
	}
    return item;
}

QStandardItem*  NonogramModel::setupDataItem(const QModelIndex& index, Nonogram::CellStatus status){
	QBrush background;
	switch (status) {
	case Nonogram::Full:    background=Qt::darkGray; break;
	case Nonogram::Free:    background=Qt::white; break;
	case Nonogram::Unknown: background=Qt::lightGray; break;
	default: break;
	}
	QStandardItem* item = itemFromIndex(index);
	if (!item || item->data(CellTypeRole).toInt() != CellTypeData) {
		item = itemPrototype()->clone();
		item->setData(CellTypeInfo, CellTypeData);
		item->setBackground(background);
		setItem(index.row(), index.column(), item);
	}
	else
		setItemData(index, { { Qt::BackgroundRole, background } });
    return item;
}

QStandardItem* NonogramModel::setupInfoItem(const QModelIndex& index, const QString& value, const Nonogram::LineStatus status){
	QBrush background;
    switch (status) {
    case Nonogram::WillSolve: background=Qt::gray;break;
    case Nonogram::Solving: background=Qt::darkGray;break;
    case Nonogram::Solved: background=Qt::lightGray;break;
    default:background=Qt::gray; break;
    }
	QStandardItem* item = itemFromIndex(index);
	if (!item || item->data(CellTypeRole).toInt() != CellTypeInfo) {
		item = itemPrototype()->clone();
		item->setTextAlignment(Qt::AlignCenter);
		item->setData(CellTypeInfo, CellTypeRole);
		item->setBackground(background);
		item->setText(value);
		item->setEditable(true);
		setItem(index.row(), index.column(), item);
	}
	else 
		setItemData(index, { {Qt::BackgroundRole, background}, {Qt::DisplayRole, value} });
	return item;
}

Qt::ItemFlags NonogramModel::flags(const QModelIndex & index) const{
	Qt::ItemFlags flags = QStandardItemModel::flags(index);
	if ((flags&Qt::ItemIsEditable) && !_editing)
		flags &= ~Qt::ItemIsEditable;
	return flags;
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

void NonogramModel::setNonogram(Nonogram* nonogram) {
	_nonogram = nonogram;
	if (_nonogram) {
		connect(_nonogram, &Nonogram::columnInfoChanged, this, &NonogramModel::refreshColumn);
		connect(_nonogram, &Nonogram::rowInfoChanged, this, &NonogramModel::refreshRow);
		//connect(_nonogram, &Nonogram::columnStatusChanged, this, &NonogramModel::refreshColumn, Qt::BlockingQueuedConnection);
		//connect(_nonogram, &Nonogram::rowStatusChanged, this, &NonogramModel::refreshRow, Qt::BlockingQueuedConnection);
		connect(_nonogram, &Nonogram::dataChanged, this, &NonogramModel::refreshData/*, Qt::BlockingQueuedConnection*/);
		connect(_nonogram, &Nonogram::rowInserted, this, &NonogramModel::rowInserted);
		connect(_nonogram, &Nonogram::rowRemoved, this, &NonogramModel::rowRemoved);
		connect(_nonogram, &Nonogram::columnInserted, this, &NonogramModel::columnInserted);
		connect(_nonogram, &Nonogram::columnRemoved, this, &NonogramModel::columnRemoved);
	}
	else {
		_editing = false;
	}
    refreshInfo();
}

void NonogramModel::refreshInfo(){
    int columnCount= _nonogram ? _nonogram->width():0;
    int rowCount= _nonogram ? _nonogram->height():0;
    _maxColumnInfo=0;
    _maxRowInfo=0;
    if (_headerVisible && _nonogram){
        for(int i=0; i<columnCount; i++)
            if (_nonogram->columnInfo(i).count()>_maxColumnInfo)
                _maxColumnInfo=_nonogram->columnInfo(i).count();

        for(int i=0; i<rowCount; i++)
            if (_nonogram->rowInfo(i).count()>_maxRowInfo)
                _maxRowInfo=_nonogram->rowInfo(i).count();
    }
    _dataBlockColumn=_maxRowInfo+(_editing?1:0);
    _dataBlockRow=_maxColumnInfo+(_editing?1:0);

	redrawCount.resize((_dataBlockColumn + columnCount)*(_dataBlockRow + rowCount));
	redrawCount.fill(0);

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
			setupAddItem(index(r+_dataBlockRow, _maxRowInfo));
        for(int c=0; c<columnCount; c++)
			setupAddItem(index(_maxColumnInfo, c+_dataBlockColumn));
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
            newColumn.append(setupInfoItem(QModelIndex(), ""));
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
            newRow.append(setupInfoItem(QModelIndex(), ""));
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
        setupInfoItem(index(row + _dataBlockRow, _maxRowInfo - c - 1), text, _nonogram->rowStatus(row));
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
        setupInfoItem(index(_maxColumnInfo - r - 1, column + _dataBlockColumn), text, _nonogram->columnStatus(column));
    }
}

void NonogramModel::refreshData(int row, int column){
	setupDataItem(index(row + _dataBlockRow, column + _dataBlockColumn), _nonogram->data(row, column));
}

void NonogramModel::rowInserted(int row){
    QList<QStandardItem*> newRow;
    InfoListType info=_nonogram->rowInfo(row);
    for(int c=0; c<_maxRowInfo; c++){
        QString text;
        if (c<info.count())
            text=QString::number(info[info.count()-c-1]);
        newRow.append(setupInfoItem(QModelIndex(), text));
    }
    if (_editing)
        newRow.append(setupAddItem(QModelIndex()));
    for(int c=0; c<_nonogram->width(); c++)
        newRow.append(setupDataItem(QModelIndex(), _nonogram->data(row, c)));
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
        newColumn.append(setupInfoItem(QModelIndex(), text));
    }
    if (_editing)
        newColumn.append(setupAddItem(QModelIndex()));
    for(int r=0; r<_nonogram->height(); r++)
        newColumn.append(setupDataItem(QModelIndex(), _nonogram->data(r, column)));
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
