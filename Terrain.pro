TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Terrain.cpp \
    Random.cpp \
    Object.cpp \
    common.cpp

OTHER_FILES += \
    CMakeLists.txt

HEADERS += \
    Terrain.h \
    Random.h \
    common.h \
    Object.h

