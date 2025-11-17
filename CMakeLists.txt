cmake_minimum_required(VERSION 3.16)
project(MiniGit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(USE_LOG4CPLUS "Enable log4cplus backend" OFF)
option(USE_MYSQL "Enable MySQL database" OFF)

add_library(minigit
  src/infrastructure/config/config_reader.cc
)
target_include_directories(minigit PUBLIC src)
target_compile_definitions(minigit PRIVATE $<$<BOOL:${USE_LOG4CPLUS}>:MINIGIT_USE_LOG4CPLUS> $<$<BOOL:${USE_MYSQL}>:MINIGIT_USE_MYSQL>)
find_package(nlohmann_json QUIET)
if(nlohmann_json_FOUND)
  target_link_libraries(minigit PUBLIC nlohmann_json::nlohmann_json)
else()
  message(STATUS "nlohmann_json not found; assuming header-only is available in include path")
endif()

add_executable(minigit_app src/main.cpp)
target_link_libraries(minigit_app PRIVATE minigit)

enable_testing()