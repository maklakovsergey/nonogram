#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <QList>

class Nonogram
{
public:
    Nonogram(const int width, const int height);
    ~Nonogram();

    int width();
    int height();
    bool * const * data();
    QList<int>* columnData();
    QList<int>* rowData();
private:
    const int _width;
    const int _height;
    bool** _data;
    QList<int>* _columnData;
    QList<int>* _rowData;
};

#endif // NONOGRAM_H
