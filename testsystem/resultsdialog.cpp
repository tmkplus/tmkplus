#include <QFile>
#include "resultsdialog.h"
#include "ui_resultsdialog.h"
//#include <QScreen>
#include <QMessageBox>
#include <QTextStream>
#include <tmktranslate.h>

ResultsDialog::ResultsDialog(QWidget *parent, QString whereToSaveScreenshot) :
    QDialog(parent),
    ui(new Ui::ResultsDialog)
{
    screenshotPath = whereToSaveScreenshot.replace(".png", ".txt");
    if (QFile::exists(screenshotPath)) {
        try {
            QFile::remove(screenshotPath);
        }
        catch (...) {
            screenshotPath = screenshotPath.replace(".png", QString::number(qrand()) + ".png");
        }
    }
    ui->setupUi(this);
    ui->pushButton->setText(tmktr("Закрыть"));
}

void ResultsDialog::SetTextBoxText(QString text) {
    ui->plainTextEdit->setPlainText(text);
    return;
}

ResultsDialog::~ResultsDialog()
{
    delete ui;
}

void ResultsDialog::on_pushButton_clicked()
{
    try {
        QFile file(screenshotPath);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        QTextStream stream(&file);
        stream << ui->plainTextEdit->toPlainText().replace("\r\n", "\n").replace("\n", "\r\n");
    } catch (...) {
        QMessageBox::critical(this, tmktr("Ошибка"), "Access is denied.", QMessageBox::Ok);
    }
    this->close();
}

void ResultsDialog::closeEvent(QCloseEvent *cev) {
    cev->accept();
    on_pushButton_clicked();
}
