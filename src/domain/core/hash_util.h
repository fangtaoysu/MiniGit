#pragma once
#include <cstdint>
#include <string>
#include <span>

namespace minigit::domain::core {
class HashUtil {
public:
    static std::string Sha1Hex(const std::string& header,
                               std::span<const uint8_t> data);
};
}  // namespace domain::core
