TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ram.cpp \
    instruction.cpp \
    cpu.cpp \
    ozones.cpp \
    rom.cpp \
    ppu.cpp

SUBDIRS += \
    ozones.pro

DISTFILES += \
    LICENSE

HEADERS += \
    ram.h \
    instruction.h \
    cpu.h \
    rom.h \
    ines.h \
    ppu.h

unix|win32: LIBS += -lsfml-window \
    -lsfml-graphics \
    -lsfml-audio \
    -lsfml-system
