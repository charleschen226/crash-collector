#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T16:36:46
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app

DESTDIR += $$PWD/bin

SOURCES += main.cpp\
        mainwindow.cpp \
    CrashHandle.cpp

HEADERS  += mainwindow.h \
    CrashHandle.h

FORMS    += mainwindow.ui

LIBS += -lDbgHelp

QMAKE_LFLAGS+=-Wl,-Map=$${TARGET}.map
