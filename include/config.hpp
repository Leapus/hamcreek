#pragma once

/*
* Program runtime configuration state. Options selected, etc. 
*
*/

#include <exception>
#include <filesystem>
#include "channel.hpp"

namespace leapus::configuration{

class config_exception:public std::runtime_error{
public:
    using std::runtime_error::runtime_error;
};

struct Config{
private:
    int m_argc;
    const char **m_argv;

    void handle_chirp_channel_flag(int &argi);
    void handle_radioid_repeater_flag(int &argi);
    void require_next_arg(int &argi);
    void handle_mexico_flag(int &argi);
    void handle_ordinal_flag(int &argi);
    void handle_rb_json_flag(int &argi);

public:
    static const std::filesystem::path stdout_path;
    std::filesystem::path chirp_channels_in;
    std::filesystem::path radioid_repeaters_in;
    std::filesystem::path repeaterbook_json_in;
    bool mexico=false;

    //Allows the user to specifiy the initial ordinal to use for generated channels. However, the maximum ordinal found among
    //and previously imported channels will override this.
    leapus::hamconf::ordinal_t max_ordinal=0;

    int argc() const;
    const char **argv() const;
    void parse(int argc, const char *argv[]);

    static void usage();
};

//extern Config config;

}
