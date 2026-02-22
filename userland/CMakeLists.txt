cmake_minimum_required(VERSION 3.16.0)

project(userland C ASM)

add_compile_options(-nostdlib -fno-common -ggdb3)
add_link_options(-static -nostdlib)

add_subdirectory(roottask)

set(IMAGE_NAME "${CMAKE_BINARY_DIR}/meL4.elf")

find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_custom_command(
    OUTPUT roottask.bin
    COMMAND
        ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:roottask.elf> roottask.bin
    VERBATIM
    DEPENDS
        $<TARGET_FILE:roottask.elf>
)

add_custom_command(
    OUTPUT userspace.bin
    COMMAND
        ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/userland_image.py
            -O userspace.bin
            --kernel $<TARGET_FILE:kernel.elf>
            --roottask $<TARGET_FILE:roottask.elf>
            --roottask_bin roottask.bin
            --gen_config $<TARGET_FILE_DIR:kernel.elf>/gen_config/kernel/gen_config.json
    VERBATIM
    DEPENDS
        roottask.bin
        $<TARGET_FILE:roottask.elf>
        $<TARGET_FILE:kernel.elf>
        "${CMAKE_CURRENT_SOURCE_DIR}/userland_image.py"
)

add_custom_command(
    OUTPUT "${IMAGE_NAME}"
    COMMAND
        ${CMAKE_OBJCOPY} --update-section .userspace=userspace.bin $<TARGET_FILE:kernel.elf> ${IMAGE_NAME}
    COMMAND
        ${CMAKE_STRIP} ${IMAGE_NAME}
    VERBATIM
    DEPENDS "$<TARGET_FILE:kernel.elf>" userspace.bin
)

add_custom_target(
    final_image ALL
    DEPENDS "${IMAGE_NAME}"
)
