include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(HEADERS
${HEADERS}
src/platform/clipboard.h
src/platform/autostart.h
CACHE INTERNAL "Headers")

if(WIN32)
    set(PLATFORM_SOURCES
    src/platform/clipboard_win.cpp
    src/platform/autostart_win.cpp)
elseif(APPLE)
    set(PLATFORM_SOURCES
    src/platform/clipboard_mac.cpp
    src/platform/autostart_mac.cpp)
elseif(UNIX)
    set(PLATFORM_SOURCES
    src/platform/clipboard_x11.cpp
    src/platform/autostart_x11.cpp)
endif()

set(SOURCES ${SOURCES} ${PLATFORM_SOURCES} CACHE INTERNAL "Sources")
