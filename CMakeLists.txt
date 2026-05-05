cmake_minimum_required(VERSION 3.16)
project(PolygonEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Widgets)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

set(SOURCES
    main.cpp
    MainWindow.cpp
    PolygonScene.cpp
)

set(HEADERS
    MainWindow.h
    PolygonScene.h
)

add_executable(PolygonEditor ${SOURCES} ${HEADERS})

target_link_libraries(PolygonEditor Qt${QT_VERSION_MAJOR}::Widgets)
