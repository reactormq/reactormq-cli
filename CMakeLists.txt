
cmake_minimum_required(VERSION 3.15)

project(reactormq-cli)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(${PROJECT_NAME}
  deps/linenoise.c
  src/main.c)

target_link_libraries(${PROJECT_NAME} hiredis)
