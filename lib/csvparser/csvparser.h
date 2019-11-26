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
#include <QFile>
#include <QString>
#include <QStringList>
#include <QList>
#include <QFileInfo>
using namespace std;

class CSVParser {
	private:
		QString usedFilename;
		QList<QStringList> values;
		bool FileExists(QString name) const;
                QString ProbeFileSeparator(QString filename);
                QString FixLine(QString line, QString separator);
	public:
		CSVParser(QString filename);
		int ReadFile();
        QList<QStringList> GetAllValues() const;
        QString GetValue(int row, int column) const;
		~CSVParser();
		int SetValue(int row, int column, QString value);
        int WriteCSV() const;
        int WriteCSV(QString outFile) const;
        int GetVariantsCount() const;
        int GetQuestionsCount() const;
        QString GetBaseDirectory() const;
        QString GetRealAnswer(int question_number, int variant) const;
        int GetQuestionRank(int variant) const;
	
};
