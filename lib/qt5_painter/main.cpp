#include <QApplication>

#include "qt5paint.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
#ifdef STANDALONE_BUILD
    QString openPath = "/usr/share/icons/kde_section.png";
    QString savePath = "";
    bool sco = true;
#endif
    Qt5Paint window(openPath, savePath, sco);
    window.show();
    return app.exec();
}
