#include <QApplication>
#include "test/test.h"
#include "UI/mainwindow.h"
#include "test/testnonogram.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
#ifdef QT_DEBUG
    runTests();
#endif
    MainWindow window;
    if (application.arguments().count()>1)
        window.open(application.arguments()[1]);
    application.installEventFilter(&window);
    window.show();

    return application.exec();
}
