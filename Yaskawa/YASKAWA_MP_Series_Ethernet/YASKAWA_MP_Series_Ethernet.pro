#-------------------------------------------------
#
# Project created by QtCreator 2012-03-30T08:27:45
#
#-------------------------------------------------

QT       -= core gui

TARGET = YASKAWA_MP_Series_Ethernet
TEMPLATE = lib

SOURCES += \
    tools/dealwithstring.cpp \
    tools/plcmathmethod.cpp \
    drive/plcprotocol.cpp \
    drive/datacenter.cpp \
    compileinterface.cpp \
    driveinterface.cpp

HEADERS += \
    tools/stdafx.h \
    tools/structHead.h \
    tools/enumhead.h \
    tools/dealwithstring.h \
    tools/plcmathmethod.h \
    drive/datacenter.h \
    drive/plcprotocol.h
