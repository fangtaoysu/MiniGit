#include "infrastructure/logging/logger.h"

#include <iostream>

log4cplus::Logger gLogger = log4cplus::Logger::getRoot();

static void init_im_log4cplus_logger(const std::string &logfile) {
    if (logfile.empty()) {
        return;
    }

    log4cplus::initialize();

    try {
        log4cplus::PropertyConfigurator::doConfigure(
            LOG4CPLUS_STRING_TO_TSTRING(logfile));
        gLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("slogger"));
    } catch (const std::exception &e) {
        // 如果配置失败，仍然使用 root logger
        std::cerr << "Failed to initialize log4cplus logger: " << e.what()
                  << std::endl;
        gLogger = log4cplus::Logger::getRoot();
    }
}

namespace minigit::infrastructure::logging {
void InitImLogger(const std::string &logfile) {
    init_im_log4cplus_logger(logfile);
}
}  // namespace minigit::infrastructure::logging