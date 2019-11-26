#ifndef INTRODIALOG_H
#define INTRODIALOG_H

#include <QDialog>
#include <tmktranslate.h>

namespace Ui {
class IntroDialog;
}

class IntroDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IntroDialog(QWidget *parent = 0);
    ~IntroDialog();
    bool createNewTest;
    QString testPath;
    QString testName;
    int variantsCount;

private slots:
    void on_radioButton_2_clicked();

    void on_radioButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::IntroDialog *ui;
    QString historyFile;
};

#endif // INTRODIALOG_H
