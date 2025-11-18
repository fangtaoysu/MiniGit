#include "infrastructure/logging/logger_impl.h"

#include "infrastructure/logging/logger.h"

namespace icd::infrastructure::logging
{

void LoggerImpl::Log(domain::interfaces::LogLevel level, const std::string &message)
{
    switch (level)
    {
        case domain::interfaces::LogLevel::Debug:
            LOG_DEBUG(message);
            break;
        case domain::interfaces::LogLevel::Info:
            LOG_INFO(message);
            break;
        case domain::interfaces::LogLevel::Warn:
            LOG_WARN(message);
            break;
        case domain::interfaces::LogLevel::Error:
            LOG_ERROR(message);
            break;
    }
}

}  // namespace icd::infrastructure::logging
