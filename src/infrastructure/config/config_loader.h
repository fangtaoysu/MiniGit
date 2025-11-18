#pragma once

#include <string>
#include <filesystem>

#include "shared/path_utils.h"

namespace minigit::infrastructure::config {

struct LoggingSettings {
    std::string level{"info"};
};

struct MySqlSettings {
    bool enable{false};
    std::string host{"127.0.0.1"};
    int port{3306};
    std::string user{"root"};
    std::string password{};
    std::string database{};
};

struct ThreadPoolSettings {
    int size{0};
};

struct Config {
    LoggingSettings logging;
    MySqlSettings mysql;
    ThreadPoolSettings thread_pool;
};

class ConfigLoader {
public:
    void LoadConfig();
    const Config& GetConfig() const;

private:
    Config config_;
};

}  // namespace minigit::infrastructure::config