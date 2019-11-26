#ifndef TESTMKREQUESTHANDLER_H
#define TESTMKREQUESTHANDLER_H
#include <QObject>
#include <QList>
#include <QStringList>
class TestMkRequestHandler : public QObject {
	Q_OBJECT
	public:
		explicit TestMkRequestHandler(QObject *parent = 0);
		void SetTestFileName(QString fn);
		void SetTestBaseFolder(QString bf);
		QByteArray ParseRequest(QString request);
		QString testShortFileName;
		QString testBaseFolder;

	
	signals:
		
	public slots:

};
#endif
