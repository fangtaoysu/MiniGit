#ifndef __COMMIT_H__
#define __COMMIT_H__

#include <sstream>


struct CommitObject{
    std::string father_ref;
    std::string current_ref;
    std::string current_timestamp;
    std::string msg;
};

class Commit {
    public:
        Commit(const std::string& msg, const std::string& father_ref);
        ~Commit() {}
        const CommitObject& get_commit_object() const;
        void run();

    private:
        const std::string& msg_;
        CommitObject commit_object_;
        const std::string father_ref_;

        // 返回值的const：调用者不能修改返回的字符串
        // 末尾的 const: 常量成员函数，函数不能修改类的成员变量
        const std::string get_hash() const;
        const std::string get_current_timestamp() const;
};

#endif