add_subdirectory(qxtshortcut)
set(THIRD_PARTY_DIRS src/3rdparty/qxtshortcut)
set(THIRD_PARTY_LIBS qxtshortcut)

if("${Qt5Core_VERSION_STRING}" VERSION_LESS "5.2.0")
    add_subdirectory(qkeysequenceedit)
    LIST(APPEND THIRD_PARTY_DIRS src/3rdparty/qkeysequenceedit)
    LIST(APPEND THIRD_PARTY_LIBS qkeysequenceedit)
endif()

set(THIRD_PARTY_DIRS ${THIRD_PARTY_DIRS} PARENT_SCOPE)
set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} PARENT_SCOPE)
