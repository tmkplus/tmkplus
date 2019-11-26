// КОНСТРУКТОР ТЕСТОВ (TESTMAKER PLUS) - mainwindow.cpp
// Главное окно программы

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "choosequestiondialog.h"
#include <loadingdialog.h>
#include <csvparser.h>
#ifdef INCLUDE_QT5PAINT
#include <qt5paint.h>
#endif
#ifdef QT5_BUILD
#include <QThread>
#endif
#include <QFile>
#include <QScreen>
#include <QMessageBox>
#include <settingsdialog.h>
#include <QImage>
#include <resultsdialog.h>
#include <QPixmap>
#include <QInputDialog>
#include <QTimer>
#include <QEventLoop>
#include <QFont>
#include <QTextStream>
#include <QTextCodec>
#include <QCloseEvent>
#ifndef sun
#include <math.h>
#endif
#include <QTime>
#include <stdio.h>
#include <iostream>
#include <QDir>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QProcess>
#include <QCryptographicHash>
#include <tmktranslate.h>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
#ifndef QT5_BUILD
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
//#endif
    //CSVConstr = csv_constr;
    ui->setupUi(this);
    //QMessageBox::critical(this, tmktr("Ошибка"), "Testing locales one two", QMessageBox::Ok);
#ifdef Q_OS_ANDROID
    MobileWindowResize();
#endif
    SetWindowSize();
    useTeacherPassword = false;
    progUserAgent = "TESTMK32 TestMakerPlus/1.0R"; // 1.0R means 1.0 Refresh
#ifdef WINDOWS_10
    progUserAgent = progUserAgent + " MSWindows";
#elif defined(MAC_COMPUTER)
    progUserAgent = progUserAgent + " MacOSX";
#elif defined(Q_OS_LINUX)
    if (QFile::exists("/etc/mandriva-release"))
        progUserAgent = progUserAgent + " Linux/Mandriva";
    else if (QFile::exists("/etc/ubuntu-release") || QFile::exists("/etc/debian-release"))
        progUserAgent = progUserAgent + " Linux/Debian";
    else if (QFile::exists("/etc/redhat-release"))
        progUserAgent = progUserAgent + " Linux/RedHat";
    else if (std::system("which pacman") == 0)
	progUserAgent = progUserAgent + " Linux/Arch";
    else
        progUserAgent = progUserAgent + " Linux/Other";
#elif defined(Q_OS_ANDROID)
    progUserAgent = "TESTMK32 TestMakerMOBI/Any Linux/GoogleAndroid";
#elif defined(Q_OS_SYMBIAN)
    progUserAgent = "TESTMK32 TestMakerMOBI/Any NokiaMobilePhone";
#elif defined(Q_OS_BLACKBERRY)
    progUserAgent = "TESTMK32 TestMakerMOBI/Any Blackberry/Ten";
#elif defined(Q_OS_IOS)
    progUserAgent = "TESTMK32 TestMakerMOBI/Any MacOSX";
#elif defined(Q_OS_QNX)
    progUserAgent = "TESTMK32 TestMakerMOBI/Any Blackberry/QNX";
#elif defined(Q_OS_SOLARIS)
    progUserAgent = progUserAgent + " SunMicrosystems/Solaris";
#elif defined(Q_OS_UNIX)
    progUserAgent = progUserAgent + " UNIX/compatible";
#else
    progUserAgent = progUserAgent + " NEW_PLATFORM";
#endif
    teacherPasswordEncoded = "";
    doingQuestionRightNow = false;
    allowWaitSecs = true;
    closing = false;
    directorySeparator = "/";
    needToDownloadTest = false;
#ifdef WINDOWS_10
    newLine = "\r\n";
#else
    newLine = "\n";
#endif
    askQuestionsCount = false;
    goFullScreen = false;
    showSkipQuestionButtonDuringTest = false;
    oldActivationKey = "";
    testQuestions = QStringList();
    finishedAnswers = QList<bool>();
    allowMessageBoxErrors = false;
    ui->csvconstrname->hide();
    ui->label_4->hide();
    userAnswers = QList<QString>();
    freezeNewQuestion = false;
    ui->label_5->hide();
    ui->label_5->setText(tmktr("Добро пожаловать!"));
    ui->lineEdit->hide();
    ui->pushButton_4->hide();
    ui->pushButton->hide();
    md5_sum1 = "";
    ui->label_6->hide();
    ui->pushButton_5->hide();
    ui->menu->setTitle(tmktr("Файл"));
    ui->menu_2->setTitle(tmktr("Помощь"));
    ui->action_4->setText(tmktr("Настройки теста"));
    ui->action->setText(tmktr("Настройки TestMaker Plus"));
    ui->action_3->setText(tmktr("Закрыть"));
    ui->action_5->setText(tmktr("Аварийный выход из программы"));
    ui->action_2->setText(tmktr("О программе"));
    ui->label->setText(tmktr("Имя ученика"));
    ui->label_2->setText(tmktr("Введите свое имя и нажмите \"Далее\", чтобы начать выполнять тест."));
    ui->label_4->setText(tmktr("Введите ваш ответ"));
    ui->pushButton_3->setText(tmktr("Далее"));
    ui->pushButton_4->setText(tmktr("Дальше"));
    ui->pushButton_5->setText(tmktr("Открыть черновик"));
    ui->pushButton->setText(tmktr("Перейти к вопросу"));
    activateOnExit = false;
    newActivationKey = "";
#ifndef WINDOWS_10
    excelCompatibleResults = false;
#else
    excelCompatibleResults = true;
#endif
    QFont modified_font = this->font();
    int size_of_font = 12;
#ifdef Q_OS_ANDROID
    size_of_font = 26;
    ui->pushButton_4->hide();
    ui->pushButton_5->hide();
#else
    this->setMaximumHeight(515);
#endif
    try {
        bool stop = false;
        QString userHomeDir = QDir::homePath();
        QString settingsFile = "";
        if (QFile::exists(userHomeDir + "/.konstrqt_settings"))
            settingsFile = userHomeDir + "/.konstrqt_settings";
        else if (QFile::exists(userHomeDir + "/konstrqt_settings.dat"))
            settingsFile = userHomeDir + "/konstrqt_settings.dat";
        else {
            askQuestionsCount = true;
        QString user_activation_first_run = "activationdisabled";
#ifdef WINDOWS_10
            settingsFile = userHomeDir + "/konstrqt_settings.dat";
#else
            settingsFile = userHomeDir + "/.konstrqt_settings";
#endif
#ifdef Q_OS_ANDROID
	    settingsFile = "/storage/emulated/0/konstrqt_settings.dat";
#endif
#ifdef ENABLE_TEST_PASSWORDS
#  ifndef Q_OS_ANDROID
            QString password_tmp_input = QInputDialog::getText(this, tmktr("Настройки TestMaker Plus"), tmktr("Спасибо, что установили TestMaker Plus! Чтобы изменять настройки программы или теста будет требоваться пароль учителя. Придумайте его прямо сейчас."), QLineEdit::Password);
#  else
	    QString password_tmp_input = "Q_OS_ANDROID";
#  endif
            QCryptographicHash hashString(QCryptographicHash::Md5);
            QString password_encoded = hashString.hash(password_tmp_input.toUtf8(), QCryptographicHash::Md5).toHex();
        password_encoded = password_encoded + QString(hashString.hash(password_tmp_input.toUtf8(), QCryptographicHash::Sha1).toHex());

#endif
            QFile tmp_first_run_config(settingsFile);
            if (tmp_first_run_config.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
#ifdef ENABLE_TEST_PASSWORDS
                QString to_write_first_run = "teacher_password=" + password_encoded + newLine + "darkthemeqt=false" + newLine;
#else
		QString to_write_first_run = "";
#endif
                // GNOME integration - used only once
        if (std::system("which gconftool-2") == 0) {
            qDebug() << "This is GNOME";
            int gconfreturned = std::system("gconftool-2 -g /desktop/gnome/interface/document_font_name > /tmp/timkoisoft_gnomeintegration.socket");
            if (gconfreturned != 0) {
                qDebug() << "finishing up, gconf failed";
                std::system("rm -r -f /tmp/timkoisoft_gnomeintegration.socket");
            }
            else {
                QFile* qfgnome = new QFile("/tmp/timkoisoft_gnomeintegration.socket");
                QString valgnomefont = "";
                if (qfgnome->open(QIODevice::Text | QIODevice::ReadOnly) == false) {
                    valgnomefont = "";
                    qDebug() << "QFile failed, finishing up";
                }
                else
                    valgnomefont = QString(qfgnome->readAll());
                qfgnome->close();
                delete qfgnome;
                std::system("rm -r -f /tmp/timkoisoft_gnomeintegration.socket");
                valgnomefont.replace("\r\n", "\n");
                valgnomefont.replace("\n", "");
                qDebug() << "preferred font is " << valgnomefont;
                if (valgnomefont.contains(" ")) {
                    QStringList splitgconf = valgnomefont.split(" ", QString::SkipEmptyParts);
                    to_write_first_run = to_write_first_run + "# GNOME 3 integration - automatic font family detection, etc" + newLine + "GNOMEIntegrationFontFamily=" + splitgconf[0] + newLine;
                    modified_font.setFamily(splitgconf[0]);
                    size_of_font = splitgconf[1].toInt();
                }
                else
                    qDebug() << "finishing up, key format invalid";
            }
        }
                to_write_first_run = to_write_first_run + "ask_questions_count=true" + newLine + "activation_key=" + user_activation_first_run + newLine + "ru.timkoi.konstrqt.EXPERIMENTAL_FEATURES=false" + newLine + "acc_fontsize=" + QString::number(size_of_font);
                tmp_first_run_config.write(to_write_first_run.toUtf8());
            }
            else {
                tmp_first_run_config.close();
                QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Произошла ошибка во время записи файла с настройками программы."), QMessageBox::Ok);
                exit(1);
            }
            tmp_first_run_config.close();
#           ifndef Q_OS_ANDROID
            stop = true;
            QProcess::startDetached(QApplication::applicationFilePath());
            exit(0);
#           else
            stop = false;
#           endif
        }
        if (stop == false) {
            QFile confFile(settingsFile.toUtf8());
            if (confFile.open(QIODevice::ReadOnly)) {
                QTextStream stream_reads(&confFile);
                QStringList conf_file_contents_tmp = QString(stream_reads.readAll().toUtf8()).replace("\r\n", "\n").split("\n");
                confFile.close();
                for (int cnt = 0; cnt < conf_file_contents_tmp.size(); cnt++) {
                QString line = conf_file_contents_tmp[cnt];
                if (line[0] == '#')
                    line = "";
                if (line.toLower().contains("ask_questions_count=true"))
                    askQuestionsCount = true;
# ifndef WINDOWS_10
                else if (line.toLower().contains("darkthemeqt=true"))
                    this->setStyleSheet("background-color: dimgray; color: white;");
# endif
                else if (line.toLower().contains("ru.timkoi.konstrqt.experimental_features=true"))
                    showSkipQuestionButtonDuringTest = true;
                else if (line.toLower().contains("acc_fontsize="))
                    size_of_font = line.toLower().split("=").at(1).toInt();
                else if (line.toLower().contains("gnomeintegrationfontfamily="))
                    modified_font.setFamily(line.toLower().split("=").at(1));
                else if (line.toLower().contains("teacher_password=")) {
# ifdef ENABLE_TEST_PASSWORDS
                    useTeacherPassword = true;
		    QString tmp_line = line.toLower().toUtf8();
		    int b = 0;
		    while (tmp_line.at(b) != '=') {
			b = b + 1;
		    }
		    b = b + 1;
		    teacherPasswordEncoded = line.right(tmp_line.length() - b).replace("\r\n", "").replace("\n", "").replace(newLine, "");
                    //QMessageBox::information(this, "Info", teacherPasswordEncoded, QMessageBox::Ok);
# endif
                }
                else if (line.toLower().contains("activation_key=")) {
		    std::cout << "deprecated" << std::endl;
	        }
            }
	    }
            else
                confFile.close();
        }
    }
    catch (...) {
        std::cout << "a huge try catch block ended here" << std::endl;
    }
    modified_font.setPixelSize(size_of_font);
    this->setFont(modified_font);
    if (useTeacherPassword == false) {
        QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Во время установки TestMaker Plus не был установлен пароль учителя."), QMessageBox::Ok);
        exit(1);
    }

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::ShowAbout() {
    //SettingsDialog::ShowAbout(this);
    SettingsDialog dialog;
    dialog.setFont(this->font());
    dialog.setStyleSheet(this->styleSheet());
    dialog.ShowAbout();
}

void MainWindow::NeedToDownloadTest(bool val) {
    needToDownloadTest = val;
}

void MainWindow::on_label_3_linkActivated(QString link)
{

}

void MainWindow::OpenInPaintProgram(QString file_path) {
#ifdef FORCE_USE_PAINTER
    Qt5Paint qp5(file_path, "", false);
//# ifndef Q_OS_ANDROID
    qp5.showMaximized();
//# else
//    qp5.showFullScreen();
//# endif
    while (qp5.isVisible()) {
        QApplication::processEvents(QEventLoop::WaitForMoreEvents, 500);
    }
    return;
#endif
#ifdef WINDOWS_10
    QProcess process;
    process.start("mspaint.exe " + file_path.replace("/", "\\"));
    process.waitForFinished();
    WaitSecs(6500, true);
    process.close();
#else
    QProcess process;
# ifdef MAC_COMPUTER
    QString editor = "/Applications/Preview.app/Contents/MacOS/Preview";
# else
    QString editor;
    bool notkde5 = true;
    if (QFile::exists("/usr/bin/kdeinit5") || QFile::exists("/usr/local/bin/kdeinit5") || QFile::exists("/bin/kdeinit5"))
        notkde5 = false;
    if (QFile::exists("/usr/bin/gimp") || QFile::exists("/bin/gimp") || QFile::exists("/usr/local/bin/gimp"))
            editor = "gimp";
    else if (QFile::exists("/usr/bin/xpaint") || QFile::exists("/bin/xpaint") || QFile::exists("/usr/local/bin/xpaint"))
            editor = "xpaint";
    else if (QFile::exists("/usr/bin/pinta") || QFile::exists("/usr/local/bin/pinta") || QFile::exists("/bin/pinta"))
            editor = "pinta";
    else if ((QFile::exists("/usr/bin/kolourpaint") || QFile::exists("/bin/kolourpaint") || QFile::exists("/usr/local/bin/kolourpaint")) && notkde5 == true)
            editor = "kolourpaint";
    else if ((QFile::exists("/usr/bin/krita") || QFile::exists("/bin/krita") || QFile::exists("/usr/local/bin/krita")) && notkde5 == true)
            editor = "krita";
    else if ((QFile::exists("/Applications/GIMP.app/Contents/MacOS/GIMP")))
            editor = "/Applications/GIMP.app/Contents/MacOS/GIMP";
    else if (QFile::exists("/usr/bin/gpaint"))
            editor = "gpaint";
    else {
	    editor = "echo";
#           ifdef DISALLOW_BUILTIN_PAINTER
            QMessageBox msg;
#               ifndef MAC_COMPUTER
            msg.critical(this, tmktr("Ошибка"), tmktr("Не могу найти поддерживаемый графический редактор."), QMessageBox::Ok);
#               else
            msg.critical(this, tmktr("Ошибка"), tmktr("Похоже, GIMP для Mac не установлен на вашем компьютере. Пожалуйста, установите его отсюда:") + "http://www.gimp.org/", QMessageBox::Ok);
#               endif
            if (QFile::exists(file_path))
                QFile::remove(file_path);
#           else
		Qt5Paint mypaint(file_path, file_path, false);
		mypaint.showMaximized();
		while (mypaint.isVisible())
            QApplication::processEvents(QEventLoop::WaitForMoreEvents, 500);
#           endif
        return;
    }

# endif
    process.start(editor + " " + file_path);
    process.waitForFinished(-1);
    process.close();
    if (QFile::exists(file_path))
        QFile::remove(file_path);
    return;
#endif
}

void MainWindow::closeEvent(QCloseEvent *close_event) {
    if (ui->label_5->text() != tmktr("Идет выполнение теста"))
        close_event->accept();
    else {
        close_event->ignore();
        ConfirmExit();
    }

}

void MainWindow::on_pushButton_3_clicked()
{
    if (ui->plainTextEdit1->text() == "") {
        QMessageBox msg;
        QString error1(tmktr("Ошибка"));
        QString error2(tmktr("Перед тем, как делать тест, нужно ввести свое имя"));
        msg.critical(this, error1, error2, QMessageBox::Abort, QMessageBox::Cancel);
        return;
    }
    //ui->pushButton->hide();
    //ui->pushButton_2->hide();
    ui->plainTextEdit1->setDisabled(true);
    ui->label_3->hide();
    ui->pushButton_3->hide();
    ui->label_2->hide();
//#ifndef Q_OS_ANDROID
    ui->label_4->show();
    ui->lineEdit->show();
    ui->pushButton_4->show();
#ifndef Q_OS_ANDROID
    ui->pushButton_5->show();
#else
    ui->lineEdit->move(this->ui->label_3->x(), this->ui->label_3->y() + this->ui->label_3->height() + 2);
    ui->pushButton_4->resize(this->ui->pushButton_4->width(), this->ui->lineEdit->height());
    ui->lineEdit->setStyleSheet("border: 1px solid steelblue");
    ui->lineEdit->resize(this->ui->lineEdit->width() * 2, this->ui->lineEdit->height() / 2);
    ui->label_4->hide();
    ui->pushButton_4->move(this->ui->lineEdit->x() + this->ui->lineEdit->width() + 1, this->ui->lineEdit->y());
    ui->lineEdit->resize(this->ui->lineEdit->width(), this->ui->lineEdit->height() - 15);
    ui->pushButton_4->resize(this->ui->pushButton_4->width(), this->ui->pushButton_4->height() - 28);
#endif
//#else
    //ui->android_lineEdit->show();
    //ui->android_nextButton->show();
    //ui->android_lineEdit->move(ui->label_3->x(), ui->label_3->y() + ui->label_3->height() + 4);
    //ui->android_nextButton->move(ui->android_lineEdit->x() + ui->android_lineEdit->width() + 1, ui->android_lineEdit->y());
//#endif
    //ui->label_5->show();
    ui->label_5->setText(tmktr("Идет выполнение теста"));
    ReadTestFiles();
}

void MainWindow::on_action_2_triggered()
{
    if (ui->label_5->text() != tmktr("Идет выполнение теста"))
        ShowAbout();
    else {
         QMessageBox msg;
         msg.critical(this, tmktr("Ошибка"), tmktr("Во время выполнения теста диалог \"О программе\" не показывается."), QMessageBox::Abort, QMessageBox::Cancel);
    }

}

void MainWindow::on_pushButton_4_clicked()
{
    this->repaint();
    if(ui->lineEdit->text() == "") {
        QMessageBox msg;
        msg.critical(this, tmktr("Ошибка"), tmktr("Введите ваш ответ в белое окно ввода."), QMessageBox::Abort, QMessageBox::Cancel);
    }
    else {
        doingQuestionRightNow = false;
    }
}

void MainWindow::ReadTestFiles() {
    qDebug() << "started reading";
    freezeNewQuestion = false;
    this->ui->pushButton->setVisible(showSkipQuestionButtonDuringTest);
    QString csv_constr = ui->csvconstrname->text();
    QMessageBox msg;
    bool sendTestResults = false;
    int only_question_to_ask = 0;
    bool test_with_one_question = false;
    QFile file_temp(csv_constr);
    if (file_temp.exists() == false) {
        msg.critical(this, tmktr("Ошибка"), tmktr("Не могу прочитать CSV-файл с заданиями."), QMessageBox::Abort, QMessageBox::Cancel);
        file_temp.close();
#ifndef CSVPARSER_DO_NOT_CLOSE_PROGRAM_ON_ERROR
        exit(1);
#endif
    }
    file_temp.close();
    CSVParser parser(csv_constr);
    if (parser.ReadFile() == 1) {
        msg.critical(this, tmktr("Ошибка"), tmktr("Не могу прочитать CSV-файл с заданиями."), QMessageBox::Abort, QMessageBox::Cancel);
#ifndef CSVPARSER_DO_NOT_CLOSE_PROGRAM_ON_ERROR
        exit(1);
#endif
    }
    QString csv_header = parser.GetValue(1, 1);
    QString test_media = parser.GetBaseDirectory();
    if (QFile::exists(test_media + "/konstrqt_settings.dat")) {
        QFile tmp_read_konstrqt_settings(test_media + "/konstrqt_settings.dat");
        if (tmp_read_konstrqt_settings.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString contents_tmp_settings = tmp_read_konstrqt_settings.readAll().replace("\r\n", "\n");
            QStringList lines_tmp = contents_tmp_settings.split("\n");
            for (int ln_tmp = 0; ln_tmp < lines_tmp.size(); ln_tmp++) {
                QString line_tmp = lines_tmp[ln_tmp].toLower().replace("\r\n", "\n").replace("\n", "");
                if (line_tmp.contains("verify_results=")) {
#ifdef ENABLE_TEST_RESULTS_PROTECTION
                    if (QFile::exists(test_media + "/test_result.csv")) {
                        monitorTestResults = true;
                        int get_letter_tmp = 0;
                        QString currentChar = "";
                        while (currentChar != "=") {
                            get_letter_tmp = get_letter_tmp + 1;
                            currentChar = line_tmp[get_letter_tmp].toLower();
                        }
                        get_letter_tmp = get_letter_tmp + 1;
                        md5_sum1 = line_tmp.mid(get_letter_tmp, line_tmp.length() - get_letter_tmp);
                        MonitorTestResults2(test_media + "/test_result.csv");
                    }
                    else
                        monitorTestResults = false;
#else
                    monitorTestResults = false;
                    cout << "WARNING! Test results protection is not enabled.\n";
#endif
                }
                else if (line_tmp.contains("ask_only_question=")) {
                    int get_letter_tmp = 0;
                    QString currentChar = "";
                    while (currentChar != "=") {
                        get_letter_tmp = get_letter_tmp + 1;
                        currentChar = line_tmp[get_letter_tmp].toLower();
                    }
                    get_letter_tmp = get_letter_tmp + 1;
                    QString tmp_only_question_to_ask = line_tmp.mid(get_letter_tmp, line_tmp.length() - get_letter_tmp);
                    //QMessageBox::information(this, "Info", tmp_only_question_to_ask, QMessageBox::Ok);
                    only_question_to_ask = tmp_only_question_to_ask.toInt();
                    test_with_one_question = true;
                }
                else if (line_tmp.contains("securetestmode=")) {
                    if (line_tmp.split("=", QString::SkipEmptyParts).at(1) == "true")
                        goFullScreen = true;
                    else
                        goFullScreen = false;
                }
                else if (line_tmp.contains("ru.timkoi.konstrqt.excelCompatibleResults=")) {
                    int get_letter_tmp = 0;
                    QString currentChar = "";
                    while (currentChar != "=") {
                        get_letter_tmp = get_letter_tmp + 1;
                        currentChar = line_tmp[get_letter_tmp].toLower();
                    }
                    get_letter_tmp = get_letter_tmp + 1;
                    QString tmp_what_to_set = line_tmp.mid(get_letter_tmp, line_tmp.length() - get_letter_tmp);
                    if (tmp_what_to_set.toLower() == "true")
                        excelCompatibleResults = true;
                    else
                        excelCompatibleResults = false;
                }
            }
        }
    }
    qDebug() << "goFullScreen = " << goFullScreen;
    QString baseWebsite;
    if (needToDownloadTest == true) 
        baseWebsite = GetBaseWebsite(downloadLink);
    QString imageFileToSave = test_media + directorySeparator + "test_result_" + ui->plainTextEdit1->text() + "_" + QString::number(QDate::currentDate().day()) + "_" + QString::number(QDate::currentDate().month()) + "_" + QString::number(QDate::currentDate().year()) + ".png";
    if (needToDownloadTest == false) {
        if (QFile::exists(test_media + directorySeparator + "Images_m"))
            test_media = test_media + directorySeparator + "Images_m";
        else if (QFile::exists(test_media + directorySeparator + "Image_m"))
            test_media = test_media + directorySeparator + "Image_m";
        else if (QFile::exists(test_media + directorySeparator + "media"))
            test_media = test_media + directorySeparator + "media";
        else if (QFile::exists(test_media + directorySeparator + "pictures"))
            test_media = test_media + directorySeparator + "pictures";
        else {
            msg.critical(this, tmktr("Ошибка"), tmktr("Не могу найти сами задания теста."), QMessageBox::Abort, QMessageBox::Cancel);
            exit(1);
        }
    }
    else {
        test_media = baseWebsite + "/media";
#ifndef ALLOW_DOWNLOADS_FEATURE
        QMessageBox::critical(this, "Ошибка", "Во время сборки функция скачивания тестов из Интернета была отключена, так как она еще в разработке. Чтобы собрать Констуктор тестов с этой функцией, откройте файл TestMaker.pro в текстовом редакторе и добавьте эту строку в конце файла: \"DEFINES += ALLOW_DOWNLOADS_FEATURE\".", QMessageBox::Ok);
        exit(1);
#endif
    }
    if (needToDownloadTest == true) {
    if (Qt5_DownloadFile(baseWebsite + "/test_connection.mmp", QDir::tempPath() + "/konstrqt_response.txt") == 0) {
        QFile tmp_qfile_response(QDir::tempPath() + "/konstrqt_response.txt");
        tmp_qfile_response.open(QIODevice::ReadOnly | QIODevice::Text);
        QString fileContents_tmp = tmp_qfile_response.readAll();
        //QMessageBox::information(this, "Info", fileContents_tmp, QMessageBox::Ok);
        if (fileContents_tmp.toLower().contains("ready"))
            sendTestResults = true;
        else
            sendTestResults = false;
        tmp_qfile_response.close();
        QFile::remove(QDir::tempPath() + "/konstrqt_response.txt");
    }
    }
    int variants_total = 0;
    variants_total = parser.GetVariantsCount();
    int questions_total = 0;
    questions_total = parser.GetQuestionsCount();
    if (test_with_one_question == true) {
        if (questions_total < only_question_to_ask || only_question_to_ask < 1) {
            QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Вопроса [subst] нет в тесте. Буду спрашивать все вопросы.").replace("[subst]", QString::number(only_question_to_ask)), QMessageBox::Ok);
            test_with_one_question = false;
            only_question_to_ask = 0;
        }
    }
    doingQuestionRightNow = false;
    if (askQuestionsCount == true) {
        QInputDialog inDialog(this);
        inDialog.setLabelText(tmktr("Введите количество вопросов (максимальное количество: ") + QString::number(questions_total) + ")");
        inDialog.setInputMode(QInputDialog::IntInput);
        inDialog.setIntValue(questions_total);
        inDialog.setIntMinimum(1);
        inDialog.setIntMaximum(questions_total);
        inDialog.setStyleSheet(this->styleSheet());
        inDialog.exec();
        int val = inDialog.intValue();
        if (val > questions_total || val < 1) { // QString::number(questions_total)  + QString::number(questions_total) +
            QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Неправильный формат ввода. Значение не может быть больше [subst] и меньше 1. Будет использовано максимальное количество вопросов ([subst])").replace("[subst]", QString::number(questions_total)));
            val = questions_total;
        }
        questions_total = val;
    }
#ifndef QLIST_WORKAROUND
    int variants_of_questions[questions_total];
#else
    QList<int> variants_of_questions;
    for (int tmp_int_var = 0; tmp_int_var < questions_total; tmp_int_var++) {
	variants_of_questions.append(0);
    }
#endif
    QStringList online_questions_downloaded;
    finishedAnswers.clear();
    testQuestions.clear();
    for (int tmp_change_qlist_bool = 0; tmp_change_qlist_bool < questions_total; tmp_change_qlist_bool++)
        finishedAnswers.append(false);
#ifdef ENABLE_LOADING_DIALOG
    LoadingDialog dialog;
    dialog.SetLabelText("Создаю тест...");
    dialog.show();
#endif
    //WaitSecs(3, false);
    if (goFullScreen == true) {
        qDebug() << "initializing SecureTest by TestMaker Plus";
        this->setMaximumWidth(700000);
        this->setMaximumHeight(700000);
        this->ui->action_3->setEnabled(false);
        this->ui->pushButton->setVisible(false);
        this->showFullScreen();
        this->move(0, 0);
        QMessageBox::information(this, "TestMaker Plus", tmktr("Сейчас вы начнете выполнение теста. Он состоит из [subst] вопросов. Выполнять вы его будете до тех пор, пока вас не остановит учитель. Внимательно читайте каждый вопрос. Если вам нужен черновик, нажмите на кнопку \"Открыть черновик\" и попытайтесь решить задание там.\n\nУдачи!\nКоманда разработчиков TestMaker Plus").replace("[subst]", QString::number(questions_total)), QMessageBox::Ok);
    }
    for (int i = 0; i < questions_total; i++) {
        int times_regen = 0;
        bool stop_regen = false;
        int card_variant;
        QString download_out_path;
        while (stop_regen == false) {
            //card_variant = rand() % variants_total + 1;
            int question_id_tmp = i + 1;
            if (test_with_one_question == true)
                question_id_tmp = only_question_to_ask;
#ifndef RANDOMNUMBERS_VB6
            card_variant = (qrand() % variants_total + QTime::currentTime().second() + QTime::currentTime().minute()) % variants_total;
#else
            int __RNUMVB__TIMESREGEN = 0;
            int __RNUMVB__PRPNUM = 0;
            bool __RNUMVB__FOUNDNEEDEDVAL = false;
            bool __RNUMVB__NEEDTOSTOP = false;
            while (__RNUMVB__NEEDTOSTOP == false) {
                __RNUMVB__PRPNUM = (qrand() % (variants_total + 1));
                if (__RNUMVB__PRPNUM == 0)
                    __RNUMVB__PRPNUM = 1;
                if (__PRIV__INARRAY(variants_of_questions, __RNUMVB__PRPNUM)) {
                    if (__RNUMVB__TIMESREGEN > 4)
                        __RNUMVB__NEEDTOSTOP = true;
                    else
                        __RNUMVB__TIMESREGEN = __RNUMVB__TIMESREGEN + 1;
                }
                else
                    __RNUMVB__FOUNDNEEDEDVAL = true;
                if (__RNUMVB__FOUNDNEEDEDVAL)
                    __RNUMVB__NEEDTOSTOP = true;
            }
            if (__RNUMVB__FOUNDNEEDEDVAL == false)
                card_variant = (qrand() % (variants_total + 1));
            else
                card_variant = __RNUMVB__PRPNUM;
            if (card_variant == 0)
                card_variant = 1;
#endif
            if (variants_total == 2) {
		if (QTime::currentTime().second() % 2 == 0)
			card_variant = 2;
		else
			card_variant = 1;
	    }
	    if (card_variant == 0)
               card_variant = card_variant + 1;
	    QString mediacardimg = test_media + directorySeparator + "kart" + QString::number(question_id_tmp) + "_" + QString::number(card_variant) + ".png";
        //if (QFile::exists(mediacardimg.replace(".png", ".jpg")))
        //mediacardimg = mediacardimg.replace(".png", ".jpg");
        //std::cout << mediacardimg.toStdString() << "\n" << card_variant << "\n";
        //qDebug() << card_variant;
            //if (QFile::exists(test_media + directorySeparator + "kart" + QString::number(question_id_tmp) + "_" + QString::number(card_variant) + ".png") == false) {
            if (QFile::exists(mediacardimg) == false) {
	        if (needToDownloadTest == false) {
                    times_regen = times_regen + 1;
                    if (times_regen > 5) {
                        msg.critical(this, tmktr("Ошибка"), "Aborted (core dumped).", QMessageBox::Abort, QMessageBox::Cancel);
                        exit(1);
                    }
                }
                else if (needToDownloadTest == true) {
                    download_out_path = QDir::tempPath() + "/konstrqt_question_" + QString::number(qrand() % 20 + QTime::currentTime().second() + QTime::currentTime().minute()) + ".png";
#ifndef USE_QT_NATIVE_DOWNLOAD_MANAGER
                   int download_result_tmp = Qt4_DownloadFile(test_media + "/kart" + QString::number(question_id_tmp) + "_" + QString::number(card_variant) + ".png", download_out_path);
                   if (download_result_tmp == 0) {
                       //QMessageBox::warning(this,0 "Внимание", "Не могу завершить скачивание теста.", QMessageBox::Ok);
                       times_regen = 0;
                       stop_regen = true;
                   }
                   else {
                        times_regen = times_regen + 1;
                        if (times_regen > 5) {
                            QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Не могу завершить скачивание теста."), QMessageBox::Ok);
                            //proc.close();
                            exit(1);
                        }
                   }
                   //proc.close();


#else
                    //QMessageBox::critical(this, "", (test_media + "/kart" + QString::number(i + 1) + "_" + QString::number(card_variant) + ".png") + " to " + download_out_path, QMessageBox::Ok);
                    QString download_url_tmp = test_media + "/kart" + QString::number(question_id_tmp) + "_" + QString::number(card_variant) + ".png";
                    if (QFile::exists(download_out_path)) {
                        while (QFile::exists(download_out_path)) {
                            download_out_path = QDir::tempPath() + "/konstrqt_question_" + QString::number(qrand() % 20 + QTime::currentTime().second() + (QTime::currentTime().minute()) * 2) + ".png";
                        }
                    }
                    int run_function_result = Qt5_DownloadFile(download_url_tmp, download_out_path);
                    cout << "finished" << "\n";
                    if (run_function_result == 0) {
                        times_regen = 0;
                        stop_regen = true;
                    }
                    else {
                        times_regen = times_regen + 1;
                        if (times_regen > 5) {
                            QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Не могу завершить скачивание теста."), QMessageBox::Ok);
                            //proc.close();
                            exit(1);
                        }
                    }
#endif
                }
            }
            else {
               stop_regen = true;
            }
#ifdef ENABLE_LOADING_DIALOG
             dialog.ChangeProgressBarValue(10);
#endif
        }
        variants_of_questions[i] = card_variant;
        if (needToDownloadTest == true) {
            online_questions_downloaded.append(download_out_path);
            testQuestions.append(download_out_path);
        }
        else {
            int question_to_add_tmp = i + 1;
            if (test_with_one_question == true)
                question_to_add_tmp = only_question_to_ask;
            testQuestions.append(test_media + directorySeparator + "kart" + QString::number(question_to_add_tmp) + "_" + QString::number(card_variant) + ".png");
            //QMessageBox::information(this, "Info", testQuestions.at(testQuestions.length() - 1));
        }
    }
#ifdef ENABLE_LOADING_DIALOG
    dialog.hide();
    dialog.close();
#endif
    //delete dialog;

    //QList<QString> userAnswers;
    userAnswers.clear();
    for (int tmp_add_questions_count = 0; tmp_add_questions_count < questions_total; tmp_add_questions_count++)
         userAnswers.append("");
    for (int j = 0; j < questions_total; j++) {
        if (closing == false) {
        //msg.information(this, "", QString::number(j + 1) + " " + QString::number(variants_of_questions[j]), QMessageBox::Ok);
       // msg.information(this, "", QString::number(variants_of_questions[j]), QMessageBox::Ok);
        QString to_add;
#ifndef Q_OS_ANDROID
        ui->label_6->show();
        ui->label_6->setText("№" + QString::number(j + 1));
#else
        ui->label->hide();
        ui->plainTextEdit1->hide();
        ui->label_6->hide();
        ui->label_5->hide();
#endif
        ui->label_3->show();
        int question_id_tmp2 = j + 1;
        if (test_with_one_question == true)
            question_id_tmp2 = only_question_to_ask;
        bool eraseAnswer_tmp = true;
        if (finishedAnswers[j] == true) {
            eraseAnswer_tmp = false;
            this->ui->lineEdit->setText(userAnswers[j]);
        }
        if (needToDownloadTest == true)
            to_add = AnswerQuestionOnline(online_questions_downloaded[j], eraseAnswer_tmp);
        else
            to_add = AnswerQuestion(question_id_tmp2, variants_of_questions[j], test_media, eraseAnswer_tmp);
        //to_add = parser.GetRealAnswer(j + 1, variants_of_questions[j]);
        userAnswers[j] = to_add;
        finishedAnswers[j] = true;
        if (freezeNewQuestion == true) {
            while (freezeNewQuestion == true) {
                QApplication::processEvents(QEventLoop::WaitForMoreEvents, 500);
            }
        }
        freezeNewQuestion = false;

        }
        else
        return;
    }
    int return_dialog = msg.critical(this, tmktr("Внимание"), tmktr("Вы готовы сдать вашу работу?"), QMessageBox::Yes, QMessageBox::No);
    if (return_dialog == QMessageBox::No) {
#ifndef NEW_CHECK_ANSWERS_CODE
        msg.information(this, "Информация", "Сейчас вы посмотрите ваши ответы на все вопросы еще один раз. После того, как вы посмотрите вашу работу еще раз, работа автоматически будет сдана на проверку.", QMessageBox::Ok);
        for (int k = 0; k < questions_total; k++) {
#  ifndef Q_OS_ANDROID
            ui->lineEdit->setText(userAnswers[k]);
#  else
	    ui->android_lineEdit->setText(userAnswers[k]);
#  endif
            ui->label_3->show();
            QString final_answer;
            if (needToDownloadTest == false)
                final_answer = AnswerQuestion(k + 1, variants_of_questions[k], test_media, false);
            else
                final_answer = AnswerQuestionOnline(online_questions_downloaded[k], false);
            userAnswers[k] = final_answer;
        }
#else
        bool stop_asking = false;
        while (stop_asking == false) {
            ChooseQuestionDialog question_dialog(this, finishedAnswers, testQuestions);
            question_dialog.setStyleSheet(this->styleSheet());
            question_dialog.ModifyText(tmktr("Введите номер вопроса, который вы хотите решить заново. Если вы закончили проверять работу, нажмите \"Сдать работу\"."), tmktr("Сдать работу"));
            question_dialog.exec();
            int question_to_repeat = question_dialog.GetSelectedQuestion();
            if (question_to_repeat != -1) {
                question_to_repeat = question_to_repeat + 1;
                QString answer_new;
                ui->label_3->show();
#              ifndef Q_OS_ANDROID
                ui->label_6->show();
                ui->label_6->setText("№" + QString::number(question_to_repeat));
#               endif
                ui->lineEdit->setText(userAnswers[question_to_repeat - 1]);
                int question_id_tmp3 = question_to_repeat;
                if (test_with_one_question == true)
                    question_id_tmp3 = only_question_to_ask;

                if (needToDownloadTest == false)
                    answer_new = AnswerQuestion(question_id_tmp3, variants_of_questions[question_to_repeat - 1], test_media, false);
                else
                    answer_new = AnswerQuestionOnline(online_questions_downloaded[question_to_repeat - 1], false);
                userAnswers[question_to_repeat - 1] = answer_new;
            }
            else
                stop_asking = true;
        };
#endif

    }
    ui->label_6->hide();
    int final_points = 0;
    int max_points = 0;
    QString messageBoxOutput = tmktr("Результат проверки:");
    for (int l = 0; l < questions_total; l++) {
        int qrank = parser.GetQuestionRank(l + 1);
        if (qrank == -1) {
            msg.critical(this, tmktr("Ошибка"), "Assert at line " + QString::number(__LINE__) + ": assert(qrank == -1)", QMessageBox::Ok);
        }
        max_points = max_points + qrank;
        messageBoxOutput = messageBoxOutput + "\n" + tmktr("Вопрос") + " " + QString::number(l + 1) + ": ";
        QString answer = parser.GetRealAnswer(l + 1, variants_of_questions[l]);
        //msg.information(this, "", answer, QMessageBox::Ok);
        ui->label_3->show();
        if (test_with_one_question == true)
            answer = parser.GetRealAnswer(only_question_to_ask, variants_of_questions[l]).replace("$QUOTES$", "\"");
        if (answer.toLower() == userAnswers[l].toLower().replace("\"", "$QUOTES$")) {
            final_points = final_points + qrank;
            messageBoxOutput = messageBoxOutput + tmktr("правильно") + " (" + QString::number(qrank) + " " + tmktr("баллов") + ", " + tmktr("ваш ответ:") + " " + userAnswers[l] + ")";
        }
        else {
            messageBoxOutput = messageBoxOutput + tmktr("неправильно") + " (0 " + tmktr("баллов") + ", " + tmktr("ваш ответ:") + " " + userAnswers[l] + ", " + tmktr("правильный ответ:") + " " + answer.replace("$QUOTES$", "\"") + ")";
        }
    }
    messageBoxOutput = tmktr("Вы набрали [subst] баллов из [subst2]").replace("[subst]", QString::number(final_points)).replace("[subst2]", QString::number(max_points)) + "\n\n" + messageBoxOutput;
    messageBoxOutput = messageBoxOutput + "\n\n" + tmktr("Процент выполнения:") + " ";
    if (needToDownloadTest == true) {
        try {
            for (int to_remove = 0; to_remove < online_questions_downloaded.length(); to_remove++)
                QFile::remove(online_questions_downloaded[to_remove]);

        }
        catch (...) {

        }
    }
    int mark = 0;
    if (final_points == max_points)
        mark = 100;
    else {
#ifndef WINDOWS_10
        //int percent = round(final_points / max_points) * 100;
	int percent = (int)(((double)final_points / (double)max_points) * 100);
#else
	int percent = (int)(((double)final_points / (double)max_points) * 100);
#endif
        if (percent > 88)
            mark = 5;
        else if (percent < 88 && percent > 79)
            mark = 4;
        else if (percent < 79 && percent > 45)
            mark = 3;
        else
            mark = 2;
        mark = percent;
    }
    messageBoxOutput = messageBoxOutput + QString::number(mark) + "%";
    QFile out_file(parser.GetBaseDirectory() + directorySeparator + "test_result.csv");
    QString to_write = ui->plainTextEdit1->text() + "," + QDate::currentDate().toString() + " " + QTime::currentTime().toString() + "," + QString::number(final_points) + "," + QString::number(mark);
    if (sendTestResults == true) {
        int tmp_result_download = Qt5_DownloadFile(baseWebsite + "/tres.mmp?" + RusCharWeb(to_write), QDir::tempPath() + "/konstrqt_response2.txt");
        if (tmp_result_download == 0)
            QFile::remove(QDir::tempPath() + "/konstrqt_response2.txt");
    }
    if (out_file.exists() == false)
        to_write = "Имя ученика,Закончил выполнять тест в,Количество баллов,Процент выполнения\n" + to_write;
    to_write = newLine + to_write.replace("\n", newLine);
    if (out_file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)) {
	if (excelCompatibleResults == false)
        	out_file.write(to_write.toUtf8());
	else {
		QTextStream tmp_write_excel(&out_file);
		tmp_write_excel.setCodec("Windows-1251");
		tmp_write_excel << to_write.replace(",", ";");
	}
    }
    out_file.close();
    QFile additional_config(parser.GetBaseDirectory() + directorySeparator + "konstrqt_settings.dat");
    md5_sum1 = GetMD5Sum(parser.GetBaseDirectory() + directorySeparator + "test_result.csv").toHex();
    QString to_write2 = "verify_results=" + md5_sum1;
    if (goFullScreen == true)
        to_write2 = to_write2 + newLine + "securetestmode=true";
    if (test_with_one_question == true)
        to_write2 = to_write2 + newLine + "ask_only_question=" + QString::number(only_question_to_ask);
    if (additional_config.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream additional_config_stream(&additional_config);
        additional_config_stream << to_write2 << endl;
#ifdef ENABLE_TEST_RESULTS_PROTECTION
        monitorTestResults = true;
#else
        monitorTestResults = false;
#endif
    }
    else {
#ifndef Q_OS_ANDROID
        QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Внутренняя ошибка. Свяжитесь с разработчиками. Код ошибки: ^C^").replace("^C^", "254"));
#endif
        monitorTestResults = false;
    }
    //ui->pushButton->hide();
    //ui->pushButton_2->hide();
    ui->plainTextEdit1->setDisabled(false);
    ui->label_3->show();
    ui->pushButton_3->show();
    ui->label_2->show();
    ui->label_4->hide();
    ui->lineEdit->hide();
    //ui->android_lineEdit->hide();
    //ui->android_nextButton->hide();
    ui->pushButton_4->hide();
    ui->pushButton_5->hide();
    ui->label_5->hide();
    ui->label_5->setText(tmktr("Добро пожаловать!"));
    QFile tempFile("bg_current.png");
    if (tempFile.exists()) {
        QImage temp_image(tempFile.fileName());
        //ui->label_3->setPixmap(QPixmap::fromImage(temp_image));
        this->ui->label_3->setIcon(QIcon(QPixmap::fromImage(temp_image)));
        this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
    }
    else {
        QImage temp_image2("bg_default.png");
        //ui->label_3->setPixmap(QPixmap::fromImage(temp_iВнутренняя ошибка. Свяжитесь с разработчиками. Код ошибки: ^C^mage2));
        this->ui->label_3->setIcon(QIcon(QPixmap::fromImage(temp_image2)));
        this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
     }
    //msg.information(this, "Информация", messageBoxOutput, QMessageBox::Ok);
    ResultsDialog dialog2(this, imageFileToSave);
    this->ui->pushButton->setVisible(false);
    dialog2.SetTextBoxText(messageBoxOutput.replace("\n", newLine));
    dialog2.setStyleSheet(this->styleSheet());
    dialog2.exec();
    doingQuestionRightNow = false;
    allowWaitSecs = true;
//#ifdef Q_OS_ANDROID
//    QString link = "mailto:ВВЕДИТЕ_EMAIL_УЧИТЕЛЯ_СЮДА?subject=Выполнение теста " + QString::number(QDate::currentDate().day()) + "." + QString::number(QDate::currentDate().month()) + "." + QString::number(QDate::currentDate().year()) + "&body=Я написал ваш тест на " + QString::number(mark) + " процентов. Мой результат:%0A" + messageBoxOutput.replace("\n", "%0A");
//    QProcess proc;
//    proc.start("am", QStringList() << "start -a android.intent.action.VIEW -c android.intent.category.DEFAULT -d \"" + link + "\"");
//#endif
#ifdef Q_OS_ANDROID
    QString baseSavePath = "/storage/emulated/0/konstr_qt_results";
    if (QDir("/storage/emulated/0").exists() == false)
        baseSavePath = "/storage/emulated/0/konstr_qt_results";
    QString fileTemplate = "$TYPE_OF_REPORT$_" + ui->plainTextEdit1->text().replace(" ", "_") + "_" + GetBasename(ui->csvconstrname->text().replace(" ", "_")).replace("/", "") + "_" + QString::number(QDate::currentDate().day()) + "_" + QString::number(QDate::currentDate().month()) + "_" + QString::number(QDate::currentDate().year()) + "_" + QString::number(QTime::currentTime().hour()) + "_" + QString::number(QTime::currentTime().minute()) + "_" + QString::number(QTime::currentTime().second());
    QString screenshotResults = baseSavePath + "/photos/" + fileTemplate.replace("$TYPE_OF_REPORT$", "test") + ".png";
    QString reportResults = baseSavePath + "/reports/" + fileTemplate.replace("$TYPE_OF_REPORT$", "result") + ".txt";
    if (QDir(baseSavePath).exists() == false) {
        QDir().mkdir(baseSavePath);
        QDir().mkdir(baseSavePath + "/photos");
        QDir().mkdir(baseSavePath + "/reports");
    }
    try {
        //QFile::copy(imageFileToSave, screenshotResults);
        //QFile::remove(imageFileToSave);
        QFile file2_tmp(reportResults);
        file2_tmp.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        file2_tmp.write(messageBoxOutput.toUtf8());
        QMessageBox::information(this, "Информация", "Результаты теста были сохранены на карту памяти телефона под названием " + reportResults, QMessageBox::Ok);
    } catch (...) {
        QMessageBox::critical(this, "Ошибка", "Не могу сохранить результаты теста.", QMessageBox::Ok);
    }
#endif
    this->ui->label->show();
    this->ui->plainTextEdit1->show();
    this->ui->plainTextEdit1->setText("");
    this->repaint();
    if (goFullScreen == true)
        std::exit(0);
    else
        MonitorTestResults(parser.GetBaseDirectory() + "/test_result.csv");
    return;
}

QString MainWindow::AnswerQuestion(int question, int variant, QString cards_location, bool eraseAnswer) {
    ui->label_3->show();
    if (eraseAnswer == true)
//#ifndef Q_OS_ANDROID
	ui->lineEdit->setText("");
    ui->lineEdit->setFocus();
//#else
//	ui->android_lineEdit->setText("");
//    ui->android_lineEdit->setFocus();
//#endif
#ifdef OLD_IMAGE_LOADER
    QImage temp_image(cards_location + directorySeparator + "kart" + QString::number(question) + "_" + QString::number(variant) + ".png");
    ui->label_3->setPixmap(QPixmap::fromImage(temp_image).scaled(ui->label_3->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
#else
    QPixmap *temp_image = new QPixmap;
    temp_image->load(cards_location + directorySeparator + "kart" + QString::number(question) + "_" + QString::number(variant) + ".png");
    //temp_image = temp_image->scaled(ui->label_3->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //ui->label_3->setPixmap(temp_image->scaled(ui->label_3->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    this->ui->label_3->setIcon(QIcon(*temp_image));
    this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
#endif
    //ui->label_3->setScaledContents(true);
    //ui->label_3->setSizePolicy(QSizePolicy::IgnoreFlag);
    doingQuestionRightNow = true;
    while (doingQuestionRightNow == true) {
      QApplication::processEvents(QEventLoop::WaitForMoreEvents, 500);
    };
    QString to_ret = ui->lineEdit->text();
    //ui->label_3->setPixmap(QPixmap());
    this->ui->label_3->setIcon(QIcon());
    this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
#ifndef OLD_IMAGE_LOADER
    delete temp_image;
#endif
    return to_ret;
}

QString MainWindow::AnswerQuestionOnline(QString use_image, bool eraseAnswer) {
    if (needToDownloadTest == true || freezeNewQuestion == true) {
#ifndef Q_OS_ANDROID
    ui->label_3->show();
#endif
    if (eraseAnswer == true)
        ui->lineEdit->setText("");
    ui->lineEdit->setFocus();

#ifdef OLD_IMAGE_LOADER
    QImage temp_image(use_image);
    ui->label_3->setPixmap(QPixmap::fromImage(temp_image).scaled(ui->label_3->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
#else
    QPixmap *temp_image = new QPixmap;
    temp_image->load(use_image);
    //temp_image = temp_image->scaled(ui->label_3->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //ui->label_3->setPixmap(temp_image->scaled(ui->label_3->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    this->ui->label_3->setIcon(QIcon(*temp_image));
    this->ui->label_3->setIconSize(this->ui->label_3->rect().size());
#endif
    
    //ui->label_3->setScaledContents(true);
    //ui->label_3->setSizePolicy(QSizePolicy::IgnoreFlag);
    doingQuestionRightNow = true;
    while (doingQuestionRightNow == true) {
        QApplication::processEvents(QEventLoop::WaitForMoreEvents, 500);
    };
    QString to_ret = ui->lineEdit->text();
#ifndef OLD_IMAGE_LOADER
    delete temp_image;
#endif
    return to_ret;
    }
}

void MainWindow::WaitSecs(int seconds, bool useMilliseconds) {
    /*
    QTime time_to_stop;
    if (useMilliseconds == true)
        time_to_stop = QTime::currentTime().addMSecs(seconds);
    else
        time_to_stop = QTime::currentTime().addSecs(seconds);
    while (QTime::currentTime() < time_to_stop) {
        if (allowWaitSecs == true)
            QApplication::processEvents(QEventLoop::AllEvents, 10);
        else
            printf("MainWindow::WaitSecs is currently unavailable.\n");
   }
   */
#ifdef VERBOSE_DUMMY_WAITSECS
    cout << "void MainWindow::WaitSecs(int seconds = " << seconds << ", bool useMilliseconds = ";
    if (useMilliseconds == true)
        cout << "true";
    else
        cout << "false";
    cout << ");" << "\n";
#endif
    QCoreApplication::sendPostedEvents();
    return;
}

void MainWindow::SetCSVConstrValue(QString csv_constr) {
    allowMessageBoxErrors = true;
    QFile file(csv_constr);
    if (needToDownloadTest == false && file.exists())
        ui->csvconstrname->setText(csv_constr);
    else {
        if (needToDownloadTest == false) {
            QMessageBox msg;
            msg.critical(this, tmktr("Ошибка"), tmktr("Внутренняя ошибка. Свяжитесь с разработчиками. Код ошибки: ^C^").replace("^C^", "253"), QMessageBox::Abort, QMessageBox::Cancel);
        }
        else {
#ifdef USE_QT_NATIVE_DOWNLOAD_MANAGER
            QString to_save = QDir::tempPath();
            to_save = to_save + "/konstr_qt_temp" + QString::number(qrand() % 20 + QTime::currentTime().second() + QTime::currentTime().minute()) + ".csv";
            int returns_download = Qt5_DownloadFile(csv_constr, to_save);
            if (returns_download != 0)
                exit(returns_download);
            ui->csvconstrname->setText(to_save);
            downloadLink = csv_constr;
#else
           QString to_save;
# ifdef WINDOWS_10
           to_save = QDir::tempPath().replace("/", "\\") + "\\konstrqt_temp_" + QString::number(rand()) + ".csv";
# else
           //QString downloader = "curl";
           to_save = QDir::tempPath() + "/konstrqt_temp_" + QString::number(rand()) + ".csv";
# endif
           int download_result = Qt4_DownloadFile(csv_constr, to_save);
           if (download_result != 0) {
               QMessageBox::critical(this, "Ошибка", "Произошла ошибка во время скачивания " + csv_constr + ". ", QMessageBox::Ok);
               exit(0);
           }
           else {
                ui->csvconstrname->setText(to_save);
                downloadLink = csv_constr;
           }
           //process.close();
#endif
        }
    }
    file.close();
}

void MainWindow::ConfirmExit() {

    if (ui->label_5->text() != tmktr("Идет выполнение теста")) {
        if (needToDownloadTest == true)
            QFile::remove(this->ui->csvconstrname->text());
        exit(0);
    }
    else {
        QMessageBox msg;
        int return_dialog = msg.critical(this, tmktr("Внимание"), tmktr("Сейчас идет выполнение теста. Вы действительно хотите выйти из программы? Ваши ответы НЕ БУДУТ сохранены!"), QMessageBox::Yes, QMessageBox::No);
        if (return_dialog == QMessageBox::Yes) {
            allowWaitSecs = false;
            doingQuestionRightNow = false;
            closing = true;
            monitorTestResults = false;
            if (needToDownloadTest == true)
                QFile::remove(this->ui->csvconstrname->text());
            exit(0);
        }
    }
}

void MainWindow::on_action_3_triggered()
{
    ConfirmExit();

}

void MainWindow::on_pushButton_5_clicked()
{
    QFile file_to_save(QDir::tempPath() + directorySeparator + "konstr_qt_temp_" + QString::number(rand()) + ".png");
    file_to_save.open(QIODevice::WriteOnly);
    //ui->label_3->pixmap()->save(&file_to_save, "PNG");
    this->ui->label_3->icon().pixmap(this->ui->label_3->rect().size()).save(&file_to_save, "PNG");
    QString filePath = file_to_save.fileName();
    file_to_save.close();
    OpenInPaintProgram(filePath);

}

void MainWindow::on_lineEdit_returnPressed()
{
    on_pushButton_4_clicked();
}

void MainWindow::on_action_triggered()
{
#ifdef ENABLE_TEST_PASSWORDS
    if (AskTeacherPassword() == false)
        return;
#endif
    if (doingQuestionRightNow == false) {
        SettingsDialog dialog;
        dialog.setStyleSheet(this->styleSheet());
        dialog.setFont(this->font());
        dialog.exec();
    }
    else
        QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Во время решения заданий теста нельзя открывать окно настроек."), QMessageBox::Ok);
}

QString MainWindow::GetDownloaderProgram() {
           QString to_ret;
#ifdef WINDOWS_10
           QString tmp_app_path = QApplication::applicationDirPath();
           //to_save = QDir::tempPath().replace("/", "\\") + "\\konstrqt_temp_" + QString::number(rand()) + ".csv";
           to_ret = "cscript " + tmp_app_path.replace("/", "\\") + "\\downloader_windows.vbs \"$WEBSITE$\" \"$OUTPUT_FILE$\"";
#else
           //QString downloader = "curl";
           //to_save = QDir::tempPath() + "/konstrqt_temp_" + QString::number(rand()) + ".csv";
           if (QFile::exists("/usr/bin/aria2c") || QFile::exists("/bin/aria2c"))
                to_ret = "aria2c \"$WEBSITE\" -o \"$OUTPUT_FILE\"";
           else if (QFile::exists("/usr/bin/curl") || QFile::exists("/bin/curl"))
               to_ret = "curl \"$WEBSITE\" -o \"$OUTPUT_FILE\"";
           else if (QFile::exists("/usr/bin/wget") || QFile::exists("/bin/wget"))
               to_ret = "wget \"$WEBSITE\" -O \"$OUTPUT_FILE\"";
           else {
               QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Не могу найти поддерживаемый менеджер загрузок."), QMessageBox::Ok);
               exit(0);
           }
#endif
           return to_ret;
}

QString MainWindow::GetBaseWebsite(QString orig_site) {
    QString letter;
    int i = orig_site.length() - 1;
    do {
        letter = orig_site.at(i);
        i = i - 1;
    } while (letter != "/");
    //QMessageBox::warning(this, "debug", orig_site.left(i + 1));
    return orig_site.left(i + 1);
}

int MainWindow::Qt5_DownloadFile(QString name, QString to_save) {
#ifdef USE_QT_NATIVE_DOWNLOAD_MANAGER
    QNetworkAccessManager download_manager;
    //download_manager.execute();
    QNetworkRequest download_request;
    download_request.setUrl(QUrl(name));
    download_request.setRawHeader("User-Agent", progUserAgent.toLatin1());
    QNetworkReply *download_reply = download_manager.get(download_request);
    QEventLoop event;
    connect(download_reply, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();
    if (download_reply->error()) {
        if (allowMessageBoxErrors == true)
            QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Не могу скачать тест ") + download_reply->url().toString(), QMessageBox::Ok);
        return 1;
    }
    else {
        //QString to_save = QDir::tempPath();
        //to_save = to_save + "/konstr_qt_temp" + QString::number(rand()) + ".csv";
        QFile to_save_file(to_save);
        if (to_save_file.open(QIODevice::WriteOnly) == false) {
            QMessageBox::critical(this, tmktr("Ошибка"), "Access is denied.", QMessageBox::Ok);
            to_save_file.close();
            return 1;
        }
        else {
        download_reply->deleteLater();
# ifdef ALWAYS_USE_QTEXTSTREAM
            QTextStream stream(&to_save_file);
	    QString to_write = download_reply->readAll();
        //QMessageBox::information(this, "", to_write, QMessageBox::Ok);
            stream << to_write;
# else
            to_save_file.write(download_reply->readAll());
# endif
            to_save_file.close();
            return 0;
        }
    }
#else
    return 1;
#endif
}

int MainWindow::Qt4_DownloadFile(QString name, QString to_save) {
#ifndef USE_QT_NATIVE_DOWNLOAD_MANAGER
    QProcess proc;

    proc.start(GetDownloaderProgram().replace("$WEBSITE$", name).replace("$OUTPUT_FILE$", to_save));
    proc.waitForFinished(-1);
    int result = 0;
    if (QFile::exists(to_save) == true)
        result = 0;
    else
        result = 1;
    proc.close();
    return result;
#else
    return 1;
#endif
}

QByteArray MainWindow::GetMD5Sum(QString filename) {
    QFile file(filename);
    QByteArray to_ret;
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QCryptographicHash temp_count(QCryptographicHash::Md5);
        try {
            to_ret = temp_count.hash(data, QCryptographicHash::Md5);
        }
        catch (...) {
            to_ret = QByteArray();
        }
    }
    file.close();
    return to_ret;
}

int MainWindow::MonitorTestResults2(QString path) {
#ifdef ENABLE_TEST_RESULTS_PROTECTION
# ifndef Q_OS_ANDROID
    QString md5_sum2 = QString(GetMD5Sum(path).toHex());
    md5_sum2 = md5_sum2.toLower();
    if (md5_sum1 != md5_sum2) {
        QMessageBox::critical(this, tmktr("Внимание"), tmktr("Один из учеников, кажется, исправил список оценок за тест."), QMessageBox::Ok);
        return 1;
    }
    else
        return 0;
# else
    return 0;
# endif
#else
    return 1;
#endif
}

void MainWindow::MonitorTestResults(QString path) {
#ifdef ENABLE_TEST_RESULTS_PROTECTION
# ifndef Q_OS_ANDROID
    while (closing == false) {
        if (closing == true)
            return;
        if (monitorTestResults == true) {
            QApplication::processEvents(QEventLoop::WaitForMoreEvents, 500);
            if (MonitorTestResults2(path) == 1) {
                return;
            }
        }
        else
            QApplication::processEvents(QEventLoop::WaitForMoreEvents, 500);
    };
# else
    return;
# endif
#else
    return;
#endif
}

void MainWindow::on_action_4_triggered()
{
#ifdef ENABLE_TEST_PASSWORDS
    if (AskTeacherPassword() == false)
        return;
#endif
    QFileInfo tmp_file_info(ui->csvconstrname->text());
    QString test_path = tmp_file_info.path();;
    QString to_write = "";
    int messageBoxResult1 = QMessageBox::question(this, tmktr("Настройки теста"), tmktr("Включить защиту от подправки учениками результатов теста?"), QMessageBox::Yes, QMessageBox::No);
    if (messageBoxResult1 == QMessageBox::Yes) {
        if (needToDownloadTest == false) {
            QString path_to_results_file = test_path;
            path_to_results_file = path_to_results_file + "/test_result.csv";
            if (QFile::exists(path_to_results_file)) {
                QString md5sum_tmp = GetMD5Sum(path_to_results_file).toHex();
                to_write = to_write + "verify_results=" + md5sum_tmp;
            }
            else
                to_write = to_write + "verify_results=null";
        }
        else
            QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Эта функция не работает для тестов, скачанных из Интернета."));
        to_write = to_write + newLine;
    }
    int messageBoxResult2 = QInputDialog::getInt(this, tmktr("Настройки теста"), "Вы можете сделать так, чтобы программа задавала разные варианты одного\nи того же вопроса. Введите номер вопроса. Если вы не\nхотите включить эту функцию, введите 0.", 0, 0, 10000);
    if (messageBoxResult2 != 0)
        to_write = to_write + "ask_only_question=" + QString::number(messageBoxResult2);
    to_write = to_write + newLine;
    int messageBoxResult3 = QMessageBox::question(this, tmktr("Настройки теста"), tmktr("Сохранять файл с результатами теста в формате, поддерживаемым программой Microsoft Office Excel?"), QMessageBox::Yes, QMessageBox::No);
    if (messageBoxResult3 == QMessageBox::Yes) {
        excelCompatibleResults = true;
        to_write = to_write + "ru.timkoi.konstrqt.excelCompatibleResults=true";
    }
    to_write = to_write + newLine;
    int messageBoxResult4 = QMessageBox::question(this, tmktr("Настройки теста"), tmktr("Включить TestMaker Plus SecureTest (выполнение теста в полноэкранном защищенном режиме без возможности запуска остальных программ, кроме черновика и самой тестирующей системы)?"), QMessageBox::Yes, QMessageBox::No);
    if (messageBoxResult4 == QMessageBox::Yes) {
        goFullScreen = true;
        to_write = to_write + "securetestmode=true" + newLine;
    }

    try {
        QFile config_writer(test_path + "/konstrqt_settings.dat");
        config_writer.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        QTextStream config_writer_stream(&config_writer);
        config_writer_stream << to_write;
        config_writer.close();
        QMessageBox::information(this, tmktr("Настройки теста"), tmktr("Настройки были успешно сохранены!"), QMessageBox::Ok);
    }
    catch (...) {
        QMessageBox::critical(this, tmktr("Настройки теста"), tmktr("Произошла ошибка во время сохранения настроек теста."), QMessageBox::Ok);
    }
}

void MainWindow::on_action_5_triggered()
{
    exit(0);
}


bool MainWindow::AskTeacherPassword() {
#ifndef ENABLE_TEST_PASSWORDS
    return true;
#else
    if (useTeacherPassword == true) {
        QString user_input_tmp = QInputDialog::getText(this, "TestMaker Plus", tmktr("Введите пароль учителя"), QLineEdit::Password);
        QCryptographicHash hash_count_tmp(QCryptographicHash::Md5);
        QString hash_user_input = hash_count_tmp.hash(user_input_tmp.toUtf8(), QCryptographicHash::Md5).toHex();
	QString legacyhash = hash_user_input;
	hash_user_input = hash_user_input + QString(hash_count_tmp.hash(user_input_tmp.toUtf8(), QCryptographicHash::Sha1).toHex());
    hash_user_input = hash_user_input.toLower().replace(" ", "");
	legacyhash = legacyhash.toLower().replace(" ", "");
        //QMessageBox::information(this, "Info", teacherPasswordEncoded + " and " + hash_user_input, QMessageBox::Ok);
#  ifdef WINDOWS_10
	if (hash_user_input.length() != teacherPasswordEncoded.length())
		teacherPasswordEncoded = teacherPasswordEncoded.mid(0, teacherPasswordEncoded.length() - 1);
#  endif
    qDebug() << "user hash: " << hash_user_input << ", teacher password encoded: " << teacherPasswordEncoded << ", legacy MD5-only hash: " << legacyhash;
	if (hash_user_input == teacherPasswordEncoded || hash_user_input == legacyhash)
            return true;
        else {
            QMessageBox::critical(this, "TestMaker Plus", tmktr("Ошибка. Неправильный пароль учителя."), QMessageBox::Ok);
            return false;
        }
    }
    else
        return true;
#endif
}


void MainWindow::SetWindowSize() {
    int fixed_y_position = this->size().height() - 95;
    QRect geometry_label_5 = this->ui->label_5->geometry();
    geometry_label_5.setWidth(this->size().width() / 4);
    this->ui->label_5->setGeometry(geometry_label_5);
    QRect geometry_label_4 = this->ui->label_4->geometry();
#ifndef Q_OS_ANDROID
    geometry_label_4.setWidth(this->size().width() / 4);
#endif
    geometry_label_4.setY(fixed_y_position);
    this->ui->label_4->setGeometry(geometry_label_4);
    QRect geometry_lineEdit = this->ui->lineEdit->geometry();
    geometry_lineEdit.setWidth(this->size().width() / 2 - 138);
#ifndef Q_OS_ANDROID
    geometry_lineEdit.setY(fixed_y_position);
#else
    geometry_lineEdit.setY(this->size().height() - 30);
    geometry_lineEdit.setX(geometry_label_4.x() + geometry_label_4.width() + 1);
#endif
    this->ui->lineEdit->setGeometry(geometry_lineEdit);

    QSize pushbutton5_size = this->ui->pushButton_5->size();
#ifdef Q_OS_ANDROID
    pushbutton5_size.setHeight(40);
#endif
    QRect geometry_pushbutton_5 = this->ui->pushButton_5->geometry();
    geometry_pushbutton_5.setX(this->ui->lineEdit->geometry().x() + this->ui->lineEdit->geometry().width());
    geometry_pushbutton_5.setY(fixed_y_position);
    geometry_pushbutton_5.setSize(pushbutton5_size);
    this->ui->pushButton_5->setGeometry(geometry_pushbutton_5);

    QSize pushbutton4_size = this->ui->pushButton_4->size();
    QRect geometry_pushbutton_4 = this->ui->pushButton_4->geometry();
    geometry_pushbutton_4.setX(this->ui->pushButton_5->x() + this->ui->pushButton_5->width() + 1);
#ifdef Q_OS_ANDROID
    //this->ui->pushButton_5->show();
    //this->ui->pushButton_4->show();
    //this->ui->lineEdit->show();
    geometry_pushbutton_4.setHeight(25);
    geometry_pushbutton_4.setWidth(182);
#else
    geometry_pushbutton_4.setSize(pushbutton4_size);
#endif

    geometry_pushbutton_4.setY(fixed_y_position);
    this->ui->pushButton_4->setGeometry(geometry_pushbutton_4);
    //WaitSecs(100, true);

    QSize label_size = this->ui->label_3->size();
    label_size.setWidth(this->size().width() - 27);
    label_size.setHeight(this->size().height() - 134);
    QRect geometry_label_3 = this->ui->label_3->geometry();
    geometry_label_3.setSize(label_size);
    this->ui->label_3->setGeometry(geometry_label_3);
    //WaitSecs(100, true);
    

#ifdef Q_OS_ANDROID
    this->ui->pushButton_3->move((this->size().width() - 132), fixed_y_position);
#else
    this->ui->pushButton_3->move((this->size().width() - 98), fixed_y_position);
#endif
    this->ui->label_2->move(this->ui->label_2->geometry().x(), fixed_y_position);
#ifndef Q_OS_ANDROID
    this->ui->label->move((this->size().width() - 348), 0);
    this->ui->plainTextEdit1->move((this->size().width() - 248), 0);
    this->ui->lineEdit->resize(this->ui->lineEdit->size().width(), 30);
    this->ui->pushButton_4->resize(this->ui->pushButton_4->size().width(), 30);
    this->ui->pushButton_5->resize(this->ui->pushButton_5->size().width(), 30);
#endif
}

void MainWindow::resizeEvent(QResizeEvent* resize) {
    QMainWindow::resizeEvent(resize);
    SetWindowSize();
}

void MainWindow::MobileWindowResize() {
#ifdef Q_OS_ANDROID
    QScreen *screen = QApplication::screens().at(0);
    int width = screen->size().width();
    int height = screen->size().height();
    QRect window_geometry = this->geometry();
    window_geometry.setWidth(width);
    window_geometry.setHeight(height);
    this->setGeometry(window_geometry);
    QRect geometry_label = this->ui->label->geometry();
    geometry_label.setX(0);
    geometry_label.setWidth(180);
    geometry_label.setHeight(geometry_label.height() * 2);
    geometry_label.setY(8);
    this->ui->label->setGeometry(geometry_label);
    QRect geometry_plaintextedit = this->ui->plainTextEdit1->geometry();
    geometry_plaintextedit.setX(geometry_label.x() + geometry_label.width() + 1);
    geometry_plaintextedit.setY(8);
    geometry_plaintextedit.setHeight(28);
    this->ui->plainTextEdit1->setStyleSheet("border: 1px solid steelblue");
    geometry_plaintextedit.setWidth(this->size().width() - geometry_label.width() - 4);
    this->ui->plainTextEdit1->setGeometry(geometry_plaintextedit);
    this->ui->pushButton_3->resize(140, 60);
    this->ui->label_2->resize(this->size().width() + 490, 31);
    this->ui->label_4->resize(this->ui->label_4->size().width() + 140, 100);
#endif
}

void MainWindow::on_android_nextButton_clicked()
{
    /*
    if(ui->android_lineEdit->text() == "") {
        QMessageBox msg;
        msg.critical(this, "Ошибка", "Введите ваш ответ в белое окно ввода.", QMessageBox::Abort, QMessageBox::Cancel);
    }
    else {
        doingQuestionRightNow = false;
    }
    */
    return;
}

QString MainWindow::GetBasename(QString filename) {
    int i = filename.length();
    QChar currentChar = ' ';
    while (currentChar != directorySeparator.at(0)) {
        i = i - 1;
        currentChar = filename.at(i);
    }
    return filename.mid(i + 1, filename.length() - (i + 1));
}

void MainWindow::on_pushButton_clicked()
{
    ChooseQuestionDialog dlg(this, finishedAnswers, testQuestions);
    dlg.setStyleSheet(this->styleSheet());
    dlg.exec();
    int question_to_jump = dlg.GetSelectedQuestion();
    if (question_to_jump != -1) {
        this->ui->label->setPixmap(QPixmap());
        if (question_to_jump > 0 && question_to_jump < 1)
            question_to_jump = 0;
        freezeNewQuestion = true;
        if (this->ui->lineEdit->text() == "")
            this->ui->lineEdit->setText(tmktr("Вопрос пропущен."));
        on_pushButton_4_clicked();
        bool eraseAnswer_tmp = true;
        if (finishedAnswers[question_to_jump] == true) {
            this->ui->lineEdit->setText(userAnswers[question_to_jump]);
            eraseAnswer_tmp = false;
        }
        QString userAnswer = AnswerQuestionOnline(testQuestions[question_to_jump], eraseAnswer_tmp);
        userAnswers[question_to_jump] = userAnswer;
        finishedAnswers[question_to_jump] = true;
        freezeNewQuestion = false;
        return;
    }
    else
        return;
}

void MainWindow::on_label_3_clicked()
{
    if (doingQuestionRightNow == true)
        on_pushButton_5_clicked();
}


QString MainWindow::RusCharWeb(QString orig) {
    QString to_ret = orig.toLower().replace("ё", "е").replace("а", "a").replace("б", "b").replace("в", "v").replace("г", "g").replace("д", "d").replace("е", "ye").replace("ж", "j").replace("з", "z").replace("и", "i").replace("й", "yi").replace("к", "k").replace("л", "l").replace("м", "m").replace("о", "o").replace("п", "p").replace("р", "r").replace("с", "s").replace("т", "t").replace("у", "y").replace("ф", "f").replace("х", "h").replace("ц", "ts").replace("ч", "ch").replace("ш", "sh").replace("щ", "sch").replace("ъ", "").replace("ы", "e").replace("ь", "").replace("э", "e").replace("ю", "yu").replace("я", "ya");
    return to_ret.replace("!", "!----1----");
}

#ifdef RANDOMNUMBERS_VB6
bool MainWindow::__PRIV__INARRAY(int array_orig[], int item) {
    int arr_size = (sizeof(array_orig)/sizeof(*array_orig));;
    for (int iindex = 0; iindex < arr_size; iindex++) {
        if (array_orig[iindex] == item)
            return true;
    }
    return false;
}
#endif
