#include "arborescence.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    arborescence w;
    w.show();
    return a.exec();
}
