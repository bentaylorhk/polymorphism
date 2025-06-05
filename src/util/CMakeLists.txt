# Benjamin Michael Taylor (bentaylorhk)
# 2025

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(util STATIC ${SOURCES})

target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(util PRIVATE ncurses)

