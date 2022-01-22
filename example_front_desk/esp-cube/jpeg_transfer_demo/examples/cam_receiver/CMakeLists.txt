cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS
    ../../components
    $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

add_compile_options(-fdiagnostics-color=always)

project(cam_receiver)
