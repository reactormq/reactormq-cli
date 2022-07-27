
cmake_minimum_required(VERSION 3.15)

project(reactormq-cli)

if(APPLE)
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
endif()

if(MSVC)
  find_package(hiredis CONFIG REQUIRED)
endif()

add_executable(${PROJECT_NAME}
  deps/crossline.c
  src/main.c)

if(MSVC)
  target_link_libraries(${PROJECT_NAME} PRIVATE hiredis::hiredis)
else()
  target_link_libraries(${PROJECT_NAME} hiredis)
endif()
