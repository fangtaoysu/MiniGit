#ifndef __GIT_DB_H__
#define __GIT_DB_H__
#include <sqlite3.h>
#include <string>
#include <stdexcept>
#include <variant>
#include <vector>
#include <unordered_map>
#include <variant>



using SQL_param = std::variant<int, double, std::string, std::nullptr_t>;

class Database {
    public:
        explicit Database(const std::string& filename);
        ~Database();

        // 执行无返回的sql
        void execute(const std::string& sql);

        // 执行查询、返回结果
        std::vector<std::unordered_map<std::string, std::string>> query(const std::string& sql);

        // 参数化查询
        void execute_with_params(const std::string& sql, const std::vector<SQL_param>& params);
        std::vector<std::unordered_map<std::string, std::string>> query_with_params(const std::string& sql, const std::vector<SQL_param>& params);

    private:
        sqlite3* db_;

        void check_error(int rc, const std::string& context);
};

#endif