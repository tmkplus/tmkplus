// tmktranslate.h - translation library for TestMaker Plus
#ifndef TMKTRANSLATE_H
#define TMKTRANSLATE_H
#include <iostream>
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QStringList>
#include <QCoreApplication>
#include <QtDebug>
#include <string>
#include <iostream>
#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#define TMKTRANSLATE_ERRORDUMP std::cerr

QString tmktranslate(const char* instr);
QString tmktr(const char* instr);
#endif
