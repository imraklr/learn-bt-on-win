cmake_minimum_required(VERSION 3.19...4.0)
project(
	n-radio
	DESCRIPTION "A program to find out the number of bluetooth radios available on a Windows system."
	VERSION 0.1.0
	LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_definitions(UNICODE _UNICODE)
add_executable(app "${CMAKE_CURRENT_SOURCE_DIR}/src/app.c")
target_link_libraries(app PRIVATE ws2_32 Bthprops)