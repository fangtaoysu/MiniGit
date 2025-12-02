#include "domain/core/blob.h"

#include <algorithm>
#include <fstream>

#include "domain/core/hash_util.h"
#include "infrastructure/logging/logger.h"

namespace minigit::domain::core {
std::string Blob::CalculateSha1() {
    return HashUtil::Sha1Hex(
        header_, std::span<const uint8_t>(data_.data(), data_.size()));
}

std::string Blob::GetSha1() const { return sha1_; }

std::span<const uint8_t> Blob::Serialize() const {
    return std::span<const uint8_t>(data_.data(), data_.size());
}

Blob Blob::Deserialize(const std::span<const uint8_t>& data) {
    std::array<uint8_t, 20> blob_data{};
    if (data.size() < blob_data.size()) {
        LOG_ERROR("Invalid blob data size.");
        return Blob(blob_data);
    }
    std::copy_n(data.begin(), blob_data.size(), blob_data.begin());
    return Blob(blob_data);
}

Blob Blob::CreateFromFile(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        LOG_ERROR("Failed to open file: " << file_path);
        return Blob(std::array<uint8_t, 20>{});
    }
    std::array<uint8_t, 20> file_data{};
    file.read(reinterpret_cast<char*>(file_data.data()), file_data.size());
    return Blob(file_data);
}

}  // namespace minigit::domain::core