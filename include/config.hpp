#pragma once

/*
* Program runtime configuration state. Options selected, etc. 
*
*/

#include <filesystem>
#include "channel.hpp"

namespace leapus::configuration{

struct Config{
    static const std::filesystem::path stdout_path;
    std::filesystem::path chirp_channels_in;
    void parse(int argc, const char *argv[]);
};

extern Config config;

}

