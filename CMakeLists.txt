cmake_minimum_required(VERSION 3.25)
project(ray_tracing_in_one_weekend)

# Use Ninja as the default build tool instead of make.
# ninja main / ninja clean
set(CMAKE_GENERATOR "Ninja")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 20)

configure_file(${CMAKE_SOURCE_DIR}/images/earthmap.jpg ${CMAKE_BINARY_DIR}/earthmap.jpg COPYONLY)

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_executable(main main.cpp)
if (OpenMP_CXX_FOUND)
    target_link_libraries(main PUBLIC OpenMP::OpenMP_CXX)
endif()
