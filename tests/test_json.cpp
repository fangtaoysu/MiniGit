#include "../third_party/nlohmann/json.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>


int main() {
    using json = nlohmann::json;
    namespace fs = std::filesystem;

    // 写入
    json index = {
    {"文件哈希值1", {{"path", "src/index.cpp"}, {"size", 10032}, {"modified", 3428027352}}}
    };
    const std::string file_path = "../tmp/.mgit/index";
    fs::path path(file_path);
    fs::path parent = path.parent_path();

    if (!parent.empty() && !fs::exists(parent)) {
        fs::create_directories(parent);
    }
    std::ofstream(file_path) << index.dump(2);

    // 读取
    json index1;
    std::ifstream(file_path) >> index1;
    std::string path1 = index1["文件哈希值1"]["path"];
    std::cout << path1 << std::endl;

    return 0;
}