cmake_minimum_required(VERSION 3.19..4.0)
project(
    get-device-info-app
    DESCRIPTION "A project to demonstrate how to retrieve a device info."
    VERSION 0.1.0
    LANGUAGES C
)

add_executable(get-device-info-app "src/app.c")
target_link_libraries(get-device-info-app PRIVATE ws2_32 Bthprops)