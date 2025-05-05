# Benjamin Michael Taylor (bentaylorhk)
# 2025

# Command line argument parsing
include(FetchContent)
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(CLI11)

add_subdirectory(animations)

add_executable(polyphonic_rsvp main.cpp)
target_link_libraries(polyphonic_rsvp PRIVATE animations CLI11::CLI11)

install(TARGETS polyphonic_rsvp RUNTIME DESTINATION bin)
