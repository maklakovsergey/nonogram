
#include "testnonogram.h"
#include <QtTest/QTest>

TestNonogram::TestNonogram(QObject *parent) : QObject(parent)
{

}
#ifdef QT_DEBUG

void TestNonogram::initHeartNonogram(Nonogram& n){
    //https://upload.wikimedia.org/wikipedia/ru/a/ad/Step4.png
    n.init(9,9);
    InfoListType columns[9]= {
        {4},
        {6},
        {7},
        {7},
        {7},
        {7},
        {7},
        {6},
        {4}
    };
    for(int i=0; i<9; i++)
        n.setColumnInfo(i, columns[i]);
    InfoListType rows[9]= {
        {2,2},
        {4,4},
        {9},
        {9},
        {9},
        {7},
        {5},
        {3},
        {1}
    };
    for(int i=0; i<9; i++)
        n.setRowInfo(i, rows[i]);
}

void TestNonogram::initCarNonogram(Nonogram& n){
    //http://www.nonograms.ru/files/nonograms/large/dzhip_12_1_1p.png
    n.init(25,20);
    InfoListType columns[25]= {
        {3},
        {5,4},
        {2,1,6},
        {1,4,3},
        {1,3,2},
        //------
        {1,3,2},
        {1,4,3},
        {1,1,6},
        {1,2,4},
        {4,2},
        //------
        {2,3,3},
        {2,2,1,2},
        {2,1,1,2},
        {2,2,2},
        {2,2,3},
        //------
        {2,5,2},
        {9,2,4},
        {2,1,1,6},
        {2,2,4,3},
        {2,2,3,2},
        //------
        {3,4,3,2},
        {14,3},
        {4,2,6},
        {8,2,4},
        {8,2}
    };
    for(int i=0; i<25; i++)
        n.setColumnInfo(i, columns[i]);

    InfoListType rows[20]={
        {9},
        {11},
        {1,1,2,2},
        {1,1,1,2},
        {1,1,4},
        //------
        {1,1,2,5},
        {1,2,2,5},
        {9,2,7},
        {2,2,9,2},
        {1,6,1,2},
        //------
        {1,2},
        {9,8},
        {2,4,2,2,4,2},
        {1,6,2,2,6,1},
        {3,3,5,3,3},
        //------
        {2,2,5,2,2},
        {2,2,2,2},
        {3,3,3,3},
        {6,6},
        {4,4}
    };
    for(int i=0; i<20; i++)
        n.setRowInfo(i, rows[i]);
}

void TestNonogram::builtinNonograms(){
    Nonogram heart;
    initHeartNonogram(heart);
    QVERIFY(heart.isSolveable());

    Nonogram car;
    initCarNonogram(car);
    QVERIFY(car.isSolveable());
}

void TestNonogram::fillRandomData(Nonogram* n){
    int width=n->width(), height=n->height();
    for(int r=0; r<height; r++)
        for(int c=0; c<width; c++)
            n->setData(r, c, (Nonogram::CellStatus)(rand()%3));
}

void TestNonogram::solve(){
    Nonogram heart;
    initHeartNonogram(heart);
    QVERIFY(heart.solve());
    Nonogram car;
    initCarNonogram(car);
    QVERIFY(car.solve());
}

void TestNonogram::saveLoad(){
    Nonogram n;
    initCarNonogram(n);
    fillRandomData(&n);

    QByteArray storage(10240, 0);
    QDataStream writestream(&storage, QIODevice::WriteOnly);
    writestream << n;
    QDataStream readstream(&storage, QIODevice::ReadOnly);
    Nonogram m;
    readstream >> m;
    QCOMPARE(m, n);
}

#endif //QT_DEBUG
