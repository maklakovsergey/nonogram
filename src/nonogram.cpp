#include "nonogram.h"
#include <QDebug>
#include <algorithm>
#include <QImage>
#include <QColor>

#define ASSERT_ROW(row)       Q_ASSERT((row)    >= 0 && (row)    < _height)
#define ASSERT_COLUMN(column) Q_ASSERT((column) >= 0 && (column) < _width)

Nonogram::Nonogram(QObject *parent) : QObject(parent){
    _width=0;
    _height=0;
}

Nonogram::Nonogram(int width, int height, QObject *parent) : Nonogram(parent){
    init(width, height);
}

Nonogram::Nonogram(const Nonogram& nonogram):Nonogram(nonogram.width(), nonogram.height()){
    memcpy(_dataGrid.data(), nonogram._dataGrid.data(), _dataGrid.size()*sizeof(CellStatus));
    for(int i=0; i<_width; i++)
        _columnInfo[i]=nonogram._columnInfo[i];
    for(int i=0; i<_height; i++)
        _rowInfo[i]=nonogram._rowInfo[i];
}

Nonogram::~Nonogram(){
}

void Nonogram::init(int width, int height){
    _width=width;
    _height=height;
    _dataGrid=QVector<CellStatus>(_width*_height, CellStatus::Unknown);
    _columnInfo=QVector<LineInfoType>(_width);
    _rowInfo=QVector<LineInfoType>(_height);
}

bool Nonogram::isValid() const{
    return (_width>0 && _height>0 && !_dataGrid.isEmpty() && !_columnInfo.isEmpty() && !_rowInfo.isEmpty());
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

CellStatus Nonogram::data(int row, int column) const {
    ASSERT_ROW   (row);
    ASSERT_COLUMN(column);
    return _dataGrid[row*_width+column];
}

const LineInfoType& Nonogram::columnInfo(int column) const {
    ASSERT_COLUMN(column);
    return _columnInfo[column];
}

const LineInfoType& Nonogram::rowInfo(int row) const {
    ASSERT_ROW(row);
    return _rowInfo[row];
}

void Nonogram::setData(int row, int column, CellStatus value) {
    ASSERT_ROW   (row);
    ASSERT_COLUMN(column);
    _dataGrid[row*_width+column]=value;
    emit dataChanged(row, column);
}

void Nonogram::setColumnInfo(int column, const LineInfoType& newInfo) {
    ASSERT_COLUMN(column);
    LineInfoType& info=_columnInfo[column];
    info.clear();
    info.append(newInfo);
    emit columnInfoChanged(column);
}

void Nonogram::setRowInfo(int row, const LineInfoType& newInfo) {
    ASSERT_ROW(row);
    LineInfoType& info=_rowInfo[row];
    info.clear();
    info.append(newInfo);
    emit rowInfoChanged(row);
}


void Nonogram::insertRow(int position){
    ASSERT_ROW(position);
	QVector<CellStatus> dataGrid(_width*_height);
    memcpy(dataGrid.data(), _dataGrid.data(), _width*_height*sizeof(CellStatus));

    int newHeight=_height+1;
    _dataGrid.resize(_width*newHeight);
    for(int r=0; r<position; r++)
        for(int c=0; c<_width; c++)
            _dataGrid[r*_width+c]=dataGrid[r*_width+c];
    for(int c=0; c<_width; c++)
        _dataGrid[position*_width+c]=CellStatus::Unknown;
    for(int r=position+1; r<newHeight; r++)
        for(int c=0; c<_width; c++)
            _dataGrid[r*_width+c]=dataGrid[(r-1)*_width+c];
    _rowInfo.insert(position, LineInfoType());
    _height=newHeight;

    emit rowInserted(position);
}

void Nonogram::insertColumn(int position){
    ASSERT_COLUMN(position);
	QVector<CellStatus> dataGrid(_width*_height);
	memcpy(dataGrid.data(), _dataGrid.data(), _width*_height*sizeof(CellStatus));

    int newWidth=_width+1;
    _dataGrid.resize(newWidth*_height);
    for(int c=0; c<position; c++)
        for(int r=0; r<_height; r++)
            _dataGrid[r*newWidth+c]=dataGrid[r*_width+c];
    for(int r=0; r<_height; r++)
        _dataGrid[r*newWidth+position]=CellStatus::Unknown;
    for(int c=position+1; c<newWidth; c++)
        for(int r=0; r<_height; r++)
            _dataGrid[r*newWidth+c]=dataGrid[r*_width+c-1];
    _columnInfo.insert(position, LineInfoType());
    _width=newWidth;

    emit columnInserted(position);
}

void Nonogram::removeRow(int position){
    ASSERT_ROW(position);
	QVector<CellStatus> dataGrid(_width*_height);
	memcpy(dataGrid.data(), _dataGrid.data(), _width*_height*sizeof(CellStatus));

    int newHeight=_height-1;
    _dataGrid.resize(_width*newHeight);
    for(int r=0; r<position; r++)
        for(int c=0; c<_width; c++)
            _dataGrid[r*_width+c]=dataGrid[r*_width+c];
    for(int r=position; r<newHeight; r++)
        for(int c=0; c<_width; c++)
            _dataGrid[r*_width+c]=dataGrid[(r+1)*_width+c];
    _rowInfo.removeAt(position);
    _height=newHeight;

    emit rowRemoved(position);
}

void Nonogram::removeColumn(int position){
    ASSERT_COLUMN(position);

	QVector<CellStatus> dataGrid(_width*_height);
	memcpy(dataGrid.data(), _dataGrid.data(), _width*_height*sizeof(CellStatus));
	int newWidth=_width-1;
    _dataGrid.resize(newWidth*_height);
    for(int c=0; c<position; c++)
        for(int r=0; r<_height; r++)
            _dataGrid[r*newWidth+c]=dataGrid[r*_width+c];
    for(int c=position; c<newWidth; c++)
        for(int r=0; r<_height; r++)
            _dataGrid[r*newWidth+c]=dataGrid[r*_width+c+1];
    _columnInfo.removeAt(position);
    _width=newWidth;

    emit columnRemoved(position);
}

void Nonogram::saveImage(const QString &fileName){
    QImage image(_width, _height, QImage::Format_RGB888);
    for(int r=0;r<_height; r++)
        for(int c=0;c<_width; c++){
            Qt::GlobalColor color;
            switch(data(r,c)){
            case CellStatus::Free:    color=Qt::white; break;
            case CellStatus::Full:    color=Qt::black; break;
            case CellStatus::Unknown: color=Qt::gray;  break;
            default:      color=Qt::gray;  break;
            }
            image.setPixel(c, r, QColor(color).rgb());
        }
    image.save(fileName);
}

void Nonogram::swap(Nonogram& other) noexcept{
    std::swap(_width, other._width);
    std::swap(_height, other._height);
    _dataGrid.swap(other._dataGrid);
    _columnInfo.swap(other._columnInfo);
    _rowInfo.swap(other._rowInfo);
}

Nonogram& Nonogram::operator=(Nonogram other){
    swap(other);
    return *this;
}

bool Nonogram::operator== (const Nonogram& n)const{
    return (   n._width       ==_width
            && n._height      ==_height
            && n._dataGrid    ==_dataGrid
            && n._columnInfo  ==_columnInfo
            && n._rowInfo     ==_rowInfo);
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

