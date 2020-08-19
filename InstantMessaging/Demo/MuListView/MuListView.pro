#-------------------------------------------------
#
# Project created by QtCreator 2018-11-21T13:19:56
#
#-------------------------------------------------

QT       += core gui

#QMAKE_CXXFLAGS += -execution-charset:gbk

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MuListView
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
    dialog.cpp \
        main.cpp \
        Widget.cpp \
    MuItemDelegate.cpp \
    source/src/CharacterCodes.cpp \
    source/src/JsonUtility.cpp \
    source/src/Server.cpp \
    source/src/SocketUtility.cpp \
    userclient.cpp

HEADERS += \
        Widget.h \
    MuItemDelegate.h \
    MuListItemData.h \
    dialog.h \
    source/src/CharacterCodes.h \
    source/src/JsonUtility.h \
    source/src/Server.h \
    source/src/SocketUtility.h \
    userclient.h

FORMS += \
        Widget.ui \
        dialog.ui

RESOURCES += \
    Style.qrc \
    imgs.qrc

win32: LIBS += -L$$PWD/source/lib/ -ljsoncpp

INCLUDEPATH += $$PWD/source/include
DEPENDPATH += $$PWD/source/include

INCLUDEPATH += $$PWD/source/src

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/source/lib/jsoncpp.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/source/lib/libjsoncpp.a
