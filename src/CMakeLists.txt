# Benjamin Michael Taylor (bentaylorhk)
# 2025

add_subdirectory(animations)
add_subdirectory(util)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(polyphonic_rsvp ${SOURCES})

target_link_libraries(polyphonic_rsvp PRIVATE animations ncurses CLI11::CLI11 util spdlog::spdlog)

install(TARGETS polyphonic_rsvp RUNTIME DESTINATION bin)
