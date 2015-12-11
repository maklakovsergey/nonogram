#include "nonogram.h"
#include <QDebug>
#include <algorithm>
#include <QImage>
#include <QColor>
#include <QTest>

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

bool canPlaceBlock(Nonogram::CellStatus line[], int lineSize, int offset, int blockSize){
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

int placeVariantsCount(int variants[], Nonogram::CellStatus line[], const int lineSize, const InfoListType& info){
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

bool solveLine(Nonogram::CellStatus line[], const int lineSize, const InfoListType& info){
    int placeVariants[lineSize];
    memset(placeVariants, 0, sizeof(placeVariants));
    int variantsCount=placeVariantsCount(placeVariants, line, lineSize, info);
    if (variantsCount==0)
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

bool Nonogram::solve(){
    if (!isSolveable())
        return false;
    CellStatus row[_width];
    CellStatus column[_height];
    bool error=false;
    int unsolvedCount=_dataGrid.count(Nonogram::Unknown);
    qDebug()<<"unsolved"<<unsolvedCount;
    while (unsolvedCount>0 && !error){
        for(int r=0;r<_height && !error; r++){
            for(int c=0;c<_width; c++)
                row[c]=data(r,c);
            if (!solveLine(row, _width, _rowInfo[r])){
                qDebug()<<"error in row"<<r;
                error=true;
            }
            else
                for(int c=0; c<_width; c++)
                    if (row[c]!=Unknown && data(r,c)==Unknown)
                        setData(r,c,row[c]);
        }
        for(int c=0;c<_width && !error; c++){
            for(int r=0;r<_height; r++)
                column[r]=data(r,c);
            if (!solveLine(column, _height, _columnInfo[c])){
                qDebug()<<"error in column"<<c;
                error=true;
            }
            else
                for(int r=0;r<_height; r++)
                    if (column[r]!=Unknown && data(r,c)==Unknown)
                        setData(r,c,column[r]);
        }
        int newUnsolvedCount=_dataGrid.count(Nonogram::Unknown);
        if (newUnsolvedCount==unsolvedCount && !error){
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
            qDebug()<<"creating branch";
            if (branch.solve()){
                qDebug()<<"branch solved";
                newUnsolvedCount=0;
                for(int r=0;r<_height; r++)
                    for(int c=0;c<_width; c++)
                        setData(r, c, branch.data(r, c));
            }
            else{
                qDebug()<<"closing branch";
                newUnsolvedCount--;
                setData(row, column, Free);
            }
        }
        unsolvedCount=newUnsolvedCount;
        qDebug()<<"unsolved"<<unsolvedCount;
        saveImage();
    }
    qDebug("end solve");
    return !error;
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
            default: break;
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
