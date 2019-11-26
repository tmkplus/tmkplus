#include "newquestiondialog.h"
#include "ui_newquestiondialog.h"

NewQuestionDialog::NewQuestionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewQuestionDialog)
{
    ui->setupUi(this);
    ui->label->setText(tmktr("Добавить вопрос"));
    ui->label_2->setText(tmktr("Сейчас вы можете добавить новый вопрос в тест."));
    ui->label_3->setText(tmktr("Цена вопроса (кол-во баллов):"));
    ui->pushButton->setText(tmktr("Добавить"));
    ui->pushButton_2->setText(tmktr("Отмена"));
    QuestionPoints = 1;
}

NewQuestionDialog::~NewQuestionDialog()
{
    delete ui;
}

void NewQuestionDialog::on_pushButton_clicked()
{
    QuestionPoints = this->ui->spinBox->value();
    this->close();
}

void NewQuestionDialog::on_pushButton_2_clicked()
{
    QuestionPoints = 0;
    this->close();
}
