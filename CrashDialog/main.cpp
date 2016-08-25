#include "CrashDialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CrashDialog w(argv[1], argv[2]);
    w.show();

    return a.exec();
}
