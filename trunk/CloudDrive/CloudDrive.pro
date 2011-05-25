#-------------------------------------------------
#
# Project created by QtCreator 2011-04-23T16:56:55
#
#-------------------------------------------------

QT       += core gui phonon network

TARGET = CloudDrive
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        amazonwebsite.cpp \
        htmlparser.cpp \
        qjson/src/serializerrunnable.cpp \
        qjson/src/serializer.cpp \
        qjson/src/qobjecthelper.cpp \
        qjson/src/parserrunnable.cpp \
        qjson/src/parser.cpp \
        qjson/src/json_scanner.cpp \
        qjson/src/json_parser.cc \
    formpost.cpp \
    utils.cpp \
    javascriptparser.cpp \
    userconfigdialog.cpp \
    httppostsendbuffer.cpp \
    jsonoperation.cpp \
    downloadqueue.cpp \
    uploadqueue.cpp \
    deferredmimedata.cpp \
    filetablewidget.cpp


HEADERS  += mainwindow.h \
        amazonwebsite.h \
        htmlparser.h \
        qjson/src/json_parser.hh \
        qjson/src/location.hh \
        qjson/src/position.hh \
        qjson/src/stack.hh \
        qjson/src/json_scanner.h \
        qjson/src/parser.h \
        qjson/src/parser_p.h \
        qjson/src/parserrunnable.h \
        qjson/src/qjson_debug.h \
        qjson/src/qjson_export.h \
        qjson/src/qobjecthelper.h \
        qjson/src/serializer.h \
        qjson/src/serializerrunnable.h \
    formpost.h \
    utils.h \
    javascriptparser.h \
    userconfigdialog.h \
    httppostsendbuffer.h \
    jsonoperation.h \
    generalconfig.h \
    downloadqueue.h \
    uploadqueue.h \
    deferredmimedata.h \
    filetablewidget.h

FORMS    += mainwindow.ui \
    userconfigdialog.ui

wince*{
    DEPLOYMENT_PLUGIN += phonon_ds9 phonon_waveout
}

symbian:TARGET.UID3 = 0xA000CF6A
