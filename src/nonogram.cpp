#include "nonogram.h"

Nonogram::Nonogram(QObject *parent) : QObject(parent)
{
    _width=0;
    _height=0;
    _dataGrid=NULL;
    _columnInfo=NULL;
    _rowInfo=NULL;
}

Nonogram::Nonogram(quint16 width, quint16 height){
    _width=width;
    _height=height;
    _dataGrid=new CellStatus[_width*_height];
    memset(_dataGrid, Unknown, _width*_height);
    _columnInfo=new QList<quint16>[_width];
    _rowInfo=new QList<quint16>[_height];
}

Nonogram::Nonogram(QDataStream &dataStream) throw(std::runtime_error){
    dataStream >> _width >> _height;
    if (_width==0 || _height==0)
        throw new std::runtime_error("Wrong file format");
    _dataGrid=new CellStatus[_width*_height];
    int dataSize=dataStream.readRawData((char*)_dataGrid, _width*_height);
    if (dataSize!=_width*_height)
        throw new std::runtime_error("Wrong file format");
    _columnInfo=new QList<quint16>[_width];
    for(quint16 i=0; i<_width; i++)
        dataStream >> _columnInfo[i];
    _rowInfo=new QList<quint16>[_height];
    for(quint16 i=0; i<_height; i++)
        dataStream >> _rowInfo[i];
}

Nonogram::~Nonogram(){
    delete[] _dataGrid;
    delete[] _columnInfo;
    delete[] _rowInfo;
}

void Nonogram::save(QDataStream &dataStream){
    dataStream << _width << _height;
    dataStream.writeRawData((char*)(_dataGrid), _width*_height);
    for(uint i=0; i<_width; i++)
        dataStream << _columnInfo[i];
    for(uint i=0; i<_height; i++)
        dataStream << _rowInfo[i];
}
