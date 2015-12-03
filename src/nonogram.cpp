#include "nonogram.h"

Nonogram::Nonogram(const int width, const int height)
    :_width(width), _height(height)
{
    _data=new bool*[width];
    for(int i=0; i<width; i++)
        _data[i]=new bool[height];
    _columnData=new QList<int>[width];
    _rowData=new QList<int>[height];
}

Nonogram::~Nonogram(){
    for(int i=0; i<_width; i++)
        delete[] _data[i];
    delete[] _data;
    delete[] _columnData;
    delete[] _rowData;
}

int Nonogram::width(){
    return _width;
}

int Nonogram::height(){
    return _height;
}

bool * const * Nonogram::data(){
    return _data;
}

QList<int>* Nonogram::columnData(){
    return _columnData;
}

QList<int>* Nonogram::rowData(){
    return _rowData;
}
