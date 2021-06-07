cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

project(network-monitor)

# Add the local CMake modules folder to the CMake search path.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Dependencies - managed with Conan
# If dependencies are managed with conan, we prepend the current build folder
# to the Cmake modeule path:

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt)
    # This will prepend the module path with the binary directory (i.e build folder)
    list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# Once we linked the modules path to the Find*.cmake inside the build folder, we find the packages
find_package(Boost 1.74 REQUIRED COMPONENTS system unit_test_framework)
find_package(OpenSSL REQUIRED)
find_package(Filesystem REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(CURL REQUIRED)


# Called before any other target is defined.
#When we run the test target, any executable registered as a test is executed; 
# test results are collected and presented together. CTest is the CMake utility that handles the test execution for us
enable_testing()


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

# NOTE: ecause websocket-client.h is consumed by library users, 
# we need OpenSSL to be a transitive dependency — that's why it is marked as a PUBLIC target dependency.

target_link_libraries(network-monitor
    PUBLIC
    Boost::Boost
    OpenSSL::OpenSSL
    nlohmann_json::nlohmann_json
    PRIVATE
        CURL::CURL
)

#### TESTS
set(TESTS_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/test/websocket-client.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp"

)

add_executable(network-monitor-tests ${TESTS_SOURCES})
target_compile_features(network-monitor-tests
    PRIVATE
        cxx_std_17
)

#To hardcode the cacert.pem location in the network-monitor-tests executable, modify the CMake 
# network-monitor-tests target to define the path as a preprocessor symbol, TESTS_CACERT_PEM
target_compile_definitions(network-monitor-tests
    PRIVATE 
        TESTS_CACERT_PEM="${CMAKE_CURRENT_SOURCE_DIR}/test/cacert.pem"
        TESTS_NETWORK_LAYOUT_JSON="${CMAKE_CURRENT_SOURCE_DIR}/tests/network-layout.json"
)

target_link_libraries(network-monitor-tests
    PRIVATE
        network-monitor
        Boost::Boost
        OpenSSL::OpenSSL
        std::filesystem
)

#You can define a special build system target called test. 
# This target is a catch-all way to run all Project tests, 
# including tests that may use different testing frameworks. 
# When we run the test target, any executable registered as a 
#test is executed; test results are collected and presented together. 

add_test(
    NAME network-monitor-tests
    COMMAND $<TARGET_FILE:network-monitor-tests>
)

#This tells CMake to check for a specific output to verify the test outcome.
# When all unit tests pass, Boost.Test prints "No errors detected".
set_tests_properties(network-monitor-tests PROPERTIES
    PASS_REGULAR_EXPRESSION ".*No errors detected"
)