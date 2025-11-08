#include "../include/git_db.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    const std::string DB_FILE = "/data/fangtao/MiniGit/MiniGit.db";  // 写死的数据库文件名
    
    try {
        // 初始化数据库连接
        Database db(DB_FILE);
        
        std::cout << "SQLite Interactive Shell (Database: " << DB_FILE << ")\n";
        std::cout << "Enter SQL commands or 'exit' to quit\n";
        
        while (true) {
            std::cout << "> ";
            std::string input;
            
            // 读取输入（支持多行输入直到分号结束）
            while (true) {
                std::string line;
                if (!std::getline(std::cin, line)) {  // 处理Ctrl+D
                    std::cout << "\nExiting...\n";
                    return 0;
                }
                
                if (line == "exit") {
                    return 0;
                }
                
                input += line;
                
                // 检查是否以分号结束（简单的SQL语句结束判断）
                if (!input.empty() && input.back() == ';') {
                    input.pop_back();  // 移除分号
                    break;
                }
                
                input += " ";  // 添加空格分隔多行
            }
            
            try {
                // 判断是查询还是执行
                if (input.find("SELECT") == 0 || input.find("select") == 0) {
                    auto results = db.query(input);
                    
                    // 打印结果
                    if (!results.empty()) {
                        // 打印表头
                        for (const auto& [key, _] : results[0]) {
                            std::cout << key << "\t| ";
                        }
                        std::cout << "\n---------------------------------\n";
                        
                        // 打印数据
                        for (const auto& row : results) {
                            for (const auto& [_, value] : row) {
                                std::cout << value << "\t| ";
                            }
                            std::cout << "\n";
                        }
                    }
                    std::cout << results.size() << " row(s) returned.\n";
                } else {
                    db.execute(input);
                    std::cout << "Command executed successfully.\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Database error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}