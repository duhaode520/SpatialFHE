# 设置项目名称变量
set(PROJECT_NAME SpatialFHE)
# set(CMAKE_TOOLCHAIN_FILE /home/duhaode/pri_comp/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "")
# set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
# 使用变量设置项目名称
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME} VERSION 0.1.0)
option(BUILD_JAVA "Build Java Library" ON)

find_package(SEAL CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(unofficial-b64 CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)

add_definitions(-DBUFFERSIZE=65536)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(src)
add_subdirectory(test)
include(java)
