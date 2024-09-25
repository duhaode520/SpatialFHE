cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(/usr/local/lib)
# 添加所有源文件到库中
file(GLOB_RECURSE SOURCES "*.cpp")
# 添加所有头文件到库中
file(GLOB_RECURSE HEADERS "*.h")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SEAL::seal)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} PRIVATE zstd::libzstd)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::b64::b64)
target_link_libraries(${PROJECT_NAME} PRIVATE rapidjson)
target_link_libraries(${PROJECT_NAME} PRIVATE libhdfs3.so)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)