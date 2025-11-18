#include "infrastructure/config/config_impl.h"
#include <sstream>

namespace minigit::infrastructure::config {

ConfigurationImpl::ConfigurationImpl(const AppConfig& config)
    : config_(config) {}

std::string ConfigurationImpl::GetDatabaseConnectionString() const {
    const auto& db_config = config_.GetMySqlSettings();
    if (!db_config.enable) {
        return "";
    }

    std::stringstream ss;
    ss << "mysql://" << db_config.user << ":" << db_config.password
       << "@" << db_config.host << ":" << db_config.port
       << "/" << db_config.db_name;
    return ss.str();
}

int ConfigurationImpl::GetThreadPoolSize() const {
    const auto& pool_config = config_.GetThreadPoolSettings();
    if (pool_config.size < 0) {
        return 0; 
    }
    return pool_config.size;
}

std::string ConfigurationImpl::GetLogLevel() const {
    return config_.GetLoggingSettings().level;
}

std::optional<std::string> ConfigurationImpl::GetConfigValue(const std::string& key) const {

    if (key == "log_level") {
        return GetLogLevel();
    }
    
    return std::nullopt;
}

} // namespace minigit::infrastructure::config