#-------------------------------------------------
#
# Project created by QtCreator 2010-12-12T13:01:18
#
#-------------------------------------------------

QT += network
TARGET = talkclient
#CONFIG += static

INCLUDEPATH += ../common

SOURCES += main.cpp \
    clientwindow.cpp \
    ../common/addrselectiondlg.cpp \
    ../common/protocolparser.cpp \
    regsettingsdlg.cpp \
    chatdialog.cpp

HEADERS += \
    clientwindow.h \
    ../common/addrselectiondlg.h \
    regsettingsdlg.h \
    chatdialog.h \
    ../common/protocolparser.h
