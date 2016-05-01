#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QDataStream>
#include <QReadWriteLock>

/*!
 * \brief list of values, that describes the line (row or column)
 * these values are written on header block
*/
typedef QList<quint16> LineInfoType;

/*!
 * \brief The Nonogram class
 */
class Nonogram : public QObject
{
    Q_OBJECT
public:
    explicit Nonogram(QObject *parent = 0);
    Nonogram(int width, int height, QObject *parent = 0);
    Nonogram(const Nonogram& nonogram);
    virtual ~Nonogram();

    enum CellStatus:qint8{
        Unknown,
        Free,
        Full
    };

    enum LineStatus:qint8{
        Normal,
        WillSolve,
        Solving,
        Solved
    };

    void init(int width, int height);
    bool isValid() const;
    bool isSolveable() const;

    inline int width() const                                { return _width; }
    inline int height() const                               { return _height; }
    inline CellStatus data(int row, int column) const       { return _dataGrid[row*_width+column]; }
    inline const LineInfoType& columnInfo(int column) const { return _columnInfo[column]; }
    inline const LineInfoType& rowInfo(int row) const       { return _rowInfo[row]; }
    inline LineStatus columnStatus(int column) const        { return _columnStatus.count()? _columnStatus[column]:Normal; }
    inline LineStatus rowStatus(int row) const              { return _rowStatus.count()? _rowStatus[row]:Normal; }

    void setData(int row, int column, CellStatus value);
    void setColumnInfo(int column, const LineInfoType& newInfo);
    void setRowInfo(int row, const LineInfoType& newInfo);

    void insertRow(int position);
    void insertColumn(int position);
    void removeRow(int position);
    void removeColumn(int position);

    bool operator== (const Nonogram& n)const;
    bool operator!= (const Nonogram& n)const {return !(*this == n);}

    bool solve();

    bool isAborted();

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
public slots:

    void abort();
protected:
    int _width;
    int _height;
    QVector<CellStatus> _dataGrid;
    QVector<LineInfoType> _columnInfo;
    QVector<LineInfoType> _rowInfo;
    QVector<LineStatus> _columnStatus;
    QVector<LineStatus> _rowStatus;
    bool _aborted;

    QReadWriteLock _abortedLock;

    void saveImage();

    QVector<bool> rowsForInitialCheck();
    QVector<bool> columnsForInitialCheck();
    bool solveRow(int r, QVector<bool>* needCheckColumn);
    bool solveColumn(int c, QVector<bool>* needCheckRow);
    void solveBranch();
    bool solveLine(Nonogram::CellStatus line[], const int lineSize, const LineInfoType& info);
    int placeVariantsCount(int variants[], Nonogram::CellStatus line[], const int lineSize, const LineInfoType& info);
    bool canPlaceBlock(Nonogram::CellStatus line[], int lineSize, int offset, int blockSize);

    void setRowStatus(int row, LineStatus status);
    void setColumnStatus(int column, LineStatus status);

    friend QDataStream& operator>>(QDataStream& in, Nonogram& nonogram);
    friend QDataStream& operator<<(QDataStream& out, const Nonogram& nonogram);

signals:
    void aborted();
};

#endif // NONOGRAM_H
