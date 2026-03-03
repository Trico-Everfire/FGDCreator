cmake_minimum_required(VERSION 3.31)
project(FGDCreator CXX)

set(CMAKE_SKIP_BUILD_RPATH OFF)
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
set(CMAKE_BUILD_RPATH $ORIGIN)
set(CMAKE_INSTALL_RPATH $ORIGIN)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (UNIX)
    set(QT_INCLUDE "$ENV{HOME}/Qt/$ENV{QT_VERSION}/gcc_64/include/")
    if (NOT DEFINED QT_BASEDIR)
        set(QT_BASEDIR "$ENV{HOME}/Qt/$ENV{QT_VERSION}/gcc_64/")
    endif ()
    if (DEFINED QT_BASEDIR)
        set(CMAKE_PREFIX_PATH "${QT_BASEDIR}/lib/cmake")
    else ()
        message(FATAL_ERROR "--!@ Please define your QT install dir with -DQT_BASEDIR=C:/your/qt6/here")
    endif ()
elseif (WIN32)
    # On Windows, the install dir must be specified, as it's not standardized
    if (DEFINED QT_BASEDIR)
        # Add it to the prefix path so find_package can find it
        list(APPEND CMAKE_PREFIX_PATH ${QT_BASEDIR})
        set(QT_INCLUDE ${QT_BASEDIR}/include)
        # CMake has an odd policy that links a special link lib for Qt on newer versions of CMake. Enable it so we don't get spammed, and I get to write less
        cmake_policy(SET CMP0020 NEW)
    else ()
        message(FATAL_ERROR "--!@ Please define your QT install dir with -DQT_BASEDIR=C:/your/qt6/here")
    endif ()
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

set(SOURCEPP_LIBS_START_ENABLED OFF CACHE INTERNAL "" FORCE)
set(SOURCEPP_USE_FSPP            ON CACHE INTERNAL "" FORCE)
set(SOURCEPP_USE_TOOLPP           ON CACHE INTERNAL "" FORCE)
add_subdirectory("libs/sourcepp")

set(SRC
        main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
)

add_executable(FGDCreator ${SRC})

target_link_libraries(FGDCreator
        Qt::Core
        Qt::Gui
        Qt::Widgets
        sourcepp::toolpp
        sourcepp::fspp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${QT_INCLUDE} ${QT_INCLUDE}/QtWidgets" "${QT_INCLUDE}/QNetwork" "${QT_INCLUDE}/QtDBus" "${QT_INCLUDE}/QtGui" "${QT_INCLUDE}/QtCore" "${QT_INCLUDE}/QtOpenGLWidgets" ${QT_INCLUDE}/QtOpenGL)
