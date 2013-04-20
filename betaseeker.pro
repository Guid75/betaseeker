#-------------------------------------------------
#
# Project created by QtCreator 2013-04-12T10:05:44
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = betaseeker
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    showmanager.cpp \
    requestmanager.cpp \
    jsonparser.cpp \
    databasemanager.cpp

HEADERS  += mainwindow.h \
    showmanager.h \
    requestmanager.h \
    jsonparser.h \
    databasemanager.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    TODO.txt
