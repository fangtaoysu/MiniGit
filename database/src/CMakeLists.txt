# 查找 sqlite3 依赖
find_package(SQLite3 REQUIRED)

# 添加主库目标为共享库
add_library(git_db SHARED
    git_db.cpp
)

set_target_properties(git_db PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    OUTPUT_NAME git_db
)

# 设置包含目录
target_include_directories(git_db
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    PRIVATE
    ${SQLite3_INCLUDE_DIRS}
)

# 链接 SQLite3
target_link_libraries(git_db
    PRIVATE
    SQLite::SQLite3
)

# 添加 SQLite 实现子目录
add_subdirectory(sqlite)

