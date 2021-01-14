#include "AnimeHub.h"

#include <QApplication>

// Documentation in .cpp files

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AnimeHub w;
    w.show();
    return a.exec();
}
