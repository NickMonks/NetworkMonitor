cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

project(network-monitor)

# Dependencies - managed with Conan
# If dependencies are managed with conan, we prepend the current build folder
# to the Cmake modeule path:

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt)
    # This will prepend the module path with the binary directory (i.e build folder)
    list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# Once we linked the modules path to the FInd*.cmake inside the build folder, we find the packages
find_package(Boost 1.74 REQUIRED COMPONENTS system)

set(SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_executable(network-monitor ${SOURCES})
target_compile_features(network-monitor
    PRIVATE
    cxx_std_11
)

target_link_libraries(network-monitor
PRIVATE
    Boost::Boost
)