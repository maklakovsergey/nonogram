#include "nonogram.h"
#include <QDebug>
#include <algorithm>
#include <QImage>
#include <QColor>
#include <QtConcurrent>

Nonogram::Nonogram(QObject *parent) : QObject(parent){
    _width=0;
    _height=0;
}

Nonogram::Nonogram(int width, int height, QObject *parent) : Nonogram(parent){
    init(width, height);
}

Nonogram::Nonogram(const Nonogram& nonogram):Nonogram(nonogram.width(), nonogram.height()){
    memcpy(_dataGrid.data(), nonogram._dataGrid.data(), _dataGrid.size()*sizeof(CellStatus));
    for(int i=0; i<_width; i++)
        _columnInfo[i].append(nonogram._columnInfo[i]);
    for(int i=0; i<_height; i++)
        _rowInfo[i].append(nonogram._rowInfo[i]);
}

Nonogram::~Nonogram(){
}

void Nonogram::init(int width, int height){
    _width=width;
    _height=height;
    _dataGrid=QVector<CellStatus>(_width*_height, Unknown);
    _columnInfo=QVector<InfoListType>(_width);
    _rowInfo=QVector<InfoListType>(_height);
}

bool Nonogram::isValid() const{
    return (_width && _height && _dataGrid.count() && _columnInfo.count() && _rowInfo.count());
}

bool Nonogram::isSolveable() const{
    if (!isValid())
        return false;
    int columnSum=0;
    for(int c=0; c<_width; c++){
        int infoSum=0;
        for(int value:_columnInfo[c])
            infoSum+=value;
        if (infoSum+_columnInfo[c].count()-1>_height)
            return false;
        columnSum+=infoSum;
    }
    int rowSum=0;
    for(int r=0; r<_height; r++){
        int infoSum=0;
        for(int value:rowInfo(r))
            infoSum+=value;
        if (infoSum+rowInfo(r).count()-1>_width)
            return false;
        rowSum+=infoSum;
    }
    return columnSum==rowSum;
}

bool Nonogram::canPlaceBlock(Nonogram::CellStatus line[], int lineSize, int offset, int blockSize){
    for(int i=0; i<blockSize; i++)
        if (line[offset+i]==Nonogram::Free)
            return false;
    for(int i=0; i<offset; i++)
        if (line[i]==Nonogram::Full)
            return false;
    if (offset+blockSize<lineSize && line[offset+blockSize]==Nonogram::Full)
        return false;
    return true;
}

int Nonogram::placeVariantsCount(int variants[], Nonogram::CellStatus line[], const int lineSize, const InfoListType& info){
    if (isAborted())
        return 0;
    if (info.count()==0){
        for(int i=0; i<lineSize; i++)
            if (line[i]==Nonogram::Full)
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

bool Nonogram::solveLine(Nonogram::CellStatus line[], const int lineSize, const InfoListType& info){
    int placeVariants[lineSize];
    memset(placeVariants, 0, sizeof(placeVariants));
    int variantsCount=placeVariantsCount(placeVariants, line, lineSize, info);
    if (variantsCount==0 || isAborted())
        return false;
    for(int i=0; i<lineSize; i++){
        if (placeVariants[i]==0){
            if (line[i]==Nonogram::Full)
                return false;
            line[i]=Nonogram::Free;
        }
        if (placeVariants[i]==variantsCount){
            if (line[i]==Nonogram::Free)
                return false;
            line[i]=Nonogram::Full;
        }
    }
    return true;
}

bool Nonogram::solveRow(int r, QVector<bool>* needCheckColumn){
    CellStatus row[_width];
    setRowStatus(r, Solving);
    for(int c=0;c<_width; c++)
        row[c]=data(r,c);
    if (!solveLine(row, _width, _rowInfo[r])){
        if (!isAborted())
            qDebug()<<"error in row"<<r;
        return false;
    }
    else
        for(int c=0; c<_width; c++)
            if (row[c]!=Unknown && data(r,c)==Unknown){
                setData(r,c,row[c]);
                (*needCheckColumn)[c]=true;
            }
    setRowStatus(r, Solved);
    return true;
}

bool Nonogram::solveColumn(int c, QVector<bool>* needCheckRow){
    CellStatus column[_height];
    setColumnStatus(c, Solving);
    for(int r=0;r<_height; r++)
        column[r]=data(r,c);
    if (!solveLine(column, _height, _columnInfo[c])){
        if (!isAborted())
            qDebug()<<"error in column"<<c;
        return false;
    }
    else
        for(int r=0;r<_height; r++)
            if (column[r]!=Unknown && data(r,c)==Unknown){
                setData(r,c,column[r]);
                (*needCheckRow)[r]=true;
            }
    setColumnStatus(c, Solved);
    return true;
}

void Nonogram::solveBranch(){
    saveImage();
    int row=0, column=0;
    for(int r=0;r<_height; r++)
        for(int c=0;c<_width; c++)
            if (data(r, c)==Unknown){
                row=r;
                column=c;
                c=_width;
                r=_height;
            }

    Nonogram branch(*this);
    branch.setData(row, column, Full);
    connect(this, &Nonogram::aborted, &branch, &Nonogram::abort);
    qDebug()<<"creating branch";
    if (branch.solve()){
        qDebug()<<"branch solved";
        for(int r=0;r<_height; r++)
            for(int c=0;c<_width; c++)
                setData(r, c, branch.data(r, c));
    }
    else{
        qDebug()<<"closing branch";
        setData(row, column, Free);
    }
}

QVector<bool> Nonogram::rowsForInitialCheck(){
    QVector<bool> needCheckRow;
    needCheckRow.fill(true, _height);
    for(int r=0;r<_height; r++){
        bool needCheck=false;
        for(int c=0;c<_width; c++)
            if (data(r,c)!=Unknown){
                needCheck=true;
                break;
            }
        if (!needCheck){
            int sum=0,max=0;
            for(int value:_rowInfo[r]){
                sum+=value+1;
                if (value>max)
                    max=value;
            }
            if (_width-(sum-1)<max)
                needCheck=true;
        }
        needCheckRow[r]=needCheck;
    }
    return needCheckRow;
}

QVector<bool> Nonogram::columnsForInitialCheck(){
    QVector<bool> needCheckColumn;
    needCheckColumn.fill(true, _width);
    for(int c=0;c<_width; c++){
        bool needCheck=false;
        for(int r=0;r<_height; r++)
            if (data(r,c)!=Unknown){
                needCheck=true;
                break;
            }
        if (!needCheck){
            int sum=0,max=0;
            for(int value:_columnInfo[c]){
                sum+=value+1;
                if (value>max)
                    max=value;
            }
            if (_height-(sum-1)<max)
                needCheck=true;
        }
        needCheckColumn[c]=needCheck;
    }
    return needCheckColumn;
}

bool Nonogram::solve(){
    if (!isSolveable())
        return false;
    _aborted=false;
    bool error=false;
    QVector<bool> needCheckRow=rowsForInitialCheck();
    QVector<bool> needCheckColumn=columnsForInitialCheck();
    QList<QFuture<bool>> operations;

    int unsolvedCount=_dataGrid.count(Nonogram::Unknown);
    qDebug()<<"unsolved"<<unsolvedCount;
    _rowStatus.fill(Normal, _height);
    _columnStatus.fill(Normal, _width);
    QTime solveTime;
    solveTime.start();
    while (unsolvedCount>0 && !error && !isAborted()){
        for(int r=0; r<_height&& !error; r++)
            if (needCheckRow[r]){
                setRowStatus(r, WillSolve);
                operations.append(QtConcurrent::run(this, &Nonogram::solveRow, r, &needCheckColumn));
            }
            else
                setRowStatus(r, Normal);

        for(auto future:operations){
            future.waitForFinished();
            if (!future.result())
                error=true;
        }
        operations.clear();
        needCheckRow.fill(false);

        for(int c=0; c<_width && !error; c++)
            if (needCheckColumn[c]){
                setColumnStatus(c, WillSolve);
                operations.append(QtConcurrent::run(this, &Nonogram::solveColumn, c, &needCheckRow));
            }
            else
                setColumnStatus(c, Normal);

        for(auto future:operations){
            future.waitForFinished();
            if (!future.result())
                error=true;
        }
        operations.clear();
        needCheckColumn.fill(false);

        int newUnsolvedCount=_dataGrid.count(Nonogram::Unknown);
        if (newUnsolvedCount==unsolvedCount && !error && !isAborted()){
            solveBranch();
            newUnsolvedCount=_dataGrid.count(Nonogram::Unknown);
        }
        unsolvedCount=newUnsolvedCount;
        qDebug()<<"unsolved"<<unsolvedCount<<solveTime.elapsed()<<"ms";
    }
    _rowStatus.clear();
    _columnStatus.clear();
    saveImage();
    qDebug("end solve");
    return !error;
}

bool Nonogram::isAborted(){
    bool aborted;
    _abortedLock.lockForRead();
    aborted=_aborted;
    _abortedLock.unlock();
    return aborted;
}

void Nonogram::abort(){
    _abortedLock.lockForWrite();
    _aborted=true;
    _abortedLock.unlock();
    emit aborted();
}

void Nonogram::saveImage(){
    QImage image(_width, _height, QImage::Format_RGB888);
    for(int r=0;r<_height; r++)
        for(int c=0;c<_width; c++){
            Qt::GlobalColor color;
            switch(data(r,c)){
            case Free:    color=Qt::white; break;
            case Full:    color=Qt::black; break;
            case Unknown: color=Qt::gray;  break;
            default:      color=Qt::gray;  break;
            }
            image.setPixel(c, r, QColor(color).rgb());
        }
    image.save("1.png");
}

void Nonogram::setData(int row, int column, CellStatus value) {
    _dataGrid[row*_width+column]=value;
    emit dataChanged(row, column);
}

void Nonogram::setColumnInfo(int column, const InfoListType& newInfo) {
    InfoListType& info=_columnInfo[column];
    info.clear();
    info.append(newInfo);
    emit columnInfoChanged(column);
}

void Nonogram::setRowInfo(int row, const InfoListType& newInfo) {
    InfoListType& info=_rowInfo[row];
    info.clear();
    info.append(newInfo);
    emit rowInfoChanged(row);
}

void Nonogram::setRowStatus(int row, LineStatus status){
    _rowStatus[row]=status;
    emit rowStatusChanged(row);
}

void Nonogram::setColumnStatus(int column, LineStatus status){
    _columnStatus[column]=status;
    emit columnStatusChanged(column);
}

void Nonogram::insertRow(int position){
    if (position<0|| position>_height){
        qDebug()<<"cannot insert row at"<<position<<"current height"<<_height;
        return;
    }
    CellStatus dataGrid[_width*_height];
    memcpy(dataGrid, _dataGrid.data(), _width*_height*sizeof(CellStatus));

    int newHeight=_height+1;
    _dataGrid.resize(_width*newHeight);
    for(int r=0; r<position; r++)
        for(int c=0; c<_width; c++)
            _dataGrid[r*_width+c]=dataGrid[r*_width+c];
    for(int c=0; c<_width; c++)
        _dataGrid[position*_width+c]=Unknown;
    for(int r=position+1; r<newHeight; r++)
        for(int c=0; c<_width; c++)
            _dataGrid[r*_width+c]=dataGrid[(r-1)*_width+c];
    _rowInfo.insert(position, InfoListType());
    _height=newHeight;

    emit rowInserted(position);
}

void Nonogram::insertColumn(int position){
    if (position<0 || position>_width){
        qDebug()<<"cannot insert column at"<<position<<"current width"<<_width;
        return;
    }
    CellStatus dataGrid[_width*_height];
    memcpy(dataGrid, _dataGrid.data(), _width*_height*sizeof(CellStatus));

    int newWidth=_width+1;
    _dataGrid.resize(newWidth*_height);
    for(int c=0; c<position; c++)
        for(int r=0; r<_height; r++)
            _dataGrid[r*newWidth+c]=dataGrid[r*_width+c];
    for(int r=0; r<_height; r++)
        _dataGrid[r*newWidth+position]=Unknown;
    for(int c=position+1; c<newWidth; c++)
        for(int r=0; r<_height; r++)
            _dataGrid[r*newWidth+c]=dataGrid[r*_width+c-1];
    _columnInfo.insert(position, InfoListType());
    _width=newWidth;

    emit columnInserted(position);
}

void Nonogram::removeRow(int position){
    if (position<0|| position>_height){
        qDebug()<<"cannot insert row at"<<position<<"current height"<<_height;
        return;
    }
    CellStatus dataGrid[_width*_height];
    memcpy(dataGrid, _dataGrid.data(), _width*_height*sizeof(CellStatus));

    int newHeight=_height-1;
    _dataGrid.resize(_width*newHeight);
    for(int r=0; r<position; r++)
        for(int c=0; c<_width; c++)
            _dataGrid[r*_width+c]=dataGrid[r*_width+c];
    for(int r=position; r<newHeight; r++)
        for(int c=0; c<_width; c++)
            _dataGrid[r*_width+c]=dataGrid[(r+1)*_width+c];
    _rowInfo.removeAt(position);
    _height=newHeight;

    emit rowRemoved(position);
}

void Nonogram::removeColumn(int position){
    if (position<0 || position>_width){
        qDebug()<<"cannot insert column at"<<position<<"current width"<<_width;
        return;
    }
    CellStatus dataGrid[_width*_height];
    memcpy(dataGrid, _dataGrid.data(), _width*_height*sizeof(CellStatus));

    int newWidth=_width-1;
    _dataGrid.resize(newWidth*_height);
    for(int c=0; c<position; c++)
        for(int r=0; r<_height; r++)
            _dataGrid[r*newWidth+c]=dataGrid[r*_width+c];
    for(int c=position; c<newWidth; c++)
        for(int r=0; r<_height; r++)
            _dataGrid[r*newWidth+c]=dataGrid[r*_width+c+1];
    _columnInfo.removeAt(position);
    _width=newWidth;

    emit columnRemoved(position);
}

bool Nonogram::operator== (const Nonogram& n)const{
    return (n.width()==_width && n.height()== _height && n._dataGrid==_dataGrid &&
            n._columnInfo==_columnInfo && n._rowInfo==_rowInfo);
}

QDataStream& operator>>(QDataStream& dataStream, Nonogram& nonogram){
    int width=0, height=0;
    dataStream >> width >> height;
    if (width==0 || height==0)
        return dataStream;
    nonogram.init(width, height);

    int dataSize=dataStream.readRawData((char*)nonogram._dataGrid.data(), width*height);
    if (dataSize!=width*height)
        return dataStream;
    dataStream >> nonogram._columnInfo >> nonogram._rowInfo;
    return dataStream;
}

QDataStream& operator<<(QDataStream& dataStream, const Nonogram& nonogram){
    dataStream << nonogram._width << nonogram._height;
    dataStream.writeRawData((char*)nonogram._dataGrid.data(), nonogram._width * nonogram._height);
    dataStream << nonogram._columnInfo << nonogram._rowInfo;
    return dataStream;
}
