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
        src/mainwindow.h
        src/operator.cpp
        src/operator.h
        src/point.cpp
        src/point.h
        src/qubit.cpp
        src/qubit.h
        src/sphere.cpp
        src/sphere.h
        src/vector.cpp
        src/vector.h
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
    -Wall -Wextra -pedantic -Werror
)

# GTests

add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/gtest" 
    "gtest"
    )

add_executable(
    test
    test/main.cpp
    src/operator.cpp
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
    -Wall -Wextra -pedantic -Werror
)
