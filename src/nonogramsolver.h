#ifndef NONOGRAMSOLVER_H
#define NONOGRAMSOLVER_H

#include <QObject>
#include <QReadWriteLock>
#include "nonogram.h"

class NonogramSolver : public QObject
{
    Q_OBJECT
public:
    explicit NonogramSolver(QObject *parent = 0);
    bool solve(Nonogram* nonogram);
    bool isAborted();
public slots:
    void abort();
signals:
    void aborted();
protected:
    Nonogram* _nonogram;
    bool _aborted;
    QReadWriteLock _abortedLock;

    QVector<bool> rowsForInitialCheck();
    QVector<bool> columnsForInitialCheck();
    bool solveRow(int r, QVector<bool>* needCheckColumn);
    bool solveColumn(int c, QVector<bool>* needCheckRow);
    void solveBranch();
    bool solveLine(CellStatus line[], const int lineSize, const LineInfoType& info);
    int placeVariantsCount(int variants[], CellStatus line[], const int lineSize, const LineInfoType& info);
    bool canPlaceBlock(CellStatus line[], int lineSize, int offset, int blockSize);
    int unknownCount();
};

#endif // NONOGRAMSOLVER_H
