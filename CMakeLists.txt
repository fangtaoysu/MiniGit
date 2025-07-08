cmake_minimum_required(VERSION 3.12)
project(MiniGit
    VERSION 1.0
    LANGUAGES CXX
)

# 强制使用系统GTest（避开conda环境）
set(GTest_DIR "/usr/lib/x86_64-linux-gnu/cmake/GTest")  # 关键修改点

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找依赖
find_package(OpenSSL REQUIRED)
find_package(GTest REQUIRED)  # 必须放在OpenSSL之后

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
)

target_include_directories(mini_git_lib
    PUBLIC 
    include
    ${OPENSSL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

target_link_libraries(mini_git_lib
    PRIVATE
    OpenSSL::Crypto
)
# ========== 项目入口 mini_git ==========
add_executable(main
    main.cpp
)

target_link_libraries(main
    PRIVATE mini_git_lib
)

set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# ========== 构建测试文件 ==========
# 1. commit_test
add_executable(test_commit
    tests/commit_test.cpp
)

target_link_libraries(test_commit
    PRIVATE
    mini_git_lib
    GTest::GTest
    GTest::Main  # 必须包含Main库
)

set_target_properties(test_commit PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 2. file_system_test
add_executable(test_file_system
    tests/file_system_test.cpp
)

target_link_libraries(test_file_system
    PRIVATE
    mini_git_lib
    GTest::GTest
    GTest::Main
)

set_target_properties(test_file_system PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
        
# 3. index_test
add_executable(test_index
    tests/index_test.cpp
)

target_link_libraries(test_index
    PRIVATE
    mini_git_lib
    GTest::GTest
    GTest::Main
)

set_target_properties(test_index PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 4.utils_test
add_executable(test_utils
    tests/utils_test.cpp
)

target_link_libraries(test_utils
    PRIVATE    
    mini_git_lib
    GTest::GTest
    GTest::Main
)

set_target_properties(test_utils PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 集成测试
add_executable(integration_test
    examples/integration_test.cpp
)

target_link_libraries(integration_test
    PRIVATE
    mini_git_lib
)

set_target_properties(integration_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 测试配置
enable_testing()
add_test(NAME CommitTest COMMAND test_commit)
add_test(NAME FileSystemTest COMMAND test_file_system)
add_test(NAME IndexTest COMMAND test_index)
add_test(NAME UtilsTest COMMAND test_utils)