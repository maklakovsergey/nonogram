#include "nonogram.h"
#include "strings.h"

using namespace std;

Nonogram::Nonogram(){
    _width=0;
    _height=0;
    _dataColumns=NULL;
    _columnInfo=NULL;
    _rowInfo=NULL;
}

Nonogram::Nonogram(quint16 width, quint16 height){
    _width=width;
    _height=height;
    _dataColumns=new CellStatus*[_width];
    for(qint32 i=0; i<_width; i++){
        _dataColumns[i]=new CellStatus[_height];
        memset(_dataColumns[i], Unknown, _height);
    }
    _columnInfo=new QList<quint16>[_width];
    _rowInfo=new QList<quint16>[_height];
}

Nonogram::Nonogram(QDataStream &dataStream) throw(runtime_error){
    dataStream >> _width >> _height;
    if (_width==0 || _height==0)
        throw new runtime_error("Wrong file format");
    _dataColumns=new CellStatus*[_width];
    for(quint16 i=0; i<_width; i++){
        _dataColumns[i]=new CellStatus[_height];
        int dataSize=dataStream.readRawData((char*)_dataColumns[i], _height*sizeof(CellStatus));
        if (dataSize!=_height)
            throw new runtime_error("Wrong file format");
    }
    _columnInfo=new QList<quint16>[_width];
    for(quint16 i=0; i<_width; i++){
        dataStream >> _columnInfo[i];
        if (_columnInfo[i].count()==0)
            throw new runtime_error("Wrong file format");
    }
    _rowInfo=new QList<quint16>[_height];
    for(quint16 i=0; i<_height; i++){
        dataStream >> _rowInfo[i];
        if (_rowInfo[i].count()==0)
            throw new runtime_error("Wrong file format");
    }
}

Nonogram::~Nonogram(){
    for(uint i=0; i<_width; i++)
        delete[] _dataColumns[i];
    delete[] _dataColumns;
    delete[] _columnInfo;
    delete[] _rowInfo;
}

quint16 Nonogram::width(){
    return _width;
}

quint16 Nonogram::height(){
    return _height;
}

Nonogram::CellStatus * const * Nonogram::data(){
    return _dataColumns;
}

QList<quint16> *Nonogram::columnInfo(){
    return _columnInfo;
}

QList<quint16> *Nonogram::rowInfo(){
    return _rowInfo;
}

void Nonogram::save(QDataStream &dataStream){
    dataStream << _width << _height;
    for(uint i=0; i<_width; i++)
        dataStream.writeRawData((char*)(_dataColumns[i]), _height*sizeof(CellStatus));
    for(uint i=0; i<_width; i++)
        dataStream << _columnInfo[i];
    for(uint i=0; i<_height; i++)
        dataStream << _rowInfo[i];
}
