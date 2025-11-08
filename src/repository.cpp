#include "../include/repository.h"
#include <stdexcept>
#include <filesystem>
#include "../third_party/nlohmann/json.hpp"
#include "../include/config.h"


using json = nlohmann::json;
namespace fs = std::filesystem;

Repository::Repository() {
    project_path_ = fs::current_path().string();
}

/**初始化仓库
 * 在project_path路径下，创建.mgit/config文件，写入当前项目的路径：project_path
 */
void Repository::init_repository() {
    if (!project_path_.empty() && !fs::exists(project_path_)) {
        throw std::runtime_error("项目路径不存在：" + project_path_);
    }

    const fs::path mgit_path =  fs::path(project_path_) / ".mgit";
    fs::create_directories(mgit_path);

    Config config_obejct(project_path_);
    config_obejct.save_config();
    Remote remote_object;
    remote_object.create_database();
}

void Repository::config(const std::unordered_map<std::string, std::string>& info) {
    Config config_obejct(project_path_);
    for (auto& item : info) {
        if (item.first == "user.name") {
            config_obejct.user_.name = item.second;
        } else if (item.first == "user.email") {
            config_obejct.user_.email = item.second;
        }
    }

    config_obejct.save_config();
}
