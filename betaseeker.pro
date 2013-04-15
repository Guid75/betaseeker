#-------------------------------------------------
#
# Project created by QtCreator 2013-04-12T10:05:44
#
#-------------------------------------------------

QT       += core gui network

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
    episode.cpp

HEADERS  += mainwindow.h \
    showlistmodel.h \
    showmanager.h \
    show.h \
    requestmanager.h \
    jsonparser.h \
    season.h \
    episode.h

FORMS    += mainwindow.ui
