cmake_minimum_required(VERSION 3.19..4.0)
project(
    find-device
    DESCRIPTION "A project to find all bluetooth devices available on a Windows PC."
    VERSION 0.1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(app "src/app.c")
target_link_libraries(app PRIVATE ws2_32 Bthprops)