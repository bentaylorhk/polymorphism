# Benjamin Michael Taylor (bentaylorhk)
# 2025

add_subdirectory(animations)
add_subdirectory(util)

set(SOURCES main.cpp)

add_executable(polymorphism ${SOURCES})
target_compile_definitions(polymorphism PRIVATE IS_POLYMORPHISM=1 IS_SCREENSAVER=0 IS_BOOT_ANIMATION=0)
target_link_libraries(polymorphism PRIVATE animations ncurses CLI11::CLI11 util)
set_target_properties(polymorphism PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_executable(screensaver ${SOURCES})
target_compile_definitions(screensaver PRIVATE IS_POLYMORPHISM=0 IS_SCREENSAVER=1 IS_BOOT_ANIMATION=0)
target_link_libraries(screensaver PRIVATE animations ncurses CLI11::CLI11 util)
set_target_properties(screensaver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_executable(boot-animation ${SOURCES})
target_compile_definitions(boot-animation PRIVATE IS_POLYMORPHISM=0 IS_SCREENSAVER=0 IS_BOOT_ANIMATION=1)
target_link_libraries(boot-animation PRIVATE animations ncurses CLI11::CLI11 util)
set_target_properties(boot-animation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS polymorphism screensaver boot-animation RUNTIME DESTINATION bin)