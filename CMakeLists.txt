# Novara: a simple world as integration test and reference 
cmake_minimum_required(VERSION 3.24)
project(Ceto)

if(PROJECT_IS_TOP_LEVEL)
    include("${RHEA_CMAKE_MODULE_DIR}/rhea_project.cmake")
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