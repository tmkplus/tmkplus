#ifndef CHOOSEQUESTIONDIALOG_H
#define CHOOSEQUESTIONDIALOG_H

#include <QStringList>
#include <QDialog>
#include <QList>

namespace Ui {
class ChooseQuestionDialog;
}

class ChooseQuestionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseQuestionDialog(QWidget *parent = 0, QList<bool> listOfItems = QList<bool>(), QStringList thumbnails = QStringList());
    ~ChooseQuestionDialog();
    int GetSelectedQuestion();
    void ModifyText(QString labelText, QString closeButtonText);

private slots:
    void on_listWidget_itemSelectionChanged();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ChooseQuestionDialog *ui;
    QStringList ListThumbnails;
    int currentSelection;
};

#endif // CHOOSEQUESTIONDIALOG_H
