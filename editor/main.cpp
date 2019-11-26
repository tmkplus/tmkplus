#include "mainwindow.h"
#include <QString>
#include <QApplication>
#include <signal.h>
using namespace std;

int main(int argc, char *argv[])
{
#ifdef USE_OLD_RELOADTEST_FUNCTION
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
#else
    int what_to_return = 0;
    QString what_mainwindow_opens = "none";
    int mainwindow_width = 669;
    int mainwindow_height = 441;
    do {
        try {
            QApplication a(argc, argv);
            MainWindow w(0, what_mainwindow_opens);
            w.resize(mainwindow_width, mainwindow_height);
            w.showMaximized();
            what_to_return = a.exec();
            what_mainwindow_opens = w.currentTest;
            mainwindow_width = w.width();
            mainwindow_height = w.height();
        }
        catch (...) {

        }
    } while (what_to_return == 8745);
    return 0;
#endif
}

