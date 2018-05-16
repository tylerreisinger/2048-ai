cmake_minimum_required(VERSION 2.8)
project(2048_ai)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -march=native")
endif()


include_directories("src")
include_directories("3rdparty")
include_directories("3rdparty/result")
include_directories("3rdparty/cxxopts/include")
add_subdirectory("src")
add_subdirectory("3rdparty")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(2048 ${SOURCES} ${EXTERNAL_SOURCES})
target_link_libraries(2048 dl)

find_package(SFML 2 REQUIRED graphics window system)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    message(STATUS ${SFML_INCLUDE_DIR})
    target_link_libraries(2048 ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()

