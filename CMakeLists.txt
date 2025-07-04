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

# 主库配置
add_library(mini_git_lib
    src/commit.cpp
    src/file_system.cpp
    src/mini_git.cpp
    src/repository.cpp
    src/diff_engine.cpp
    src/utils.cpp
)

target_include_directories(mini_git_lib
    PUBLIC 
    include
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(mini_git_lib
    PRIVATE
    OpenSSL::Crypto
)

# 测试可执行文件
add_executable(test_commit
    tests/commit_test.cpp
)

add_executable(test_file_system
    tests/file_system_test.cpp
)

# 关键链接配置
target_link_libraries(test_commit
    PRIVATE
        mini_git_lib
        GTest::GTest
        GTest::Main  # 必须包含Main库
)

target_link_libraries(test_file_system
    PRIVATE
        mini_git_lib
        GTest::GTest
        GTest::Main
)

# 设置输出路径
set_target_properties(test_commit PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_commit
)
set_target_properties(test_file_system PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_file_system
)

# 测试配置
enable_testing()
add_test(NAME CommitTest COMMAND test_commit)
add_test(NAME FileSystemTest COMMAND test_file_system)