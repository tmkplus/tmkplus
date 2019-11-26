#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QList>
#include <QResizeEvent>
#include <QCloseEvent>
#include <tmktranslate.h>

#ifndef TMKVERSION
#define TMKVERSION 1.5
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0, QString the_test = "none");
    QString currentTest;
    ~MainWindow();
    void EditQuestion(int question, int variant);
    void resizeEvent(QResizeEvent* resize);
    void closeEvent(QCloseEvent *event);
    void HandleSelection(int currentRow, int variant);
    bool reloadTheSameThing;
    bool changedSomething;
    bool preparingForFakeCrash;
    void SaveFile(QString output);
    bool saveReal;
    QString CSVRowToString(QStringList row);
    QString FixFirstRowCSV(QString orig);
    void ReloadTest(QString csvFile);

private slots:
    int on_pushButton_9_clicked();

    void on_pushButton_8_clicked();

    void on_listWidget_itemSelectionChanged();

    void on_listWidget_currentRowChanged(int currentRow);

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_action_2_triggered();

    void on_action_4_triggered();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_label_3_clicked();

    void on_action_Qt_triggered();

private:
    Ui::MainWindow *ui;
    QString currentTestName;
    QString mediaDirectory;
    int currentTestVariants;
    int questionsCount;
    bool editingQuestion;
    bool allowSaveQuestion;
    bool justSaved;
    QList<QStringList> outCSVFile;
    QStringList picturesToMove1;
    bool allowWaitSecs;
    QStringList picturesToMove2;
    void WaitSecs(int seconds, bool useMilliseconds);
};

#endif // MAINWINDOW_H
