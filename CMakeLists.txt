cmake_minimum_required(VERSION 2.8.10)

set(APP_EXECUTABLE_NAME litetran)
set(APP_NAME LiteTran)

exec_program(git ${CMAKE_CURRENT_SOURCE_DIR} ARGS describe --long --always --tags OUTPUT_VARIABLE GIT_VERSION RETURN_VALUE GIT_ERROR)

if(${GIT_ERROR})
    set(APP_VERSION 1.3)
else()
    set(APP_VERSION ${GIT_VERSION})
endif()

if(NOT UNIX)
    message(FATAL_ERROR "Sorry, this program does not support non-X11 systems")
endif()




project(${APP_NAME})
set(PROJECT_VERSION ${APP_VERSION})


if(NOT MSVC)
    if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()


find_package(Qt5Widgets 5.2 REQUIRED)
find_package(Qt5Core 5.2 REQUIRED)
find_package(Qt5Network 5.2 REQUIRED)
#find_package(Qt5Multimedia 5.2 REQUIRED)
find_package(Qt5LinguistTools 5.2 REQUIRED)

include_directories(
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_CURRENT_BINARY_DIR}/src
${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/qxtshortcut)

configure_file(src/defines.h.in src/defines.h)

set(HEADERS
src/settings.h
src/trayicon.h
src/translate.h
src/mainwindow.h
src/models.h
src/popup.h
CACHE INTERNAL "Headers")

set(SOURCES
src/main.cpp
src/settings.cpp
src/trayicon.cpp
src/translate.cpp
src/mainwindow.cpp
src/models.cpp
src/popup.cpp
CACHE INTERNAL "Sources")

set(FORMS
src/mainwindow.ui
src/settings.ui
src/popup.ui)


set(TRANSLATIONS_FILES
i18n/en.ts
i18n/ru.ts)

set(RESOURCES resources.qrc)

add_subdirectory(src/3rdparty)

set(FILES_TO_TRANSLATE ${SOURCES} ${HEADERS} ${FORMS})

option(UPDATE_TRANSLATIONS OFF)
set(UPDATE_TRANSLATIONS OFF)
if(UPDATE_TRANSLATIONS)
    message(WARNING "Updating translation files")
    qt5_create_translation(QM_FILES ${FILES_TO_TRANSLATE} ${TRANSLATIONS_FILES})
else()
    qt5_add_translation(QM_FILES ${TRANSLATIONS_FILES})
endif()

add_custom_target(lupdate DEPENDS ${QM_FILES})

qt5_add_resources(RC_SOURCES ${RESOURCES})
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})
qt5_wrap_ui(FORM_HEADERS ${FORMS})

add_executable(${APP_EXECUTABLE_NAME} ${APP_GUI_TYPE} ${RC_SOURCES} ${SOURCES} ${MOC_SOURCES} ${FORM_HEADERS} ${QM_FILES} )
qt5_use_modules(${APP_EXECUTABLE_NAME} Widgets Core Network)

target_link_libraries(${APP_EXECUTABLE_NAME} qxtshortcut)



install(TARGETS ${APP_EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/litetran/i18n)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/ui/litetran.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/litetran.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)



