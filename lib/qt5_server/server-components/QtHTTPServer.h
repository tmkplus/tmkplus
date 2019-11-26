#ifndef QTHTTPSERVER_H
#define QTHTTPSERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QDateTime>
#ifdef CONCURRENT_SERVER
#include <QFuture>
#include <QtConcurrent>
#endif
#ifndef STANDALONE_BUILD
#include <QApplication>
#endif

class QtHTTPServer : public QTcpServer {
	Q_OBJECT

	public:
        explicit QtHTTPServer(QObject *parent, QString baseDir, QString tFile);
		void incomingConnection(int handle);
        bool stillRunning;
        void stop();
        void run();

	signals:

    private:
        QString priv__BASEDIR;
        QString priv__TFILE;
#ifdef CONCURRENT_SERVER
        QFuture<void> priv__FTRTHREAD;
#endif
        void run2();
        void WaitSecsWithoutLimit(int seconds, bool useMilliseconds);

	public slots:
		void onReadyRead();
		void onDisconnected();

};
#endif
