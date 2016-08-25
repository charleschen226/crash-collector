#-------------------------------------------------
#
# Project created by QtCreator 2016-08-24T15:58:31
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CrashDialog
TEMPLATE = app

DESTDIR += $$PWD/bin

SOURCES += main.cpp\
        CrashDialog.cpp

HEADERS  += CrashDialog.h

FORMS    += CrashDialog.ui

LIBS += -lDbgHelp

#QMAKE_LFLAGS+=-Wl,-Map=$${TARGET}.map

RESOURCES += \
    icon.qrc
