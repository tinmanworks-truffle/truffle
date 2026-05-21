#pragma once

#include <string>

namespace truffle::core {

struct RuntimeConfig {
    std::string application_name = "Truffle Consumer";
    bool enable_validation = true;
};

} // namespace truffle::core

