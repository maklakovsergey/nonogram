#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QDataStream>

/*!
 * \brief list of values, that describes the line (row or column)
 * these values are written on header block
 */
typedef QList<quint16> LineInfoType;

enum class CellStatus:qint8{
    Unknown,
    Free,
    Full
};

enum class LineStatus:qint8{
    Normal,
    WillSolve,
    Solving,
    Solved
};

/*!
 * \brief The Nonogram class
 * This class creates, stores and modifies a nonogram
 */
class Nonogram : public QObject
{
    Q_OBJECT
public:
    explicit Nonogram(QObject *parent = 0);
    Nonogram(int width, int height, QObject *parent = 0);
    Nonogram(const Nonogram& nonogram);
    virtual ~Nonogram();

    void init(int width, int height);
    bool isValid() const;
    bool isSolveable() const;

    inline int width () const { return _width;  }
    inline int height() const { return _height; }
    CellStatus data(int row, int column) const;
    const LineInfoType& columnInfo(int column) const;
    const LineInfoType& rowInfo(int row) const;
    LineStatus columnStatus(int column) const;
    LineStatus rowStatus(int row) const;

    void setData(int row, int column, CellStatus value);
    void setColumnInfo(int column, const LineInfoType& newInfo);
    void setRowInfo(int row, const LineInfoType& newInfo);
    void setRowStatus(int row, LineStatus status);
    void setColumnStatus(int column, LineStatus status);

    void insertRow(int position);
    void insertColumn(int position);
    void removeRow(int position);
    void removeColumn(int position);

    void saveImage(const QString &fileName);

    void swap(Nonogram& other) noexcept;
    Nonogram& operator=(Nonogram other);
    bool operator== (const Nonogram& n)const;
    bool operator!= (const Nonogram& n)const {return !(*this == n);}
signals:
    void dataChanged(int row, int column);
    void rowInfoChanged(int row);
    void columnInfoChanged(int column);
    void rowInserted(int row);
    void columnInserted(int column);
    void rowRemoved(int row);
    void columnRemoved(int column);
    void rowStatusChanged(int row);
    void columnStatusChanged(int column);

protected:
    int _width;
    int _height;
    QVector<CellStatus> _dataGrid;
    QVector<LineInfoType> _columnInfo;
    QVector<LineInfoType> _rowInfo;
    QVector<LineStatus> _columnStatus;
    QVector<LineStatus> _rowStatus;

    friend QDataStream& operator>>(QDataStream& in, Nonogram& nonogram);
    friend QDataStream& operator<<(QDataStream& out, const Nonogram& nonogram);
};

#endif // NONOGRAM_H
