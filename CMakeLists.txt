cmake_minimum_required(VERSION 3.12)
project(MiniGit
    VERSION 1.0
    LANGUAGES CXX
)

# 强制使用系统GTest（避开conda环境）
set(GTest_DIR "/usr/lib/x86_64-linux-gnu/cmake/GTest")
# 关键配置：强制生成调试符号，禁用优化
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找依赖
find_package(OpenSSL REQUIRED)
find_package(GTest REQUIRED)
find_package(SQLite3 REQUIRED)

# 添加子目录
add_subdirectory(database/src)
add_subdirectory(database/tests)

# ========== 构建主库 mini_git_lib ==========
add_library(mini_git_lib
    src/commit.cpp
    src/file_system.cpp
    src/diff_engine.cpp
    src/utils.cpp
    src/index.cpp
    src/repository.cpp
    src/config.cpp
    src/object_db.cpp
    src/thread_pool.cpp
    src/remote.cpp
    src/command_parser/parser.cpp
    src/command_parser/command_dispatcher.cpp
    src/command_parser/commands/cmd_init.cpp
    src/command_parser/commands/cmd_add.cpp
    src/command_parser/commands/cmd_commit.cpp
    src/command_parser/commands/cmd_config.cpp
    src/command_parser/commands/cmd_status.cpp
    src/command_parser/commands/cmd_push.cpp
)

target_include_directories(mini_git_lib
    PUBLIC 
    include
    include/command_parser
    include/command_parser/commands
    ${OPENSSL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${CMAKE_CURRENT_SOURCE_DIR}/database/include
)

target_link_libraries(mini_git_lib
    PRIVATE
    OpenSSL::Crypto
    git_db
)

# ========== 项目入口 mini_git ==========
add_executable(main
    main.cpp
)

target_link_libraries(main
    PRIVATE mini_git_lib git_db
)

# ========== 构建测试文件 ==========
# 1. commit_test
add_executable(test_commit tests/commit_test.cpp)
target_link_libraries(test_commit PRIVATE mini_git_lib GTest::GTest GTest::Main)

# 2. file_system_test
add_executable(test_file_system tests/file_system_test.cpp)
target_link_libraries(test_file_system PRIVATE mini_git_lib GTest::GTest GTest::Main)
        
# 3. index_test
add_executable(test_index tests/index_test.cpp)
target_link_libraries(test_index PRIVATE mini_git_lib GTest::GTest GTest::Main)

# 4. utils_test
add_executable(test_utils tests/utils_test.cpp)
target_link_libraries(test_utils PRIVATE mini_git_lib GTest::GTest GTest::Main)

# 5. object_db_test
add_executable(test_object_db tests/object_db_test.cpp)
target_link_libraries(test_object_db PRIVATE mini_git_lib GTest::GTest GTest::Main)

# 集成测试
add_executable(integration_test examples/integration_test.cpp)
target_link_libraries(integration_test PRIVATE mini_git_lib)

# 测试配置
enable_testing()
add_test(NAME CommitTest COMMAND test_commit)
add_test(NAME FileSystemTest COMMAND test_file_system)
add_test(NAME IndexTest COMMAND test_index)
add_test(NAME UtilsTest COMMAND test_utils)
add_test(NAME ObjectDBTest COMMAND test_object_db)