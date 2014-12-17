#-------------------------------------------------
#
# Project created by QtCreator 2014-12-12T22:03:17
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = prolog-server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    prologserver.cpp \
    prologthread.cpp

HEADERS += \
    prologserver.h \
    prologthread.h
