# 设置项目名称变量
set(PROJECT_NAME SpatialFHE)
set(CMAKE_TOOLCHAIN_FILE /home/duhaode/pri_comp/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "")
set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
# 使用变量设置项目名称
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME})

find_package(SEAL CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(unofficial-b64 CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)

add_definitions(-DBUFFERSIZE=65536)

# 添加所有源文件到库中
file(GLOB SOURCES "*.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE SEAL::seal)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} PRIVATE zstd::libzstd)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::b64::b64)
target_link_libraries(${PROJECT_NAME} PRIVATE rapidjson)