#include "nonogram.h"
#include <QDebug>

Nonogram::Nonogram(QObject *parent) : QObject(parent){
    _width=0;
    _height=0;
}

Nonogram::Nonogram(int width, int height, QObject *parent) : Nonogram(parent){
    init(width, height);
}

Nonogram::Nonogram(const Nonogram& nonogram):Nonogram(nonogram.width(), nonogram.height()){
    _dataGrid.append(nonogram._dataGrid);
    for(int i=0; i<_width; i++)
        _columnInfo[i].append(nonogram._columnInfo[i]);
    for(int i=0; i<_height; i++)
        _rowInfo[i].append(nonogram._rowInfo[i]);
}

Nonogram::~Nonogram(){
}

void Nonogram::init(int width, int height){
    _width=width;
    _height=height;
    _dataGrid=QVector<CellStatus>(_width*_height, Unknown);
    _columnInfo=QVector<InfoListType>(_width);
    _rowInfo=QVector<InfoListType>(_height);
}

bool Nonogram::isValid() const{
    return (_width && _height && _dataGrid.count() && _columnInfo.count() && _rowInfo.count());
}

bool Nonogram::isSolveable() const{
    if (!isValid())
        return false;
    int columnSum=0;
    for(int c=0; c<_width; c++){
        int infoSum=0;
        for(int value:_columnInfo[c])
            infoSum+=value;
        if (infoSum+_columnInfo[c].count()-1>_height)
            return false;
        columnSum+=infoSum;
    }
    int rowSum=0;
    for(int r=0; r<_height; r++){
        int infoSum=0;
        for(int value:rowInfo(r))
            infoSum+=value;
        if (infoSum+rowInfo(r).count()-1>_width)
            return false;
        rowSum+=infoSum;
    }
    return columnSum==rowSum;
}

void Nonogram::setData(int row, int column, CellStatus value) {
    _dataGrid[row*_width+column]=value;
    emit dataChanged(row, column);
}

void Nonogram::setColumnInfo(int column, const InfoListType& newInfo) {
    InfoListType& info=_columnInfo[column];
    info.clear();
    info.append(newInfo);
    emit columnInfoChanged(column);
}

void Nonogram::setRowInfo(int row, const InfoListType& newInfo) {
    InfoListType& info=_rowInfo[row];
    info.clear();
    info.append(newInfo);
    emit rowInfoChanged(row);
}

bool Nonogram::operator== (const Nonogram& n)const{
    return (n.width()==_width && n.height()== _height && n._dataGrid==_dataGrid &&
            n._columnInfo==_columnInfo && n._rowInfo==_rowInfo);
}

QDataStream& operator>>(QDataStream& dataStream, Nonogram& nonogram){
    int width=0, height=0;
    dataStream >> width >> height;
    if (width==0 || height==0)
        return dataStream;
    nonogram.init(width, height);

    int dataSize=dataStream.readRawData((char*)nonogram._dataGrid.data(), width*height);
    if (dataSize!=width*height)
        return dataStream;
    dataStream >> nonogram._columnInfo >> nonogram._rowInfo;
    return dataStream;
}

QDataStream& operator<<(QDataStream& dataStream, const Nonogram& nonogram){
    dataStream << nonogram._width << nonogram._height;
    dataStream.writeRawData((char*)nonogram._dataGrid.data(), nonogram._width * nonogram._height);
    dataStream << nonogram._columnInfo << nonogram._rowInfo;
    return dataStream;
}
