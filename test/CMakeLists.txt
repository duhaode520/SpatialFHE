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
add_executable(seal_test InitSuite.cu FsManagerSuite.cpp SealCryptoSuite.cu)
target_link_libraries(seal_test PRIVATE gtest_main SpatialFHE SEAL::seal ZLIB::ZLIB zstd::libzstd unofficial::b64::b64 rapidjson)

add_executable(phantom_test PhantomCryptoSuite.cu)
target_link_libraries(phantom_test PRIVATE gtest_main SpatialFHE SEAL::seal ZLIB::ZLIB zstd::libzstd unofficial::b64::b64 rapidjson)
target_compile_options(phantom_test PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--default-stream per-thread>)
target_compile_options(phantom_test PRIVATE $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CUDA>>:-G;-src-in-ptx>)

#add_executable(spatialfhe_test SealSpatialFHEManagerSuite.cu PhantomSpatialFHEManagerSuite.cu)
add_executable(spatialfhe_test PhantomSpatialFHEManagerSuite.cu)
target_link_libraries(spatialfhe_test PRIVATE gtest_main SpatialFHE SEAL::seal ZLIB::ZLIB zstd::libzstd unofficial::b64::b64 rapidjson)
target_compile_options(spatialfhe_test PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--default-stream per-thread>)
target_compile_options(spatialfhe_test PRIVATE $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CUDA>>:-G;-src-in-ptx>)

add_executable(tfhers_demo TfhersDemo.cpp )
target_link_libraries(tfhers_demo PRIVATE gtest_main SpatialFHE SEAL::seal ZLIB::ZLIB zstd::libzstd unofficial::b64::b64 rapidjson)
target_link_libraries(tfhers_demo PRIVATE tfhe)

add_executable(tfhers_perf_test TFHEPerfTest.cpp)
#target_link_libraries(tfhers_test PRIVATE gtest_main SpatialFHE SEAL::seal ZLIB::ZLIB zstd::libzstd unofficial::b64::b64 rapidjson)
target_link_libraries(tfhers_perf_test PRIVATE tfhe gtest_main)

add_executable(tfhe_test TFHEContextSuite.cpp)
target_link_libraries(tfhe_test PRIVATE gtest_main SpatialFHE SEAL::seal ZLIB::ZLIB zstd::libzstd unofficial::b64::b64 rapidjson)
target_link_libraries(tfhe_test PRIVATE tfhe)

# 抓取 tfhe_geos 中的文件
#file(GLOB_RECURSE TFHE_GEOS_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/tfhe_geos/*.cpp)
file (GLOB_RECURSE TFHE_GEOS_HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/tfhe_geos/*.h)


#add_executable(tfhe_geos_test ${TFHE_GEOS_SRC_FILES})
add_executable(tfhe_geos_test tfhe_geos/operation/relateng/TFHERelateNGTest.cpp)
target_link_libraries(tfhe_geos_test PRIVATE gtest_main SpatialFHE)
target_link_libraries(tfhe_geos_test PRIVATE tfhe)

# 测试单个test
add_executable(tfhe_geos_single_test tfhe_geos/algorithm/PolygonNodeTopologyTest.cpp)
target_link_libraries(tfhe_geos_single_test PRIVATE gtest_main SpatialFHE)
target_link_libraries(tfhe_geos_single_test PRIVATE tfhe)

add_executable(remote_test RemoteDecryptSuite.cpp)
target_link_libraries(remote_test PRIVATE gtest_main SpatialFHE tfhe)

add_executable(tfhe_geos_perf_test
	perf_test/PerformanceTestCase.cpp
	perf_test/PerformanceTestRunner.cpp
	perf_test/RelateNGLinesOverlappingPerfTest.cpp
	perf_test/RelateNGPolygonOverlappingPerfTest.cpp
	perf_test/PerfTestSuite.cpp
)
target_link_libraries(tfhe_geos_perf_test PRIVATE gtest_main SpatialFHE tfhe)


#####
#
# 这里追加新的Unit Test
#
#####
#gtest register
include(GoogleTest)
gtest_discover_tests(
	seal_test phantom_test spatialfhe_test
	tfhers_demo tfhers_perf_test remote_test tfhe_test
	tfhe_geos_test tfhe_geos_single_test tfhe_geos_perf_test
)