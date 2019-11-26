#include "testmaker_request_handler/TestMkRequestHandler.h"
#include <QFile>
#include <QString>
#include <QIODevice>
#include <QTextStream>

TestMkRequestHandler::TestMkRequestHandler(QObject *parent) : QObject(parent) {
}

void TestMkRequestHandler::SetTestFileName(QString fn) {
	testShortFileName = fn;
}

void TestMkRequestHandler::SetTestBaseFolder(QString bf) {
	testBaseFolder = bf.replace("\\", "/");
}

QByteArray TestMkRequestHandler::ParseRequest(QString request) {
	QStringList request_split = request.split(" ");
	if (request_split[0] != "GET")
		return QString("error").toLocal8Bit();
	QString action = request_split[1].replace("%20", " ").replace("%D0", "").replace("%BD", "");
	if (action == "/")
#ifdef USE_HTML_OUTPUT
		return "<meta encoding=\"utf-8\" /><p>Здравствуйте! Это - сервер автоматизированной тестирующей системы TestMaker Plus. Зайти через веб-браузер, чтобы выполнить этот тест, не получится, но возможно это сделать, установив TestMaker Plus себе на компьютер и введя этот адрес в окно, которое появится при нажатии на кнопку \"Скачать тест из Интернета\".</p>";
#else
		return "The following test is not meant to be accessed from a web browser. To complete it, use the TestMaker Plus test system.";
#endif
	else if (QFile::exists(testBaseFolder + "/" + action)) {
			QFile readfiletoout(testBaseFolder + "/" + action);
			QByteArray to_ret = QString("error").toLocal8Bit();
			if (readfiletoout.open(QIODevice::ReadOnly))
				to_ret = readfiletoout.readAll();
			else
				to_ret = QString("error").toLocal8Bit();
			readfiletoout.close();
			return to_ret;
	}
	else if (action == "/test_connection.mmp")
		return QString("ready").toLocal8Bit();
	else if (action.split("?")[0] == "/tres.mmp") {
		QString arg_enc = action.split("?")[1];
		QFile writethefileout(testBaseFolder + "/test_result.csv");
		if (writethefileout.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
			QTextStream stream(&writethefileout);
			stream << arg_enc << endl;
			writethefileout.close();
			return QString("ready").toLocal8Bit();
		}
		else {
			writethefileout.close();
			return QString("error").toLocal8Bit();
		}
	}
	return QString("error").toLocal8Bit();
}
