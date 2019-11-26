#ifndef SELECTTESTDIALOG_H
#define SELECTTESTDIALOG_H

#include <QDialog>
#include <QStringList>
#include <tmktranslate.h>

namespace Ui {
class SelectTestDialog;
}

class SelectTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectTestDialog(QWidget *parent = 0);
    ~SelectTestDialog();
    QString GetSelectedTestFile();
    QString userHomeDir;
    QStringList filesToOpen;
    bool isOnlineTest();

private:
    Ui::SelectTestDialog *ui;
    QString selectedItem;
    QString configFile;
    bool download_from_internet;

private slots:
    void on_pushButton_3_clicked();
    void HideButtons();
    void ShowButtonWithText(int button, QString text);
    void on_commandLinkButton_clicked();
    void ButtonHandlerFunction(QString filename);
    void on_commandLinkButton_2_clicked();
    void on_commandLinkButton_3_clicked();
    void on_commandLinkButton_4_clicked();
    void on_commandLinkButton_5_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_4_clicked();
};

#endif // SELECTTESTDIALOG_H
