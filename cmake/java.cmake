#if(NOT BUILD_JAVA)
#  return()
#endif()

# Will need swig
set(CMAKE_SWIG_FLAGS)
find_package(SWIG REQUIRED)
include(UseSWIG)
include(${SWIG_USE_FILE})

# Find Java and JNI
find_package(Java COMPONENTS Development REQUIRED)
find_package(JNI REQUIRED)

# Find maven
find_program(MAVEN_EXECUTABLE mvn)
if(NOT MAVEN_EXECUTABLE)
  message(FATAL_ERROR "Check for maven Program: not found")
else()
  message(STATUS "Found Maven: ${MAVEN_EXECUTABLE}")
endif()

# Needed by java/CMakeLists.txt
set(JAVA_DOMAIN_NAME "ade")
set(JAVA_DOMAIN_EXTENSION "org")

set(JAVA_GROUP "${JAVA_DOMAIN_EXTENSION}.${JAVA_DOMAIN_NAME}")
set(JAVA_ARTIFACT "SpatialFHE")
set(JAVA_HOME "/usr/lib/jvm/java-8-openjdk-amd64")

set(JAVA_PACKAGE "${JAVA_GROUP}.${JAVA_ARTIFACT}")
set(NATIVE_IDENTIFIER linux-x86-64)

set(JAVA_NATIVE_PROJECT ${JAVA_ARTIFACT}-${NATIVE_IDENTIFIER})
message(STATUS "Java runtime project: ${JAVA_NATIVE_PROJECT}")
set(JAVA_NATIVE_PROJECT_DIR ${PROJECT_BINARY_DIR}/java/${JAVA_NATIVE_PROJECT})
message(STATUS "Java runtime project build path: ${JAVA_NATIVE_PROJECT_DIR}")

set(JAVA_PROJECT ${JAVA_ARTIFACT}-java)
message(STATUS "Java project: ${JAVA_PROJECT}")
set(JAVA_PROJECT_DIR ${PROJECT_BINARY_DIR}/java/${JAVA_PROJECT})
message(STATUS "Java project build path: ${JAVA_PROJECT_DIR}")

# Creative the native library
# set_target_properties(jni${JAVA_ARTIFACT} PROPERTIES
#   POSITION_INDEPENDENT_CODE ON)
# set_target_properties(jni${JAVA_ARTIFACT} PROPERTIES INSTALL_RPATH "$ORIGIN")

# Swig wrap all libraries
set(JAVA_SRC_PATH src/main/java/${JAVA_DOMAIN_EXTENSION}/${JAVA_DOMAIN_NAME}/${JAVA_ARTIFACT})
set(JAVA_TEST_PATH src/test/java/${JAVA_DOMAIN_EXTENSION}/${JAVA_DOMAIN_NAME}/${JAVA_ARTIFACT})
set(JAVA_RESSOURCES_PATH src/main/resources)

add_subdirectory(java)

#################################
##  Java Native Maven Package  ##
#################################
file(MAKE_DIRECTORY ${JAVA_NATIVE_PROJECT_DIR}/${JAVA_RESSOURCES_PATH}/${JAVA_NATIVE_PROJECT})
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/java_api/src/main/resources/${JAVA_NATIVE_PROJECT})

configure_file(${PROJECT_SOURCE_DIR}/java/pom-native.xml.in ${JAVA_NATIVE_PROJECT_DIR}/pom.xml @ONLY)

add_custom_command(
  OUTPUT ${JAVA_NATIVE_PROJECT_DIR}/timestamp
  COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:jni${JAVA_ARTIFACT}>
    ${JAVA_RESSOURCES_PATH}/${JAVA_NATIVE_PROJECT}/
  COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:jni${JAVA_ARTIFACT}>
        ${PROJECT_SOURCE_DIR}/java_api/src/main/resources/${JAVA_NATIVE_PROJECT}/
  COMMAND ${MAVEN_EXECUTABLE} clean
  COMMAND ${MAVEN_EXECUTABLE} compile -B
  COMMAND ${MAVEN_EXECUTABLE} package -B -Dfatjar=true
  COMMAND ${MAVEN_EXECUTABLE} install -B $<$<BOOL:${SKIP_GPG}>:-Dgpg.skip=true>
  COMMAND ${CMAKE_COMMAND} -E touch ${JAVA_NATIVE_PROJECT_DIR}/timestamp
  DEPENDS
    ${JAVA_NATIVE_PROJECT_DIR}/pom.xml
  BYPRODUCTS
    ${JAVA_NATIVE_PROJECT_DIR}/target
  COMMENT "Generate Java native package ${JAVA_NATIVE_PROJECT} (${JAVA_NATIVE_PROJECT_DIR}/timestamp)"
  WORKING_DIRECTORY ${JAVA_NATIVE_PROJECT_DIR})

add_custom_target(java_native_package
  DEPENDS
    ${JAVA_NATIVE_PROJECT_DIR}/timestamp
  WORKING_DIRECTORY ${JAVA_NATIVE_PROJECT_DIR})

##########################
##  Java Maven Package  ##
##########################
file(MAKE_DIRECTORY ${JAVA_PROJECT_DIR}/${JAVA_SRC_PATH})

configure_file( ${PROJECT_SOURCE_DIR}/java/pom.xml.in ${JAVA_PROJECT_DIR}/pom.xml @ONLY)

file(GLOB_RECURSE java_files RELATIVE ${PROJECT_SOURCE_DIR}/java "java/*.java")

set(JAVA_SRCS)
foreach(JAVA_FILE IN LISTS java_files)
  #message(STATUS "java: ${JAVA_FILE}")
  set(JAVA_OUT ${JAVA_PROJECT_DIR}/${JAVA_SRC_PATH}/${JAVA_FILE})
  #message(STATUS "java out: ${JAVA_OUT}")
  add_custom_command(
    OUTPUT ${JAVA_OUT}
    COMMAND ${CMAKE_COMMAND} -E copy
      ${PROJECT_SOURCE_DIR}/java/${JAVA_FILE}
      ${JAVA_OUT}
    DEPENDS ${PROJECT_SOURCE_DIR}/java/${JAVA_FILE}
    COMMENT "Copy Java file ${JAVA_FILE}"
    VERBATIM)
  list(APPEND JAVA_SRCS ${JAVA_OUT})
endforeach()

add_custom_command(
  OUTPUT ${JAVA_PROJECT_DIR}/timestamp
  COMMAND ${MAVEN_EXECUTABLE} clean
  COMMAND ${MAVEN_EXECUTABLE} compile -B
  COMMAND ${MAVEN_EXECUTABLE} package -B -Dfatjar=true
  COMMAND ${MAVEN_EXECUTABLE} install -B $<$<BOOL:${SKIP_GPG}>:-Dgpg.skip=true>
  COMMAND ${CMAKE_COMMAND} -E touch ${JAVA_PROJECT_DIR}/timestamp
  DEPENDS
    ${JAVA_PROJECT_DIR}/pom.xml
    ${JAVA_SRCS}
  java_native_package
  BYPRODUCTS
    ${JAVA_PROJECT_DIR}/target
  COMMENT "Generate Java package ${JAVA_PROJECT} (${JAVA_PROJECT_DIR}/timestamp)"
  WORKING_DIRECTORY ${JAVA_PROJECT_DIR})

add_custom_target(java_package ALL
  DEPENDS
    ${JAVA_PROJECT_DIR}/timestamp
  WORKING_DIRECTORY ${JAVA_PROJECT_DIR})
