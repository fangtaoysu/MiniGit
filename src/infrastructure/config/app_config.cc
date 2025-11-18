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
            mysql_.enable = mysql_config.value("enable", false);
            mysql_.host = mysql_config.value("host", "127.0.0.1");
            mysql_.port = mysql_config.value("port", 3306);
            mysql_.user = mysql_config.value("user", "");
            mysql_.password = mysql_config.value("password", "");
            mysql_.db_name = mysql_config.value("db_name", "");
            mysql_.pool_size = mysql_config.value("pool_size", 4);
        }

        if (json_obj.contains("thread_pool")) {
            const auto& thread_pool_config = json_obj["thread_pool"];
            thread_pool_.size = thread_pool_config.value("size", 4);
        }

        if (json_obj.contains("logging")) {
            const auto& logging_config = json_obj["logging"];
            logging_.level = logging_config.value("level", "info");
        }

        return true;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }
}