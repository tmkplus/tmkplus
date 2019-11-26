#include "mainwindow.h"
#include <cstdlib>
#include <signal.h>
#include "ui_mainwindow.h"
#include "introdialog.h"
#include <QMessageBox>
#include "csvparser.h"
#include <exception>
#include "newquestiondialog.h"
#include <QDir>
#include <QImage>
#include <QFileInfo>
#include <QTextStream>
#include <QTextCodec>
#include <QPixmap>
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <QTime>
#ifdef INCLUDE_QT5PAINT
#include <qt5paint.h>
#endif
#ifdef QT5_BUILD
#include <QtConcurrent>
#include <QtConcurrentRun>
#include <QFuture>
#endif
#ifdef USE_BUNDLED_SERVER
# ifndef STANDALONE_BINARY_SERVER
#include <QtHTTPServer.h>
# endif
#endif
#include <QFileDialog>
#include <QProcess>
#include <QList>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QHostAddress>
using namespace std;

MainWindow::MainWindow(QWidget *parent, QString the_test) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->action_2->setText(tmktr("Закрыть"));
    ui->menu->setTitle(tmktr("Файл"));
    ui->menu_2->setTitle(tmktr("Помощь"));
    ui->action_4->setText(tmktr("О программе"));
    ui->action_Qt->setText(tmktr("О библиотеке Qt"));
    ui->pushButton->setText(tmktr("Сохранить тест"));
    ui->pushButton_2->setText(tmktr("Добавить вопрос"));
    ui->pushButton_3->setText(tmktr("Удалить вопрос"));
    ui->pushButton_4->setText(tmktr("Опубликовать"));
    ui->label_7->setText(tmktr("Добро пожаловать!"));
    ui->label->setText(tmktr("Вариант"));
    ui->label_2->setText(ui->label_2->text().replace("из", tmktr("из")));
    ui->pushButton_5->setText(tmktr("Создать картинку"));
    ui->pushButton_8->setText(tmktr("Выбрать"));
    ui->label_4->setText(tmktr("Картинка вопроса:"));
    ui->label_6->setText(tmktr("правильный ответ:"));
    QMainWindow::showMaximized();
#ifndef QT5_BUILD
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

    reloadTheSameThing = false;
    IntroDialog dialog;
    saveReal = false;
    if (the_test == "none")
        dialog.exec();
    this->ui->spinBox->setEnabled(false);
#ifndef Q_OS_UNIX
    this->ui->pushButton_4->setEnabled(false);
#endif
    editingQuestion = false;
    allowWaitSecs = true;
    justSaved = false;
    changedSomething = false;
    preparingForFakeCrash = false;
    this->ui->pushButton_6->setEnabled(false);
    this->ui->pushButton_7->setEnabled(false);
    allowSaveQuestion = true;
    this->setWindowTitle("TestMaker Plus");
    if (the_test == "none")
        currentTest = dialog.testPath;
    else
        currentTest = the_test;
    if (currentTest.contains(".csv") == false) {
        mediaDirectory = currentTest + "/media";
        currentTest = currentTest + "/test.csv";
    }
    questionsCount = 0;
    if (the_test == "none")
        currentTestName = dialog.testName;
    else
        currentTestName = the_test;
    //this->ui->pushButton_9->setEnabled(false);
    this->ui->pushButton_8->setEnabled(false);
    if (the_test == "none")
       currentTestVariants = dialog.variantsCount;
    else
       currentTestVariants = -1;
    if (currentTestVariants == -1) {
        try {
            CSVParser parser_tmp(currentTest);
            if (parser_tmp.ReadFile() != 0)
                throw std::exception();
            currentTestName = parser_tmp.GetValue(1, 1);
            currentTestVariants = parser_tmp.GetAllValues().at(0).length() - 2;
            QString tmp_base_directory = parser_tmp.GetBaseDirectory();
            if (QDir(tmp_base_directory + "/media").exists())
                mediaDirectory = tmp_base_directory + "/media";
            else if (QDir(tmp_base_directory + "/Images_m").exists())
                mediaDirectory = tmp_base_directory + "/Images_m";
            else if (QDir(tmp_base_directory + "/Image_m").exists())
                mediaDirectory = tmp_base_directory + "/Image_m";
            else if (QDir(tmp_base_directory + "/pictures").exists())
                mediaDirectory = tmp_base_directory + "/pictures";
            else {
                QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Не могу найти сами задания теста."), QMessageBox::Ok);
                std::exit(1);
            }
            questionsCount = parser_tmp.GetQuestionsCount();
            outCSVFile = parser_tmp.GetAllValues();
            this->ui->label_2->setText(tmktr("из") + " " + QString::number(currentTestVariants));
            //delete parser_tmp;
        } catch (...) {
            QMessageBox::critical(this, tmktr("Ошибка"), "Не могу посчитать кол-во вариантов каждого вопроса в тесте.", QMessageBox::Ok);
            exit(1);
        }
    }
    else
        this->ui->label_2->setText("из " + QString::number(currentTestVariants));
    this->setWindowTitle(currentTestName + " - TestMaker Plus");
    this->ui->listWidget->clear();
    for (int i = 0; i < questionsCount; i++) {
        this->ui->listWidget->addItem(tmktr("Вопрос") + " №" + QString::number(i + 1));
    }
    if (outCSVFile.length() < 2) {
        QStringList to_add;
        to_add.append(currentTestName);
        for (int j = 0; j < currentTestVariants; j++)
            to_add.append(QString::number(j + 1));
        to_add.append(tmktr("баллы"));
        outCSVFile.append(to_add);
        NewQuestionDialog dialog(this);
        dialog.exec();
        this->ui->label_7->setStyleSheet("color: steelblue;");
        this->ui->label_7->setText(tmktr("Теперь создайте новую картинку вопроса или выберите существующую."));
        int question_points = dialog.QuestionPoints;
        questionsCount = questionsCount + 1;
        if (question_points < 1)
            std::exit(1);
        else {
            QStringList to_add2;
            to_add2.append(QString::number(questionsCount));
            for (int l = 0; l < currentTestVariants; l++)
                to_add2.append("");
            to_add2.append(QString::number(questionsCount));
            outCSVFile.append(to_add2);
            this->ui->listWidget->addItem(tmktr("Вопрос") + " №" + QString::number(questionsCount));
            this->showMaximized();
            this->ui->listWidget->setCurrentRow(this->ui->listWidget->count() - 1);
            EditQuestion(1, 1);
        }
    }
    else
         EditQuestion(1, 1);
#ifdef TMKEDIT_DARKTHEME
    std::cout << "TMKEDIT_DARKTHEME was defined during compile time" << std::endl;
    QString settingsFileKonstrQt = QDir::homePath() + "/.konstrqt_settings";
#  ifdef Q_OS_WIN
    settingsFileKonstrQt = QDir::homePath() + "/konstrqt_settings.dat";
#  endif
    bool needDarkTheme = false;
    if (QFile::exists(settingsFileKonstrQt)) {
	QFile* qfSettings = new QFile(settingsFileKonstrQt);
	if (qfSettings->open(QIODevice::Text | QIODevice::ReadOnly) == true)
		needDarkTheme = (QString(qfSettings->readAll()).toLower().contains("darkthemeqt=true"));
	qfSettings->close();
	delete qfSettings;
    }
    if (needDarkTheme == true)
	this->setStyleSheet("background-color: dimgray; color: white;");
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::EditQuestion(int question, int variant) {
    changedSomething = false;
    this->ui->listWidget->setCurrentRow(question - 1);
    //this->ui->pushButton_9->setEnabled(true);
    this->ui->pushButton_8->setEnabled(true);
    this->ui->pushButton_6->setEnabled(true);
    this->ui->pushButton_7->setEnabled(true);
    this->ui->spinBox->setValue(variant);
    QStringList to_replace = outCSVFile[question];
    QString currentAnswer = to_replace[variant];
    this->ui->lineEdit->setText(currentAnswer.replace("\"", "").replace("$QUOTES$", "\""));
    changedSomething = false;
    QString loadImage = mediaDirectory + "/kart" + QString::number(question) + "_" + QString::number(variant) + ".png";
    try {
        if (QFile::exists(loadImage)) {
            this->ui->label_3->setIcon(QIcon(QPixmap::fromImage(QImage(loadImage))));
            this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
            //this->ui->label_3->setPixmap(QPixmap::fromImage(QImage(loadImage)).scaled(this->ui->label_3->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            this->ui->label_5->setText(loadImage);
        }
        else {
            throw std::exception();

        }
    } catch (...) {
        this->ui->label_5->setText(tmktr("Не выбрана"));
    }
    editingQuestion = true;
    while (editingQuestion == true) {
        //WaitSecs(5, true);
        QApplication::processEvents(QEventLoop::WaitForMoreEvents, 5);
        if (editingQuestion == false)
            break;
    };
    //QMessageBox::information(this, "Info", "\"reached if (allowSaveQuestion == true);\"\nbool allowWaitSecs = " + QString(allowWaitSecs), QMessageBox::Ok);
    changedSomething = false;
}

void MainWindow::WaitSecs(int seconds, bool useMilliseconds) {
    if (useMilliseconds == true)
        QApplication::processEvents(QEventLoop::WaitForMoreEvents, seconds);
    else
        QApplication::processEvents(QEventLoop::WaitForMoreEvents, seconds * 1000);
    qDebug() << "MainWindow::WaitSecs(" << seconds << ", " << useMilliseconds << ");";
    return;
}

int MainWindow::on_pushButton_9_clicked()
{
    allowSaveQuestion = true;
    int to_ret = 5;
    if (editingQuestion == true) {
        if (this->ui->label_5->text().toLower() == tmktr("Не выбрана").toLower())
            QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Нужно выбрать картинку с заданием вопроса."), QMessageBox::Ok);
        else if (this->ui->lineEdit->text() == "")
            QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Введите правильный ответ к вопросу в белое поле для ввода текста."), QMessageBox::Ok);
        else {
            //this->ui->pushButton_9->setDisabled(true);
            this->ui->label_7->setStyleSheet("color: forestgreen;");
            this->ui->label_7->setText(tmktr("Сохранение вопроса..."));
            justSaved = true;
            int question = this->ui->listWidget->currentRow() + 1;
            int variant = this->ui->spinBox->value();
            QString loadImage = mediaDirectory + "/kart" + QString::number(question) + "_" + QString::number(variant) + ".png";
            QStringList to_replace = outCSVFile[question];
            if (allowSaveQuestion == true) {
                to_replace[variant] = "\"" + this->ui->lineEdit->text().replace("\"", "$QUOTES$") + "\"";
                if (this->ui->label_5->text().toLower() != loadImage.toLower()) {
                    if (QDir(mediaDirectory).exists() == false)
                        QDir().mkdir(mediaDirectory);
                    QString path1 = this->ui->label_5->text();
                    QString path2 = loadImage;
                    //QMessageBox::information(this, "", path1 + " " + path2, QMessageBox::Ok);
                    if (QFile::exists(path2))
                        QFile::remove(path2);
                    QPixmap pixmap(path1);
                    pixmap.save(path2, "PNG");
                }
            }
            outCSVFile[question] = to_replace;
            allowWaitSecs = false;
            changedSomething = false;
            editingQuestion = false;
            allowWaitSecs = true;
            QApplication::processEvents(QEventLoop::WaitForMoreEvents, 1);
            this->ui->pushButton->setEnabled(true);
            saveReal = true;
            if (saveReal == false) {
                //int result = QMessageBox::question(this, "Внимание", "Продолжить редактировать этот вопрос?", QMessageBox::Yes, QMessageBox::No);
                //if (result == QMessageBox::Yes) {
                    //this->ui->pushButton_9->setEnabled(true);
                    //HandleSelection(0, 0);
                    //HandleSelection(question, variant);
                    //EditQuestion(this->ui->listWidget->currentRow(), this->ui->spinBox->value());
                    to_ret = 4;
                //}
                //else {
                    /*
                    this->ui->pushButton_9->setEnabled(false);
                    this->ui->pushButton_8->setEnabled(false);
                    this->ui->listWidget->selectionModel()->clear();
                    this->ui->label_3->setIcon(QIcon(QPixmap()));
                    this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
                    this->ui->lineEdit->setText("");
                    this->ui->pushButton_6->setEnabled(false);
                    this->ui->pushButton_7->setEnabled(false);
                    this->ui->label_5->setText("Не выбрана");
                    to_ret = 0;
                }
                */
            }
            else {
                //this->ui->pushButton_9->setEnabled(false);
                this->ui->pushButton_8->setEnabled(false);
                this->ui->listWidget->selectionModel()->clear();
                this->ui->label_3->setIcon(QIcon(QPixmap()));
                this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
                this->ui->lineEdit->setText("");
                this->ui->pushButton_6->setEnabled(false);
                this->ui->pushButton_7->setEnabled(false);
                this->ui->label_5->setText(tmktr("Не выбрана"));
                to_ret = 0;
            }

        }
    }
    else
	to_ret = 0;
    allowSaveQuestion = true;
    justSaved = true;
    return to_ret;
}

void MainWindow::on_pushButton_8_clicked()
{
    changedSomething = true;
    QString selected = QFileDialog::getOpenFileName(this, tmktr("Выберите картинку"), QDir::homePath());
    if (selected != "") {
        this->ui->pushButton->setEnabled(true);
        this->ui->label_7->setStyleSheet("color: steelblue;");
        this->ui->label_7->setText(tmktr("Нажмите на картинку, чтобы отредактировать ее."));
        this->ui->label_5->setText(selected);
        this->ui->label_3->setIcon(QIcon(QPixmap::fromImage(QImage(selected))));
        this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
        //this->ui->label_3->setPixmap(QPixmap::fromImage(QImage(selected)).scaled(this->ui->label_3->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MainWindow::on_listWidget_itemSelectionChanged()
{

}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    justSaved = false;
    HandleSelection(currentRow + 1, 1);
}

void MainWindow::resizeEvent(QResizeEvent* resize) {
    QMainWindow::resizeEvent(resize);
    this->ui->listWidget->resize(this->ui->listWidget->width(), this->size().height() - 90);
    this->ui->label_5->resize(this->size().width() - 388, this->ui->label_5->height());
    this->ui->lineEdit->resize(this->ui->label_5->size().width() - 10, this->ui->lineEdit->height());
    this->ui->pushButton_8->move(this->ui->label_5->x() + this->ui->label_5->width() - this->ui->pushButton_8->width() - 1, this->ui->label_5->y());
    this->ui->frame->resize(this->size().width() - (this->ui->pushButton->width() + this->ui->pushButton_2->width() + this->ui->pushButton_3->width() + this->ui->pushButton_4->width()), 31);
    this->ui->label_7->resize(this->ui->frame->width(), this->ui->frame->height());
    //this->ui->pushButton_9->move(this->ui->pushButton_8->x() - 70, this->ui->listWidget->y() + this->ui->listWidget->height() - this->ui->pushButton_9->height());
    this->ui->label_6->move(this->ui->label_6->x(), this->ui->listWidget->height() - 21);
    this->ui->lineEdit->move(this->ui->lineEdit->x(), this->ui->label_6->y() - 10);
    this->ui->label_4->move(this->ui->label_4->x(), this->ui->label_6->y() - 40);
    this->ui->label_5->move(this->ui->label_5->x(), this->ui->label_4->y());
    this->ui->pushButton_8->move(this->ui->pushButton_8->x(), this->ui->label_5->y());
    this->ui->label_3->resize(this->size().width() - 248, this->size().height() - 220);
    this->ui->pushButton_5->move(this->ui->pushButton_8->x() - 20, this->ui->pushButton_8->y() - 30);
}

void MainWindow::on_pushButton_6_clicked()
{
    if (this->ui->spinBox->value() > 0 && this->ui->spinBox->value() < currentTestVariants) {
        justSaved = false;
        HandleSelection(this->ui->listWidget->currentRow() + 1, this->ui->spinBox->value() + 1);
        this->ui->spinBox->setValue(this->ui->spinBox->value() + 1);
    }
}

void MainWindow::on_pushButton_7_clicked()
{
    if ((this->ui->spinBox->value() - 1) != 0) {
        justSaved = false;
        HandleSelection(this->ui->listWidget->currentRow() + 1, this->ui->spinBox->value() - 1);
        this->ui->spinBox->setValue(this->ui->spinBox->value() - 1);
    }
}

void MainWindow::HandleSelection(int currentRow, int variant) {
    if (currentRow == variant && variant == 0) {
        allowSaveQuestion = false;
        editingQuestion = false;
        QApplication::processEvents(QEventLoop::WaitForMoreEvents, 100);
        //this->ui->pushButton_9->setEnabled(false);
        this->ui->pushButton_8->setEnabled(false);
        //this->ui->listWidget->selectionModel()->clear();
        //this->ui->label_3->setPixmap(QPixmap());
        this->ui->label_3->setIcon(QIcon());
        this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
        this->ui->listWidget->clearSelection();
        this->ui->lineEdit->setText("");
        this->ui->label_5->setText(tmktr("Не выбрана"));
        allowSaveQuestion = true;
        return;
    }
    if (editingQuestion == true && changedSomething == true) {
        int result = QMessageBox::question(this, tmktr("Внимание"), tmktr("Вы еще не сохранили этот вопрос. Сохранить его?"), QMessageBox::Yes, QMessageBox::No);
        if (result == QMessageBox::Yes) {
# ifdef USE_OBSOLETE_CODE_MAY2018
            allowSaveQuestion = true;
            editingQuestion = false;
            WaitSecs(1, false);
            //this->ui->pushButton_9->setEnabled(false);
            this->ui->pushButton_8->setEnabled(false);
            //this->ui->listWidget->selectionModel()->clear();
            //this->ui->label_3->setPixmap(QPixmap());
            this->ui->label_3->setIcon(QIcon(QPixmap()));
            this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
            this->ui->lineEdit->setText("");
            this->ui->label_5->setText("Не выбрана");
            if (currentRow != 0 && variant != 0)
                EditQuestion(currentRow, variant);
# else
            SaveFile(currentTest);
# endif
        }
        else {
            allowSaveQuestion = false;
            editingQuestion = false;
            QApplication::processEvents(QEventLoop::WaitForMoreEvents, 100);
            //this->ui->pushButton_9->setEnabled(false);
            this->ui->pushButton_8->setEnabled(false);
            //this->ui->listWidget->selectionModel()->clear();
            //this->ui->label_3->setPixmap(QPixmap());
            this->ui->label_3->setIcon(QIcon());
            this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
            this->ui->lineEdit->setText("");
            this->ui->label_5->setText(tmktr("Не выбрана"));
            allowSaveQuestion = true;
            if (currentRow != 0 && variant != 0)
                EditQuestion(currentRow, variant);
        }
    }
    else {
        allowSaveQuestion = false;
        editingQuestion = false;
        //WaitSecs(2, false);
        //this->ui->pushButton_9->setEnabled(false);
        this->ui->pushButton_8->setEnabled(false);
        //this->ui->listWidget->selectionModel()->clear();
        //this->ui->label_3->setPixmap(QPixmap());
        this->ui->label_3->setIcon(QIcon());
        this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
        this->ui->lineEdit->setText("");
        this->ui->label_5->setText(tmktr("Не выбрана"));
        allowSaveQuestion = true;
        if (currentRow != 0 && variant != 0)
            EditQuestion(currentRow, variant);
    }
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    changedSomething = true;
    if (justSaved == false) {
        this->ui->label_7->setStyleSheet("color: steelblue;");
        this->ui->label_7->setText(tmktr("Закончили редактировать вопрос? Нажмите кнопку \"Сохранить тест\"!"));
    }
    else {
        this->ui->label_7->setStyleSheet("color: forestgreen;");
        this->ui->label_7->setText(tmktr("Все сохранилось! Можете редактировать дальше."));
    }
}

/*
void MainWindow::closeEvent(QCloseEvent *event) {
    if (on_pushButton_9_clicked() != 0) {
        event->ignore();
        return;
    }
    else {
        int result = QMessageBox::information(this, "Внимание", "Сохранить тест?", QMessageBox::Yes, QMessageBox::No);
        if (result == QMessageBox::Yes)
            SaveFile(currentTest);
        event->accept();
        std::exit(0);
    }
}
*/

void MainWindow::SaveFile(QString output) {
    try {
        int curvar = this->ui->spinBox->value();
        int currow = this->ui->listWidget->currentRow();
        currow = currow + 1;
        //if (changedSomething == true)
            on_pushButton_9_clicked();
        QString finalCSVFile = "";
        QString newLine;
        newLine = "\n";
        if (QDir(mediaDirectory).exists() == false)
            QDir().mkdir(mediaDirectory);
	finalCSVFile = FixFirstRowCSV(outCSVFile[0].join(";")) + newLine;
        for (int i = 1; i < outCSVFile.length(); i++)
            finalCSVFile = finalCSVFile + CSVRowToString(outCSVFile[i]) + newLine;
        finalCSVFile = finalCSVFile.replace("\"", "");
	for (int j = 0; j < picturesToMove1.length(); j++) {
            QString path1 = picturesToMove1[j];
            QString path2 = picturesToMove2[j];
            if (path1.toLower() != path2.toLower()) {
                if (QFile::exists(path2))
                    QFile::remove(path2);
                QPixmap pixmap(path1);
                pixmap.save(path2, "PNG");
            }
            picturesToMove1.clear();
            picturesToMove2.clear();
        }
        QFile file(output);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        QTextStream stream(&file);
	stream.setCodec("UTF-8");
        stream << finalCSVFile;
        file.close();
#ifdef WINDOWS_10
	QString hist_file_path = QDir::homePath() + "/konstrqt_history.dat";
#else
	QString hist_file_path = QDir::homePath() + "/.konstrqt_history";
#endif
	QFile konstr_history(hist_file_path);
	konstr_history.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);
	bool write_stuff_to_hist = true;
	if (QFile::exists(hist_file_path)) {
		QString tmp_everything_hist = konstr_history.readAll();
		QString nlsep = "\n";
#ifdef WINDOWS_10
		nlsep = "\n";
#endif
		if (tmp_everything_hist.split(nlsep).contains(hist_file_path))
			write_stuff_to_hist = false;
	}
	if (write_stuff_to_hist == true) {
        QTextStream stream2(&konstr_history);
		stream2 << output << endl;
	}
	konstr_history.close();
    if (preparingForFakeCrash == false)
        HandleSelection(currow, curvar);
    }
    catch (...) {

    }
}

QString MainWindow::CSVRowToString(QStringList row) {
    QString to_ret = "";
    for (int i = 0; i < row.length(); i++)
        to_ret = to_ret + row[i] + ";";
    return to_ret.mid(0, to_ret.length() - 1);
}

void MainWindow::ReloadTest(QString csvFile) {
    this->ui->pushButton_6->setEnabled(false);
    this->ui->pushButton_7->setEnabled(false);
    allowSaveQuestion = true;
    outCSVFile.clear();
    picturesToMove1.clear();
    picturesToMove2.clear();
    this->ui->listWidget->clear();
    HandleSelection(0, 0);
    changedSomething = false;
    editingQuestion = false;
    mediaDirectory = "";
    questionsCount = 0;
    this->setWindowTitle("TestMaker Plus");
    currentTest = csvFile;
    currentTestName = "";
    //this->ui->pushButton_9->setEnabled(false);
    this->ui->pushButton_8->setEnabled(false);
    currentTestVariants = -1;
    if (currentTestVariants == -1) {
        try {
            CSVParser parser_tmp(currentTest);
            if (parser_tmp.ReadFile() != 0)
                throw std::exception();
            currentTestName = parser_tmp.GetValue(1, 1);
            currentTestVariants = parser_tmp.GetVariantsCount();
            QString tmp_base_directory = parser_tmp.GetBaseDirectory();
            if (QDir(tmp_base_directory + "/media").exists())
                mediaDirectory = tmp_base_directory + "/media";
            else if (QDir(tmp_base_directory + "/Images_m").exists())
                mediaDirectory = tmp_base_directory + "/Images_m";
            else if (QDir(tmp_base_directory + "/Image_m").exists())
                mediaDirectory = tmp_base_directory + "/Image_m";
            else if (QDir(tmp_base_directory + "/pictures").exists())
                mediaDirectory = tmp_base_directory + "/pictures";
            else {
                QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Не могу найти сами задания теста."), QMessageBox::Ok);
                std::exit(1);
            }
            questionsCount = parser_tmp.GetQuestionsCount();
            outCSVFile = parser_tmp.GetAllValues();
            this->ui->label_2->setText(tmktr("из") + " " + QString::number(currentTestVariants));
            //delete parser_tmp;
        } catch (...) {
            QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Не могу посчитать кол-во вариантов каждого вопроса в тесте."), QMessageBox::Ok);
            exit(1);
        }
    }
    else
        this->ui->label_2->setText(tmktr("из") + " " + QString::number(currentTestVariants));
    this->setWindowTitle(currentTestName + " - TestMaker Plus");
    this->ui->listWidget->clear();
    for (int i = 0; i < questionsCount; i++) {
        this->ui->listWidget->addItem(tmktr("Вопрос") + " №" + QString::number(i + 1));
    }
    if (outCSVFile.length() < 2) {
        QStringList to_add;
        to_add.append(currentTestName);
        for (int j = 0; j < currentTestVariants; j++)
            to_add.append(QString::number(j + 1));
        to_add.append(tmktr("баллы"));
        outCSVFile.append(to_add);
        NewQuestionDialog dialog(this);
        dialog.exec();
        int question_points = dialog.QuestionPoints;
        questionsCount = questionsCount + 1;
        if (question_points < 1)
            std::exit(1);
        else {
            QStringList to_add2;
            to_add2.append(QString::number(questionsCount));
            for (int l = 0; l < currentTestVariants; l++)
                to_add2.append("");
            to_add2.append(QString::number(question_points));
            outCSVFile.append(to_add2);
            this->ui->listWidget->addItem(tmktr("Вопрос") + " №" + QString::number(questionsCount));
            this->show();
            this->ui->listWidget->setCurrentRow(this->ui->listWidget->count() - 1);
            EditQuestion(1, 1);
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    NewQuestionDialog dialog(this);
    dialog.exec();
    int question_points = dialog.QuestionPoints;
    questionsCount = questionsCount + 1;
    if (question_points < 1) {
        questionsCount = questionsCount - 1;
        return;
    }
    else {
        QStringList to_add2;
        to_add2.append(QString::number(questionsCount));
        for (int l = 0; l < currentTestVariants; l++)
            to_add2.append("");
        to_add2.append(QString::number(question_points));
        outCSVFile.append(to_add2);
        this->ui->listWidget->addItem(tmktr("Вопрос") + " №" + QString::number(questionsCount));
        this->show();
        //this->ui->listWidget->setCurrentRow(this->ui->listWidget->count() - 1);
        //EditQuestion(this->ui->listWidget->currentRow(), 1);
        //HandleSelection();
        HandleSelection(0, 0);
        HandleSelection(this->ui->listWidget->count(), 1);
    }
}

void MainWindow::on_pushButton_clicked()
{
    saveReal = true;
    SaveFile(currentTest);
    saveReal = false;
#ifdef USE_OLD_RELOADTEST_FUNCTION
        ReloadTest(output);
#else
	QApplication::exit(8745);
#endif

}

void MainWindow::on_pushButton_3_clicked()
{
    if (this->ui->listWidget->selectedItems().length() < 1) {
        QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Сначала нужно выбрать вопрос теста."), QMessageBox::Ok);
        return;
    }
    else {
        int result = QMessageBox::question(this, tmktr("Внимание"), tmktr("Вы уверены?"), QMessageBox::Yes, QMessageBox::No);
        if (result == QMessageBox::Yes) {
            changedSomething = false;
            editingQuestion = false;
            int currentquestion = this->ui->listWidget->currentRow() + 1;
            outCSVFile.removeAt(currentquestion);
            questionsCount = questionsCount - 1;
            if (currentquestion < outCSVFile.length()) {
                for (int i = currentquestion; i < outCSVFile.length(); i++) {
                    QStringList to_replace = outCSVFile[i];
                    if (to_replace[0] != "1")
                        to_replace[0] = QString::number(to_replace[0].toInt() - 1);
                    outCSVFile[i] = to_replace;
                }
            }
            //QMessageBox::critical(this, "", "Come on. " + QString::number(questionsCount) + " " + QString::number(currentquestion));
            for (int j = 0; j < currentTestVariants; j++)
                    QFile::remove(mediaDirectory + "/kart" + QString::number(currentquestion) + "_" + QString::number(j + 1) + ".png");
            //QMessageBox::critical(this, "", "It passed.");
            for (int k = currentquestion; k < (questionsCount + 2); k++) {
                QString the_file = mediaDirectory + "/kart" + "%QNUM%" + "_%NUM%" + ".png";
                //QMessageBox::critical(this, "", QString::number(k));
                if (QFile::exists(the_file.replace("%QNUM%", QString::number(k)).replace("%NUM%", "1"))) {
                    for (int l = 0; l < (currentTestVariants + 1); l++) {
                        //QMessageBox::critical(this, "", QString::number(k));
                        QFile myfile(mediaDirectory + "/kart" + QString::number(k) + "_" + QString::number(l + 1) + ".png");
                        QString final_name = mediaDirectory + "/kart" + QString::number(k - 1) + "_" + QString::number(l + 1) + ".png";
                        //QMessageBox::critical(this, "", myfile.fileName());
                        //QMessageBox::critical(this, "", final_name);
                        myfile.copy(final_name);
                        myfile.remove();
                        myfile.close();
                    }
                }
            }

        this->ui->listWidget->clear();
        for (int m = 0; m < questionsCount; m++)
            this->ui->listWidget->addItem(tmktr("Вопрос") + " №" + QString::number(m + 1));
        //HandleSelection(1, 1);
        editingQuestion = false;
        changedSomething = false;
        preparingForFakeCrash = true;
	SaveFile(currentTest);
    preparingForFakeCrash = false;
    ReloadTest(currentTest);
    HandleSelection(1, 1);

        }
        else
            return;
    }
}

void MainWindow::on_action_2_triggered()
{
    //this->close();
    exit(0);
}

void MainWindow::on_action_4_triggered()
{
    int qvernum = 5;
#ifndef QT5_BUILD
    qvernum = 4
#endif
    QMessageBox::information(this, tmktr("О программе"), tmktr("Редактор тестов TestMaker Plus") + " (" + tmktr("версия") + " " + QString::number(TMKVERSION) + ")\n" + tmktr("Разработчики") + ": Тимофей Корякин, Иван Ширяев, Тамара Куликова\n\n\n" + tmktr("Эта программа использует библиотеку Qt") + " " + QString::number(qvernum), QMessageBox::Ok);
}

void MainWindow::on_pushButton_4_clicked()
{
#ifndef QT5_BUILD
    QMessageBox::critical(this, "Ошибка", "Эта функция недоступна в сборках TestMaker Plus, использующих Qt 4.", QMessageBox::Ok);
    return;
#endif
     if (QFile::exists(currentTest) == false) {
        QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Нужно сначала сохранить тест."), QMessageBox::Ok);
        return;
    }
    QString baseDirectory = QFileInfo(currentTest).canonicalPath();
    QString ipAddress = "127.0.0.1";
    int port = 8000;
#ifndef USE_BUNDLED_SERVER
    QDir().mkdir(baseDirectory + "/cgi-bin");
    try {
        QFile scriptFile(baseDirectory + "/cgi-bin/testmaker_handler.cgi");
        scriptFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        QTextStream stream(&scriptFile);
        QString commentHeader = "#!/usr/bin/env python2";
# ifdef WINDOWS_10
	commentHeader = "#!" + QDir::currentPath().replace("/", "\\") + "\\python\\python.exe";
# endif
        stream << commentHeader << endl;
	stream << "# -*- coding: UTF-8 -*-" << endl;
        stream << "import cgi" << endl;
        stream << "print \"Content-Type: text/plain\"" << endl;
        stream << "print " << endl;
        stream << "args_data = cgi.FieldStorage()" << endl;
        stream << "if \"action\" not in args_data or \"data\" not in args_data:" << endl;
        stream << "    print \"error\"" << endl;
        stream << "action_to_do = args_data[\"action\"].value" << endl;
        stream << "action_arguments = args_data[\"data\"].value" << endl;
        stream << "if action_to_do == \"get_version\":" << endl;
        stream << "    print \"TestMaker Plus server 1.0\"" << endl;
        stream << "elif action_to_do == \"write_test_result\":" << endl;
	QString outputFile = "    myfile = open('$BASEDIR$test_result.csv', 'a')";
# ifdef WINDOWS_10
	QString baseDirectoryWithSlash = baseDirectory + "/";
        baseDirectoryWithSlash.replace("/", "\\");
        stream << outputFile.replace("$BASEDIR$", baseDirectoryWithSlash) << endl;
# else
        stream << outputFile.replace("$BASEDIR$", baseDirectory + "/") << endl;
# endif
        stream << "    myfile.write(action_arguments + '\\n')" << endl;
        stream << "    myfile.close()" << endl;
        stream << "elif action_to_do == \"test_connection\":" << endl;
        stream << "    print \"ready\"" << endl;
        stream << "elif action_to_do == \"welcome\":" << endl;
        stream << "    print \"This server hosts a test made using TestMaker Plus. You can only do this test using TestMaker plus runtime.\"" << endl;
        stream << "else:" << endl;
        stream << "    print \"error\"" << endl;
        scriptFile.close();
        QFile indexHtmlFile(baseDirectory + "/index.html");
        indexHtmlFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        QTextStream stream2(&indexHtmlFile);
        stream2 << "<p>This website cannot be loaded in a web browser. See <a href=\"/cgi-bin/testmaker_handler.cgi?action=welcome&data=none\">this page</a> for more info.</p>" << endl;
        indexHtmlFile.close();
# ifndef WINDOWS_10
        QProcess tmp_proc_chmod;
        tmp_proc_chmod.setWorkingDirectory(baseDirectory + "/cgi-bin");
        tmp_proc_chmod.start("chmod 777 testmaker_handler.cgi");
        tmp_proc_chmod.waitForFinished(-1);
        tmp_proc_chmod.close();
# endif
    } catch (...) {
#       ifdef QT5_BUILD
        QDir(baseDirectory + "/cgi-bin").removeRecursively();
#       else
	QDir().rmdir(baseDirectory + "/cgi-bin");
#       endif
        QFile::remove(baseDirectory + "/index.html");
        QMessageBox::critical(this, "Ошибка", "Не могу запустить сервер.", QMessageBox::Ok);
    }
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for (int i = 0; i < list.length(); i++) {
#       ifdef QT5_BUILD
        if (list[i].protocol() == QAbstractSocket::IPv4Protocol && list[i].isLoopback() == false && list[i] != QHostAddress(QHostAddress::LocalHost)) {
#       else
        if (list[i].protocol() == QAbstractSocket::IPv4Protocol && list[i] != QHostAddress(QHostAddress::LocalHost)) {
#       endif
            ipAddress = list[i].toString();
            i = list.length();
        }
    }
    QProcess proc;
    proc.setWorkingDirectory(baseDirectory);
#ifndef WINDOWS_10
    port = 8000;
    QString pythonPath = "python";
# ifndef MAC_COMPUTER
    if (QFile::exists("/bin/python2") || QFile::exists("/usr/bin/python2"))
        pythonPath = "python2";
    else if (QFile::exists("/bin/python") || QFile::exists("/usr/bin/python"))
        pythonPath = "python";
    else if (QFile::exists("/bin/python2.7") || QFile::exists("/usr/bin/python2.7"))
        pythonPath = "python2.7";
    else if (QFile::exists("/bin/python2.6") || QFile::exists("/usr/bin/python2.6"))
        pythonPath = "python2.6";
    else {
        QMessageBox::critical(this, "Ошибка", "Не могу найти установленный Python 2.6 или 2.7.", QMessageBox::Ok);
        return;
    }
# endif
    proc.start(pythonPath + " -m CGIHTTPServer");
#else
    port = 80;
    proc.start(QDir::currentPath().replace("/", "\\") + "\\python\\python.exe -m CGIHTTPServer -p " + QString::number(port));
#endif
    QMessageBox::information(this, "Внимание", "Вы опубликовали ваш тест!\nНа компьютерах учеников (которые подключены к школьной локальной сети) откройте тестирующую систему TestMaker Plus, нажмите кнопку \"Скачать тест из Интернета\" и введите эту ссылку:\n\nhttp://" + ipAddress + ":" + QString::number(port) + "/" + QFileInfo(currentTest).baseName() + ".csv\n\nКогда все ученики закончат выполнять работу, нажмите \"ОК\".", QMessageBox::Ok);
#  ifndef WINDOWS_10
    kill(proc.pid(), SIGKILL);
#  else
    QProcess::startDetached("taskkill /f /im python.exe");
#  endif
    proc.close();
    try {
        QFile::remove(baseDirectory + "/index.html");
#       ifdef QT5_BUILD
        QDir(baseDirectory + "/cgi-bin").removeRecursively();
#       else
	QDir().rmdir(baseDirectory + "/cgi-bin");
#       endif
    } catch (...) {

    }
#else
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for (int i = 0; i < list.length(); i++) {
#       ifdef QT5_BUILD
        if (list[i].protocol() == QAbstractSocket::IPv4Protocol && list[i].isLoopback() == false && list[i] != QHostAddress(QHostAddress::LocalHost)) {
#       else
        if (list[i].protocol() == QAbstractSocket::IPv4Protocol && list[i] != QHostAddress(QHostAddress::LocalHost)) {
#       endif
            ipAddress = list[i].toString();
            i = list.length();
        }
    }
#  ifndef STANDALONE_BINARY_SERVER
#    ifdef CONCURRENT_SERVER
    QtHTTPServer tHTTPServ(this, baseDirectory, QFileInfo(currentTest).fileName());
    tHTTPServ.run();
    QMessageBox::information(this, "Внимание", "Вы опубликовали ваш тест!\nНа компьютерах учеников (которые подключены к школьной локальной сети) откройте тестирующую систему TestMaker Plus, нажмите кнопку \"Скачать тест из Интернета\" и введите эту ссылку:\n\nhttp://" + ipAddress + ":" + QString::number(port) + "/" + QFileInfo(currentTest).baseName() + ".csv\n\nКогда все ученики закончат выполнять работу, нажмите \"ОК\".", QMessageBox::Ok);
    tHTTPServ.stop();
#    else
    QtHTTPServer tHTTPServ_nonconc(this, baseDirectory, QFileInfo(currentTest).fileName());
    QFuture<void> asyncsrvrun = QtConcurrent::run(&tHTTPServ_nonconc, &QtHTTPServer::run);
    QMessageBox::information(this, "Внимание", "Вы опубликовали ваш тест!\nНа компьютерах учеников (которые подключены к школьной локальной сети) откройте тестирующую систему TestMaker Plus, нажмите кнопку \"Скачать тест из Интернета\" и введите эту ссылку:\n\nhttp://" + ipAddress + ":" + QString::number(port) + "/" + QFileInfo(currentTest).baseName() + ".csv\n\nКогда все ученики закончат выполнять работу, нажмите \"ОК\".", QMessageBox::Ok);
    asyncsrvrun.cancel();
    tHTTPServ_nonconc.stop();
#    endif
#  else
   QString applicationBaseDir = QFileInfo(QApplication::applicationFilePath()).dir().absolutePath();
   QString serverBin = applicationBaseDir + "/konstrhttp";
#       ifdef WINDOWS_10
   serverBin = serverBin + ".exe";
   serverBin = serverBin.replace("/", "\\");
#       endif
   if (QFile::exists(serverBin) == false) {
        this->ui->label_7->setText(tmktr("Не найден встраиваемый сервер редактора тестов. Пожалуйста, переустановите TestMaker Plus"));
        this->ui->label_7->setStyleSheet("color: red;");
        return;
   }
   QProcess standaloneBinLaunch;
   standaloneBinLaunch.setWorkingDirectory(baseDirectory);
   standaloneBinLaunch.start(serverBin + " \"" + baseDirectory + "\" \"" + QFileInfo(currentTest).fileName() + "\"");
   QMessageBox::information(this, tmktr("Внимание"), tmktr("Вы опубликовали ваш тест!") + "\n" + tmktr("На компьютерах учеников (которые подключены к школьной локальной сети) откройте тестирующую систему TestMaker Plus, нажмите кнопку \"Скачать тест из Интернета\" и введите эту ссылку:") + "\n\nhttp://" + ipAddress + ":" + QString::number(port) + "/" + QFileInfo(currentTest).baseName() + ".csv\n\n" + tmktr("Когда все ученики закончат выполнять работу, нажмите \"ОК\"."), QMessageBox::Ok);
#        ifdef WINDOWS_10
   QProcess::start("taskkill /f /im konstrhttp.exe");
#        else
   kill(standaloneBinLaunch.pid(), SIGKILL);
#        endif
   standaloneBinLaunch.close();
#  endif
#endif
    return;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (changedSomething == false && editingQuestion == false)
        event->accept();
    else {
        if (allowSaveQuestion == false) {
            event->accept();
            exit(0);
        }
        else {
            //event->ignore();
            int allowClose = QMessageBox::question(this, tmktr("Внимание"), tmktr("Вы уверены, что хотите выйти, не сохранив тест?"), QMessageBox::Yes, QMessageBox::No);
            if (allowClose == QMessageBox::Yes) {
                event->accept();
                exit(0);
            }
            else
                event->ignore();

        }
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    bool painterfound = false;
    if (QDir(mediaDirectory).exists() == false)
        QDir().mkdir(mediaDirectory);
    this->ui->pushButton->setEnabled(true);
   // QString gde_fail = mediaDirectory + "/kart" + QString::number(this->ui->listWidget->currentRow() + 1)  + "_" + QString::number(this->ui->spinBox->value()) + ".png";
    QString gde_fail = QDir::tempPath() + "/kart" + QString::number(this->ui->listWidget->currentRow() + 1)  + "_" + QString::number(this->ui->spinBox->value()) + ".png";
    QPixmap pxmap(450, 360);
    //QPixmap pxmap(300, 190);
    pxmap.fill(Qt::white);
    pxmap.save(gde_fail, "PNG");
    QProcess proc;
#ifndef USE_BUNDLED_PAINT
#  ifdef WINDOWS_10
    painterfound = true;
    proc.start("mspaint \"" + gde_fail.replace("/", "\\") + "\"");
#  else
    painterfound = true;
    bool notkde5 = false;
    if ((QFile::exists("/usr/bin/kdeinit5") == false) && (QFile::exists("/bin/kdeinit5") == false) && (QFile::exists("/usr/local/bin/kdeinit5") == false))
        notkde5 = true;
    if ((QFile::exists("/usr/bin/kolourpaint") || QFile::exists("/bin/kolourpaint") || QFile::exists("/usr/local/bin/kolourpaint")) && notkde5 == true)
	proc.start("kolourpaint \"" + gde_fail + "\"");
    else if (QFile::exists("/usr/bin/pinta") || QFile::exists("/bin/pinta") || QFile::exists("/usr/local/bin/pinta"))
	proc.start("pinta \"" + gde_fail + "\"");
    else if (QFile::exists("/usr/bin/gpaint") || QFile::exists("/bin/gpaint") || QFile::exists("/usr/local/bin/gpaint"))
	proc.start("gpaint \"" + gde_fail + "\"");
    else if (QFile::exists("/usr/bin/gimp") || QFile::exists("/bin/gimp") || QFile::exists("/usr/local/bin/gimp"))
	proc.start("gimp \"" + gde_fail + "\"");
    else if (QFile::exists("/usr/bin/xpaint") || QFile::exists("/bin/xpaint") || QFile::exists("/usr/local/bin/xpaint"))
	proc.start("xpaint \"" + gde_fail + "\"");
    else if ((QFile::exists("/usr/bin/digikam") || QFile::exists("/bin/digikam") || QFile::exists("/usr/local/bin/digikam")) && notkde5 == true)
	proc.start("digikam \"" + gde_fail + "\"");
    else if ((QFile::exists("/Applications/GIMP.app/Contents/MacOS/GIMP")))
    proc.start("/Applications/GIMP.app/Contents/MacOS/GIMP \"" + gde_fail + "\"");
    else {
    painterfound = false;
    proc.start("sh -c \"echo no paint programs found\"");
    }
#  endif
#else
    painterfound = false;
#endif
    proc.waitForFinished(3600000);
    proc.close();
#ifdef INCLUDE_QT5PAINT
    if (painterfound == false) {
        Qt5Paint painter_embedded(gde_fail, gde_fail, true);
        painter_embedded.showMaximized();
        while (painter_embedded.isVisible()) {
            QApplication::processEvents(QEventLoop::WaitForMoreEvents, 5);
        }
    }
#else
    QMessageBox::critical(this, "Ошибка", "Не найден ни один поддерживаемый графический редактор.", QMessageBox::Ok);
#endif
    if (QFile::exists(gde_fail)) {
        this->ui->label_3->setIcon(QIcon(QPixmap::fromImage(QImage(gde_fail))));
        this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
        //this->ui->label_3->setPixmap(QPixmap::fromImage(QImage(gde_fail)).scaled(this->ui->label_3->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        this->ui->label_5->setText(gde_fail);
    }
    this->ui->label_7->setStyleSheet("color: steelblue;");
    this->ui->label_7->setText(tmktr("Если вы заметили, что допустили ошибку, нажимайте прямо на картинку, чтобы исправить допущенную неточность."));
}

void MainWindow::on_label_3_clicked()
{
    bool painterfound = false;
    if (this->ui->label_5->text().toLower().contains(tmktr("не выбрана").toLower()))
        on_pushButton_5_clicked();
    else {
        QProcess proc;
        QString gde_fail = this->ui->label_5->text();
#ifndef USE_BUNDLED_PAINT
# ifdef WINDOWS_10
        painterfound = true;
        gde_fail = gde_fail.replace("/", "\\");
        proc.start("mspaint " + gde_fail);
# else
        painterfound = true;
        bool notkde5 = false;
        if ((QFile::exists("/usr/bin/kdeinit5") == false) && (QFile::exists("/bin/kdeinit5") == false) && (QFile::exists("/usr/local/bin/kdeinit5") == false))
            notkde5 = true;
        if ((QFile::exists("/usr/bin/kolourpaint") || QFile::exists("/bin/kolourpaint") || QFile::exists("/usr/local/bin/kolourpaint")) && notkde5 == true)
        proc.start("kolourpaint \"" + gde_fail + "\"");
        else if (QFile::exists("/usr/bin/pinta") || QFile::exists("/bin/pinta") || QFile::exists("/usr/local/bin/pinta"))
        proc.start("pinta \"" + gde_fail + "\"");
        else if (QFile::exists("/usr/bin/gpaint") || QFile::exists("/bin/gpaint") || QFile::exists("/usr/local/bin/gpaint"))
        proc.start("gpaint \"" + gde_fail + "\"");
        else if (QFile::exists("/usr/bin/gimp") || QFile::exists("/bin/gimp") || QFile::exists("/usr/local/bin/gimp"))
        proc.start("gimp \"" + gde_fail + "\"");
        else if (QFile::exists("/usr/bin/xpaint") || QFile::exists("/bin/xpaint") || QFile::exists("/usr/local/bin/xpaint"))
        proc.start("xpaint \"" + gde_fail + "\"");
        else if ((QFile::exists("/usr/bin/digikam") || QFile::exists("/bin/digikam") || QFile::exists("/usr/local/bin/digikam")) && notkde5 == true)
        proc.start("digikam \"" + gde_fail + "\"");
        else if ((QFile::exists("/Applications/GIMP.app/Contents/MacOS/GIMP")))
        proc.start("/Applications/GIMP.app/Contents/MacOS/GIMP \"" + gde_fail + "\"");
        else {
        painterfound = false;
        proc.start("sh -c \"echo no paint programs found\"");
        }
# endif
#else
        painterfound = false;
#endif
        proc.waitForFinished(3600000);
        proc.close();
#ifdef INCLUDE_QT5PAINT
        if (painterfound == false) {
            Qt5Paint painter_embedded(gde_fail, gde_fail, true);
            painter_embedded.showMaximized();
            while (painter_embedded.isVisible()) {
                QApplication::processEvents(QEventLoop::WaitForMoreEvents, 1000);
            }
        }
#else
        if (painterfound == false)
            QMessageBox::critical(this, "Ошибка", "Не найден ни один поддерживаемый графический редактор.", QMessageBox::Ok);
#endif
        if (QFile::exists(gde_fail)) {
            this->ui->label_3->setIcon(QIcon(QPixmap::fromImage(QImage(gde_fail))));
            this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
            this->ui->label_5->setText(gde_fail);
        }
    }
}

void MainWindow::on_action_Qt_triggered()
{
    QMessageBox::aboutQt(this, tmktr("О библиотеке Qt"));
}

QString MainWindow::FixFirstRowCSV(QString orig) {
	QString out = "";
	if (orig.size() > 1) {
		QString sep_used = ";";
		if (orig.contains(","))
			sep_used = ",";
		else if (orig.contains(":"))
			sep_used = ":";
		QStringList mylist = orig.split(sep_used);
		for (int i = 1; i < (mylist.length() - 1); i++)
			mylist[i] = QString::number(i);
		for (int j = 0; j < mylist.length(); j++)
			out = out + sep_used + mylist[j];
		out = out.mid(1, (out.length() - 1));
	}
	return out;
}
