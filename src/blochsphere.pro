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

QT += core opengl

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    operator.cpp \
    point.cpp \
    qubit.cpp \
    sphere.cpp \
    vector.cpp

HEADERS += \
    mainwindow.h \
    operator.h \
    point.h \
    qubit.h \
    sphere.h \
    vector.h

TARGET = blochsphere

LIBS += libopengl32

CONFIG += qt wanr_on
