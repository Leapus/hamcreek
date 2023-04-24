#include <string>
#include "console.hpp"
#include "config.hpp"
#include "main.hpp"

using namespace std::string_literals;
using namespace leapus::configuration;


const std::filesystem::path leapus::configuration::Config::stdout_path{"/dev/stdout"};
//leapus::configuration::Config leapus::configuration::config = {};

void Config::usage(){
    print_info("hamcreek radio config exchanger");
    //print_info("Usage: hamcreek [-C <chirp.csv>] [-R <rptrs.json>]");
    print_info("Usage: hamcreek [-C <chirp.csv>] [-RBJ filename] [-i ordinal] [-MEX]");
    print_info("    -C  CHIRP channel data to import");
    print_info("    -R  Repeaterbook API channel export data to import in json format");
    print_info("    -i Specify the first ordinal number to prefer for generated channels");
    print_info("    -MEX Generate channels for Mexican open bands having MD5-compatible 12.5kHz bandwidth");
    print_info("        (see https://www.ift.org.mx/sites/default/files/contenidogeneral/espectro-radioelectrico/inventariodebandasdefrecuenciasdeusolibrev.pdf)");
    //print_info("    -R  DMR digital repeater settings to merge, obtainable from radioid.net"); //unimplemented
    print_info("");
}

int Config::argc() const{ return m_argc; }
const char **Config::argv() const{ return m_argv; }

void Config::parse(int argc_, const char *argv_[]){

    //if(argc>1)
    //    chirp_channels_in = argv[1];

    m_argc = argc_;
    m_argv = argv_;
    for(int i=1;i<argc_;++i){
        auto a=argv_[i];
        if("-C"s == a)
            handle_chirp_channel_flag(i);
        else if("-R"s == a)
            handle_radioid_repeater_flag(i);
        else if("-MEX"s == a)
            handle_mexico_flag(i);
        else if("-i"s == a)
            handle_ordinal_flag(i);
        else if("-J"s == a)
            handle_rb_json_flag(i);
        else{
            throw config_exception("Invalid flag: "s + argv_[i]);
        }     
    }
}

void Config::require_next_arg(int &argi){
    ++argi;
    if(argi >= m_argc){
        throw config_exception("Missing expecteed argument");
    }
}

void Config::handle_mexico_flag(int &argi){
    mexico=true;
}

void Config::handle_chirp_channel_flag(int &argi){
    require_next_arg(argi);
    chirp_channels_in = argv()[argi];
}

void Config::handle_rb_json_flag(int &argi){
    require_next_arg(argi);
    repeaterbook_json_in = argv()[argi];
}

void Config::handle_radioid_repeater_flag(int &argi){
    require_next_arg(argi);
    radioid_repeaters_in = argv()[argi];
}

void Config::handle_ordinal_flag(int &argi){
    require_next_arg(argi);
    max_ordinal=std::stoi(argv()[argi])-1;  
}
