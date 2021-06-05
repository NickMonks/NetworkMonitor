cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

project(network-monitor)

# Dependencies - managed with Conan
# If dependencies are managed with conan, we prepend the current build folder
# to the Cmake modeule path:

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt)
    # This will prepend the module path with the binary directory (i.e build folder)
    list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# Once we linked the modules path to the Find*.cmake inside the build folder, we find the packages
find_package(Boost 1.74 REQUIRED COMPONENTS system)

set(SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/websocket-client.cpp"
)

set(LIB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/websocket-client.cpp"
    )

add_library(network-monitor STATIC ${LIB_SOURCES})

# set C++14 as the requirement for both private and public interfaces.
target_compile_features(network-monitor
    PUBLIC
        cxx_std_14
)

#include the header files in the source folder
target_include_directories(network-monitor
    PUBLIC
        inc
)

# We set the CMake dependency as public, so it propagates the dependencies
# to the users of this library
target_link_libraries(network-monitor
    PUBLIC
    Boost::Boost
)

#### TESTS
set(TESTS_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/websocket-client.cpp"
)

add_executable(network-monitor-tests ${TESTS_SOURCES})
target_compile_features(network-monitor-tests
    PRIVATE
        cxx_std_14
)

target_link_libraries(network-monitor-tests
    PRIVATE
        network-monitor
        Boost::Boost
)
