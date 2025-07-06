/** 程序的入口位置
 * 初始化仓库，加载配置文件 */

#include <filesystem>
#include <string>
#include <stdexcept>


namespace fs = std::filesystem;
void init_repository(const std::string& project_path) {
    if (!project_path.empty() && !fs::exists(project_path)) {
        throw std::runtime_error("项目路径不存在：" + project_path);
    }
    const fs::path mgit_path =  fs::path(project_path) / ".mgit";
    fs::create_directories(mgit_path);
}

int main() {
    const std::string project_path("/data/fangtao/MiniGit/tmp");
    return 0;
}