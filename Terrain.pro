TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Terrain.cpp \
    Random.cpp \
    Object.cpp \
    common.cpp

OTHER_FILES +=

HEADERS += \
    Terrain.h \
    Random.h \
    common.h \
    Object.h


win32: LIBS += -lfreeglut -lopengl32 -lglu32
