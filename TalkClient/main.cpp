#include <QApplication>
#include "clientwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ClientWindow win;
    win.show();
    return app.exec();
}
