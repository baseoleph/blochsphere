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

cmake_minimum_required(VERSION 3.5)

project(blochsphere VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

set(PROJECT_SOURCES
        src/main.cpp
        src/utility.cpp
        src/utility.h
        src/quantum/Operator.cpp
        src/quantum/Operator.h
        src/quantum/Point.cpp
        src/quantum/Point.h
        src/quantum/Qubit.cpp
        src/quantum/Qubit.h
        src/quantum/UnitaryMatrix2x2.cpp
        src/quantum/UnitaryMatrix2x2.h
        src/quantum/Vector.cpp
        src/quantum/Vector.h
        src/widgets/BlochDialog.cpp
        src/widgets/BlochDialog.h
        src/widgets/Circuit.cpp
        src/widgets/Circuit.h
        src/widgets/CircuitOperator.cpp
        src/widgets/CircuitOperator.h
        src/widgets/CircuitQubit.cpp
        src/widgets/CircuitQubit.h
        src/widgets/MainWindow.cpp
        src/widgets/MainWindow.h
        src/widgets/OpItem.cpp
        src/widgets/OpItem.h
        src/widgets/Sphere.cpp
        src/widgets/Sphere.h
        src/widgets/VectorWidget.cpp
        src/widgets/VectorWidget.h
        )

add_executable(blochsphere ${PROJECT_SOURCES})

target_link_libraries(blochsphere PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::OpenGL
        ${OPENGL_LIBRARIES}
        )

set_target_properties(blochsphere PROPERTIES
        WIN32_EXECUTABLE TRUE
        )

target_compile_options(
        blochsphere PRIVATE
        #    -Wall -Wextra -pedantic -Werror
        -Wall -Wextra
)

# GTests

add_subdirectory(
        "${CMAKE_CURRENT_SOURCE_DIR}/gtest"
        "gtest"
)

add_executable(
        test
        test/testOperatorDecompositions.cpp
        test/unitaryOperators.cpp
        src/quantum/Operator.cpp
        src/quantum/UnitaryMatrix2x2.cpp
        src/quantum/Qubit.cpp
        src/quantum/Point.cpp
        src/utility.cpp
        src/utility.h
        test/testOperator.cpp
        test/main.cpp
        test/identityOperatorPairs.cpp
        test/identityOperatorPairs.h
        test/testQubit.cpp
        test/qubits.cpp test/qubits.h
)

target_include_directories(
        test PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
        test PRIVATE
        gtest
        gtest_main
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::OpenGL
        ${OPENGL_LIBRARIES}
)

target_compile_options(
        test PRIVATE
        #    -Wall -Wextra -pedantic -Werror
        -Wall -Wextra
)
