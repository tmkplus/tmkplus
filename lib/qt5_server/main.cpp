#include <QCoreApplication>
#include <server-components/QtHTTPServer.h>
#include <QStringList>
#include <QString>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();
    QString baseDir = args[1];
    QString testName = args[2];
    //std::cout << testName.toStdString() << "\n";
    QtHTTPServer server(0, baseDir, testName);
    server.run();
    return a.exec();
}
