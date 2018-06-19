#include "ashurawin.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AshuraWin w;
    w.show();

    return a.exec();
}
