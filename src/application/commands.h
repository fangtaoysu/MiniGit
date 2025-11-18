#pragma once

#include <string>
#include <vector>
#include <optional>

namespace application {

// Command DTO for 'init'
struct InitCommand {
    // 'init' currently takes no arguments or flags.
};

// Command DTO for 'add'
struct AddCommand {
    std::vector<std::string> path_specs;
    bool all = false;
};

// Command DTO for 'commit'
struct CommitCommand {
    std::optional<std::string> message;
    bool amend = false;
    bool all = false;
};

} // namespace application