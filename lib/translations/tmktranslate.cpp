// tmktranslate.h - translation library for TestMaker Plus
#include <tmktranslate.h>

QString tmktranslate(const char* instr) {
	QString availableTranslationsLocation = QFileInfo(QCoreApplication::applicationFilePath()).dir().canonicalPath() + "/../Translations";
	qDebug() << "all translations should be in " << availableTranslationsLocation;
#ifndef Q_OS_WIN
	QString thisLang = qgetenv("LANG");
	if (thisLang == "C")
		thisLang = "en_US.utf8";
#else
	QString thisLang = "en_US.utf8";
	if (GetUserDefaultLCID() == 1049)
		thisLang = "ru_RU.utf8";
#endif
	thisLang = thisLang.toLower();
	if (thisLang.contains("."))
		thisLang = thisLang.split(".", QString::SkipEmptyParts).at(0);
	if (thisLang.contains("_"))
		thisLang = thisLang.split("_", QString::SkipEmptyParts).at(0);
	qDebug() << "current language is " << thisLang;
	QString langFilePath = availableTranslationsLocation + "/" + thisLang + ".tmklocale";
	if (QFile::exists(langFilePath) == false)
		return QString::fromUtf8(instr);
	QString fcontents = "";
	QFile* myfile = new QFile(langFilePath);
	if (myfile->open(QIODevice::ReadOnly | QIODevice::Text) == true)
		fcontents = QString(myfile->readAll());
	else {
		myfile->close();
		delete myfile;
		return QString::fromUtf8(instr);
	}
	myfile->close();
	delete myfile;
    QString awaitedStringToLower = QString::fromUtf8(instr).toLower().replace("\r\n", "\n").replace("\n", "[sep]n");
	fcontents.replace("\r\n", "\n");
	QStringList qstrlist = fcontents.split("\n");
	qDebug() << "locale list is " << qstrlist;
	for (int i = 0; i < qstrlist.size(); i++) {
		if (qstrlist.at(i).at(0) == '#' || qstrlist.at(i).isEmpty() == true)
			qDebug() << "skip line " << i << ", it's a comment";
		else if (qstrlist.at(i).toLower() == "loopback") {
			qDebug() << "this is a loopback locale, so let's return the original value";
			return QString::fromUtf8(instr);
		}
		else if (qstrlist.at(i).contains("=")) {
			QString sideone = qstrlist.at(i).split("=").at(0).toLower();
			QString sidetwo = qstrlist.at(i).split("=").at(1);
            sidetwo.replace("[sep]n", "\n");
			if (sideone == awaitedStringToLower) {
				qDebug() << "found the correct translation on line " << i << ", returning it";
				return sidetwo;
			}
		}
		else
			TMKTRANSLATE_ERRORDUMP << "Syntax error at line " << (i + 1) << " in locale " << qstrlist.at(i).toStdString() << ", please contact the author of this translation";
	}
    return QString::fromUtf8(instr);
}

QString tmktr(const char* instr) { return tmktranslate(instr); }
