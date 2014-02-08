include_directories(${Qt5Widgets_PRIVATE_INCLUDE_DIRS})

set(SOURCES qkeysequenceedit.cpp)
set(HEADERS qkeysequenceedit.h qkeysequenceedit_p.h)

qt5_wrap_cpp(SOURCES-MOC ${HEADERS})
add_library(qkeysequenceedit STATIC ${SOURCES} ${SOURCES-MOC})
qt5_use_modules(qkeysequenceedit Widgets Core)
