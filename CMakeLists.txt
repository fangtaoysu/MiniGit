cmake_minimum_required(VERSION 3.16)
project(MiniGit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(USE_LOG4CPLUS "Enable log4cplus backend" ON)
option(USE_MYSQL "Enable MySQL database" OFF)

add_library(minigit
  src/infrastructure/config/config_loader.cc
  src/infrastructure/logging/logger.cc
)
target_include_directories(minigit PUBLIC src)

set(CONFIG_FILE_PATH "${CMAKE_SOURCE_DIR}/config/config.json")
target_compile_definitions(minigit PRIVATE CONFIG_FILE_PATH=\"${CONFIG_FILE_PATH}\")
target_compile_definitions(minigit PRIVATE $<$<BOOL:${USE_LOG4CPLUS}>:MINIGIT_USE_LOG4CPLUS> $<$<BOOL:${USE_MYSQL}>:MINIGIT_USE_MYSQL>)

set(JSON_INCLUDE_DIR "/data/local/json_3.11.3/include")
target_include_directories(minigit PUBLIC ${JSON_INCLUDE_DIR})

# Manually specify log4cplus paths
set(LOG4CPLUS_INCLUDE_DIR "/usr/local/include")
set(LOG4CPLUS_LIBRARY "/usr/local/lib/liblog4cplus.so")
target_include_directories(minigit PUBLIC ${LOG4CPLUS_INCLUDE_DIR})

add_executable(minigit_app src/main.cpp)
target_link_libraries(minigit_app PRIVATE minigit ${LOG4CPLUS_LIBRARY})

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Add config_loader_test executable
add_executable(
  config_loader_test
  tests/infrastructure/config/config_loader_test.cc
)
target_link_libraries(
  config_loader_test
  minigit
  gtest_main
  ${LOG4CPLUS_LIBRARY}
)

#  Add logger_test executable
add_executable(
  logger_test
  tests/infrastructure/logging/logger_tests.cc
)
target_link_libraries(
  logger_test
  minigit
  gtest_main
  ${LOG4CPLUS_LIBRARY}
)

add_test(NAME config_loader_test COMMAND config_loader_test)
add_test(NAME logger_test COMMAND logger_test)
