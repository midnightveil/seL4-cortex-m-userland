cmake_minimum_required(VERSION 3.16.0)

add_executable(roottask main.c crt0.S)
target_link_libraries(roottask sel4 sel4_autoconf)
