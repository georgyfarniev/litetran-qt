project(LiteTran)
cmake_minimum_required(VERSION 2.8.10)
set(EXECUTABLE_NAME litetran)
set(PROJECT_VERSION 0.1)
set(CMAKE_BUILD_TYPE Debug)

option(SELECTION_BUFFER "Use X11 selection buffer" ON)
option(TEXT_PRONOUNCE "Pronounce text via QtMultimedia" ON)


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

if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
    set(I18N_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/litetran/i18n)
endif()

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_CURRENT_BINARY_DIR}/src
src/3rdparty/qxtshortcut)

configure_file(src/defines.h.in src/defines.h)

add_subdirectory(src/3rdparty/qxtshortcut)

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
src/mainwindow.ui)

set(RESOURCES resources.qrc)
set(FILES_TO_TRANSLATE ${SOURCES} ${HEADERS} ${FORMS})
set(TRANSLATIONS_FILES
i18n/English.ts
i18n/Russian.ts)

option(UPDATE_TRANSLATIONS OFF)
if(UPDATE_TRANSLATIONS)
    message("-- Updating translation files")
    qt5_create_translation(QM_FILES ${FILES_TO_TRANSLATE} ${TRANSLATIONS_FILES})
else()
    qt5_add_translation(QM_FILES ${TRANSLATIONS_FILES})
endif()

add_custom_target (lupdate DEPENDS ${QM_FILES})

qt5_add_resources(RC_SOURCES ${RESOURCES})
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})
qt5_wrap_ui(UI_HEADERS ${FORMS})


add_executable(${EXECUTABLE_NAME} ${RC_SOURCES} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS})
qt5_use_modules(${EXECUTABLE_NAME} Widgets Core Network Multimedia)
target_link_libraries(${EXECUTABLE_NAME} qxtshortcut)


install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${QM_FILES} DESTINATION ${I18N_DIRECTORY})
