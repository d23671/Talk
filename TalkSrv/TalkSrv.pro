#-------------------------------------------------
#
# Project created by QtCreator 2010-12-12T09:23:52
#
#-------------------------------------------------

QT += network
TARGET = talksrv

INCLUDEPATH += ../common

SOURCES += main.cpp \
    serverwindow.cpp \
    ../common/protocolparser.cpp \
    ../common/addrselectiondlg.cpp

HEADERS += \
    serverwindow.h \
    ../common/addrselectiondlg.h \
    ../common/protocolparser.h
