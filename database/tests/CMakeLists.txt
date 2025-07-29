# 添加库搜索路径
link_directories(${CMAKE_BINARY_DIR}/lib)

add_executable(test_database
    test_database.cpp
)

target_link_libraries(test_database
    PRIVATE
    git_db
    GTest::GTest
    GTest::Main
)

enable_testing()
add_test(NAME DATABASETEST COMMAND test_database)