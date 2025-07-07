#include "../include/repository.h"
#include <stdexcept>
#include <filesystem>
#include "../thrid_party/nlohmann/json.hpp"
#include "../include/config.h"


using json = nlohmann::json;
namespace fs = std::filesystem;

Repository::Repository() {

}

/**初始化仓库
 * 在project_path路径下，创建.mgit/config文件，写入当前项目的路径：project_path
 */
void Repository::init_repository(const std::string& project_path) {
    if (!project_path.empty() && !fs::exists(project_path)) {
        throw std::runtime_error("项目路径不存在：" + project_path);
    }

    project_path_ = project_path;
    const fs::path mgit_path =  fs::path(project_path) / ".mgit";
    fs::create_directories(mgit_path);

    Config config_obejct(project_path);
    config_obejct.user_.name = "fangtaoysu";
    config_obejct.user_.email = "fangtao@stumail.ysu.edu.cn";
    config_obejct.save_config();
}