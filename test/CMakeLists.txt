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

#生成test选项和可执行文件 并进行链接
# add_executable(testA testA.cpp)
# target_link_libraries(testA gtest_main)

# add_executable(testB testB.cpp)
# target_link_libraries(testB gtest_main)

#这里创建了一个TestAll选项
FILE(GLOB_RECURSE CURRENT_TEST *.cpp)
add_executable(testAll ${CURRENT_TEST})
target_link_libraries(testAll gtest_main)
target_link_libraries(testAll SpatialFHE)

target_link_libraries(testAll SEAL::seal)
target_link_libraries(testAll ZLIB::ZLIB)
target_link_libraries(testAll zstd::libzstd)
target_link_libraries(testAll unofficial::b64::b64)
target_link_libraries(testAll rapidjson)
#####
#
# 这里追加新的Unit Test
#
#####
#gtest register
include(GoogleTest)
gtest_discover_tests(
	# testA
	# testB
	testAll
)