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
    databasemanager.cpp \
    showdetailwidget.cpp \
    loadingwidget.cpp \
    subtitlewidget.cpp \
    episodewidget.cpp \
    seasonwidget.cpp \
    settings.cpp

HEADERS  += mainwindow.h \
    showmanager.h \
    requestmanager.h \
    jsonparser.h \
    databasemanager.h \
    showdetailwidget.h \
    loadingwidget.h \
    subtitlewidget.h \
    episodewidget.h \
    seasonwidget.h \
    settings.h

FORMS    += mainwindow.ui \
    showdetailwidget.ui \
    loadingwidget.ui \
    episodewidget.ui \
    subtitlewidget.ui

OTHER_FILES += \
    TODO.txt

RESOURCES += \
    icons.qrc
