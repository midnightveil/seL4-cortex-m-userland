cmake_minimum_required(VERSION 3.16.0)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")
list(
    APPEND
        CMAKE_MODULE_PATH
        ${project_dir}/kernel
)

find_package(seL4 REQUIRED)
sel4_configure_platform_settings()

project(meL4)

seL4_import_kernel()
sel4_import_libsel4()

add_subdirectory(userland)
