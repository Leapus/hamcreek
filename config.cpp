#include "config.hpp"

using namespace leapus::configuration;


const std::filesystem::path leapus::configuration::Config::stdout_path{"/dev/stdout"};
leapus::configuration::Config leapus::configuration::config = {};

void Config::parse(int argc, const char *argv[]){

    if(argc>1)
        chirp_channels_in = argv[1];
}
