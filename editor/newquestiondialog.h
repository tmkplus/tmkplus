#ifndef NEWQUESTIONDIALOG_H
#define NEWQUESTIONDIALOG_H

#include <QDialog>
#include <tmktranslate.h>

namespace Ui {
class NewQuestionDialog;
}

class NewQuestionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewQuestionDialog(QWidget *parent = 0);
    ~NewQuestionDialog();
    int QuestionPoints;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::NewQuestionDialog *ui;
};

#endif // NEWQUESTIONDIALOG_H
