#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

#include "shared/model.h"

namespace utils = minigit::shared;

// 用于管理所有应用配置的单例类
namespace minigit::infrastructure::config {

class AppConfig {
public:
    // 获取 AppConfig 的唯一实例
    static AppConfig& GetInstance();

    // 从 JSON 文件加载配置
    // 成功返回 true，失败返回 false
    bool LoadConfig(const std::filesystem::path& config_file);

    // 获取配置结构体的 const 引用
    const utils::LoggingSettings& GetLoggingSettings() const {
        return logging_;
    }
    const utils::MySqlSettings& GetMySqlSettings() const { return mysql_; }
    const utils::ThreadPoolSettings& GetThreadPoolSettings() const {
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
    utils::MySqlSettings mysql_{};
    utils::LoggingSettings logging_{"info"};
    utils::ThreadPoolSettings thread_pool_{4};
};

}  // namespace minigit::infrastructure::config
