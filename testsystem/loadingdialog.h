#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include <QDialog>

namespace Ui {
class LoadingDialog;
}

class LoadingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadingDialog(QWidget *parent = 0);
    ~LoadingDialog();
    void ChangeProgressBarValue(int val);
    void SetLabelText(QString text);

private slots:
    void on_progressBar_valueChanged(int value);

private:
    Ui::LoadingDialog *ui;
};

#endif // LOADINGDIALOG_H
