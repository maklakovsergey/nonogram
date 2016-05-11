
#include "testnonogram.h"
#include <QtTest/QTest>
#include "test.h"

TestNonogram::TestNonogram(QObject *parent) : QObject(parent){
}

void saveAndLoad(const Nonogram &in, Nonogram& out){
    int memoryUsage=in.width()*in.height()*5;
    QByteArray storage(memoryUsage, 0);
    QDataStream writestream(&storage, QIODevice::WriteOnly);
    writestream << in;
    QDataStream readstream(&storage, QIODevice::ReadOnly);
    readstream >> out;
}

void fillRandomData(Nonogram& n){
    int width=n.width(), height=n.height();
    for(int r=0; r<height; r++)
        for(int c=0; c<width; c++)
            n.setData(r, c, (CellStatus)(rand()%3));
}

void fillRandomHeader(Nonogram& n){
    const int width=n.width(), height=n.height();
    for(int r=0; r<height; r++){
        int recordCount=rand()%4;
        LineInfoType line;
        for (int i=0; i<recordCount; i++)
            line.append(rand()%width+1);
        n.setRowInfo(r,line);
    }
    for(int c=0; c<width; c++){
        int recordCount=rand()%4;
        LineInfoType line;
        for (int i=0; i<recordCount; i++)
            line.append(rand()%height+1);
        n.setColumnInfo(c,line);
    }
}

void TestNonogram::initialize(){
    Nonogram n1;
    QCOMPARE(n1.width(), 0);
    QCOMPARE(n1.height(), 0);
    QVERIFY(!n1.isValid());
    n1.init(10, 20);
    QCOMPARE(n1.width(), 10);
    QCOMPARE(n1.height(), 20);
    QVERIFY(n1.isValid());
    QCOMPARE(n1.columnInfo(0), LineInfoType());
    QCOMPARE(n1.rowInfo(0), LineInfoType());
    QCOMPARE(n1.columnInfo(9), LineInfoType());
    QCOMPARE(n1.rowInfo(19), LineInfoType());
    Nonogram n2(10,20);
    QCOMPARE(n2.width(), 10);
    QCOMPARE(n2.height(), 20);
    QVERIFY(n1.isValid());
    QCOMPARE(n2.columnInfo(0), LineInfoType());
    QCOMPARE(n2.rowInfo(0), LineInfoType());
    QCOMPARE(n2.columnInfo(9), LineInfoType());
    QCOMPARE(n2.rowInfo(19), LineInfoType());
}

void TestNonogram::copy(){
    const int width=5, height=10;
    Nonogram n1(width, height);
    fillRandomHeader(n1);
    fillRandomData(n1);
    Nonogram n2(n1);
    QCOMPARE(n2.width(), n1.width());
    QCOMPARE(n2.height(), n1.height());
    for(int r=0; r<height; r++)
        QCOMPARE(n2.rowInfo(r), n1.rowInfo(r));
    for(int c=0; c<width; c++)
        QCOMPARE(n2.columnInfo(c), n1.columnInfo(c));
    for(int r=0; r<height; r++)
        for(int c=0; c<width; c++)
            QCOMPARE(n2.data(r, c), n1.data(r, c));
    Nonogram n3(1,1);
    n3=n1;
    QCOMPARE(n3.width(), n1.width());
    QCOMPARE(n3.height(), n1.height());
    for(int r=0; r<height; r++)
        QCOMPARE(n3.rowInfo(r), n1.rowInfo(r));
    for(int c=0; c<width; c++)
        QCOMPARE(n3.columnInfo(c), n1.columnInfo(c));
    for(int r=0; r<height; r++)
        for(int c=0; c<width; c++)
            QCOMPARE(n3.data(r, c), n1.data(r, c));
}

void TestNonogram::equals(){
    const int width=5, height=10;
    Nonogram n1(width, height);
    fillRandomHeader(n1);
    fillRandomData(n1);
    Nonogram n2(n1);
    QVERIFY( (n2==n1));
    QVERIFY(!(n2!=n1));

    CellStatus oldCellStatus=n2.data(0,0);
    CellStatus newCellStatus=CellStatus::Free;
    if (oldCellStatus==CellStatus::Free)
        newCellStatus= CellStatus::Full;
    n2.setData(0,0,newCellStatus);
    QVERIFY(!(n2==n1));
    QVERIFY( (n2!=n1));

    n2 = n1;
    LineInfoType rowInfo=n2.rowInfo(0);
    rowInfo.append(1);
    n2.setRowInfo(0, rowInfo);
    QVERIFY(!(n2==n1));
    QVERIFY( (n2!=n1));

    n2 = n1;
    LineInfoType columnInfo=n2.columnInfo(0);
    columnInfo.append(1);
    n2.setColumnInfo(0, columnInfo);
    QVERIFY(!(n2==n1));
    QVERIFY( (n2!=n1));
}

void TestNonogram::solveable(){
    Nonogram n1(2, 2);
    n1.setRowInfo   (0, LineInfoType({1}));
    n1.setRowInfo   (1, LineInfoType({1}));
    n1.setColumnInfo(0, LineInfoType({1}));
    n1.setColumnInfo(1, LineInfoType({1}));
    QVERIFY(n1.isSolveable());

    //rows and columns sum are different
    Nonogram n2=n1;
    n2.setRowInfo   (0, LineInfoType({2}));
    QVERIFY(!n2.isSolveable());

    //column value is too high
    n2=n1;
    n2.setRowInfo   (0, LineInfoType({2}));
    n2.setRowInfo   (1, LineInfoType({2}));
    n2.setColumnInfo(0, LineInfoType({3}));
    QVERIFY(!n2.isSolveable());
    n2.setColumnInfo(0, LineInfoType({1, 1}));
    QVERIFY(!n2.isSolveable());

    //row value is too high
    n2=n1;
    n2.setRowInfo   (0, LineInfoType({3}));
    n2.setColumnInfo(0, LineInfoType({2}));
    n2.setColumnInfo(1, LineInfoType({2}));
    QVERIFY(!n2.isSolveable());
    n2.setRowInfo   (0, LineInfoType({1, 1}));
    QVERIFY(!n2.isSolveable());
}

void TestNonogram::stateChanging(){
    Nonogram n1(5, 10);
    fillRandomData(n1);
    fillRandomHeader(n1);

    const int dataRow=3, dataColumn=2;

    bool dataChanged=false;
    bool rowInfoChanged=false;
    bool rowInserted=false;
    bool rowRemoved=false;
    bool columnInfoChanged=false;
    bool columnInserted=false;
    bool columnRemoved=false;
    connect(&n1, &Nonogram::dataChanged, [&](int row, int column){
        dataChanged=true;
        QCOMPARE(row, dataRow);
        QCOMPARE(column, dataColumn);
    });
    connect(&n1, &Nonogram::rowInfoChanged, [&](int row){
        rowInfoChanged=true;
        QCOMPARE(row, dataRow);
    });
    connect(&n1, &Nonogram::rowInserted, [&](int row){
        rowInserted=true;
        QCOMPARE(row, dataRow);
    });
    connect(&n1, &Nonogram::rowRemoved, [&](int row){
        rowRemoved=true;
        QCOMPARE(row, dataRow);
    });
    connect(&n1, &Nonogram::columnInfoChanged, [&](int column){
        columnInfoChanged=true;
        QCOMPARE(column, dataColumn);
    });
    connect(&n1, &Nonogram::columnInserted, [&](int column){
        columnInserted=true;
        QCOMPARE(column, dataColumn);
    });
    connect(&n1, &Nonogram::columnRemoved, [&](int column){
        columnRemoved=true;
        QCOMPARE(column, dataColumn);
    });

    CellStatus oldCellStatus=n1.data(dataRow,dataColumn);
    CellStatus newCellStatus=CellStatus::Free;
    if (oldCellStatus==CellStatus::Free)
        newCellStatus= CellStatus::Full;
    n1.setData(dataRow,dataColumn,newCellStatus);
    QVERIFY(dataChanged);
    QCOMPARE(n1.data(dataRow,dataColumn), newCellStatus);

    LineInfoType rowInfo=n1.rowInfo(dataRow);
    rowInfo.append(rand()%10+1);
    n1.setRowInfo(dataRow, rowInfo);
    QVERIFY(rowInfoChanged);
    QCOMPARE(n1.rowInfo(dataRow), rowInfo);

    LineInfoType columnInfo=n1.columnInfo(dataColumn);
    columnInfo.append(rand()%10+1);
    n1.setColumnInfo(dataColumn, columnInfo);
    QVERIFY(columnInfoChanged);
    QCOMPARE(n1.columnInfo(dataColumn), columnInfo);

    const Nonogram n2(n1);
    n1.insertRow(dataRow);
    QVERIFY(rowInserted);
    for(int r=0; r<dataRow; r++)
        QCOMPARE(n1.rowInfo(r), n2.rowInfo(r));
    QCOMPARE(n1.rowInfo(dataRow), LineInfoType());
    for(int r=dataRow; r<n2.height(); r++)
        QCOMPARE(n1.rowInfo(r+1), n2.rowInfo(r));

    n1.removeRow(dataRow);
    QVERIFY(rowRemoved);
    QCOMPARE(n2, n1);

    n1.insertColumn(dataColumn);
    QVERIFY(columnInserted);
    for(int c=0; c<dataColumn; c++)
        QCOMPARE(n1.columnInfo(c), n2.columnInfo(c));
    QCOMPARE(n1.columnInfo(dataColumn), LineInfoType());
    for(int c=dataColumn; c<n2.width(); c++)
        QCOMPARE(n1.columnInfo(c+1), n2.columnInfo(c));
    n1.removeColumn(dataColumn);
    QVERIFY(columnRemoved);
    QCOMPARE(n2, n1);
}

void TestNonogram::saveLoad(){
    Nonogram heart=heartNonogram();
    Nonogram loaded;
    fillRandomData(heart);
    saveAndLoad(heart, loaded);
    QCOMPARE(heart, loaded);

    Nonogram car=carNonogram();
    fillRandomData(car);
    saveAndLoad(car, loaded);
    QCOMPARE(car, loaded);

    Nonogram clef=clefNonogram();
    fillRandomData(clef);
    saveAndLoad(clef, loaded);
    QCOMPARE(clef, loaded);
}
