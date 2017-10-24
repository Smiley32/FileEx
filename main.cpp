#include "fileex.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!createConnection()) {
        return 1;
    }
    FileEx w;
    w.show();

    return a.exec();
}
