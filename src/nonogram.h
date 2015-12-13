#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QDataStream>
#include <memory>

typedef QList<quint16> InfoListType;

class Nonogram : public QObject
{
    Q_OBJECT
public:
    explicit Nonogram(QObject *parent = 0);
    Nonogram(int width, int height, QObject *parent = 0);
    Nonogram(const Nonogram& nonogram);
    ~Nonogram();

    enum CellStatus:qint8{
        Unknown,
        Free,
        Full
    };

    void init(int width, int height);
    bool isValid() const;
    bool isSolveable() const;

    inline int width() const {return _width; }
    inline int height() const {return _height; }
    inline CellStatus data(int row, int column) const       { return _dataGrid[row*_width+column]; }
    inline const InfoListType& columnInfo(int column) const { return _columnInfo[column]; }
    inline const InfoListType& rowInfo(int row) const       { return _rowInfo[row]; }

    void setData(int row, int column, CellStatus value);
    void setColumnInfo(int column, const InfoListType& newInfo);
    void setRowInfo(int row, const InfoListType& newInfo);

    void insertRow(int position);
    void insertColumn(int position);
    void removeRow(int position);
    void removeColumn(int position);

    bool operator== (const Nonogram& n)const;
    bool operator!= (const Nonogram& n)const {return !(*this == n);}

    bool solve();
signals:
    void dataChanged(int row, int column);
    void rowInfoChanged(int row);
    void columnInfoChanged(int column);
    void rowInserted(int row);
    void columnInserted(int column);
    void rowRemoved(int row);
    void columnRemoved(int column);
public slots:

protected:
    int _width;
    int _height;
    QVector<CellStatus> _dataGrid;
    QVector<InfoListType> _columnInfo;
    QVector<InfoListType> _rowInfo;

    void saveImage();

    friend QDataStream& operator>>(QDataStream& in, Nonogram& nonogram);
    friend QDataStream& operator<<(QDataStream& out, const Nonogram& nonogram);
};



#endif // NONOGRAM_H
