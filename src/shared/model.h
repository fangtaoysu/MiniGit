#pragma once
#include <string>

namespace minigit::shared {

struct LoggingSettings {
    std::string level{"info"};
};

struct MySqlSettings {
    bool enable{false};
    std::string host{"127.0.0.1"};
    int port{3306};
    std::string user{"root"};
    std::string password{};
    std::string db_name{""};
    int pool_size{0};
};

struct ThreadPoolSettings {
    int size{0};
};
}  // namespace minigit::shared