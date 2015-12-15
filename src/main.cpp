#include <QApplication>
#include <QtTest/QTest>
#include "mainwindow.h"
#include "testnonogram.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef QT_DEBUG
    TestNonogram test;
    QTest::qExec(&test);
#endif
    MainWindow w;
    if (a.arguments().count()>1)
        w.open(a.arguments()[1]);
    a.installEventFilter(&w);
    w.show();

    return a.exec();
}
