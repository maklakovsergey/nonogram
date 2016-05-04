#include "test.h"
#include <QtTest/QTest>
#include "testnonogram.h"
#include "testsolve.h"

void runTests(){
    TestNonogram testNonogram;
    QTest::qExec(&testNonogram);
    TestSolve testSolve;
    QTest::qExec(&testSolve);
}

Nonogram heartNonogram(){
    //https://upload.wikimedia.org/wikipedia/ru/a/ad/Step4.png
    Nonogram n(9,9);
    LineInfoType columns[9]= {
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
    LineInfoType rows[9]= {
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
    return n;
}

Nonogram carNonogram(){
    //http://www.nonograms.ru/files/nonograms/large/dzhip_12_1_1p.png
    Nonogram n(25,20);
    LineInfoType columns[25]= {
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

    LineInfoType rows[20]={
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
    return n;
}

Nonogram clefNonogram(){
    //http://en.japonskie.ru/crossword/skripichniy_klyuch1
    Nonogram n(9,20);
    LineInfoType columns[9]= {
        {5},
        {7},
        {2,2},
        {1,2,1,1},
        {20},
        //------
        {3,1,1,1},
        {5,2,2},
        {5},
        {3},
    };
    for(int i=0; i<9; i++)
        n.setColumnInfo(i, columns[i]);

    LineInfoType rows[20]={
        {1},
        {2},
        {3},
        {3},
        {1,1},
        //------
        {1,1},
        {1,1},
        {2},
        {3},
        {2,1},
        //------
        {2,3},
        {2,2,2},
        {2,2,2},
        {2,1,2},
        {2,1,2},
        //------
        {2,1,2},
        {5},
        {1},
        {1},
        {2},
    };
    for(int i=0; i<20; i++)
        n.setRowInfo(i, rows[i]);

    return n;
}

