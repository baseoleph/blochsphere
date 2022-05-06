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
    src/main.cpp \
    src/utility.cpp \
    src/quantum/Operator.cpp \
    src/quantum/Point.cpp \
    src/quantum/Qubit.cpp \
    src/quantum/UnitaryMatrix2x2.cpp \
    src/quantum/Vector.cpp \
    src/widgets/BlochDialog.cpp \
    src/widgets/Circuit.cpp \
    src/widgets/CircuitOperator.cpp \
    src/widgets/CircuitQubit.cpp \
    src/widgets/MainWindow.cpp \
    src/widgets/OpItem.cpp \
    src/widgets/Sphere.cpp \
    src/widgets/VectorWidget.cpp


HEADERS += \
    src/utility.h \
    src/quantum/Operator.hpp \
    src/quantum/Point.hpp \
    src/quantum/Qubit.hpp \
    src/quantum/UnitaryMatrix2x2.hpp \
    src/quantum/Vector.hpp \
    src/widgets/Circuit.h \
    src/widgets/CircuitOperator.h \
    src/widgets/CircuitQubit.h \
    src/widgets/VectorWidget.h \
    src/widgets/BlochDialog.hpp \
    src/widgets/MainWindow.hpp \
    src/widgets/OpItem.hpp \
    src/widgets/Sphere.hpp

LIBS += libopengl32

TARGET = blochsphere

CONFIG += qt warn_on
