#include "server-components/QtHTTPServer.h"
#include "testmaker_request_handler/TestMkRequestHandler.h"

QtHTTPServer::QtHTTPServer(QObject *parent, QString baseDir, QString tFile) : QTcpServer(parent) {
    priv__BASEDIR = baseDir;
    priv__TFILE = tFile;
}

void QtHTTPServer::run2() {
    if (listen(QHostAddress::Any, 8000)) {
        qDebug() << "Server is working...";
    }
    else {
        qDebug() << "Error while initializing server. " << errorString();
    }
}

void QtHTTPServer::run() {
#ifndef CONCURRENT_SERVER
    stillRunning = true;
    run2();
#else
    stillRunning = true;
    priv__FTRTHREAD = QtConcurrent::run(run2());
#endif
}

void QtHTTPServer::WaitSecsWithoutLimit(int seconds, bool useMilliseconds) {
#ifndef STANDALONE_BUILD
    QTime time_to_stop;
    if (useMilliseconds == true)
        time_to_stop = QTime::currentTime().addMSecs(seconds);
    else
        time_to_stop = QTime::currentTime().addSecs(seconds);
    while (QTime::currentTime() < time_to_stop) {
            QApplication::processEvents(QEventLoop::AllEvents, 5);
   }
#else
   return;
#endif
}

void QtHTTPServer::incomingConnection(int handle) {
	QTcpSocket* socket = new QTcpSocket();
	socket->setSocketDescriptor(handle);
	
	qDebug() << "Incoming connection";
	connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void QtHTTPServer::stop() {
#ifndef CONCURRENT_SERVER
    stillRunning = false;
#  ifdef QT5_BUILD
    this->~QTcpServer();
#  else
    this->~QtHTTPServer();
#  endif
#else
    stillRunning = false;
    priv__FTRTHREAD.cancel();
#endif
}

void QtHTTPServer::onReadyRead() {
	TestMkRequestHandler ctrl_car;
    ctrl_car.SetTestBaseFolder(priv__BASEDIR);
    ctrl_car.SetTestFileName(priv__TFILE);
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
	QString request = socket->readAll();
	qDebug() << request;
	
	QByteArray response;
	if (request.contains("TESTMK32") && (request.contains("TestMakerPlus/") || request.contains("TestMakerMOBI/Any"))) {
		response = ctrl_car.ParseRequest(request.split("\r\n")[0]);
		response.prepend(QString("HTTP/1.0 200 OK\r\n\r\n").toLocal8Bit());
		socket->write(response);
		qDebug() << "Using desktop/mobile version of TestMaker Plus test system, sending normal response";
	}
	else {
		socket->write("HTTP/1.0 200 OK\r\n\r\nPlease, enter this URL into TestMaker Plus test system. It is not designed to be viewed with a web browser.");
		qDebug() << "Using a web browser, displayed an error message";
	}
	socket->disconnectFromHost();
}

void QtHTTPServer::onDisconnected() {
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
	socket->close();
	socket->deleteLater();
}
