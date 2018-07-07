TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ram.cpp \
    instruction.cpp \
    cpu.cpp \
    ozones.cpp

SUBDIRS += \
    ozones.pro

DISTFILES += \
    LICENSE

HEADERS += \
    ram.h \
    instruction.h \
    cpu.h

unix|win32: LIBS += -lsfml-window \
    -lsfml-graphics \
    -lsfml-audio \
    -lsfml-system
