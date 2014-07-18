#-------------------------------------------------
#
# Project created by QtCreator 2014-06-26T15:21:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GoATLive
TEMPLATE = app

DESTDIR=bin #Target file directory
OBJECTS_DIR=generated_files #Intermediate object files directory
MOC_DIR=generated_files #Intermediate moc files directory

SOURCES += main.cpp\
    MainWindow.cpp \
    ConfigurationDialog.cpp \
    ConfigFile.cpp \
    TabRunByRun.cpp \
    TabComplete.cpp \
    ConfigGui.cpp \
    TabLog.cpp \
    QueueEditDialog.cpp

HEADERS  += \
    MainWindow.h \
    ConfigurationDialog.h \
    ConfigFile.h \
    TabRunByRun.h \
    TabComplete.h \
    ConfigGui.h \
    TabLog.h \
    Functions.h \
    QueueEditDialog.h

FORMS    += \
    configurationdialog.ui \
    mainwindow.ui \
    tabrunbyrun.ui \
    tabcomplete.ui \
    tablog.ui \
    queueeditdialog.ui


include("$(ROOTSYS)/include/rootcint.pri")

QMAKE_CXXFLAGS += -std=c++0x
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

RESOURCES += \
    Resources.qrc \
    icons.qrc

QMAKE_CXXFLAGS += -Wno-write-strings
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-function


