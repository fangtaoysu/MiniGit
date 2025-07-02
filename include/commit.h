#pragma once
#include <iostream>
#include <unordered_map>
#include <sstream>

class Commit {
    public:
        Commit(const std::string& msg, const std::string& father_ref, std::unordered_map<std::string, std::string> changed_files);
        ~Commit() {}
        const std::stringstream& get_commit_item() const;

        // 返回值的const：调用者不能修改返回的字符串
        // 末尾的 const: 常量成员函数，函数不能修改类的成员变量
        const std::string get_hash(const std::string& msg) const;
        const std::string get_current_timestamp() const;
        
    private:
        const std::string& msg_;
        std::stringstream commit_item_;
        const std::string father_ref_;
        
};