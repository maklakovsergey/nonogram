
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
