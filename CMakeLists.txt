cmake_minimum_required(VERSION 3.16)

project(IdealGas_ VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets OpenGL)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    SimulationWindow.h
    SimulationWindow.cpp
    SimulationWidget.h
    SimulationWidget.cpp
)

qt_add_executable(IdealGas_
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    SimulationWidget.h
    SimulationWidget.cpp
    GraphsWindow.h
    GraphsWindow.cpp
)

target_link_libraries(IdealGas_ PRIVATE
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::OpenGL
)

set_target_properties(IdealGas_ PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)

install(TARGETS IdealGas_
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(IdealGas_)