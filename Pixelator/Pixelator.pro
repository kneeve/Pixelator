#-------------------------------------------------
#
# Project created by QtCreator 2018-11-01T15:02:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pixelator
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    frame.cpp \
    clickablelabel.cpp

HEADERS += \
        mainwindow.h \
    frame.h \
    clickablelabel.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    square.png \
    square.png \
    addFrame.png \
    circle.png \
    dot.png \
    eraser.png \
    paintbucket.png \
    pencil.png \
    square.png \
    trash.png
