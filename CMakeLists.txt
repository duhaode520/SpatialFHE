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

option(BUILD_JAVA "Build Java Library" ON)

# Phantom
set(Phantom_DIR /cmake/phantom)
include_directories(/phantom)

# ----------------------------------------
# Concrete
# ----------------------------------------
# concrete-compiler
set(CONCRETE_DIR /home/ubuntu/github/concrete)
set(CONCRETE_COMPILER_DIR ${CONCRETE_DIR}/compilers/concrete-compiler/compiler)
set(CONCRETE_COMPILER_BUILD_DIR ${CONCRETE_COMPILER_DIR}/build_Debug)
include_directories(${CONCRETE_COMPILER_BUILD_DIR}/install/concretecompiler/include)
link_directories(${CONCRETE_COMPILER_BUILD_DIR}/install/concretecompiler/lib)
# concrete-protocol
set(CONCRETE_PROTOCOL_DIR ${CONCRETE_COMPILER_BUILD_DIR}/tools/concretelang/concrete-protocol)
include_directories(${CONCRETE_PROTOCOL_DIR})
include_directories(${CONCRETE_PROTOCOL_DIR}/capnp_src_dir/c++/src)
# concrete-optimizer
set(CONCRETE_OPTIMIZER_DIR ${CONCRETE_DIR}/compilers/concrete-optimizer)
include_directories(${CONCRETE_OPTIMIZER_DIR}/concrete-optimizer-cpp/src/cpp)
# llvm
include_directories(${CONCRETE_COMPILER_BUILD_DIR}/include) # for llvm-config.h
include_directories(${CONCRETE_COMPILER_DIR}/../llvm-project/llvm/include) # for some .def files
# concrete-cpu
include_directories(${CONCRETE_DIR}/backends/concrete-cpu/implementation/include)
set(CONCRETE_CPU_STATIC_LIB ${CONCRETE_DIR}/backends/concrete-cpu/implementation/target/release/libconcrete_cpu.a)
set(CONCRETE_CPU_NOISE_MODEL_STATIC_LIB ${CONCRETE_DIR}/backends/concrete-cpu/noise-model/target/release/libconcrete_cpu_noise_model.a)
link_directories(${CONCRETE_DIR}/backends/concrete-cpu/implementation/target/release)
link_directories(${CONCRETE_DIR}/backends/concrete-cpu/noise-model/target/release)

# ----------------------------------------
# TFHE-rs
# ----------------------------------------
set(TFHE_C_API "/home/ubuntu/github/tfhe-rs/target/debug")
include_directories(${TFHE_C_API})
add_library(tfhe SHARED IMPORTED)
set_target_properties(tfhe PROPERTIES IMPORTED_LOCATION ${TFHE_C_API}/libtfhe.so)

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