#include <gtest/gtest.h>
#include "../include/index.h"
#include <fstream>
#include "../thrid_party/nlohmann/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

TEST(IndexTest, ReadIndex) {
    const std::string project_path("/data/fangtao/MiniGit/tmp");
    Index index_obejct(project_path);
    json result = index_obejct.read_index();
    json true_result = {};
    if (fs::exists(project_path + "/.mgit/index")) {
        std::ifstream(project_path + "/.mgit/index") >> true_result;
    }

    EXPECT_EQ(result, true_result);
}