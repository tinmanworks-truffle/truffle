#pragma once

#include "application.hpp"

#include <ostream>
#include <string>

namespace truffle::examples::host_workspace {

struct CommandLineParse {
    bool ok = true;
    bool showHelp = false;
    ApplicationOptions options;
    std::string error;
};

[[nodiscard]] CommandLineParse parse_command_line(int argc, char* argv[]);
void print_usage(std::ostream& output);

} // namespace truffle::examples::host_workspace
