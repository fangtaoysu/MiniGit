#pragma once

#include <span>
#include <cstdint>
#include <array>
#include <string>

namespace minigit::domain::core {
/**
 * Blob 对象：表示文件的原始内容（不含文件名和路径）。
 * - 负责计算并暴露内容的 SHA-1 哈希
 * - 提供序列化/反序列化接口以读写对象数据
 * - 在 Tree 条目中以 Blob 形式被引用
 */
class Blob {
public:
    explicit Blob(const std::array<uint8_t, 20>& data) : data_(data) {
        header_ = "blob " + std::to_string(data_.size()) + '\0';
        sha1_ = CalculateSha1();
    }

    std::string CalculateSha1();
    std::string GetSha1() const;
    std::span<const uint8_t> Serialize() const;

    /**
     * @brief 从字节数据反序列化Blob对象
     * 
     * @param data 包含Blob对象序列化数据的字节span
     * @return Blob 反序列化后的Blob对象
     */
    static Blob Deserialize(const std::span<const uint8_t>& data);
    /**
     * @brief 根据文件创建Blob对象
     * 
     * @return std::span<const uint8_t> Blob对象的内容数据span
     */
    static Blob CreateFromFile(const std::string& file_path);


private:
    // array是在栈上分配的，vector是在堆上分配的，因此当前场景下array性能更好
    std::array<uint8_t, 20> data_;
    std::string sha1_;
    std::string header_;
};



} // namespace domain::core