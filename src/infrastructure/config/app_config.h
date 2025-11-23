#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

#include "shared/model.h"

// 用于管理所有应用配置的单例类
class AppConfig {
public:
    // 获取 AppConfig 的唯一实例
    static AppConfig& GetInstance();

    // 从 JSON 文件加载配置
    // 成功返回 true，失败返回 false
    bool LoadConfig(const std::filesystem::path& config_file);

    // 获取配置结构体的 const 引用
    const LoggingSettings& GetLoggingSettings() const { return logging_; }
    const MySqlSettings& GetMySqlSettings() const { return mysql_; }
    const ThreadPoolSettings& GetThreadPoolSettings() const {
        return thread_pool_;
    }

    AppConfig() = default;
    ~AppConfig() = default;

    // Deleting copy/move constructors and assignment operators to prevent
    // duplication.
    AppConfig(const AppConfig&) = delete;
    AppConfig& operator=(const AppConfig&) = delete;
    AppConfig(AppConfig&&) = delete;
    AppConfig& operator=(AppConfig&&) = delete;

private:
    // Configuration members initialized with default values.
    MySqlSettings mysql_{};
    LoggingSettings logging_{"info"};
    ThreadPoolSettings thread_pool_{4};
};
