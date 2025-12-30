cmake_minimum_required(VERSION 3.16.0)

project(userland C ASM)

add_compile_options(-nostdlib -fno-common)
add_link_options(-static -nostdlib)

add_subdirectory(roottask)

set(IMAGE_NAME "${CMAKE_BINARY_DIR}/meL4.elf")

add_custom_command(
    OUTPUT "${IMAGE_NAME}"
    COMMAND cp $<TARGET_FILE:kernel.elf> ${IMAGE_NAME}
    VERBATIM
    DEPENDS "$<TARGET_FILE:kernel.elf>"
)
add_custom_target(
    roottask_image ALL
    DEPENDS "${IMAGE_NAME}"
)
