#include "domain/core/hash_util.h"

#include <openssl/evp.h>
#include <openssl/sha.h>

#include <iomanip>
#include <memory>
#include <sstream>

namespace minigit::domain::core {
std::string HashUtil::Sha1Hex(const std::string& header,
                              std::span<const uint8_t> data) {
    unsigned char digest[SHA_DIGEST_LENGTH];
    unsigned int out_len = 0;
    std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx(
        EVP_MD_CTX_new(), &EVP_MD_CTX_free);
    if (!ctx) return {};
    if (EVP_DigestInit_ex(ctx.get(), EVP_sha1(), nullptr) != 1) return {};
    if (EVP_DigestUpdate(ctx.get(), header.data(), header.size()) != 1)
        return {};
    if (EVP_DigestUpdate(ctx.get(), data.data(), data.size()) != 1) return {};
    if (EVP_DigestFinal_ex(ctx.get(), digest, &out_len) != 1 ||
        out_len != SHA_DIGEST_LENGTH)
        return {};
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
        oss << std::setw(2) << static_cast<int>(digest[i]);
    return oss.str();
}
}  // namespace minigit::domain::core