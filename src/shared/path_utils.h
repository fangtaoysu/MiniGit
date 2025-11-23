#pragma once

#include <filesystem>

namespace PathUtils {
// 虽然不希望修改返回值，但是当前返回的是副本，因此返回值用const没有价值，除非将返回值设为引用
inline std::filesystem::path GetProjectRoot() {
    return std::filesystem::path(__FILE__)
        .parent_path()
        .parent_path()
        .parent_path();
}

}  // namespace PathUtils