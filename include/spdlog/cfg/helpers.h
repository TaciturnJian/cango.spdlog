

#pragma once

#include <spdlog/common.h>
#include <unordered_map>

namespace spdlog::cfg::helpers {
    //
    // Init levels from given string
    //
    // Examples:
    //
    // set global level to debug: "debug"
    // turn off all logging except for logger1: "off,logger1=debug"
    // turn off all logging except for logger1 and logger2: "off,logger1=debug,logger2=info"
    //
    void load_levels(const std::string &txt);
}
