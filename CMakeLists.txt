cmake_minimum_required(VERSION 2.8.10)

set(APP_EXECUTABLE_NAME litetran)
set(APP_NAME LiteTran)

exec_program(git ${CMAKE_CURRENT_SOURCE_DIR} ARGS describe --long --always --tags OUTPUT_VARIABLE GIT_VERSION RETURN_VALUE GIT_ERROR)

if(${GIT_ERROR})
    set(APP_VERSION 1.3)
else()
    set(APP_VERSION ${GIT_VERSION})
endif()

if(APPLE)
    set(APP_GUI_TYPE MACOSX_BUNDLE)
elseif(UNIX)
    set(APP_BINARY_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/bin)
    set(APP_ICONS_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/share/pixmaps)
    set(APP_I18N_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/share/litetran/i18n)
    set(APP_I18N_LOAD_PATH ${APP_I18N_INSTALL_PATH})
elseif(WIN32)
    set(APP_BINARY_INSTALL_PATH ${CMAKE_INSTALL_PREFIX})
    set(APP_ICONS_INSTALL_PATH ${CMAKE_INSTALL_PREFIX})
    set(APP_I18N_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/i18n)
    set(APP_I18N_LOAD_PATH i18n)
    set(APP_GUI_TYPE WIN32)
    set(QT_USE_QTMAIN TRUE)
endif()
project(${APP_NAME})
set(PROJECT_VERSION ${APP_VERSION})

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    if(CMAKE_COMPILER_IS_GNUCC AND UNIX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
    endif()
endif()

if(NOT MSVC)
    if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
endif()

find_package(Qt5Widgets 5.2 REQUIRED)
find_package(Qt5Core 5.2 REQUIRED)
find_package(Qt5Network 5.2 REQUIRED)
find_package(Qt5Multimedia 5.2 REQUIRED)
find_package(Qt5LinguistTools 5.2 REQUIRED)

include_directories(
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_CURRENT_BINARY_DIR}/src
${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/qxtshortcut)

configure_file(src/defines.h.in src/defines.h)

set(HEADERS
src/mainwindow.h
src/texttoolbar.h
src/settings.h
src/trayicon.h
src/languagedb.h
src/translate.h
src/networkmanager.h
src/pronounce.h
src/popup.h
src/textedit.h
src/menubutton.h
src/languages.h
CACHE INTERNAL "Headers")

set(SOURCES
src/main.cpp
src/mainwindow.cpp
src/texttoolbar.cpp
src/settings.cpp
src/trayicon.cpp
src/languagedb.cpp
src/translate.cpp
src/networkmanager.cpp
src/pronounce.cpp
src/popup.cpp
src/textedit.cpp
src/menubutton.cpp
src/languages.cpp
CACHE INTERNAL "Sources")

set(TRANSLATIONS_FILES
i18n/English.ts
i18n/Russian.ts)

set(ABOUT_TS
i18n/about/English.html
i18n/about/Russian.html)

set(RESOURCES resources.qrc)

add_subdirectory(src/platform)
add_subdirectory(src/3rdparty)


option(UPDATE_TRANSLATIONS OFF)
set(UPDATE_TRANSLATIONS OFF)
if(UPDATE_TRANSLATIONS)
    message(WARNING "Updating translation files")
    qt5_create_translation(QM_FILES ${SOURCES} ${TRANSLATIONS_FILES})
else()
    qt5_add_translation(QM_FILES ${TRANSLATIONS_FILES})
endif()

add_custom_target(lupdate DEPENDS ${QM_FILES})

qt5_add_resources(RC_SOURCES ${RESOURCES})
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

add_executable(${APP_EXECUTABLE_NAME} ${APP_GUI_TYPE} ${RC_SOURCES} ${SOURCES} ${MOC_SOURCES} ${QM_FILES} litetran.rc)
qt5_use_modules(${APP_EXECUTABLE_NAME} Widgets Core Network Multimedia)

target_link_libraries(${APP_EXECUTABLE_NAME} qxtshortcut)

if(APPLE)
    find_library(CORE_SERVICES_LIBRARY CoreServices)
    find_library(CARBON_LIBRARY Carbon)
    target_link_libraries(${APP_EXECUTABLE_NAME} ${CORE_SERVICES_LIBRARY} ${CARBON_LIBRARY})
endif()

if(WIN32)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/litetranico.o
                       COMMAND  windres.exe -i${CMAKE_CURRENT_SOURCE_DIR}/litetran.rc
                       -o ${CMAKE_CURRENT_BINARY_DIR}/litetranico.o)

    # TODO: GET QT PATH PROPRELY!
    get_target_property(QT5_CORE_LOCATION Qt5::Core LOCATION)
    get_filename_component(QT_LIBRARY_PATH ${QT5_CORE_LOCATION} DIRECTORY)

    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME ${APP_NAME})
    set(CPACK_PACKAGE_VERSION ${APP_VERSION})
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${APP_NAME})
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt)
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    set(CPACK_NSIS_MENU_LINKS ./litetran.exe LiteTran)
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN litetran.exe)
    include(CPack)

    install(TARGETS ${APP_EXECUTABLE_NAME} DESTINATION .)
    install(FILES ${QM_FILES} DESTINATION ./i18n)
    install(FILES ${ABOUT_TS} DESTINATION ./about)
    install(FILES ${ABOUT_TS} DESTINATION ./about)
    file(GLOB ICU_LIBS "${QT_LIBRARY_PATH}/bin/icu*.dll")
    install(FILES ${ICU_LIBS} DESTINATION .)
    install(FILES ${QT_LIBRARY_PATH}/../plugins/platforms/qminimal.dll DESTINATION ./platforms)
    install(FILES ${QT_LIBRARY_PATH}/../plugins/platforms/qwindows.dll DESTINATION ./platforms)
    install(FILES ${QT_LIBRARY_PATH}/../plugins/mediaservice/dsengine.dll DESTINATION ./mediaservice)
    install(FILES ${QT_LIBRARY_PATH}/../plugins/mediaservice/qtmedia_audioengine.dll DESTINATION ./mediaservice)
    install(FILES ${QT_LIBRARY_PATH}/../bin/libgcc_s_dw2-1.dll DESTINATION .)
    install(FILES ${QT_LIBRARY_PATH}/../bin/libstdc++-6.dll DESTINATION .)
    install(FILES ${QT_LIBRARY_PATH}/../bin/libwinpthread-1.dll DESTINATION .)
    install(FILES ${QT_LIBRARY_PATH}/../bin/Qt5Core.dll DESTINATION .)
    install(FILES ${QT_LIBRARY_PATH}/../bin/Qt5Gui.dll DESTINATION .)
    install(FILES ${QT_LIBRARY_PATH}/../bin/Qt5Multimedia.dll DESTINATION .)
    install(FILES ${QT_LIBRARY_PATH}/../bin/Qt5MultimediaWidgets.dll DESTINATION .)
    install(FILES ${QT_LIBRARY_PATH}/../bin/Qt5Network.dll DESTINATION .)
    install(FILES ${QT_LIBRARY_PATH}/../bin/Qt5OpenGL.dll DESTINATION .)
    install(FILES ${QT_LIBRARY_PATH}/../bin/Qt5Widgets.dll DESTINATION .)
else()
    install(TARGETS ${APP_EXECUTABLE_NAME} DESTINATION ${APP_BINARY_INSTALL_PATH})
    install(FILES ${QM_FILES} DESTINATION ${APP_I18N_INSTALL_PATH})
    install(FILES ${ABOUT_TS} DESTINATION ${APP_I18N_INSTALL_PATH}/about)
endif()

if(UNIX)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/ui/litetran.png DESTINATION ${APP_ICONS_INSTALL_PATH})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/litetran.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
endif()
