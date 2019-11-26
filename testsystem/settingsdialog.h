#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#ifndef TMKVERSION
#define TMKVERSION 1.5
#endif
#include <QDialog>
#include <QCloseEvent>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void HideChangePasswordButton();
    void ShowAbout();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SettingsDialog *ui;
    void ResizeOnAndroid();
    QString currentmd5pass;
    QString newmd5pass;
    bool changepass;

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    QString GetCurrentOperatingSystem();
    void on_pushButton_3_clicked();
    void closeEvent(QCloseEvent *close_event);
};

#endif // SETTINGSDIALOG_H
