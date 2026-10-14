# Novara: a simple world as integration test and reference 
cmake_minimum_required(VERSION 3.24)
project(Novara LANGUAGES C CXX)

if(PROJECT_IS_TOP_LEVEL)
    set(RHEA_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../rhea" CACHE PATH "Path to the Rhea SDK repository")
    get_filename_component(RHEA_SDK_DIR "${RHEA_SDK_DIR}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(RHEA_CMAKE_MODULE_DIR "${RHEA_SDK_DIR}/Rhea/cmake")

    if(NOT EXISTS "${RHEA_SDK_DIR}/CMakeLists.txt" OR NOT EXISTS "${RHEA_CMAKE_MODULE_DIR}/rhea_project.cmake")
        message(FATAL_ERROR "Rhea SDK not found at '${RHEA_SDK_DIR}'. Configure with -DRHEA_SDK_DIR=/path/to/rhea.")
    endif()

    # Novara owns registry finalization; the SDK only registers its Fabrics.
    include("${RHEA_CMAKE_MODULE_DIR}/rhea_project.cmake")

    option(RHEA_BUILD_APPS "Build SDK applications alongside Novara" OFF)
    option(RHEA_BUILD_SAMPLES "Build SDK engine samples alongside Novara" OFF)
    option(POSEIDON_BUILD_SAMPLES "Build SDK renderer samples alongside Novara" OFF)
    option(RHEA_BUILD_TESTS "Build SDK tests alongside Novara" OFF)

    add_subdirectory("${RHEA_SDK_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/rhea")
endif()

add_executable(Novara
    src/main.cpp src/Novara.cpp 
 "src/TransitionArea.h" "src/TransitionArea.cpp")

target_link_libraries(Novara PRIVATE
    Rhea::Core Rhea::Client RheaFabrics
)

rhea_copy_assets(Novara "${CMAKE_CURRENT_SOURCE_DIR}/Assets" "novara")
rhea_copy_assets(Novara "${RHEA_ASSETS_DIR}" "rhea")

if(PROJECT_IS_TOP_LEVEL)
    rhea_project_end()
endif()
