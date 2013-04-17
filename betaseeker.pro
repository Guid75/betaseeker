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
    showlistmodel.cpp \
    showmanager.cpp \
    show.cpp \
    requestmanager.cpp \
    jsonparser.cpp \
    season.cpp \
    episode.cpp \
    databasemanager.cpp

HEADERS  += mainwindow.h \
    showlistmodel.h \
    showmanager.h \
    show.h \
    requestmanager.h \
    jsonparser.h \
    season.h \
    episode.h \
    databasemanager.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    TODO.txt
