#include "selecttestdialog.h"
#include "ui_selecttestdialog.h"
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QtNetwork>
#include <QUrl>
#include <csvparser.h>
#include <QFileDialog>
#include <QTextStream>
#include <QInputDialog>
#include <QStringList>
#include <exception>
#include <settingsdialog.h>
#ifdef QT5_BUILD
#include <QStandardPaths>
#endif
#include <tmktranslate.h>
using namespace std;

SelectTestDialog::SelectTestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectTestDialog)
{
    ui->setupUi(this);
    ui->label->setText(tmktr("Недавние тесты"));
    ui->pushButton_4->setText(tmktr("Скачать тест из Интернета"));
    ui->pushButton_3->setText(tmktr("Настройки"));
    ui->pushButton_2->setText(tmktr("Закрыть"));
    ui->pushButton->setText(tmktr("Открыть"));
    QFont modified_font = this->font();
    int size_of_font = 12;
#ifdef Q_OS_ANDROID
    size_of_font = 18;
    ui->pushButton_2->resize(131, 41);
    ui->pushButton_3->resize(131, 41);
    ui->pushButton_4->resize(261, 41);
    ui->pushButton->resize(ui->pushButton->geometry().width(), 41);
#endif
    modified_font.setPixelSize(size_of_font);
    this->setFont(modified_font);
    download_from_internet = false;
    selectedItem = "";
    HideButtons();
    QString historyFile = "";
    userHomeDir = QDir::homePath();
#ifdef Q_OS_ANDROID
    userHomeDir = "/storage/emulated/0";
#endif
    if (QFile::exists(userHomeDir + "/.konstrqt_history"))
        historyFile = userHomeDir + "/.konstrqt_history";
    else if (QFile::exists(userHomeDir + "/konstrqt_history.dat"))
        historyFile = userHomeDir + "/konstrqt_history.dat";

    configFile = historyFile;
    if (historyFile != "") {
        QStringList first_five_lines;
        QFile file(historyFile);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            for (int i = 0; i < 5; i++) {
                try {
                    QString line = stream.readLine();
                    first_five_lines.append(line);
                }
                catch (...) {
                    first_five_lines.append("");
                }
                //free(line);
            }
        }
        else
            HideButtons();
        file.close();
        try {
            for (int j = 4; j > -1; j = j - 1) {
               if (QFile::exists(first_five_lines[j]) || first_five_lines[j].contains("://")) {
                    QString text_to_print = first_five_lines[j];
                    try {
                        CSVParser parser(text_to_print);
                        if (parser.ReadFile() == 1)
                            throw std::exception();
                        QString val = parser.GetValue(1, 1);
                        if (val.toLower() != "варстолб" && val.toLower() != "konstr_qt")
                            text_to_print = val + " (" + text_to_print + ")";
                    }
                    catch (...) {
                        text_to_print = first_five_lines[j];
                    }
                    ShowButtonWithText(j + 1, text_to_print);
                }
            }
            filesToOpen = first_five_lines;
        }
        catch (...) {
            HideButtons();
        }
    }
    else {
#ifdef WINDOWS_10
        historyFile = userHomeDir + "/konstrqt_history.dat";
#else
        historyFile = userHomeDir + "/.konstrqt_history";
#endif
        configFile = historyFile;
        HideButtons();
    }
   // QMessageBox::information(this, "Information", historyFile, QMessageBox::Ok);
}

SelectTestDialog::~SelectTestDialog()
{
    delete ui;
}

void SelectTestDialog::ShowButtonWithText(int button, QString text) {
    if (ui->commandLinkButton->text() == text || ui->commandLinkButton_2->text() == text || ui->commandLinkButton_3->text() == text || ui->commandLinkButton_4->text() == text || ui->commandLinkButton_5->text() == text)
        return;
    if (button == 1) {
        ui->commandLinkButton->show();
        ui->commandLinkButton->setText(text);
    }
    else if (button == 2) {
        ui->commandLinkButton_2->show();
        ui->commandLinkButton_2->setText(text);
    }
    else if (button == 3) {
        ui->commandLinkButton_3->show();
        ui->commandLinkButton_3->setText(text);
    }
    else if (button == 4) {
        ui->commandLinkButton_4->show();
        ui->commandLinkButton_4->setText(text);
    }
    else if (button == 5) {
        ui->commandLinkButton_5->show();
        ui->commandLinkButton_5->setText(text);
    }
    else
        return;
}

void SelectTestDialog::HideButtons() {
    ui->commandLinkButton->hide();
    ui->commandLinkButton_2->hide();
    ui->commandLinkButton_3->hide();
    ui->commandLinkButton_4->hide();
    ui->commandLinkButton_5->hide();
}

void SelectTestDialog::on_commandLinkButton_clicked()
{
    QString filename = filesToOpen.first();
    ButtonHandlerFunction(filename);
}

void SelectTestDialog::ButtonHandlerFunction(QString filename) {
    if (QFile::exists(filename) || filename.contains("http://") || filename.contains("https://") || filename.contains("ftp://")) {
        selectedItem = filename;
        QFile out_file(configFile);
        //QMessageBox::information(this, "Info", out_file.fileName(), QMessageBox::Ok);
        QString to_write = filename;
        if (out_file.exists() == true)
            to_write = to_write + "\n" + QString(out_file.readAll());
        if (out_file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&out_file);
            stream << to_write;
        }
        out_file.close();
        if (filename.contains("http://") || filename.contains("https://") || filename.contains("ftp://"))
            download_from_internet = true;
        this->close();
    }
    else
        QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Не могу найти CSV-файл ") + filename + ".", QMessageBox::Ok);
}

QString SelectTestDialog::GetSelectedTestFile() {
    return selectedItem;
}

void SelectTestDialog::on_commandLinkButton_2_clicked()
{
    QString filename = filesToOpen.at(1);
    ButtonHandlerFunction(filename);
}

void SelectTestDialog::on_commandLinkButton_3_clicked()
{
    QString filename = filesToOpen.at(2);
    ButtonHandlerFunction(filename);
}

void SelectTestDialog::on_commandLinkButton_4_clicked()
{
    QString filename = filesToOpen.at(3);
    ButtonHandlerFunction(filename);
}

void SelectTestDialog::on_commandLinkButton_5_clicked()
{
    QString filename = filesToOpen.at(4);
    ButtonHandlerFunction(filename);
}

void SelectTestDialog::on_pushButton_clicked()
{
    QString filename_to_load = QFileDialog::getOpenFileName(this, tmktr("Открыть CSV-файл с заданиями"), userHomeDir, "CSV-файлы с заданиями (*.csv);;Все файлы (*)");
    if (filename_to_load.isEmpty())
        return;
    else
        ButtonHandlerFunction(filename_to_load);

}

void SelectTestDialog::on_pushButton_2_clicked()
{
    exit(0);
}

void SelectTestDialog::on_pushButton_3_clicked()
{
    SettingsDialog dialog(this);
    dialog.setFont(this->font());
    dialog.setStyleSheet(this->styleSheet());
    dialog.HideChangePasswordButton();
    dialog.exec();
}

bool SelectTestDialog::isOnlineTest() {
    return download_from_internet;
}

void SelectTestDialog::on_pushButton_4_clicked()
{
    QString downloadUrl = QInputDialog::getText(this, "", tmktr("Введите Интернет-адрес теста, который дал вам учитель."), QLineEdit::Normal, "");
    //QMessageBox::information(this, "", downloadUrl, QMessageBox::Ok);
    if (downloadUrl.contains("http://") || downloadUrl.contains("https://") || downloadUrl.contains("ftp://")) {
        selectedItem = downloadUrl;
        download_from_internet = true;
        ButtonHandlerFunction(downloadUrl);
        this->close();
    }
    else
        QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Неправильный Интернет-адрес теста."), QMessageBox::Ok);
}
