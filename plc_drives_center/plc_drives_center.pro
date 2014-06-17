#-------------------------------------------------
#
# Project created by QtCreator 2012-03-30T08:27:45
#
#-------------------------------------------------

QT       -= core gui

TARGET = plc_drives_center
TEMPLATE = lib

SOURCES += \
    tools/plcmathmethod.cpp \
    tools/dealwithstring.cpp \
    managecenter.cpp \
    addrmanage.cpp \
    androidjniinterface.cpp \
    openlibinterface.cpp \
    localaddrs.cpp \
    dealjnimethod.cpp

HEADERS += \
    tools/stdafx.h \
    tools/dealwithstring.h \
    tools/structHead.h \
    tools/enumhead.h \
    tools/plcmathmethod.h \
    managecenter.h \
    addrmanage.h \
    androidjniinterface.h \
    openlibinterface.h \
    localaddrs.h \
    dealjnimethod.h
