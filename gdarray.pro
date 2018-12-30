TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle qt
QMAKE_CC=clang
QMAKE_CXX=clang++
QMAKE_CFLAGS += -D__RUN_TESTS__ -D__DEBUG__ -Wall -std=c99 -fPIC -g3 -o0

OBJECTS_DIR=build

SOURCES += \
    src/vector.c

HEADERS += \
    src/vector.h \
    src/minunit.h

