// КОНСТРУКТОР ТЕСТОВ - Рисовалка
// Данный компонент был сделан на основе примера из Qt.
// Лицензия: см. ниже

/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifdef QT5_BUILD
#include <QtWidgets>
#endif
#include <QInputDialog>
#include <QLineEdit>
#include <QCursor>
#include <QTime>
#ifndef QT5_BUILD
#include <QDir>
#include <QApplication>
#include <QCoreApplication>
#include <QColorDialog>
#include <QEventLoop>
#include <QTextCodec>
#include <QMenuBar>
#include <QFileDialog>
#include <QCloseEvent>
#include <QMessageBox>
#endif
#include "qt5paint.h"
#include "scribblearea.h"

//! [0]
Qt5Paint::Qt5Paint(QString openPath, QString savePath, bool showSaveChanges)
{
    stillStarting = true;
    ftopen = openPath;
    ftsave = savePath;
    scribbleArea = new ScribbleArea;
    setCentralWidget(scribbleArea);

    createActions();
    createMenus(showSaveChanges);
    open();

    setWindowTitle("Мини-рисовалка");
    resize(500, 500);
    doShowSaveChanges = showSaveChanges;
    stillStarting = false;
}
//! [0]

//! [1]
void Qt5Paint::closeEvent(QCloseEvent *event)
//! [1] //! [2]
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}
//! [2]

//! [3]
void Qt5Paint::open()
//! [3] //! [4]
{
    if (maybeSave()) {
	QString fileName;
	if (ftopen == "")
        	fileName = QFileDialog::getOpenFileName(this,"Открыть существующую картинку", QDir::currentPath());
        else
		fileName = ftopen;
	if (!fileName.isEmpty())
            scribbleArea->openImage(fileName);
    }
}
//! [4]

//! [5]
void Qt5Paint::save()
//! [5] //! [6]
{
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}
//! [6]

//! [7]
void Qt5Paint::penColor()
//! [7] //! [8]
{
    QColor newColor = QColorDialog::getColor(scribbleArea->penColor());
    if (newColor.isValid())
        scribbleArea->setPenColor(newColor);
}
//! [8]

//! [9]
void Qt5Paint::penWidth()
//! [9] //! [10]
{
    bool ok;
    int newWidth = QInputDialog::getInt(this, "Мини-рисовалка",
                                        tr("Выберите размер кисти"),
                                        scribbleArea->penWidth(),
                                        1, 50, 1, &ok);
    if (ok)
        scribbleArea->setPenWidth(newWidth);
}
//! [10]

//! [11]
void Qt5Paint::about()
//! [11] //! [12]
{
    QMessageBox::about(this, "Кратко о мини-рисовалке", "Мини-рисовалка для TestMaker Plus 1.0\nОчень простой редактор изображений.\nCopyright (c) Тамара Куликова, Тимофей Корякин, Иван Ширяев. Сделано на основе примера из Qt 5 Scribble. Copyright (C) Digia Plc 2013-2018. All rights reserved. Uses the BSD license.");

}
//! [12]

void Qt5Paint::onDrawTextClicked() {
	QString text_totype = QInputDialog::getText(this, "Мини-рисовалка", "Введите текст", QLineEdit::Normal, "Всем привет!");
	if (text_totype == "")
		return;
	else {
		QMessageBox::information(this, "Внимание", "Сейчас вам нужно будет в течении 2 секунд навести курсор в нужное место на экране. Именно рядом с ним будет написан текст. Нажмите \"ОК\", чтобы начать.", QMessageBox::Ok);
		WaitSecs(2000, true);
		QPoint p = scribbleArea->mapFromGlobal(QCursor::pos());
		scribbleArea->drawText(p, text_totype);
	}
}

void Qt5Paint::onSizeTextClicked() {
	int new_size = QInputDialog::getInt(this, "Мини-рисовалка", "Введите размер текста", scribbleArea->OVERRIDE_FONTSIZE, 4, 96, 1);
	scribbleArea->OVERRIDE_FONTSIZE = new_size;
}

void Qt5Paint::WaitSecs(int seconds, bool useMilliseconds) {
    QApplication::processEvents(QEventLoop::AllEvents, seconds * 1000);
}

//! [13]
void Qt5Paint::createActions()
//! [13] //! [14]
{
    sizeTextAct = new QAction("Размер шрифта текста", this);
    connect(sizeTextAct, SIGNAL(triggered()), this, SLOT(onSizeTextClicked()));

    drawTextAct = new QAction("Вставить текст", this);
    connect(drawTextAct, SIGNAL(triggered()), this, SLOT(onDrawTextClicked()));

    openAct = new QAction("Открыть", this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    //foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        QByteArray format = "png";
	QString text = tr("%1...").arg(QString(format).toUpper());

        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
    //}

    printAct = new QAction(tr("&Print..."), this);
    connect(printAct, SIGNAL(triggered()), scribbleArea, SLOT(print()));

    exitAct = new QAction("Выход", this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    penColorAct = new QAction("Цвет кисти", this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

    penWidthAct = new QAction("Размер кисти", this);
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

    clearScreenAct = new QAction("Очистить карточку", this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct, SIGNAL(triggered()),
            scribbleArea, SLOT(clearImage()));

    aboutAct = new QAction("О компоненте", this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}
//! [14]

//! [15]
void Qt5Paint::createMenus(bool showSaveChanges)
//! [15] //! [16]
{
    saveAsMenu = new QMenu("Сохранить в формате", this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);

    fileMenu = new QMenu("Файл", this);
    fileMenu->addAction(openAct);
    if (showSaveChanges == true)
    	fileMenu->addMenu(saveAsMenu);
    //fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    optionMenu = new QMenu("Инструменты и настройки", this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addSeparator();
    optionMenu->addAction(drawTextAct);
    optionMenu->addAction(sizeTextAct);
    optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAct);

    helpMenu = new QMenu("Помощь", this);
    helpMenu->addAction(aboutAct);
    //helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);
}
//! [16]

//! [17]
bool Qt5Paint::maybeSave()
//! [17] //! [18]
{
	if (doShowSaveChanges == true) {
 	   if (scribbleArea->isModified()) {
	       QMessageBox::StandardButton ret;
	       ret = QMessageBox::warning(this, "Мини-рисовалка",
				  "Сохранить изменения?",
	                          QMessageBox::Save | QMessageBox::Discard
	                          | QMessageBox::Cancel);
	        if (ret == QMessageBox::Save) {
	            return saveFile("png");
	        } else if (ret == QMessageBox::Cancel) {
	            return false;
	        }
	    }
    	    return true;
	}
    else {
        if (stillStarting == false)
            return saveFile("png");
        else
            return true;
    }
}
//! [18]

//! [19]
bool Qt5Paint::saveFile(const QByteArray &fileFormat)
//! [19] //! [20]
{
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    QString fileName;
    if (ftsave == "") {
        fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                   initialPath,
                                   tr("%1 Files (*.%2);;All Files (*)")
                                   .arg(QString::fromLatin1(fileFormat.toUpper()))
                                   .arg(QString::fromLatin1(fileFormat)));
    }
    else
        fileName = ftsave;
    if (fileName.isEmpty()) {
        return false;
    } else {
        return scribbleArea->saveImage(fileName, fileFormat.constData());
    }
}
//! [20]
