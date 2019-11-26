#include "choosequestiondialog.h"
#include "ui_choosequestiondialog.h"

#include <QString>
#include <QStringList>
#include <QImage>
#include <QList>
#include <QPixmap>
#include <QMessageBox>
#include <QVariant>
#include <tmktranslate.h>

ChooseQuestionDialog::ChooseQuestionDialog(QWidget *parent, QList<bool> listOfItems, QStringList thumbnails) :
    QDialog(parent),
    ui(new Ui::ChooseQuestionDialog)
{
    ui->setupUi(this);
    ui->pushButton->setText(tmktr("Выбрать"));
    if (thumbnails.length() < 1 || listOfItems.length() < 1)
        return;
    else {
        ListThumbnails = thumbnails;
        for (int i = 0; i < listOfItems.length(); i++) {
            QString to_add = tmktr("Вопрос") + "№" + QString::number(i + 1) + " ";
            if (listOfItems[i] == true)
                to_add = to_add + "(" + tmktr("сделан") + ")";
            else
                to_add = to_add + "(" + tmktr("не сделан") + ")";
            this->ui->listWidget->addItem(to_add);
        }
        this->ui->label->setText("");
    }
}

ChooseQuestionDialog::~ChooseQuestionDialog()
{
    delete ui;
}

void ChooseQuestionDialog::on_listWidget_itemSelectionChanged()
{
    try {
        currentSelection = this->ui->listWidget->selectionModel()->selectedIndexes().last().row();
        QPixmap *img = new QPixmap(ListThumbnails[currentSelection]);
        this->ui->label->setPixmap(img->scaled(this->ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        delete img;
    }
    catch (...) {

    }
}

int ChooseQuestionDialog::GetSelectedQuestion() {
    return currentSelection;
}

void ChooseQuestionDialog::on_pushButton_clicked()
{
    this->ui->label->setPixmap(QPixmap());
    if (this->ui->listWidget->selectionModel()->selectedIndexes().length() < 1)
        QMessageBox::critical(this, tmktr("Ошибка"), tmktr("Выберите вопрос."), QMessageBox::Ok);
    else
        this->close();
}

void ChooseQuestionDialog::on_pushButton_2_clicked()
{
    this->ui->label->setPixmap(QPixmap());
    currentSelection = -1;
    this->close();
}

void ChooseQuestionDialog::ModifyText(QString labelText, QString closeButtonText) {
    this->ui->label_2->setText(labelText);
    this->ui->pushButton_2->setText(closeButtonText);
}
