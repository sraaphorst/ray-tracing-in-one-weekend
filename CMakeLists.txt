cmake_minimum_required(VERSION 3.25)
project(ray_tracing_in_one_weekend)

# Use Ninja as the default build tool instead of make.
# ninja main / ninja clean
set(CMAKE_GENERATOR "Ninja")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp)
