#include "testsolve.h"
#include "nonogramsolver.h"
#include "test.h"
#include <QtTest/QTest>

TestSolve::TestSolve(QObject *parent) : QObject(parent)
{

}

void TestSolve::solve(){
    NonogramSolver solver;
    Nonogram heart=heartNonogram();
    QVERIFY(heart.isSolveable());
    QVERIFY(solver.solve(&heart));

    Nonogram car=carNonogram();
    QVERIFY(car.isSolveable());
    QVERIFY(solver.solve(&car));

    Nonogram clef=clefNonogram();
    QVERIFY(clef.isSolveable());
    QVERIFY(solver.solve(&clef));
}
