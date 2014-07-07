#-------------------------------------------------
#
# Project created by QtCreator 2014-06-26T15:21:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GoATLive
TEMPLATE = app


SOURCES += main.cpp\
    MainWindow.cpp \
    ConfigurationDialog.cpp \
    ConfigFile.cpp \
    TabRunByRun.cpp \
    TabComplete.cpp \
    Functions.cpp \
    ConfigGui.cpp \
    TabLog.cpp

HEADERS  += \
    MainWindow.h \
    ConfigurationDialog.h \
    ConfigFile.h \
    TabRunByRun.h \
    TabComplete.h \
    ConfigGui.h \
    TabLog.h

FORMS    += \
    configurationdialog.ui \
    mainwindow.ui \
    tabrunbyrun.ui \
    tabcomplete.ui \
    tablog.ui


include("$(ROOTSYS)/include/rootcint.pri")

QMAKE_CXXFLAGS += -std=c++0x
