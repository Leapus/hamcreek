#include <string>
#include <iostream>
#include "config.hpp"
#include "main.hpp"
#include "string.hpp"
#include "radio_config.hpp"
#include "chirp_channel_file.hpp"
#include "djmd5_file.hpp"

//using leapus::csv;
using namespace std::string_literals;
using namespace leapus;
using namespace leapus::string;
using namespace leapus::hamconf;
using namespace leapus::configuration;


int main( int argc, const char *argv[]){

    channel_map_type channel_map;

    config.parse(argc, argv);

    //
    // Import here
    //
    if(!config.chirp_channels_in.empty()){
        print_info( "Loading CHIRP channel data from: "s + config.chirp_channels_in.string() );
        chirp_channel_importer import(argv[1]);
        channel_map = import.channels();
        //print_info( mkstring<>() << "Imported CHIRP channels: " << channel_map.size() );
        print_info( "Imported CHIRP channels: " + std::to_string(channel_map.size()) );
    }


    //
    //Export here
    //
    djmd5_channel_exporter djmd5_exporter(config.stdout_path);
    djmd5_exporter.write(channel_map);


    //DEBUG
    //for( auto &chan : import.channels()){
    //    std::cout << "Channel: " << chan.first << std::endl;
    //}


    return 0;
}

void leapus::print_info(const std::string &v){
    leapus::err << v << std::endl;
}
