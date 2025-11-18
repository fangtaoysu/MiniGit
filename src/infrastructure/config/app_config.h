#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>

// 用于数据库配置的结构体
struct DatabaseConfig {
    bool enable = false;
    std::string host;
    int port = 3306;
    std::string user;
    std::string password;
    std::string db_name;
    int pool_size = 4;
};

// 用于管理所有应用配置的单例类
class AppConfig {
public:
    // 获取 AppConfig 的唯一实例
    static AppConfig& GetInstance();

    // 从 JSON 文件加载配置
    // 成功返回 true，失败返回 false
    bool LoadConfig(const std::filesystem::path& config_file);

    // 获取配置结构体的 const 引用
    const DatabaseConfig& GetDatabaseConfig() const { return db_config_; }

    // 删除拷贝和移动构造函数及赋值运算符，确保单例的唯一性
    AppConfig(const AppConfig&) = delete;
    AppConfig& operator=(const AppConfig&) = delete;
    AppConfig(AppConfig&&) = delete;
    AppConfig& operator=(AppConfig&&) = delete;

private:
    // 私有构造和析构函数，用于单例模式
    AppConfig() = default;
    ~AppConfig() = default;

    // 配置成员
    DatabaseConfig db_config_;
};
