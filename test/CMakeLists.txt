cmake_minimum_required(VERSION 3.10)

include_directories(../src)
link_directories(../src)
#动态插入Gtest 代码
#这一段取自 https://google.github.io/googletest/quickstart-cmake.html
#其中zip 的hash值根据 googletest github 上release 版本选取
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()


#这里创建了一个TestAll选项
add_executable(seal_test InitSuite.cu FsManagerSuite.cpp SealCryptoSuite.cu SpatialFHEManagerSuite.cu)
target_link_libraries(seal_test PRIVATE gtest_main)
target_link_libraries(seal_test PRIVATE SpatialFHE)

target_link_libraries(seal_test PRIVATE SEAL::seal)
target_link_libraries(seal_test PRIVATE ZLIB::ZLIB)
target_link_libraries(seal_test PRIVATE zstd::libzstd)
target_link_libraries(seal_test PRIVATE unofficial::b64::b64)
target_link_libraries(seal_test PRIVATE rapidjson)

add_executable(phantom_test PhantomCryptoSuite.cu)

target_link_libraries(phantom_test PRIVATE gtest_main)
target_link_libraries(phantom_test PRIVATE SEAL::seal)
target_link_libraries(phantom_test PRIVATE SpatialFHE)
target_link_libraries(phantom_test PRIVATE ZLIB::ZLIB)
target_link_libraries(phantom_test PRIVATE zstd::libzstd)
target_link_libraries(phantom_test PRIVATE unofficial::b64::b64)
target_link_libraries(phantom_test PRIVATE rapidjson)

target_compile_options(phantom_test PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--default-stream per-thread>)
target_compile_options(phantom_test PRIVATE $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CUDA>>:-G;-src-in-ptx>)
#####
#
# 这里追加新的Unit Test
#
#####
#gtest register
include(GoogleTest)
gtest_discover_tests(
	seal_test phantom_test
)