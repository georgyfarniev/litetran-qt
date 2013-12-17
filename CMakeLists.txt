project(LiteTran)
cmake_minimum_required(VERSION 2.8.10)
set(EXECUTABLE_NAME litetran)
set(PROJECT_VERSION 0.1)
set(CMAKE_BUILD_TYPE Debug)


exec_program("git" ${CMAKE_CURRENT_SOURCE_DIR} ARGS "rev-parse HEAD" OUTPUT_VARIABLE VERSION)
string(SUBSTRING ${VERSION} 0 7 GIT_HASH)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DQP_DEBUG)
    if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
    endif(CMAKE_COMPILER_IS_GNUCC)
endif()

if(NOT MSVC)
    if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
endif()

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(src/defines.h.in src/defines.h)

set(HEADERS
src/mainwindow.h
src/texttoolbar.h
src/settings.h
src/trayicon.h
src/languagedb.h
src/translate.h
src/request.h
src/pronounce.h
src/popup.h)

set(SOURCES
src/main.cpp
src/mainwindow.cpp
src/texttoolbar.cpp
src/settings.cpp
src/trayicon.cpp
src/languagedb.cpp
src/translate.cpp
src/request.cpp
src/pronounce.cpp
src/popup.cpp)

set(FORMS
src/mainwindow.ui
src/settings.ui)

set(RESOURCES resources.qrc)

qt5_add_resources(RC_SOURCES ${RESOURCES})
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})
qt5_wrap_ui(UI_HEADERS ${FORMS})


add_executable(${EXECUTABLE_NAME} ${RC_SOURCES} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS})
qt5_use_modules(${EXECUTABLE_NAME} Widgets Core Network)


install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
