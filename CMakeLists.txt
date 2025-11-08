cmake_minimum_required(VERSION 3.20)

project(MiniGit LANGUAGES CXX) # CXX 即 cpp

# 主要的C++库
add_library(mini_git_lib STATIC
    # 主要源码
    src/repository.cpp
    src/index.cpp
    src/commit.cpp
    src/config.cpp
    src/remote.cpp
    src/utils.cpp
    src/file_system.cpp
    src/object_db.cpp
    src/thread_pool.cpp
    src/diff_engine.cpp
    src/command_parser/command_dispatcher.cpp
    src/command_parser/parser.cpp
    src/command_parser/commands/cmd_init.cpp
    src/command_parser/commands/cmd_add.cpp
    src/command_parser/commands/cmd_commit.cpp
    src/command_parser/commands/cmd_config.cpp
    src/command_parser/commands/cmd_status.cpp
    src/command_parser/commands/cmd_push.cpp

    # 数据库源码
    database/src/git_db.cpp

    # sqlite3源码
    third_party/sqlite3/sqlite3.c
)


add_executable(mini_git main.cpp)

if(MSVC)
    target_compile_options(mini_git_lib PUBLIC /utf-8)
endif()

# 为SQLite3源文件设置特定的编译属性
set_source_files_properties(third_party/sqlite3/sqlite3.c PROPERTIES
    LANGUAGE C
    COMPILE_DEFINITIONS "SQLITE_ENABLE_COLUMN_METADATA;SQLITE_ENABLE_FTS5;SQLITE_ENABLE_JSON1;SQLITE_ENABLE_RTREE;SQLITE_ENABLE_STAT4;SQLITE_ENABLE_UPDATE_DELETE_LIMIT;SQLITE_SOUNDEX;SQLITE_TEMP_STORE=1;SQLITE_THREADSAFE=1"
)

target_compile_features(
    mini_git_lib
    PUBLIC
    cxx_std_17
)

# 定义链接阶段的任务
target_include_directories(mini_git_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/database/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/nlohmann
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite3
)

# 确保C++标准只对C++文件应用
target_compile_features(mini_git_lib PUBLIC cxx_std_17)
set_source_files_properties(third_party/sqlite3/sqlite3.c PROPERTIES COMPILE_FLAGS "")

# 链接Windows加密库（用于utils.cpp中的SHA1函数）
if(WIN32)
    target_link_libraries(mini_git_lib PUBLIC advapi32)
endif()

# 链接主可执行文件
target_link_libraries(mini_git mini_git_lib)
