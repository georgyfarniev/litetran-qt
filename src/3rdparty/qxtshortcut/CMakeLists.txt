set(SOURCES qxtglobal.cpp qxtglobalshortcut.cpp)
set(HEADERS qxtglobalshortcut.h)

if(WIN32)
    set(SOURCES ${SOURCES} qxtglobalshortcut_win.cpp)
elseif(APPLE)
    set(SOURCES ${SOURCES} qxtglobalshortcut_mac.cpp)
elseif(UNIX)
    set(SOURCES ${SOURCES} qxtglobalshortcut_x11.cpp)
    find_library(XLIB NAMES libx11 X11)
endif()

qt5_wrap_cpp(SOURCES-MOC ${HEADERS})
add_library(qxtshortcut STATIC ${SOURCES} ${SOURCES-MOC})
qt5_use_modules(qxtshortcut Widgets Core)

if(UNIX)
    target_link_libraries(qxtshortcut ${XLIB})
endif()

#
# This is very ugly hack and MUST be replaced to something better.
#
include_directories(/usr/include/qt/QtGui/${Qt5Widgets_VERSION_STRING}/QtGui/qpa)

