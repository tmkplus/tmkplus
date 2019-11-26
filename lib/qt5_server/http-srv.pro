QT       += core network concurrent
QT       -= gui

TARGET = konstrhttp
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += STANDALONE_BUILD

SOURCES += main.cpp
SOURCES += server-components/QtHTTPServer.cpp
HEADERS += server-components/QtHTTPServer.h
SOURCES += testmaker_request_handler/TestMkRequestHandler.cpp
HEADERS += testmaker_request_handler/TestMkRequestHandler.h
