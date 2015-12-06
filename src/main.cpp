#include <QApplication>
#include <QTest>
#include "mainwindow.h"
#include "testnonogram.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTest::qExec(new TestNonogram, argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
