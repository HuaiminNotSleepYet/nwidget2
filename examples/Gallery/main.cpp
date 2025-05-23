#include "gallery.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Gallery w;
    w.show();
    return a.exec();
}
