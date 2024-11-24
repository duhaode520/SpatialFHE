# 设置项目名称变量
set(PROJECT_NAME SpatialFHE)
# set(CMAKE_TOOLCHAIN_FILE /home/duhaode/pri_comp/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "")
# set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
# 使用变量设置项目名称
cmake_minimum_required(VERSION 3.20)
project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CUDA CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_JAVA "Build Java Library" ON)

set(Phantom_DIR /cmake/phantom)
include_directories(/phantom)
# CUDA
enable_language(CUDA)
include(CheckLanguage)
check_language(CUDA)
find_package(CUDAToolkit REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CUDA_ARCHITECTURES native)

option(PHANTOM_USE_CUDA_PTX "Use CUDA PTX Assembly" ON)
message(STATUS "Use CUDA PTX Assembly: ${PHANTOM_USE_CUDA_PTX}")
if (PHANTOM_USE_CUDA_PTX)
    add_compile_definitions(PHANTOM_USE_CUDA_PTX)
endif ()
add_compile_definitions(PHANTOM_USE_CUDA_PTX)

find_package(SEAL CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(unofficial-b64 CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(Phantom CONFIG REQUIRED)

add_definitions(-DBUFFERSIZE=65536)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(src)
add_subdirectory(test)
include(java)
#