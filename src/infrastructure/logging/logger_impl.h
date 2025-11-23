#pragma once

#include "domain/interfaces/logger_interface.h"

namespace icd::infrastructure::logging {

class LoggerImpl : public domain::interfaces::ILogger {
public:
    void Log(domain::interfaces::LogLevel level,
             const std::string &message) override;
};

}  // namespace icd::infrastructure::logging
