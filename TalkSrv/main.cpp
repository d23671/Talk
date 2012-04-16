#include <QApplication>
#include "serverwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ServerWindow win;
    win.show();
    return app.exec();
}
