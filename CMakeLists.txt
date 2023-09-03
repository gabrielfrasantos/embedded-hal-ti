cmake_minimum_required(VERSION 3.24)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(HAL_TI_STANDALONE On)
endif()

option(CMAKE_COMPILE_WARNING_AS_ERROR "Enable warnings-as-error" ON)
option(HAL_TI_INCLUDE_DEFAULT_INIT "Include default initialization code; turn off when providing custom initialization" ON)
option(HAL_TI_BUILD_EXAMPLES "Enable build of the examples" OFF)
option(HAL_TI_BUILD_EXAMPLES_FREERTOS "Enable build of the FreeRTOS example" OFF)

if (HAL_TI_STANDALONE)
    set(EMIL_INCLUDE_FREERTOS ${HAL_TI_BUILD_EXAMPLES_FREERTOS})

    include(FetchContent)

    FetchContent_Declare(
        emil
        GIT_REPOSITORY https://github.com/gabrielfrasantos/embedded-infra-lib.git
        GIT_TAG        292feb389c1cdb4184c3ee17cb43ae5319ed2488 # Unreleased
    )

    FetchContent_MakeAvailable(emil)
endif()

project(hal_tiva LANGUAGES C CXX ASM VERSION 2.0.0) # x-release-please-version

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

set_directory_properties(PROPERTY USE_FOLDERS ON)

# When building HalSt by itself do not exclude any targets from all
# Libraries will only be excluded from all when HalSt is a consumed as a dependency.
if (HAL_TI_STANDALONE AND NOT EMIL_HOST_BUILD)
    set(HAL_TI_EXCLUDE_FROM_ALL "")
else()
    set(HAL_TI_EXCLUDE_FROM_ALL "EXCLUDE_FROM_ALL")
endif()

add_subdirectory(tiva)
add_subdirectory(hal_tiva)
add_subdirectory(examples)

emil_clangformat_directories(hal_tiva DIRECTORIES .)

if (HAL_TI_STANDALONE)
    emil_folderize_all_targets()
endif()
