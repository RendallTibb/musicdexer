#include <QtGui/QApplication>
#include "musicdexer.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Musicdexer foo;
    foo.show();
    return app.exec();
}
