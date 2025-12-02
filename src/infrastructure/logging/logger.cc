#include "infrastructure/logging/logger.h"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

log4cplus::Logger gLogger = log4cplus::Logger::getRoot();

// 更健壮的时区设置函数
static bool SetTimeZoneRobust() {
    // 方法1: 使用setenv（主要方法）
    setenv("TZ", "Asia/Shanghai", 1);
    tzset();

    // 方法2: 验证时区是否真的设置成功
    std::time_t now = std::time(nullptr);
    std::tm *local_tm = std::localtime(&now);
    std::tm *utc_tm = std::gmtime(&now);

    // 计算时区偏移
    int hour_diff = local_tm->tm_hour - utc_tm->tm_hour;
    if (hour_diff < 0) hour_diff += 24;

    // 上海时区应该是UTC+8
    bool success = (hour_diff == 8);

    if (!success) {
        std::cerr << "Timezone verification failed. Local: UTC+" << hour_diff
                  << ", Expected: UTC+8" << std::endl;
    }

    return success;
}

static void init_im_log4cplus_logger(const std::string &logfile) {
    if (logfile.empty()) {
        return;
    }

    // 设置时区，但不显示警告
    SetTimeZoneRobust();

    log4cplus::initialize();

    try {
        log4cplus::PropertyConfigurator::doConfigure(
            LOG4CPLUS_STRING_TO_TSTRING(logfile));
        gLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("slogger"));

    } catch (const std::exception &e) {
        std::cerr << "Failed to initialize log4cplus logger: " << e.what()
                  << std::endl;
        gLogger = log4cplus::Logger::getRoot();
    }
}

namespace minigit::infrastructure::logging {

void InitImLogger(const std::string &logfile) {
    // 设置时区，静默处理
    setenv("TZ", "Asia/Shanghai", 1);
    tzset();

    init_im_log4cplus_logger(logfile);

// 只在调试时显示时间信息
#ifdef DEBUG
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S %Z");
    std::cout << "Current time: " << ss.str() << std::endl;
#endif
}

}  // namespace minigit::infrastructure::logging