#ifndef QT5_BUILD
# include <QtGui/QApplication>
#else
# include <QtWidgets>
#endif
#include <selecttestdialog.h>
#include "mainwindow.h"
#include <QStringList>
#include <QString>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList cmd_line_args = a.arguments();
    bool enable_csv = false;
    QString csv_arg;
    for (int i = 0; i < cmd_line_args.count(); i++) {
        if (cmd_line_args[i] == "-testfile" || cmd_line_args[i] == "--testfile" || cmd_line_args[i] == "-t") {
            csv_arg = cmd_line_args[i+1];
            enable_csv = true;
            i = cmd_line_args.count();
        }
    }
        MainWindow w;
        if (enable_csv == true)
            w.SetCSVConstrValue(csv_arg);
        else {
            //printf("Как открыть Конструктор тестов?\n");
            //printf("konstr_qt -testfile [путь к CSV-файлу с заданиями теста]\n");
            //return 0;
            SelectTestDialog testDialog;
            testDialog.setFont(w.font());
            testDialog.setStyleSheet(w.styleSheet());
            testDialog.exec();
            QString csv_path = testDialog.GetSelectedTestFile();
            if (csv_path == "")
                return 0;
            else {
                w.NeedToDownloadTest(testDialog.isOnlineTest());
                w.SetCSVConstrValue(csv_path);
            }
        }

        w.show();
    return a.exec();
}
