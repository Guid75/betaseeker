#-------------------------------------------------
#
# Project created by QtCreator 2013-04-12T10:05:44
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = betaseeker
TEMPLATE = app

DEFINES += QUAZIP_STATIC
win32:DEFINES += ZLIB_WINAPI

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
    downloadmanager.cpp \
    episodefinder.cpp

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
    downloadmanager.h \
    episodefinder.h

FORMS    += mainwindow.ui \
    showdetailwidget.ui \
    loadingwidget.ui

OTHER_FILES += \
    TODO.txt \
    COPYING

RESOURCES += \
    icons.qrc

INCLUDEPATH += $$PWD/../quazip-0.5.1/quazip
DEPENDPATH += $$PWD/../quazip-0.5.1/quazip

win32:INCLUDEPATH += $$PWD/../zlib-1.2.8
# win32:LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_1_1_MSVC2012_OpenGL_64bit-Release/quazip/release -lquazip -lz
win32:LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_1_1_MSVC2012_OpenGL_64bit-Release/quazip/release -lquazip -L$$PWD/../zlib-1.2.8/contrib/vstudio/vc11/x64/ZlibDllRelease -lzlibwapi

unix:!macx: LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_1_0_MinGW_32bit-Release/quazip/ -lquazip -lz
unix:!macx: PRE_TARGETDEPS += $$PWD/../build-quazip-Desktop_Qt_5_1_0_MinGW_32bit-Release/quazip/libquazip.a
