TEMPLATE = app

QT += opengl gui widgets core 3dcore 3drender 3dinput 3dextras

SOURCES += \
    main.cpp \
    mainwid.cpp \
    bloch.cpp \
    qubitvect.cpp

HEADERS += \
    mainwid.hpp \
    bloch.hpp \
    qubitvect.hpp \
    glut.h
    
#LIBS += libopengl32 libglu32 c:\Users\baseoleph\git\blochsphere\bloch2\glut32.dll
LIBS += libopengl32

TARGET = bloch

CONFIG += qt warn_on
