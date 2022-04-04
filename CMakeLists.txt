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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# TODO h to hpp
set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.hpp
        src/operator.cpp
        src/operator.hpp
        src/point.cpp
        src/point.hpp
        src/qubit.cpp
        src/qubit.hpp
        src/sphere.cpp
        src/sphere.hpp
        src/vector.cpp
        src/vector.hpp
        src/unitaryMatrix2x2.cpp
        src/unitaryMatrix2x2.hpp
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
    test/testOperator.cpp
    test/unitaryOperators.cpp
    src/operator.cpp
    src/unitaryMatrix2x2.cpp
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
