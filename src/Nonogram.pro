#-------------------------------------------------
#
# Project created by QtCreator 2015-12-01T22:05:16
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Nonogram
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    nonogrammodel.cpp \
    nonogramtabledelegate.cpp \
    nonogram.cpp

HEADERS  += mainwindow.h \
    nonogrammodel.h \
    nonogramtabledelegate.h \
    nonogram.h

FORMS    += mainwindow.ui \
    newfiledialog.ui
