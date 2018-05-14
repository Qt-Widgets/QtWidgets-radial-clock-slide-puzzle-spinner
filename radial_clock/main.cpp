#include "dialog.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    std::cout << std::getenv("QTDIR") << std::endl;

    return a.exec();
}
