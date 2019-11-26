/*
Copyright (c) 2018, Tim K 2017-2018 <timprogrammer@rambler.ru>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the TestMaker Plus project.
*/

#include <stdio.h>
#include "csvparser.h"
#include <stdio.h>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QList>
#include <QTextCodec>
#include <QFileInfo>
#include <QDebug>
#include <QTextStream>
using namespace std;

CSVParser::CSVParser(QString filename) {
	usedFilename = filename;
}

int CSVParser::ReadFile() {
    QString splitOperator = ProbeFileSeparator(usedFilename);
#ifndef MODERN_FILE_READER
	QFile inputFile(usedFilename.toUtf8());
	if (inputFile.open(QIODevice::ReadOnly)) {
		QTextStream text(&inputFile);
		if (QString(text.readAll()).at(0) == "�")
			text.setCodec("Windows-1251");
		while(text.atEnd() == false) {
			QString line = text.readLine();
                        //line = FixLine(line, splitOperator);
                        QStringList listOfVals = line.replace("\"", "").split(splitOperator, QString::SkipEmptyParts);
			values.append(listOfVals);
		}
		inputFile.close();
		return 0;
	}
	else
		return 1;
#else
    int to_ret = 0;
	QFile inputFile(usedFilename.toUtf8());
	if (inputFile.open(QIODevice::ReadOnly)) {
# ifdef DO_NOT_USE_QTEXTCODEC
		QTextStream text(&inputFile);
		QString orig = text.readAll();
                QStringList or_values = orig.replace("\r\n", "\n").split("\n", QString::SkipEmptyParts);
		for (int i = 0; i < or_values.size(); i++) {    
                    QStringList listOfVals = FixLine(or_values[i], splitOperator).replace("\"", "").split(splitOperator, QString::SkipEmptyParts);
                    for (int j = 0; j < listOfVals.length(); j++)
                        listOfVals[j] = listOfVals[j].replace("^", splitOperator);
                    if (listOfVals.length() > 1)
                        values.append(listOfVals);
		}
                to_ret = 0;
# else
		QTextCodec::ConverterState state;
		QTextCodec *codec = QTextCodec::codecForName("UTF-8");
		QTextStream text(&inputFile);
		QByteArray data = text.readAll().toLocal8Bit();
		QString orig = codec->toUnicode(data.constData(), data.size(), &state);
		if (state.invalidChars > 0) {
			//codec->deleteLater();
			qDebug() << "CSVParser: invalid chars";
			QTextCodec *codec2 = QTextCodec::codecForLocale();
			if (!codec2)
				to_ret = 1;
			orig = codec2->toUnicode(text.readAll().toLocal8Bit());
			qDebug() << orig;
			//codec2->deleteLater();
		}
                QStringList or_values = orig.replace("\r\n", "\n").split("\n", QString::SkipEmptyParts);
		for (int i = 0; i < or_values.size(); i++) {    
                    QStringList listOfVals = FixLine(or_values[i], splitOperator).replace("\"", "").split(splitOperator, QString::SkipEmptyParts);
                    for (int j = 0; j < listOfVals.length(); j++)
                        listOfVals[j] = listOfVals[j].replace("^", splitOperator);
                    if (listOfVals.length() > 1)
                        values.append(listOfVals);
		}
                to_ret = 0;
# endif
	}
    else
        to_ret = 1;
    inputFile.close();
    return to_ret;
#endif
}


QList<QStringList> CSVParser::GetAllValues() const {
	return values;
}

QString CSVParser::GetValue(int row, int column) const {
    //qDebug() << "CSVParser::GetValue(int row = " << row << ", int column = " << column << ")";
    if (row > (values.count()))
		return "";
	int real_val_row = row - 1;
	if (real_val_row < 0)
		return "";
	QStringList row2 = values[real_val_row];
    if (column > (row2.count()))
		return "";
	int real_val_column = column - 1;
	if (real_val_column < 0)
		return "";
    if (row2.count() > real_val_row)
        return row2[real_val_row];
    else
        return ";";
}

int CSVParser::GetVariantsCount() const {
    int vcount = (values[0].count() - 2);
    qDebug() << "int vcount = " << vcount; 
    return vcount;
}

int CSVParser::GetQuestionsCount() const {
    int qcount = (values.count() - 1);
    qDebug() << "int qcount = " << qcount; 
    return (values.count() - 1);
}

QString CSVParser::GetBaseDirectory() const {
    QFileInfo info_about_file(usedFilename);
    QString to_return = info_about_file.path();
    return to_return;
}

int CSVParser::SetValue(int row, int column, QString value) {
	if (row > (values.count() - 1))
                return 1;
        int real_val_row = row - 1;
        if (real_val_row < 0)
                return 1;
        QStringList row2 = values[real_val_row];
        if (column > (row2.count() - 1))
                return 1;
        int real_val_column = column - 1;
        if (real_val_column < 0)
                return 1;
        /*
        try {
             	values[real_val_row][real_val_column] = value;
		return 0;
        }
	catch (...) {
                return 1;
        }
        */
        if (values.count() > real_val_row && values[real_val_row].count() > real_val_column) {
            values[real_val_row][real_val_column] = value;
           return 0;
        }
        return 1;
}

bool CSVParser::FileExists(QString name) const {
	//std::ifstream infile(name);
	//return infile.good();
	QFileInfo check_file(name);
	if (check_file.exists() && check_file.isFile())
		return true;
	else
		return false;
}

int CSVParser::GetQuestionRank(int question_number) const {
    /*
    try {
        QStringList row = values[question_number];
        QString temp = row.last();
        return temp.toInt();
    } catch (...) {
        return -1;
    }
    */
    if (values.count() > question_number)
        return QString(values[question_number].last()).toInt();
    else
        return -1;
}

int CSVParser::WriteCSV(QString outFile) const {
	if (this->FileExists(outFile) == true) {
		QFile tmp_file;
		try {
			tmp_file.remove(outFile);
		}
		catch (...) {
			return 1;
		}
		tmp_file.close();
	}
	QFile file(outFile.toUtf8());
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream stream(&file);
		for (int i = 0; i < values.size(); i++) {
			QStringList list_temp = values[i];
			QString to_write = "";
			for (int j = 0; j < list_temp.size(); j++) {
				if (to_write == "")
					to_write = list_temp[j];
				else
					to_write = to_write + "," + list_temp[j];
			}
			stream << to_write << endl;
		}
		stream.flush();
		//stream.close();
		file.close();
		return 0;
	}
	else {
		return 1;
	}
}

int CSVParser::WriteCSV() const {
	int to_ret = this->WriteCSV(usedFilename);
	return to_ret;
}

CSVParser::~CSVParser() {
	
}

QString CSVParser::GetRealAnswer(int question_number, int variant) const {
    if (values.count() < 1)
        return "";
    int column = 0;
    const QStringList &row1 = values[0];
    for (int j = 0; j < row1.count(); j++) {
        if (QString::number(variant) == row1[j]) {
            column = j + 1;
            j = row1.count();
        }
    }
    if (column == 0)
        return "";
    int row = 0;
    for (int i = 0; i < values.count(); i++) {
        if (GetValue(i + 1, 1) == QString::number(question_number)) {
            row = i + 1;
            i = values.count();
        }
    }
    if (row == 0)
        return "";

    return GetValue(row, column);
}

QString CSVParser::ProbeFileSeparator(QString filename) {
    QString to_ret = "none";
    try {
        QFile fileOpened(filename);
        fileOpened.open(QIODevice::ReadOnly | QIODevice::Text);
        QString fileContents = fileOpened.readAll();
        QString firstLine = fileContents.replace("\r\n", "\n").split("\n", QString::SkipEmptyParts).at(0);
        bool inQuotes = false;
        bool stopLoop = false;
        for (int i = 0; i < firstLine.length(); i++) {
            QString currentChar = firstLine.at(i);
            if (currentChar == "\"") {
                if (inQuotes == true)
                    inQuotes = false;
                else
                    inQuotes = true;
            }
            if (to_ret == "none") {
                if (currentChar == "," && inQuotes == false)
                    to_ret = ",";
                else if (currentChar == ";" && inQuotes == false)
                    to_ret = ";";
                else if (currentChar == ":" && inQuotes == false)
                    to_ret = ":";

                if (to_ret != "none")
                    stopLoop = true;
            }
            if (stopLoop == true)
                i = firstLine.length() + 1;
        }
        fileOpened.close();
    }
    catch (...) {
        to_ret = ";";
    }
    return to_ret;
}

QString CSVParser::FixLine(QString line, QString separator) {
    bool inQuotes = false;
    QString to_ret = line;
    for (int i = 0; i < to_ret.size(); i++) {
        QChar currentChar = to_ret.at(i);
        if (currentChar == '\"') {
            if (inQuotes == false)
                inQuotes = true;
            else
                inQuotes = false;
        }
        if (currentChar == separator.at(0) && inQuotes == true)
            to_ret[i] = '^';
    }
    //QMessageBox::information(0, "Info", to_ret, QMessageBox::Ok);
    return to_ret;
}
