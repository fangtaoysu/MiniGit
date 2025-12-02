#pragma once
#include <cstdint>
#include <span>
#include <string>

namespace minigit::domain::core {
class HashUtil {
public:
    static std::string Sha1Hex(const std::string& header,
                               std::span<const uint8_t> data);
};
}  // namespace minigit::domain::core
