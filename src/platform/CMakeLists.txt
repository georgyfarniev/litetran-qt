include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(HEADERS
${HEADERS}
src/platform/clipboard.h
src/platform/autostart.h
CACHE INTERNAL "Headers")

set(SOURCES
${SOURCES}
src/platform/clipboard.cpp
src/platform/autostart.cpp
CACHE INTERNAL "Headers")

set(SOURCES ${SOURCES} ${PLATFORM_SOURCES} CACHE INTERNAL "Sources")
