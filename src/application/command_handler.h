#pragma once

#include <any>

namespace application {

// Abstract base for type-erased command handling
class ICommandHandlerBase {
public:
    virtual ~ICommandHandlerBase() = default;
    virtual void Handle(const std::any& command) = 0;
};

// Interface for a specific command handler
template <typename TCommand>
class ICommandHandler : public ICommandHandlerBase {
public:
    virtual void Execute(const TCommand& command) = 0;

    void Handle(const std::any& command) final {
        Execute(std::any_cast<const TCommand&>(command));
    }
};

} // namespace application