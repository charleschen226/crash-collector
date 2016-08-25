#include "mainwindow.h"
#include "CrashHandle.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    CrashHandle crashHandle;

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
