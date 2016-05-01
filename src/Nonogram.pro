#-------------------------------------------------
#
# Project created by QtCreator 2015-12-01T22:05:16
#
#-------------------------------------------------

QT       += core gui concurrent
CONFIG   += c++11

Debug:QT+=testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Nonogram
TEMPLATE = app

SOURCES += \
    main.cpp\
    nonogrammodel.cpp \
    nonogram.cpp \
    test/testnonogram.cpp \
    UI/exportimagedialog.cpp \
    UI/mainwindow.cpp \
    UI/nonogramtabledelegate.cpp \
    UI/numberlineedit.cpp

HEADERS  += \
    nonogrammodel.h \
    nonogram.h \
    test/testnonogram.h \
    UI/exportimagedialog.h \
    UI/mainwindow.h \
    UI/nonogramtabledelegate.h \
    UI/numberlineedit.h

FORMS    += \
    UI/exportimagedialog.ui \
    UI/mainwindow.ui \
    UI/newfiledialog.ui

DISTFILES += \
    translations.ts
