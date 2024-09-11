# using swig to generate java code from c++ code in the src directory
cmake_minimum_required(VERSION 3.10)

#JAVA文件输出目录 -outdir
# set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/java)
#指定一个输出目录名，用于放置生成的源文件。例如输出的exampleJAVA_wrap.cxx文件等的位置
# set(SWIG_OUTFILE_DIR ${CMAKE_CURRENT_BINARY_DIR}/cpp)
set_property(SOURCE spatialfhe.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE spatialfhe.i PROPERTY COMPILE_OPTIONS
  -package ${JAVA_PACKAGE}.spatialfhe)

include_directories(${JNI_INCLUDE_DIRS})
include_directories(../src)
link_directories(../src)
swig_add_library(jni${JAVA_ARTIFACT}
    TYPE SHARED 
    LANGUAGE java 
    OUTPUT_DIR ${JAVA_PROJECT_DIR}/${JAVA_SRC_PATH}/spatialfhe
    SOURCES spatialfhe.i)

target_link_libraries(jni${JAVA_ARTIFACT} PRIVATE SEAL::seal)
target_link_libraries(jni${JAVA_ARTIFACT} PRIVATE ZLIB::ZLIB)
target_link_libraries(jni${JAVA_ARTIFACT} PRIVATE zstd::libzstd)
target_link_libraries(jni${JAVA_ARTIFACT} PRIVATE unofficial::b64::b64)
target_link_libraries(jni${JAVA_ARTIFACT} PRIVATE rapidjson)