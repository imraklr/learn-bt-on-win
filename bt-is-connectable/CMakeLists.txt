cmake_minimum_required(VERSION 3.19..4.0)
project(
    bt-is-connectable
    DESCRIPTION "A terminal app to check whether a bluetooth radio or radios is connectable."
    VERSION 0.1.0
    LANGUAGES C
)

add_executable(app "src/app.c")
target_link_libraries(app PRIVATE ws2_32 Bthprops)