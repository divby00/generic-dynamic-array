TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle qt
QMAKE_CC=clang
QMAKE_CXX=clang++

OBJECTS_DIR=build

SOURCES += \
    src/vector.c \
    src/main.c

HEADERS += \
    src/vector.h

