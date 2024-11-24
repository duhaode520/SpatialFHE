#JAVA文件输出目录 -outdir
# set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/java)
#指定一个输出目录名，用于放置生成的源文件。例如输出的exampleJAVA_wrap.cxx文件等的位置
# set(SWIG_OUTFILE_DIR ${CMAKE_CURRENT_BINARY_DIR}/cpp)
set_property(SOURCE spatialfhe.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE spatialfhe.i PROPERTY COMPILE_OPTIONS
  -package ${JAVA_PACKAGE}.spatialfhe)

# 设置CUDA编译器选项

include_directories(${JNI_INCLUDE_DIRS})
include_directories(../src)
link_directories(../src)

swig_add_library(jni${JAVA_ARTIFACT}
    TYPE SHARED 
    LANGUAGE java 
    OUTPUT_DIR ${JAVA_PROJECT_DIR}/${JAVA_SRC_PATH}/spatialfhe
    SOURCES spatialfhe.i)

set_target_properties(jni${JAVA_ARTIFACT} PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties(jni${JAVA_ARTIFACT} PROPERTIES
  SWIG_USE_TARGET_INCLUDE_DIRECTORIES ON
  POSITION_INDEPENDENT_CODE ON)

target_compile_options(jni${JAVA_ARTIFACT} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:SHELL:-xcuda --cuda-gpu-arch=sm_89>)

target_link_libraries(jni${JAVA_ARTIFACT} PRIVATE SpatialFHE)
target_link_libraries(jni${JAVA_ARTIFACT} PRIVATE SEAL::seal)
target_link_libraries(jni${JAVA_ARTIFACT} PRIVATE ZLIB::ZLIB)
target_link_libraries(jni${JAVA_ARTIFACT} PRIVATE zstd::libzstd)
target_link_libraries(jni${JAVA_ARTIFACT} PRIVATE unofficial::b64::b64)
target_link_libraries(jni${JAVA_ARTIFACT} PRIVATE rapidjson)

install(TARGETS jni${JAVA_ARTIFACT}
    EXPORT ${JAVA_ARTIFACT}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)