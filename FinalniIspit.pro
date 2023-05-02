#-------------------------------------------------
#
# Project created by QtCreator 2023-04-11T22:01:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FinalniIspit
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        dialog.cpp \
    proces.cpp

HEADERS  += dialog.h \
    proces.h

FORMS    += dialog.ui
