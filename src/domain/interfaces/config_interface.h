#pragma once

#include <optional>
#include <string>

namespace minigit::domain::interfaces {

/**
 * @brief Defines the contract for accessing application configuration.
 *
 * This interface provides an abstraction for retrieving configuration values,
 * allowing the domain layer to remain independent of the specific
 * configuration source (e.g., file, environment variables, etc.).
 */
class IConfiguration {
public:
    virtual ~IConfiguration() = default;

    /**
     * @brief Gets the database connection string.
     * @return The connection string, or an empty string if not configured.
     */
    virtual std::string GetDatabaseConnectionString() const = 0;

    /**
     * @brief Gets the configured size of the thread pool.
     * @return The size of the thread pool.
     */
    virtual int GetThreadPoolSize() const = 0;

    /**
     * @brief Gets the configured logging level (e.g., "info", "debug").
     * @return The log level string.
     */
    virtual std::string GetLogLevel() const = 0;

    /**
     * @brief Gets a configuration value by its key.
     * @param key The key of the configuration value to retrieve.
     * @return An std::optional<std::string> containing the value if the key
     *         exists, or std::nullopt if it does not.
     */
    virtual std::optional<std::string> GetConfigValue(
        const std::string& key) const = 0;
};

}  // namespace minigit::domain::interfaces
