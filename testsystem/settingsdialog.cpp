#include "settingsdialog.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QScreen>
#include <QCryptographicHash>
#include <QInputDialog>
#include "ui_settingsdialog.h"
#include <tmktranslate.h>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->pushButton->setText(tmktr("О программе"));
    ui->pushButton_2->setText(tmktr("Закрыть"));
    ui->pushButton_3->setText(tmktr("Сменить пароль"));
    ui->checkBox->setText(" " + tmktr("Спрашивать, сколько вопросов должно быть в тесте."));
    ui->checkBox_2->setText(" " + tmktr("Включить экспериментальные функции (не рекоммендуется)"));
    ui->checkBox_3->setText(" " + tmktr("Большие шрифты (для людей с плохим зрением)"));
    ui->checkBox_4->setText(" " + tmktr("Темная тема"));
    changepass = false;
    newmd5pass = "";
    QFont modified_font = this->font();
    int size_of_font = 12;
#ifdef Q_OS_ANDROID
    size_of_font = 19;
    ResizeOnAndroid();
#endif
    modified_font.setPixelSize(size_of_font);
    this->setFont(modified_font);
    bool askQuestionsCount = false;
    try {
        QString newLine;
#ifdef WINDOWS_10
        newLine = "\r\n";
#else
        newLine = "\n";
#endif
        bool stop = false;
        QString userHomeDir = QDir::homePath();
        QString settingsFile = "";
        if (QFile::exists(userHomeDir + "/.konstrqt_settings"))
            settingsFile = userHomeDir + "/.konstrqt_settings";
        else if (QFile::exists(userHomeDir + "/konstrqt_settings.dat"))
            settingsFile = userHomeDir + "/konstrqt_settings.dat";
#ifdef Q_OS_ANDROID
	else if (QFile::exists("/storage/emulated/0/konstrqt_settings.dat"))
	    settingsFile = "/storage/emulated/0/konstrqt_settings.dat";
#endif
        else {
            askQuestionsCount = true;
            stop = true;
        }
        if (stop == false) {
#ifdef EASY_CONFIG_READ
            QFile confFile(settingsFile);
            QString readall = confFile.readAll().toLower().replace(newLine, "\n");
            if (readall.contains("ask_questions_count=true"))
                askQuestionsCount = true;
#else
            QFile confFile(settingsFile.toUtf8());
            if (confFile.open(QIODevice::ReadOnly)) {
                QTextStream stream_reads(&confFile);
                QString line = stream_reads.readLine();
                if (line.toLower().toUtf8().contains("ask_questions_count=true"))
                    askQuestionsCount = true;
                else if (line.toLower().toUtf8().contains("teacher_password="))
                    currentmd5pass = line;
            }
#endif
            confFile.close();
        }
    }
    catch (...) {

    }
    if (askQuestionsCount == true)
        ui->checkBox->setChecked(true);
#ifndef WINDOWS_10
    ui->checkBox_4->setEnabled(false);
#endif
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SettingsDialog::on_pushButton_2_clicked()
{
    QString configFile = "";
    QString userHomeDir = QDir::homePath();
    if (QFile::exists(userHomeDir + "/.konstrqt_settings"))
        configFile = userHomeDir + "/.konstrqt_settings";
    else if (QFile::exists(userHomeDir + "/konstrqt_settings.dat"))
        configFile = userHomeDir + "/konstrqt_settings.dat";
    else {
#ifdef WINDOWS_10
        configFile = userHomeDir + "/konstrqt_settings.dat";
#else
        configFile = userHomeDir + "/.konstrqt_settings";
#endif
#ifdef Q_OS_ANDROID
	configFile = "/storage/emulated/0/konstrqt_settings.dat";
#endif
    }
    QString to_write = "";
    if (ui->checkBox->isChecked() == true)
        to_write = "ask_questions_count=true";
    else
        to_write = "ask_questions_count=false";
    QString fontsize = "";
    if (ui->checkBox_3->isChecked() == true)
        fontsize = "acc_fontsize=17";
    else
        fontsize = "acc_fontsize=12";
    QString exprstatus = "ru.timkoi.konstrqt.EXPERIMENTAL_FEATURES=false";
    QString darkstatus = "darkthemeqt=false";
    if (ui->checkBox_2->isChecked() == true)
        exprstatus = exprstatus.replace("false", "true");
    if (ui->checkBox_4->isChecked() == true)
        darkstatus = darkstatus.replace("false", "true");
    if (QFile::exists(configFile)) {
        QFile tmp_qfile_read(configFile);
        try {
            tmp_qfile_read.open(QIODevice::ReadOnly);
            QString fileContents = tmp_qfile_read.readAll();
            fileContents = fileContents.toLower().replace("darkthemeqt=false", darkstatus).replace("darkthemeqt=true", darkstatus).replace("acc_fontsize=", "#acc_fontsize=").replace("ask_questions_count=true", to_write).replace("ask_questions_count=false", to_write).replace("ru.timkoi.konstrqt.experimental_features=false", exprstatus).replace("ru.timkoi.konstrqt.experimental_features=true", exprstatus);
            fileContents = fileContents.replace(fontsize, "# duplicate value removed");
            fileContents = fileContents + "\n" + fontsize + "\n";
            if (changepass == true)
                fileContents = fileContents.replace(currentmd5pass, "teacher_password=" + newmd5pass);
            to_write = fileContents;
        }
        catch (...) {
            QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Произошла ошибка во время сохранения настроек теста.").replace("теста", "TestMaker Plus"), QMessageBox::Ok);
        }
        tmp_qfile_read.close();
    }
    QFile qfile(configFile);
    if (qfile.open(QIODevice::WriteOnly)) {
        QTextStream stream (&qfile);
        stream << to_write << endl;
    }
    else
        QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Произошла ошибка во время сохранения настроек теста.").replace("теста", "TestMaker Plus"), QMessageBox::Ok);
    qfile.close();
    QMessageBox::information(this, "", tmktr("Чтобы изменения вступили в силу, программа будет перезапущена."), QMessageBox::Ok);
    QProcess::startDetached(QApplication::applicationFilePath());
    QApplication::exit();
}

void SettingsDialog::ShowAbout() {
    QString about_dialog_string = "TestMaker Plus\n" + tmktr("Версия") + " " + QString::number(TMKVERSION) + "\n" + tmktr("Разработчики") + ": Тимофей Корякин, Иван Ширяев, Тамара Куликова.\n\n" + tmktr("Эта программа использует библиотеку Qt") + " ^QVER^.\n\n";
#ifdef QT5_BUILD
    about_dialog_string.replace("^QVER^", "5");
#else
    about_dialog_string.replace("^QVER^", "4");
#endif
    about_dialog_string = about_dialog_string + tmktr("Запускается на") + " " + GetCurrentOperatingSystem() + ".\n";
#ifdef Q_OS_ANDROID
    about_dialog_string = about_dialog_string + "Мобильная версия для Android (бета).\n";
#endif
    /*
    about_dialog_string = about_dialog_string + "Включенные функции: ";
#ifdef USE_QT_NATIVE_DOWNLOAD_MANAGER
    about_dialog_string = about_dialog_string + "USE_QT_NATIVE_DOWNLOAD_MANAGER, ";
#endif
#ifdef MODERN_FILE_READER
    about_dialog_string = about_dialog_string + "MODERN_FILE_READER, ";
#endif
#ifdef ALLOW_DOWNLOADS_FEATURE
    about_dialog_string = about_dialog_string + "ALLOW_DOWNLOADS_FEATURE, ";
#endif
#ifdef ENABLE_ACTIVATION
    about_dialog_string = about_dialog_string + "ENABLE_ACTIVATION, ";
#endif
#ifdef NEW_CHECK_ANSWERS_CODE
    about_dialog_string = about_dialog_string + "NEW_CHECK_ANSWERS_CODE, ";
#endif
#ifdef VERIFY_ACTIVATION_KEY_ONLINE
    about_dialog_string = about_dialog_string + "VERIFY_ACTIVATION_KEY_ONLINE, ";
#endif
#ifdef ENABLE_TEST_RESULTS_PROTECTION
    about_dialog_string = about_dialog_string + "ENABLE_TEST_RESULTS_PROTECTION, ";
#endif
    about_dialog_string = about_dialog_string.left(about_dialog_string.length() - 2);
    */
    QMessageBox::information(0, tmktr("О программе"), about_dialog_string, QMessageBox::Ok);
}

void SettingsDialog::on_pushButton_clicked()
{
    ShowAbout();
}

void SettingsDialog::ResizeOnAndroid() {
#ifdef Q_OS_ANDROID
    QScreen *screen = QApplication::screens().at(0);
    int width = screen->size().width();
    int height = screen->size().height();
    QRect window_geometry = this->geometry();
    window_geometry.setWidth(width);
    window_geometry.setHeight(height);
    this->setGeometry(window_geometry);
    this->ui->pushButton->move(0, height - 36);
    this->ui->pushButton_2->move(width - 117, this->ui->pushButton->y());
    this->ui->pushButton_3->setVisible(false);
#endif
}

QString SettingsDialog::GetCurrentOperatingSystem() {
#ifdef WINDOWS_10
    return "Microsoft Windows";
#else
# ifdef MAC_COMPUTER
    return "Mac OS X";
# else
#  ifdef Q_OS_ANDROID
    return "Android 4";
#  else
#   ifdef Q_OS_BSDI
    return "BSD/OS";
#   else
#    ifdef Q_OS_FREEBSD
    return "FreeBSD";
#    else
#     ifdef Q_OS_NETBSD
    return "NetBSD";
#     else
#      ifdef Q_OS_OPENBSD
    return "OpenBSD";
#      else
#       ifdef Q_OS_BSD4
    return "BSD-based operating system";
#       else
#        ifdef Q_OS_LINUX
    if (QFile::exists("/etc/mandriva-release"))
        return "Mandriva Linux";
    else if (QDir("/etc/YaST2").exists())
        return "SUSE Linux";
    else if (QFile::exists("/usr/bin/emerge") || QFile::exists("/bin/emerge") || QFile::exists("/usr/sbin/emerge") || QFile::exists("/sbin/emerge") || QFile::exists("/usr/local/bin/emerge") || QFile::exists("/usr/local/sbin/emerge"))
        return "Gentoo Linux";
    else if (QFile::exists("/etc/redhat-release"))
        return "Red Hat Linux";
    else if (QFile::exists("/etc/ubuntu-release"))
        return "Ubuntu Linux";
    else if (QFile::exists("/etc/debian-release"))
        return "Debian GNU/Linux";
    else if (QFile::exists("/etc/arch-release"))
        return "Arch Linux";
    else
        return "Linux";
#        else
#         ifdef Q_OS_SYMBIAN
    return "Nokia Symbian";
#         else
#          ifdef Q_OS_SOLARIS
    return "Sun Solaris";
#          else
#           ifdef Q_OS_QNX
    return "Blackberry QNX";
#           else
#            ifdef Q_OS_BLACKBERRY
    return "Blackberry 10";
#            else
#             ifdef Q_OS_UNIX
    return "UNIX-like operating system";
#             else
    return "Unknown operating system";
#             endif
#            endif
#           endif
#          endif
#         endif
#        endif
#       endif
#      endif
#     endif
#    endif
#   endif
#  endif
# endif
#endif
}

void SettingsDialog::on_pushButton_3_clicked()
{
    QString newpassword = QInputDialog::getText(this, tmktr("Смена пароля учителя"), tmktr("Введите новый пароль учителя"), QLineEdit::Password);
    if (newpassword == "")
        return;
    QCryptographicHash cryptohash(QCryptographicHash::Md5);
    QString npencoded = cryptohash.hash(newpassword.toUtf8(), QCryptographicHash::Md5).toHex();
    npencoded = npencoded + cryptohash.hash(newpassword.toUtf8(), QCryptographicHash::Sha1).toHex();
    QString newpasswordconfirm = QInputDialog::getText(this, tmktr("Смена пароля учителя"), tmktr("Подтвердите новый пароль"), QLineEdit::Password);
    if (newpasswordconfirm != newpassword) {
        QMessageBox::critical(this, tmktr("Смена пароля учителя"), tmktr("Пароли не совпадают"), QMessageBox::Ok);
        return;
    }
    changepass = true;
    newmd5pass = npencoded;
    QMessageBox::information(this, tmktr("Смена пароля учителя"), tmktr("Пароль будет изменен, как только вы сохраните настройки."), QMessageBox::Ok);
    return;
}

void SettingsDialog::HideChangePasswordButton() {
    this->ui->pushButton_3->setEnabled(false);
}

void SettingsDialog::closeEvent(QCloseEvent *close_event) {
    close_event->accept();
    on_pushButton_2_clicked();
}
