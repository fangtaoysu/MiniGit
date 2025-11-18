#pragma once

#include "domain/interfaces/config_interface.h"
#include "infrastructure/config/app_config.h"

namespace minigit::infrastructure::config {

class ConfigurationImpl : public domain::interfaces::IConfiguration {
public:
    // Dependency Injection: Constructor takes a reference to the config source.
    explicit ConfigurationImpl(const AppConfig& config);

    // No longer needed as we are injecting the config.
    // domain::interfaces::XsdConfig GetXsdConfig() const override;
    
    std::string GetDatabaseConnectionString() const override;
    int GetThreadPoolSize() const override;
    std::string GetLogLevel() const override;

    // Improved return semantics.
    std::optional<std::string> GetConfigValue(const std::string& key) const override;

private:
    // Holds a reference to the application's configuration.
    const AppConfig& config_;
};

} // namespace minigit::infrastructure::config
