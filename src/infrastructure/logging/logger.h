#pragma once

// 如下三个用于main函数中初始化日志系统
#include <log4cplus/configurator.h>
#include <log4cplus/hierarchy.h>
#include <log4cplus/initializer.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include <cstring>
#include <filesystem>
#include <sstream>
#include <string>
#include <type_traits>

#include "shared/path_utils.h"

extern log4cplus::Logger gLogger;
namespace minigit::infrastructure::logging {
void InitImLogger(const std::string &logfile);
}

namespace logging_detail {

// 括号流式包装器
template <typename Logger>
class BracketStream {
public:
    BracketStream(Logger &&logger) : m_logger(std::move(logger)) {}

    // 禁用拷贝构造和拷贝赋值
    BracketStream(const BracketStream &) = delete;
    BracketStream &operator=(const BracketStream &) = delete;

    // 添加移动构造和移动赋值
    BracketStream(BracketStream &&other) noexcept
        : m_logger(std::move(other.m_logger)) {}

    BracketStream &operator=(BracketStream &&other) noexcept {
        if (this != &other) {
            m_logger = std::move(other.m_logger);
        }
        return *this;
    }

    template <typename T>
    BracketStream &operator<<(const T &value) {
        if constexpr (std::is_same_v<T, std::filesystem::path>) {
            m_logger << value.string();
        } else {
            m_logger << value;
        }
        return *this;
    }

    // 处理特殊操作符
    BracketStream &operator<<(std::ostream &(*manip)(std::ostream &)) {
        m_logger << manip;
        return *this;
    }

private:
    Logger m_logger;
};

// 基础日志流
class BaseLogStream {
public:
    BaseLogStream(log4cplus::Logger logger, log4cplus::LogLevel level,
                  const char *file, int line)
        : m_logger(logger), m_level(level), m_moved(false) {
        std::string file_path = file;
        std::string project_src_path =
            (minigit::shared::GetProjectRoot() / "src").string();

        // 查找 "src" 在文件路径中的位置
        size_t src_pos = file_path.find(project_src_path);
        if (src_pos != std::string::npos) {
            // 提取从 "src" 开始的相对路径
            std::string relative_path = file_path.substr(
                src_pos + project_src_path.length() + 1);  // +1 跳过路径分隔符
            m_buffer << "[" << relative_path << ":" << line << "] ";
        } else {
            // 如果找不到，使用文件名
            std::filesystem::path fs_path(file);
            m_buffer << "[" << fs_path.filename().string() << ":" << line
                     << "] ";
        }
    }

    // 禁用拷贝构造和拷贝赋值
    BaseLogStream(const BaseLogStream &) = delete;
    BaseLogStream &operator=(const BaseLogStream &) = delete;

    // 添加移动构造和移动赋值
    BaseLogStream(BaseLogStream &&other) noexcept
        : m_logger(std::move(other.m_logger)),
          m_level(other.m_level),
          m_buffer(std::move(other.m_buffer)),
          m_moved(false) {
        other.m_moved = true;  // 标记源对象已被移动
    }

    BaseLogStream &operator=(BaseLogStream &&other) noexcept {
        if (this != &other) {
            m_logger = std::move(other.m_logger);
            m_level = other.m_level;
            m_buffer = std::move(other.m_buffer);
            m_moved = false;
            other.m_moved = true;  // 标记源对象已被移动
        }
        return *this;
    }

    ~BaseLogStream() {
        try {
            // 如果对象已被移动，不执行日志输出
            if (m_moved) {
                return;
            }

            // 更安全的检查方式 - 先检查缓冲区
            std::string message = m_buffer.str();
            if (message.empty()) {
                return;  // 没有内容需要输出
            }

            // 简单的日志输出 - 避免复杂的级别检查
            log4cplus::tstring tmessage = LOG4CPLUS_STRING_TO_TSTRING(message);

            // 使用最简单的日志调用方式
            switch (m_level) {
                case log4cplus::DEBUG_LOG_LEVEL:
                    if (m_logger.isEnabledFor(log4cplus::DEBUG_LOG_LEVEL)) {
                        LOG4CPLUS_DEBUG_STR(m_logger, tmessage);
                    }
                    break;
                case log4cplus::INFO_LOG_LEVEL:
                    if (m_logger.isEnabledFor(log4cplus::INFO_LOG_LEVEL)) {
                        LOG4CPLUS_INFO_STR(m_logger, tmessage);
                    }
                    break;
                case log4cplus::WARN_LOG_LEVEL:
                    if (m_logger.isEnabledFor(log4cplus::WARN_LOG_LEVEL)) {
                        LOG4CPLUS_WARN_STR(m_logger, tmessage);
                    }
                    break;
                case log4cplus::ERROR_LOG_LEVEL:
                    if (m_logger.isEnabledFor(log4cplus::ERROR_LOG_LEVEL)) {
                        LOG4CPLUS_ERROR_STR(m_logger, tmessage);
                    }
                    break;
                case log4cplus::FATAL_LOG_LEVEL:
                    if (m_logger.isEnabledFor(log4cplus::FATAL_LOG_LEVEL)) {
                        LOG4CPLUS_FATAL_STR(m_logger, tmessage);
                    }
                    break;
                default:
                    if (m_logger.isEnabledFor(log4cplus::INFO_LOG_LEVEL)) {
                        LOG4CPLUS_INFO_STR(m_logger, tmessage);
                    }
                    break;
            }
        } catch (...) {
            // 析构函数中不应该抛出异常，静默处理
            // 可以考虑输出到 stderr 进行调试
            // std::cerr << "Log output failed in destructor" << std::endl;
        }
    }

    template <typename T>
    BaseLogStream &operator<<(const T &value) {
        if constexpr (std::is_same_v<T, std::filesystem::path>) {
            m_buffer << value.string();
        } else {
            m_buffer << value;
        }
        return *this;
    }

    BaseLogStream &operator<<(std::ostream &(*manip)(std::ostream &)) {
        manip(m_buffer);
        return *this;
    }

private:
    log4cplus::Logger m_logger;
    log4cplus::LogLevel m_level;
    std::ostringstream m_buffer;
    bool m_moved;
};
}  // namespace logging_detail

// 括号风格的日志宏
#define LOG_INFO(...)                                            \
    logging_detail::BracketStream(logging_detail::BaseLogStream( \
        gLogger, log4cplus::INFO_LOG_LEVEL, __FILE__, __LINE__)) \
        << __VA_ARGS__
#define LOG_DEBUG(...)                                            \
    logging_detail::BracketStream(logging_detail::BaseLogStream(  \
        gLogger, log4cplus::DEBUG_LOG_LEVEL, __FILE__, __LINE__)) \
        << __VA_ARGS__
#define LOG_WARN(...)                                            \
    logging_detail::BracketStream(logging_detail::BaseLogStream( \
        gLogger, log4cplus::WARN_LOG_LEVEL, __FILE__, __LINE__)) \
        << __VA_ARGS__
#define LOG_ERROR(...)                                            \
    logging_detail::BracketStream(logging_detail::BaseLogStream(  \
        gLogger, log4cplus::ERROR_LOG_LEVEL, __FILE__, __LINE__)) \
        << __VA_ARGS__
#define LOG_FATAL(...)                                            \
    logging_detail::BracketStream(logging_detail::BaseLogStream(  \
        gLogger, log4cplus::FATAL_LOG_LEVEL, __FILE__, __LINE__)) \
        << __VA_ARGS__
