#include "introdialog.h"
#include "ui_introdialog.h"
#include <QTextCodec>
#include <QDir>
#include <QInputDialog>
#include <QFileDialog>

IntroDialog::IntroDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IntroDialog)
{
    ui->setupUi(this);
#ifndef QT5_BUILD
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
    ui->label->setText(tmktr("Добро пожаловать в конструктор тестов TestMaker Plus!"));
    ui->radioButton->setText(tmktr("Создать новый тест"));
    ui->radioButton_2->setText(tmktr("Открыть существующий тест"));
    ui->pushButton_3->setText(tmktr("Открыть"));
    ui->pushButton_2->setText(tmktr("Закрыть"));
    ui->pushButton->setText(tmktr("Продолжить"));
    createNewTest = true;
    historyFile = QDir::homePath() + "/.konstrqt_history";
    if (QFile::exists(historyFile) == false)
        historyFile = QDir::homePath() + "/konstrqt_history.dat";
    if (QFile::exists(historyFile) == false)
#ifdef WINDOWS_10
        historyFile = QDir::homePath() + "/konstrqt_history.dat";
#else
        historyFile = QDir::homePath() + "/.konstrqt_history";
#endif
}

IntroDialog::~IntroDialog()
{
    delete ui;
}

void IntroDialog::on_radioButton_2_clicked()
{
    if (this->ui->radioButton_2->isChecked() == true) {
        this->ui->listWidget->clear();
        this->ui->listWidget->setEnabled(true);
        this->ui->pushButton_3->setEnabled(true);
        try {
            QFile file(historyFile);
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            QString fileContents = file.readAll();
            file.close();
            QStringList fileContents2 = fileContents.replace("\r\n", "\n").split("\n");
            for (int i = 0; i < fileContents2.length(); i++) {
                if (QFile::exists(fileContents2[i]) && (this->ui->listWidget->findItems(fileContents2[i], Qt::MatchCaseSensitive).length() < 1))
                    this->ui->listWidget->addItem(fileContents2[i]);
            }
        } catch (...) {
            this->ui->listWidget->clear();
        }
    }
    createNewTest = false;
}

void IntroDialog::on_radioButton_clicked()
{
    this->ui->listWidget->clear();
    this->ui->listWidget->setEnabled(false);
    this->ui->pushButton_3->setEnabled(false);
    createNewTest = true;
}

void IntroDialog::on_pushButton_2_clicked()
{
    //std::exit(0);
    exit(0);
}

void IntroDialog::on_pushButton_clicked()
{
    if (createNewTest == true) {
        testName = QInputDialog::getText(this, "TestMaker Plus", tmktr("Введите название теста"));
        if (testName.isEmpty())
            return;
        testPath = QFileDialog::getExistingDirectory(this, tmktr("Выберите папку, в которой будет находиться тест"), QDir::homePath());
        variantsCount = QInputDialog::getInt(this, "TestMaker Plus", tmktr("Введите количество вариантов КАЖДОГО вопроса"), 20, 1, 60);
        if (testPath.isEmpty() || variantsCount < 1)
            return;
    }
    else {
        testName = this->ui->listWidget->item(this->ui->listWidget->currentRow())->text();
        testPath = testName;
        variantsCount = -1;
    }
    this->close();
}

void IntroDialog::on_pushButton_3_clicked()
{
    QString to_add = QFileDialog::getOpenFileName(this, tmktr("Открыть CSV-файл с заданиями"), QDir::homePath(), "Файлы TestMaker Plus (*.csv)");
    this->ui->listWidget->addItem(to_add);
    this->ui->listWidget->setCurrentRow(this->ui->listWidget->count() - 1);
}
