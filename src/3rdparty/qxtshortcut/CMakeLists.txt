
set(QXT-SOURCES
qxtglobal.cpp
qxtglobalshortcut.cpp)

set(QXT-MOC-HEADERS
qxtglobalshortcut.h)

if(WIN32)
  set(QXT-SOURCES ${QXT-SOURCES} qxtglobalshortcut_win.cpp)
elseif(APPLE)
  set(QXT-SOURCES ${QXT-SOURCES} qxtglobalshortcut_mac.cpp)
else(WIN32)
  set(QXT-SOURCES ${QXT-SOURCES} qxtglobalshortcut_x11.cpp)
endif(WIN32)

qt5_wrap_cpp(QXT-SOURCES-MOC ${QXT-MOC-HEADERS})

add_library(qxtshortcut STATIC ${QXT-SOURCES} ${QXT-SOURCES-MOC})


find_library(LIBRARY_X11 NAMES libx11 X11)

message(${LIBRARY_X11})

qt5_use_modules(qxtshortcut Widgets Core)

target_link_libraries(qxtshortcut ${LIBRARY_X11})
