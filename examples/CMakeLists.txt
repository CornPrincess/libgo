cmake_minimum_required(VERSION 3.5)

project(libgo_cpp14_examples CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LIBGO_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../local" CACHE PATH "Local install prefix of libgo")

include_directories("${LIBGO_PREFIX}/include")
link_directories("${LIBGO_PREFIX}/lib")

file(GLOB EXAMPLE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

foreach(example_file ${EXAMPLE_SOURCES})
    get_filename_component(example_name "${example_file}" NAME_WE)
    add_executable("${example_name}" "${example_file}")
    target_link_libraries("${example_name}" libgo pthread dl)
endforeach()
