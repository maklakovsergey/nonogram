#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <QObject>
#include <QList>
#include <QDataStream>
#include <memory>

typedef QList<quint16> InfoListType;

class Nonogram : public QObject
{
    Q_OBJECT
public:
    explicit Nonogram(QObject *parent = 0);
    Nonogram(int width, int height, QObject *parent = 0);
    ~Nonogram();

    enum CellStatus:qint8{
        Unknown,
        Free,
        Full
    };

    void init(int width, int height);
    bool isValid() const;

    inline int width() const {return _width; }
    inline int height() const {return _height; }
    inline CellStatus* data() const {return _dataGrid.get(); }
    inline const InfoListType& columnInfo(int column) const {return _columnInfo.get()[column]; }
    void setColumnInfo(int column, const InfoListType& newInfo);
    inline InfoListType& rowInfo(int row) const {return _rowInfo.get()[row]; }
    void setRowInfo(int row, const InfoListType& newInfo);

signals:
    void columnInfoChanged(int column);
    void rowInfoChanged(int row);
public slots:

private:
    int _width;
    int _height;
    std::shared_ptr<CellStatus> _dataGrid;
    std::shared_ptr<InfoListType> _columnInfo;
    std::shared_ptr<InfoListType> _rowInfo;
};

QDataStream& operator>>(QDataStream& in, Nonogram& nonogram);
QDataStream& operator<<(QDataStream& out, const Nonogram& nonogram);

#endif // NONOGRAM_H
