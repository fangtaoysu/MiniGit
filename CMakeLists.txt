cmake_minimum_required(VERSION 3.12)
project(MiniGit
    VERSION 1.0
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# 设置 GTest 路径（替换成你的实际路径）
set(GTEST_ROOT "/home/fangtao/anaconda3")
set(GTEST_LIB_DIR "${GTEST_ROOT}/lib")
set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")

# 添加 GTest 头文件路径和库路径
include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIB_DIR})

# 查找必要的库
find_package(OpenSSL REQUIRED)  # 添加OpenSSL查找
find_package(Threads REQUIRED)  # GTest需要线程库

# 添加主库
add_library(mini_git_lib
    src/commit.cpp
    src/file_system.cpp
    src/mini_git.cpp
    src/repository.cpp
)

# 头文件路径
target_include_directories(mini_git_lib
    PUBLIC 
        include
        ${OPENSSL_INCLUDE_DIR}  # 添加OpenSSL头文件路径
)

# 链接OpenSSL库
target_link_libraries(mini_git_lib
    PRIVATE
        OpenSSL::Crypto  # 链接OpenSSL加密库
)

# 添加测试可执行文件
add_executable(test_commit
    tests/commit_test.cpp
)

# 正确链接所有依赖库（注意顺序）
target_link_libraries(test_commit
    PRIVATE
        mini_git_lib
        ${GTEST_LIB_DIR}/libgtest.so
        ${GTEST_LIB_DIR}/libgtest_main.so
        Threads::Threads
)

# 启用测试
enable_testing()
add_test(NAME CommitTest COMMAND test_commit)