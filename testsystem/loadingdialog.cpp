#include "loadingdialog.h"
#include "ui_loadingdialog.h"

LoadingDialog::LoadingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadingDialog)
{
    ui->setupUi(this);
}

LoadingDialog::~LoadingDialog()
{
    delete ui;
}

void LoadingDialog::ChangeProgressBarValue(int val) {
    if (val > ui->progressBar->value())
        ui->progressBar->setValue(ui->progressBar->value() + val);
    else
        return;
}

void LoadingDialog::SetLabelText(QString text) {
    ui->label->setText(text);
}

void LoadingDialog::on_progressBar_valueChanged(int value)
{
    if (value > 89)
        ui->label->setText("Почти готово");
}
