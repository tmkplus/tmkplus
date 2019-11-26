#ifndef RESULTSDIALOG_H
#define RESULTSDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class ResultsDialog;
}

class ResultsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsDialog(QWidget *parent = 0, QString whereToSaveScreenshot = "");
    ~ResultsDialog();
    QString screenshotPath;
    void SetTextBoxText(QString text);

private slots:
    void on_pushButton_clicked();
    void closeEvent(QCloseEvent * cev);

private:
    Ui::ResultsDialog *ui;
};

#endif // RESULTSDIALOG_H
