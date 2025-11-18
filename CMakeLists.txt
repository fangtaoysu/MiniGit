cmake_minimum_required(VERSION 3.16)
project(MiniGit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(USE_LOG4CPLUS "Enable log4cplus backend" ON)
option(USE_MYSQL "Enable MySQL database" ON)

if(USE_MYSQL)
  # Manually find MySQL C++ Connector since find_package may fail
  find_path(MYSQLCPPCONN_INCLUDE_DIR cppconn/driver.h
            HINTS /usr/include /usr/local/include)
  find_library(MYSQLCPPCONN_LIBRARY NAMES mysqlcppconn
               HINTS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)

  if(NOT MYSQLCPPCONN_INCLUDE_DIR OR NOT MYSQLCPPCONN_LIBRARY)
    message(FATAL_ERROR "Could not find MySQL C++ Connector. Please ensure libmysqlcppconn-dev is installed and in a standard path.")
  else()
    message(STATUS "Found MySQL C++ Connector include: ${MYSQLCPPCONN_INCLUDE_DIR}")
    message(STATUS "Found MySQL C++ Connector library: ${MYSQLCPPCONN_LIBRARY}")
  endif()
endif()

# Automatically discover common source files (interfaces and core logic)
file(GLOB MINIGIT_COMMON_SOURCES
    "src/infrastructure/config/*.cc"
    "src/infrastructure/logging/logger.cc"  # Only include the interface
    "src/infrastructure/concurrency/thread_pool_manager.cc"
    "src/presentation/scanner.cc"
)

add_library(minigit ${MINIGIT_COMMON_SOURCES})

if(USE_LOG4CPLUS)
  # Conditionally add the log4cplus implementation
  file(GLOB MINIGIT_LOG4CPLUS_SOURCES "src/infrastructure/logging/log4cplus_impl.cc")
  target_sources(minigit PRIVATE ${MINIGIT_LOG4CPLUS_SOURCES})
endif()

if(USE_MYSQL)
  # Add database source files
  set(MINIGIT_DB_SOURCES
    "src/infrastructure/database/connection_pool.cc"
    "src/infrastructure/database/db_connection_guard.cc"
    "src/infrastructure/database/database_manager.cc"
  )
  target_sources(minigit PRIVATE ${MINIGIT_DB_SOURCES})
endif()
target_include_directories(minigit PUBLIC 
    src
    ${CMAKE_SOURCE_DIR}
)

set(CONFIG_FILE_PATH "${CMAKE_SOURCE_DIR}/config/config.json")
target_compile_definitions(minigit PRIVATE CONFIG_FILE_PATH=\"${CONFIG_FILE_PATH}\")
target_compile_definitions(minigit PRIVATE $<$<BOOL:${USE_LOG4CPLUS}>:MINIGIT_USE_LOG4CPLUS> $<$<BOOL:${USE_MYSQL}>:MINIGIT_USE_MYSQL>)

# Manually specify log4cplus paths
set(LOG4CPLUS_INCLUDE_DIR "/usr/local/include")
set(LOG4CPLUS_LIBRARY "/usr/local/lib/liblog4cplus.so")
target_include_directories(minigit PUBLIC
  ${LOG4CPLUS_INCLUDE_DIR}
  "${CMAKE_SOURCE_DIR}/third_party/json_3.11.3/include" # For header-only nlohmann
  ${MYSQLCPPCONN_INCLUDE_DIR}
)

# Link libraries to minigit
# nlohmann is header-only, no linking needed

if(USE_MYSQL)
  target_link_libraries(minigit PUBLIC ${MYSQLCPPCONN_LIBRARY})
endif()

add_executable(minigit_app src/main.cpp)
if(USE_MYSQL)
  target_link_libraries(minigit_app PRIVATE minigit ${LOG4CPLUS_LIBRARY} ${MYSQLCPPCONN_LIBRARY})
else()
  target_link_libraries(minigit_app PRIVATE minigit ${LOG4CPLUS_LIBRARY})
endif()

# 添加 Google Test
enable_testing()
# 添加 Google Test
add_subdirectory(third_party/googletest)

# 添加 config_loader_test 可执行文件
add_executable(
  config_loader_test
  tests/infrastructure/config/config_loader_test.cc
)

target_link_libraries(
  config_loader_test
  PRIVATE
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

# 添加 database_test 可执行文件
add_executable(
  database_test
  tests/infrastructure/database/database_test.cc
)

# Pass the config file path to the test executable
target_compile_definitions(database_test PRIVATE CONFIG_FILE_PATH=\"${CONFIG_FILE_PATH}\")

target_link_libraries(
  database_test
  PRIVATE
  minigit
  gtest_main
  ${LOG4CPLUS_LIBRARY}
)

add_test(NAME database_test COMMAND database_test)

# Add thread_pool_manager_test executable
add_executable(
  thread_pool_manager_test
  tests/infrastructure/concurrency/thread_pool_manager_test.cc
)

target_link_libraries(
  thread_pool_manager_test
  PRIVATE
  minigit
  gtest_main
  ${LOG4CPLUS_LIBRARY}
)

add_test(NAME thread_pool_manager_test COMMAND thread_pool_manager_test)

# Add scanner_test executable
add_executable(
  scanner_test
  tests/presentation/scanner_test.cc
)

target_link_libraries(
  scanner_test
  PRIVATE
  minigit
  gtest_main
)

add_test(NAME scanner_test COMMAND scanner_test)
