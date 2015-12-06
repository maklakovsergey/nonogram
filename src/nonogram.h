#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <QObject>
#include <QList>
#include <QDataStream>

class Nonogram : public QObject
{
    Q_OBJECT
public:
    explicit Nonogram(QObject *parent = 0);
    ~Nonogram();
    Nonogram(quint16 width, quint16 height);
    Nonogram(QDataStream& dataStream) throw(std::runtime_error);

    enum CellStatus:qint8{
        Unknown,
        Free,
        Full
    };

    inline quint16 width() const {return _width; }
    inline quint16 height() const {return _height; };
    inline CellStatus * data() const {return _dataGrid; };
    inline QList<quint16> * columnInfo() const {return _columnInfo; };
    inline QList<quint16> * rowInfo() const {return _rowInfo; };

    void save(QDataStream& dataStream);

signals:

public slots:

private:
    quint16 _width;
    quint16 _height;
    CellStatus* _dataGrid;
    QList<quint16>* _columnInfo;
    QList<quint16>* _rowInfo;
};

#endif // NONOGRAM_H
