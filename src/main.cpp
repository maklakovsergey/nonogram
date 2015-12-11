#include <QApplication>
#include <QTest>
#include "mainwindow.h"
#include "testnonogram.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTest::qExec(new TestNonogram);
    MainWindow w;
    if (a.arguments().count()>1)
        w.open(a.arguments()[1]);
    a.installEventFilter(&w);
    w.show();

    return a.exec();
}
