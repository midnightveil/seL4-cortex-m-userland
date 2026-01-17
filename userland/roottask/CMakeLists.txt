cmake_minimum_required(VERSION 3.16.0)

add_executable(roottask.elf main.c crt0.S uart.c)
target_link_libraries(roottask.elf sel4 sel4_autoconf)

set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")

target_link_options(roottask.elf PRIVATE "LINKER:-T${LINKER_SCRIPT}")
set_target_properties(roottask.elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
