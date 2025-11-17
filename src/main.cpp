#include <iostream>
#include "infrastructure/config/config_reader.h"


int main() {
  minigit::infra::config::Config cfg{};
  std::string err;
  if (!minigit::infra::config::load_from_file("config/config.json", cfg, &err)) {
    std::cerr << "Config load failed: " << err << std::endl;
    return 1;
  }
  std::cout << "Config loaded. mysql.enable=" << (cfg.mysql.enable ? "true" : "false") << std::endl;
  return 0;
}