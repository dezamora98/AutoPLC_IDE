#-------------------------------------------------
#
# Project created by QtCreator 2021-11-26T01:25:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutoIDE
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
    code_editor.cpp \
    control.cpp \
    main.cpp \
    mainwindow.cpp \
    monitor.cpp \
    hid.c \
    hexcode.cpp \
    usbtransfer.cpp

HEADERS += \
    code_editor.h \
    control.h \
    hidapi.h \
    mainwindow.h \
    monitor.h \
    hexcode.h \
    usbtransfer.h

FORMS += \
    code_editor.ui \
    control.ui \
    mainwindow.ui \
    monitor.ui \
    prog_menu.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    iconos.qrc

QT += charts
QT += core gui charts


QMAKE_CXXFLAGS += -std=gnu++17

LIBS += -lsetupapi




