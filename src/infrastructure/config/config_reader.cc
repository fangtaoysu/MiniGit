#include "infrastructure/config/config_reader.h"
#include "infrastructure/logging/logger.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace minigit::infra::config {

using minigit::infra::logging::Level;
using minigit::infra::logging::Logger;

static std::string to_lower(std::string s) {
  for (auto& c : s) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
  return s;
}

bool load_from_file(const std::string& path, Config& out, std::string* error) {
  try {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
      if (error) *error = "cannot open: " + path;
      return false;
    }
    nlohmann::json j;
    ifs >> j;

    if (j.contains("logging")) {
      auto jl = j["logging"];
      if (jl.contains("level")) out.logging.level = to_lower(jl["level"].get<std::string>());
    }

    if (j.contains("mysql")) {
      auto jm = j["mysql"];
      if (jm.contains("enable")) out.mysql.enable = jm["enable"].get<bool>();
      if (jm.contains("host")) out.mysql.host = jm["host"].get<std::string>();
      if (jm.contains("port")) out.mysql.port = jm["port"].get<int>();
      if (jm.contains("user")) out.mysql.user = jm["user"].get<std::string>();
      if (jm.contains("password")) out.mysql.password = jm["password"].get<std::string>();
      if (jm.contains("database")) out.mysql.database = jm["database"].get<std::string>();
    }

    if (j.contains("thread_pool")) {
      auto jt = j["thread_pool"];
      if (jt.contains("size")) out.thread_pool.size = jt["size"].get<int>();
    }

    Logger::instance().log(Level::Info, std::string{"Config loaded: "} + path);
    return true;
  } catch (const std::exception& ex) {
    if (error) *error = ex.what();
    Logger::instance().log(Level::Error, std::string{"Config load error: "} + ex.what());
    return false;
  }
}

}  // namespace minigit::infra::config