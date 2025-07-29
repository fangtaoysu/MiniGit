add_library(git_db_sqlite STATIC
    git_db_sqlite.cpp
)

target_link_libraries(git_db_sqlite
    PRIVATE
    git_db
)