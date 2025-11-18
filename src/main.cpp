#include <iostream>

#include "infrastructure/config/config_loader.h"

int main() {
  minigit::infrastructure::config::ConfigLoader loader;
  loader.LoadConfig(); // Or any other type
  const auto& cfg = loader.GetConfig();

  std::cout << "Config loaded. mysql.enable="
            << (cfg.mysql.enable ? "true" : "false") << std::endl;
  return 0;
}