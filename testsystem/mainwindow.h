#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QNetworkAccessManager>
#include <QList>
#include <QStringList>
#include <QString>
#include <cstdlib>
#include <QtDebug>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void SetCSVConstrValue(QString csv_constr);
    void NeedToDownloadTest(bool val);
    QString GetBasename(QString filename);
    //void SetCSVConstrValue(QString csv_constr);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    bool useTeacherPassword;
    QString teacherPasswordEncoded;
    QString progUserAgent;
    QString RusCharWeb(QString orig);
    QStringList testQuestions;
    bool freezeNewQuestion;
    bool showSkipQuestionButtonDuringTest;
#ifdef RANDOMNUMBERS_VB6
    bool __PRIV__INARRAY(int array_orig[], int item);
#endif
    QList<QString> userAnswers;
    void SetWindowSize();
    bool excelCompatibleResults;
#ifndef OLD_CPLUSPLUS_98
    bool doingQuestionRightNow = false;
    bool allowWaitSecs = true;
    bool closing = false;
    bool askQuestionsCount = false;
    bool needToDownloadTest = false;
    bool goFullScreen = false;
#else
    bool doingQuestionRightNow;
    bool allowWaitSecs;
    bool closing;
    bool askQuestionsCount;
    bool needToDownloadTest;
    bool goFullScreen;
#endif
    bool activateOnExit;
    bool allowMessageBoxErrors;
    bool monitorTestResults;
    QString newActivationKey;
    int baseSizeForLabel3;
    QString oldActivationKey;
    QList<bool> finishedAnswers;
#ifndef OLD_CPLUSPLUS_98
    QString directorySeparator = "/";
# ifdef WINDOWS_10
    QString newLine = "\r\n";
# else
    QString newLine = "\n";
# endif
#else
    QString directorySeparator;
    QString newLine;
#endif
    QString downloadLink;
    QString md5_sum1;


private slots:
    void on_action_triggered();
    void on_pushButton_3_clicked();
    void on_label_3_linkActivated(QString link);
    void ShowAbout();
    void on_action_2_triggered();
    void on_pushButton_4_clicked();
    void OpenInPaintProgram(QString file_path);
    void on_action_3_triggered();
    void ConfirmExit();
    void ReadTestFiles();
    void MobileWindowResize();
    void resizeEvent(QResizeEvent* resize);
    int MonitorTestResults2(QString path);
    void MonitorTestResults(QString path);
    void closeEvent(QCloseEvent *close_event);
    QString AnswerQuestion(int question, int variant, QString cards_location, bool eraseAnswer);
    void WaitSecs(int seconds, bool useMilliseconds);
    void on_pushButton_5_clicked();
    QString GetDownloaderProgram();
    QString AnswerQuestionOnline(QString use_image, bool eraseAnswer);
    void on_lineEdit_returnPressed();
    QString GetBaseWebsite(QString orig_site);
    int Qt5_DownloadFile(QString name, QString to_save);
    int Qt4_DownloadFile(QString name, QString to_save);
    QByteArray GetMD5Sum(QString filename);
    void on_action_4_triggered();
    void on_action_5_triggered();
    bool AskTeacherPassword();
    void on_android_nextButton_clicked();
    void on_pushButton_clicked();
    void on_label_3_clicked();
};

#endif // MAINWINDOW_H
