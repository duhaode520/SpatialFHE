cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(/usr/local/lib)


include(GNUInstallDirs)
# 添加所有源文件到库中
file(GLOB_RECURSE SOURCES "*.cpp" "*.cu")
# 添加所有头文件到库中
file(GLOB_RECURSE HEADERS "*.h" "*.cuh")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SEAL::seal)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} PRIVATE zstd::libzstd)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::b64::b64)
target_link_libraries(${PROJECT_NAME} PRIVATE rapidjson)
target_link_libraries(${PROJECT_NAME} PRIVATE libhdfs3.so)
target_link_libraries(${PROJECT_NAME} PRIVATE libPhantom.so)
target_link_libraries(${PROJECT_NAME} PRIVATE CUDA::cudart)

target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--default-stream per-thread -src-in-ptx>)
#target_compile_options(${PROJECT_NAME} PRIVATE $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CUDA>>:-G;-src-in-ptx>)
target_compile_features(${PROJECT_NAME} PUBLIC cuda_std_17 cxx_std_17)
