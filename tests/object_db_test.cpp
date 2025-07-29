#include "gtest/gtest.h"
#include "../include/object_db.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class ObjectDBTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建临时测试目录结构
        test_dir = fs::temp_directory_path() / "object_db_test";
        fs::create_directories(test_dir / ".mgit/objects");
        
        // 初始化测试文件
        CreateTestFile(test_dir / "test_json.json", R"({"key": "value"})");
        CreateTestFile(test_dir / "test_lines.txt", "line1\nline2\nline3");
        
        // 设置环境变量或修改Utils::get_project_path()返回测试目录
        setenv("MGIT_PROJECT_PATH", test_dir.c_str(), 1);
    }

    void TearDown() override {
        fs::remove_all(test_dir);
        unsetenv("MGIT_PROJECT_PATH");
    }

    void CreateTestFile(const fs::path& path, const std::string& content) {
        std::ofstream file(path);
        file << content;
        file.close();
    }

    fs::path test_dir;
};

// 测试读取JSON文件
TEST_F(ObjectDBTest, ReadJsonSuccess) {
    // 准备测试数据
    const std::string hash = "abc123";  // 测试hash值
    const fs::path obj_path = Utils::generate_obj_path(test_dir, hash);
    fs::create_directories(obj_path.parent_path());
    fs::copy_file(test_dir / "test_json.json", obj_path);

    ObjectDB db;
    auto result = db.read<nlohmann::json>(hash);
    
    EXPECT_TRUE(result.is_object());
    EXPECT_EQ(result["key"], "value");
}

// 测试读取文本行
TEST_F(ObjectDBTest, ReadLinesSuccess) {
    // 准备测试数据
    const std::string hash = "def456";  // 测试hash值
    const fs::path obj_path = Utils::generate_obj_path(test_dir, hash);
    fs::create_directories(obj_path.parent_path());
    fs::copy_file(test_dir / "test_lines.txt", obj_path);

    ObjectDB db;
    auto result = db.read<std::vector<std::string>>(hash);
    
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "line1");
    EXPECT_EQ(result[1], "line2");
    EXPECT_EQ(result[2], "line3");
}

// 测试读取不存在的文件
TEST_F(ObjectDBTest, ReadNonExistentFile) {
    ObjectDB db;
    const std::string hash = "non_existent";
    
    EXPECT_THROW({
        db.read<nlohmann::json>(hash);
    }, std::runtime_error);
    
    EXPECT_THROW({
        db.read<std::vector<std::string>>(hash);
    }, std::runtime_error);
}

// 测试空文件
TEST_F(ObjectDBTest, ReadEmptyFile) {
    const std::string hash = "empty_file";
    const fs::path obj_path = Utils::generate_obj_path(test_dir, hash);
    fs::create_directories(obj_path.parent_path());
    CreateTestFile(obj_path, "");

    ObjectDB db;
    
    EXPECT_THROW(db.read<nlohmann::json>(hash), nlohmann::json::parse_error);
    
    auto lines = db.read<std::vector<std::string>>(hash);
    EXPECT_TRUE(lines.empty());
}
