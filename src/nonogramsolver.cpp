#include "nonogramsolver.h"
#include <QDebug>
#include <QtConcurrent>

NonogramSolver::NonogramSolver(QObject *parent) : QObject(parent)
{

}

bool NonogramSolver::canPlaceBlock(CellStatus line[], int lineSize, int offset, int blockSize){
    for(int i=0; i<blockSize; i++)
        if (line[offset+i]==CellStatus::Free)
            return false;
    for(int i=0; i<offset; i++)
        if (line[i]==CellStatus::Full)
            return false;
    if (offset+blockSize<lineSize && line[offset+blockSize]==CellStatus::Full)
        return false;
    return true;
}

int NonogramSolver::placeVariantsCount(int variants[], CellStatus line[], const int lineSize, const LineInfoType& info){
    if (isAborted())
        return 0;
    if (info.count()==0){
        for(int i=0; i<lineSize; i++)
            if (line[i]==CellStatus::Full)
                return 0;
        return 1;
    }
    int blockSize=info[0];
    int totalVariantCount=0;
    for(int i=0; i<=lineSize-blockSize; i++)
        if (canPlaceBlock(line, lineSize, i, blockSize)){
            int endPos=i+blockSize+1;
            int variantCount=placeVariantsCount(variants+endPos, line+endPos, lineSize-endPos, info.mid(1));
            if (variantCount>0){
                totalVariantCount+=variantCount;
                for(int j=0; j<blockSize; j++)
                    variants[i+j]+=variantCount;
            }
        }
    return totalVariantCount;
}

bool NonogramSolver::solveLine(CellStatus line[], const int lineSize, const LineInfoType& info){
    QVector<int> placeVariants(lineSize);
    memset(placeVariants.data(), 0, sizeof(int)*lineSize);
    int variantsCount=placeVariantsCount(placeVariants.data(), line, lineSize, info);
    if (variantsCount==0 || isAborted())
        return false;
    for(int i=0; i<lineSize; i++){
        if (placeVariants[i]==0){
            if (line[i]==CellStatus::Full)
                return false;
            else
                line[i]=CellStatus::Free;
        }
        else if (placeVariants[i]==variantsCount){
            if (line[i]==CellStatus::Free)
                return false;
            else
                line[i]=CellStatus::Full;
        }
    }
    return true;
}

bool NonogramSolver::solveRow(int r, QVector<bool>* needCheckColumn){
    int width=_nonogram->width();
    QVector<CellStatus> row(width);
    _nonogram->setRowStatus(r, LineStatus::Solving);
    for(int c=0;c<width; c++)
        row[c]=_nonogram->data(r,c);
    if (!solveLine(row.data(), width, _nonogram->rowInfo(r))){
        if (!isAborted())
            qDebug()<<"error in row"<<r;
        return false;
    }
    else
        for(int c=0; c<width; c++)
            if (row[c]!=CellStatus::Unknown && _nonogram->data(r,c)==CellStatus::Unknown){
                _nonogram->setData(r,c,row[c]);
                (*needCheckColumn)[c]=true;
            }
    _nonogram->setRowStatus(r, LineStatus::Solved);
    return true;
}

bool NonogramSolver::solveColumn(int c, QVector<bool>* needCheckRow){
    int height=_nonogram->height();
    QVector<CellStatus> column(height);
    _nonogram->setColumnStatus(c, LineStatus::Solving);
    for(int r=0;r<height; r++)
        column[r]=_nonogram->data(r,c);
    if (!solveLine(column.data(), height, _nonogram->columnInfo(c))){
        if (!isAborted())
            qDebug()<<"error in column"<<c;
        return false;
    }
    else
        for(int r=0;r<height; r++)
            if (column[r]!=CellStatus::Unknown && _nonogram->data(r,c)==CellStatus::Unknown){
                _nonogram->setData(r,c,column[r]);
                (*needCheckRow)[r]=true;
            }
    _nonogram->setColumnStatus(c, LineStatus::Solved);
    return true;
}

void NonogramSolver::solveBranch(){
    //_nonogram->saveImage("1.png");
    int width=_nonogram->width();
    int height=_nonogram->height();
    int row=0, column=0;
    for(int r=0;r<height; r++)
        for(int c=0;c<width; c++)
            if (_nonogram->data(r, c)==CellStatus::Unknown){
                row=r;
                column=c;
                c=width;
                r=height;
            }
    NonogramSolver subsolver;
    Nonogram branch(*_nonogram);
    branch.setData(row, column, CellStatus::Full);
    connect(this, &NonogramSolver::aborted, &subsolver, &NonogramSolver::abort);
    qDebug()<<"creating branch";
    if (subsolver.solve(&branch)){
        qDebug()<<"branch solved";
        for(int r=0;r<height; r++)
            for(int c=0;c<width; c++)
                _nonogram->setData(r, c, branch.data(r, c));
    }
    else{
        qDebug()<<"closing branch";
        _nonogram->setData(row, column, CellStatus::Free);
    }
}

QVector<bool> NonogramSolver::rowsForInitialCheck(){
    int width=_nonogram->width();
    int height=_nonogram->height();
    QVector<bool> needCheckRow;
    needCheckRow.fill(true, height);
    for(int r=0;r<height; r++){
        bool needCheck=false;
        for(int c=0;c<width; c++)
            if (_nonogram->data(r,c)!=CellStatus::Unknown){
                needCheck=true;
                break;
            }
        if (!needCheck){
            int sum=0,max=0;
            for(int value:_nonogram->rowInfo(r)){
                sum+=value+1;
                if (value>max)
                    max=value;
            }
            if (width-(sum-1)<max)
                needCheck=true;
        }
        needCheckRow[r]=needCheck;
    }
    return needCheckRow;
}

QVector<bool> NonogramSolver::columnsForInitialCheck(){
    int width=_nonogram->width();
    int height=_nonogram->height();
    QVector<bool> needCheckColumn;
    needCheckColumn.fill(true, width);
    for(int c=0;c<width; c++){
        bool needCheck=false;
        for(int r=0;r<height; r++)
            if (_nonogram->data(r,c)!=CellStatus::Unknown){
                needCheck=true;
                break;
            }
        if (!needCheck){
            int sum=0,max=0;
            for(int value:_nonogram->columnInfo(c)){
                sum+=value+1;
                if (value>max)
                    max=value;
            }
            if (height-(sum-1)<max)
                needCheck=true;
        }
        needCheckColumn[c]=needCheck;
    }
    return needCheckColumn;
}

bool NonogramSolver::solve(Nonogram* nonogram){
    Q_ASSERT(nonogram);
    _nonogram=nonogram;
    if (!_nonogram->isSolveable())
        return false;
    _aborted=false;
    bool error=false;
    QVector<bool> needCheckRow=rowsForInitialCheck();
    QVector<bool> needCheckColumn=columnsForInitialCheck();
    int width=_nonogram->width();
    int height=_nonogram->height();

    QList<QFuture<bool>> operations;

    int unsolvedCount=unknownCount();
    qDebug()<<"unsolved"<<unsolvedCount<<"cells";
    for(int r=0; r<height; r++)
        _nonogram->setRowStatus(r, LineStatus::Normal);
    for(int c=0; c<width; c++)
        _nonogram->setColumnStatus(c, LineStatus::Normal);
    QTime solveTime;
    solveTime.start();
    while (unsolvedCount>0 && !error && !isAborted()){
        for(int r=0; r<height&& !error; r++)
            if (needCheckRow[r]){
                _nonogram->setRowStatus(r, LineStatus::WillSolve);
                operations.append(QtConcurrent::run(this, &NonogramSolver::solveRow, r, &needCheckColumn));
            }
            else
                _nonogram->setRowStatus(r, LineStatus::Normal);

        for(auto future:operations){
            future.waitForFinished();
            if (!future.result())
                error=true;
        }
        operations.clear();
        needCheckRow.fill(false);

        for(int c=0; c<width && !error; c++)
            if (needCheckColumn[c]){
                _nonogram->setColumnStatus(c, LineStatus::WillSolve);
                operations.append(QtConcurrent::run(this, &NonogramSolver::solveColumn, c, &needCheckRow));
            }
            else
                _nonogram->setColumnStatus(c, LineStatus::Normal);

        for(auto future:operations){
            future.waitForFinished();
            if (!future.result())
                error=true;
        }
        operations.clear();
        needCheckColumn.fill(false);

        int newUnsolvedCount=unknownCount();
        if (newUnsolvedCount==unsolvedCount && !error && !isAborted()){
            solveBranch();
            newUnsolvedCount=unknownCount();
        }
        unsolvedCount=newUnsolvedCount;
        qDebug()<<"unsolved"<<unsolvedCount<<"cells in"<<solveTime.elapsed()<<"ms";
    }
    //_nonogram->saveImage("1.png");
    qDebug("end solve");
    return !error;
}

int NonogramSolver::unknownCount(){
    int width=_nonogram->width();
    int height=_nonogram->height();
    int unknownCount=0;
    for(int r=0;r<height; r++)
        for(int c=0;c<width; c++)
            if (_nonogram->data(r, c)==CellStatus::Unknown)
                unknownCount++;
    return unknownCount;
}

bool NonogramSolver::isAborted(){
    bool aborted;
    _abortedLock.lockForRead();
    aborted=_aborted;
    _abortedLock.unlock();
    return aborted;
}

void NonogramSolver::abort(){
    _abortedLock.lockForWrite();
    _aborted=true;
    _abortedLock.unlock();
    emit aborted();
}
