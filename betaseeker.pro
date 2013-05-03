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
    settings.cpp \
    episodemodel.cpp \
    subtitlemodel.cpp \
    linkdelegate.cpp \
    commandmanager.cpp \
    downloadmanager.cpp

HEADERS  += mainwindow.h \
    showmanager.h \
    requestmanager.h \
    jsonparser.h \
    databasemanager.h \
    showdetailwidget.h \
    loadingwidget.h \
    settings.h \
    episodemodel.h \
    subtitlemodel.h \
    linkdelegate.h \
    commandmanager.h \
    downloadmanager.h

FORMS    += mainwindow.ui \
    showdetailwidget.ui \
    loadingwidget.ui

OTHER_FILES += \
    TODO.txt \
    COPYING

RESOURCES += \
    icons.qrc
