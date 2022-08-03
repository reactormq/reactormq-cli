
cmake_minimum_required(VERSION 3.0)

project(reactormq-cli
  DESCRIPTION "ReactorMQ CLI"
  VERSION 1.0.0
  LANGUAGES C)

set(CMAKE_C_STANDARD 99)

if(APPLE)
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
endif()

if (MSVC)
  include_directories("C:/Program Files (x86)/hiredis/include")
  link_directories("C:/Program Files (x86)/hiredis/lib")
endif()

set(BINARY_DIR ${CMAKE_BINARY_DIR}/../bin)
set(PACKAGE_DIR ${CMAKE_BINARY_DIR}/../package)

if(MSVC)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BINARY_DIR})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BINARY_DIR})
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR})
endif()

add_executable(${PROJECT_NAME}
  deps/crossline.c
  src/main.c)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(${PROJECT_NAME} hiredis)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(fmt TGZ)
set(CPACK_GENERATOR ${fmt})
set(CPACK_SOURCE_GENERATOR ${fmt})

set(CPACK_OUTPUT_FILE_PREFIX ${PACKAGE_DIR})
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

string(TOLOWER ${PROJECT_NAME} project)
string(TOLOWER ${CMAKE_SYSTEM_NAME} os)
string(TOLOWER ${CMAKE_HOST_SYSTEM_PROCESSOR} arch)

if(${os} STREQUAL "darwin")
  set(os "macos")
endif()

if(${arch} MATCHES "^(x86_64|amd64)$")
  set(arch "x64")
elseif(${arch} MATCHES "^(i386|i586|i686|i786|i886)$")
  set(arch "x86")
elseif(${arch} MATCHES "^(aarch64|armv8)$")
  set(arch "arm64")
endif()

set(CPACK_PACKAGE_FILE_NAME "${project}-${PROJECT_VERSION}-${os}-${arch}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${project}-${PROJECT_VERSION}")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

install(DIRECTORY bin/ DESTINATION bin)
install(FILES ${CPACK_RESOURCE_FILE_README} ${CPACK_RESOURCE_FILE_LICENSE} DESTINATION docs)

include(CPack)
