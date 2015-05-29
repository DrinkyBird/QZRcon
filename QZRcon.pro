#-------------------------------------------------
#
# QZrcon QMake File
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = QZRcon
TEMPLATE = app
RC_FILE = qzrcon.rc

SOURCES += \
    src/mainwindow.cpp \
    src/main.cpp \
    src/rcon.cpp \
    src/huffman.cpp \
    src/rconwindow.cpp \
    src/aboutwindow.cpp \
    src/kickwindow.cpp \
    src/banwindow.cpp \
    src/mutewindow.cpp

HEADERS  += \
    src/mainwindow.h \
    src/rcon.h \
    src/huffman.h \
    src/rconwindow.h \
    src/common.h \
    src/aboutwindow.h \
    src/kickwindow.h \
    src/banwindow.h \
    src/mutewindow.h \
    version.h \
    src/version.h

FORMS    += \
    src/mainwindow.ui \
    src/rconwindow.ui \
    src/aboutwindow.ui \
    src/kickwindow.ui \
    src/banwindow.ui \
    src/mutewindow.ui

OTHER_FILES += \
    QZrcon.txt \
    Changelog.txt \
    RconProtocol.html
