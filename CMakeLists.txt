# 设置项目名称变量
set(PROJECT_NAME MyLibrary)

# 使用变量设置项目名称
project(${PROJECT_NAME})

# 添加SEAL库
find_package(SEAL REQUIRED)

# 添加所有源文件到库中
file(GLOB SOURCES "*.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# 链接SEAL库
target_link_libraries(${PROJECT_NAME} SEAL::seal)