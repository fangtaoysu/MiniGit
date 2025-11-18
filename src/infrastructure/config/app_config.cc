#include "app_config.h"
#include <fstream>
#include <iostream>

AppConfig& AppConfig::GetInstance() {
    static AppConfig instance;
    return instance;
}

bool AppConfig::LoadConfig(const std::filesystem::path& config_file) {
    std::ifstream file(config_file);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << config_file << std::endl;
        return false;
    }

    try {
        nlohmann::json json_obj;
        file >> json_obj;

        if (json_obj.contains("mysql")) {
            const auto& mysql_config = json_obj["mysql"];
            db_config_.enable = mysql_config.value("enable", false);
            db_config_.host = mysql_config.value("host", "127.0.0.1");
            db_config_.port = mysql_config.value("port", 3306);
            db_config_.user = mysql_config.value("user", "");
            db_config_.password = mysql_config.value("password", "");
            db_config_.db_name = mysql_config.value("db_name", "");
            db_config_.pool_size = mysql_config.value("pool_size", 4);
        }

        return true;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }
}