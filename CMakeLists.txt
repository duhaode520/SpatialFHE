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
# set cmake linker to lld if which lld is available
find_program(LLD_FOUND lld)
if (LLD_FOUND)
    message(STATUS "LLD found: ${LLD_FOUND}")
    set(CMAKE_LINKER ${LLD_FOUND})
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")

    message(STATUS "Debug mode")
#    add_definitions(-DDEBUG)
    set(TFHE_TARGET "release")
else()
    message(STATUS "Release mode")
    set(TFHE_TARGET "release")
endif()

option(BUILD_JAVA "Build Java Library" ON)

# Phantom
set(Phantom_DIR /cmake/phantom)
include_directories(/phantom)

# ----------------------------------------
# TFHE-rs
# ----------------------------------------
set(TFHE_C_API /home/ubuntu/github/tfhe-rs/target/${TFHE_TARGET})
message(STATUS "TFHE_C_API: ${TFHE_C_API}")
include_directories(${TFHE_C_API})
add_library(tfhe SHARED IMPORTED)
set_target_properties(tfhe PROPERTIES IMPORTED_LOCATION ${TFHE_C_API}/libtfhe.so)
option(WITH_FEATURE_GPU "Enable TFHE-rs GPU support" OFF)
if (WITH_FEATURE_GPU)
#    find_package(OpenMP REQUIRED)
    message(STATUS "Enable TFHE-rs GPU support")
    add_definitions(-DWITH_FEATURE_GPU)
endif ()

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
find_package(rpclib CONFIG REQUIRED)

add_definitions(-DBUFFERSIZE=65536)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(src)
add_subdirectory(test)
include(java)
#