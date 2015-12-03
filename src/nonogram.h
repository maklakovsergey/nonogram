#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <QList>
#include <QDataStream>

class Nonogram
{
public:
    Nonogram(quint16 width, quint16 height);
    Nonogram(QDataStream& dataStream) throw(std::runtime_error);
    ~Nonogram();

    enum CellStatus:qint8{
        Unknown,
        Free,
        Full
    };

    quint16 width();
    quint16 height();
    CellStatus * const * data();
    QList<quint16> * columnInfo();
    QList<quint16> * rowInfo();

    void save(QDataStream& dataStream);
private:
    Nonogram();
    quint16 _width;
    quint16 _height;
    CellStatus** _dataColumns;
    QList<quint16>* _columnInfo;
    QList<quint16>* _rowInfo;
};

#endif // NONOGRAM_H
