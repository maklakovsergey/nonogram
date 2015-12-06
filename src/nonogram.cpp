#include "nonogram.h"

using namespace std;

Nonogram::Nonogram(QObject *parent) : QObject(parent){
    _width=0;
    _height=0;
    _dataGrid=NULL;
    _columnInfo=NULL;
    _rowInfo=NULL;
}

Nonogram::Nonogram(int width, int height, QObject *parent) : QObject(parent){
    init(width, height);
}

Nonogram::~Nonogram(){
}

void Nonogram::init(int width, int height){
    _width=width;
    _height=height;
    _dataGrid=shared_ptr<CellStatus>(new CellStatus[_width*_height], default_delete<CellStatus[]>());
    memset(_dataGrid.get(), Unknown, _width*_height);
    _columnInfo=shared_ptr<InfoListType>(new InfoListType[_width], default_delete<QList<quint16>[]>());;
    _rowInfo=shared_ptr<InfoListType>(new InfoListType[_height], default_delete<QList<quint16>[]>());;
}

bool Nonogram::isValid() const{
    return (_width && _height && _dataGrid && _columnInfo && _rowInfo);
}

void Nonogram::setColumnInfo(int column, const InfoListType& newInfo) {
    InfoListType& info=_columnInfo.get()[column];
    info.clear();
    info.append(newInfo);
    emit columnInfoChanged(column);
}

void Nonogram::setRowInfo(int row, const InfoListType& newInfo) {
    InfoListType& info=_rowInfo.get()[row];
    info.clear();
    info.append(newInfo);
    emit rowInfoChanged(row);
}


QDataStream& operator>>(QDataStream& dataStream, Nonogram& nonogram){
    quint16 width=0, height=0;
    dataStream >> width >> height;
    if (width==0 || height==0)
        return dataStream;
    nonogram.init(width, height);
    int dataSize=dataStream.readRawData((char*)nonogram.data(), width*height);
    if (dataSize!=width*height)
        return dataStream;
    InfoListType info;
    for(quint16 i=0; i<width; i++){
        dataStream >> info;
        nonogram.setColumnInfo(i, info);
    }
    for(quint16 i=0; i<height; i++){
        dataStream >> info;
        nonogram.setRowInfo(i, info);
    }
    return dataStream;
}

QDataStream& operator<<(QDataStream& dataStream, const Nonogram& nonogram){
    quint16 width=nonogram.width(), height=nonogram.height();
    dataStream << width << height;
    dataStream.writeRawData((char*)(nonogram.data()), width*height);
    for(uint i=0; i<width; i++)
        dataStream << nonogram.columnInfo(i);
    for(uint i=0; i<height; i++)
        dataStream << nonogram.rowInfo(i);
    return dataStream;
}
