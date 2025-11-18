#pragma once

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "application/command_handler.h"
#include "infrastructure/logging/logger.h"

namespace application {

class CommandBus {
public:
    template <typename TCommand>
    void RegisterHandler(std::unique_ptr<ICommandHandler<TCommand>> handler) {
        std::type_index type_index(typeid(TCommand));
        if (handlers_.count(type_index)) {
            LOG_WARN("Handler for command type " << type_index.name() << " is already registered. Overwriting.");
        }
        handlers_[type_index] = std::move(handler);
    }

    void Dispatch(const std::any& command) {
        std::type_index type_index(command.type());
        auto it = handlers_.find(type_index);
        if (it != handlers_.end()) {
            it->second->Handle(command);
        } else {
            LOG_ERROR("No handler registered for command type " << type_index.name());
            // Depending on requirements, this could throw an exception.
        }
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<ICommandHandlerBase>> handlers_;
};

} // namespace application