#include "application.hpp"
#include "cli.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    using namespace truffle::examples::host_workspace;

    const auto commandLine = parse_command_line(argc, argv);
    if (!commandLine.ok) {
        std::cerr << commandLine.error << '\n';
        print_usage(std::cerr);
        return 1;
    }

    if (commandLine.showHelp) {
        print_usage(std::cout);
        return 0;
    }

    return run_application(commandLine.options);
}
