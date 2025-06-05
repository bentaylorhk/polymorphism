# Benjamin Michael Taylor (bentaylorhk)
# 2025

add_subdirectory(animations)
add_subdirectory(util)

# Only add main.cpp to the executable (common.cpp will be in util library)
add_executable(polyphonic_rsvp main.cpp)

target_link_libraries(polyphonic_rsvp PRIVATE animations ncurses CLI11::CLI11 util spdlog::spdlog)

install(TARGETS polyphonic_rsvp RUNTIME DESTINATION bin)
