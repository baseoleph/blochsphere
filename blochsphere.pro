# The program helps to see a geometric representation of qubits
# Copyright (C) 2022 Vasiliy Stephanov <baseoleph@gmail.com>

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

TEMPLATE = app

#QT += opengl gui widgets core 3dcore 3drender 3dinput 3dextras
QT += opengl gui widgets core

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    src/blochUtility.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/operator.cpp \
    src/point.cpp \
    src/qubit.cpp \
    src/sphere.cpp \
    src/opitem.cpp \
    src/blochdialog.cpp \
    src/unitaryMatrix2x2.cpp \
    src/vector.cpp \
    src/VectorWidget.cpp


HEADERS += \
    src/blochUtility.h \
    src/mainwindow.hpp \
    src/operator.hpp \
    src/point.hpp \
    src/opitem.hpp \
    src/blochdialog.hpp \
    src/qubit.hpp \
    src/sphere.hpp \
    src/unitaryMatrix2x2.hpp \
    src/vector.hpp \
    src/VectorWidget.h

#LIBS += libopengl32 libglu32 c:\Users\baseoleph\git\blochsphere\bloch2\glut32.dll
LIBS += libopengl32

TARGET = blochsphere

CONFIG += qt warn_on
