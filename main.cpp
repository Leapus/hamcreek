#include <string>
#include <iostream>
#include "config.hpp"
#include "console.hpp"
#include "main.hpp"
#include "string.hpp"
#include "radio_config.hpp"
#include "chirp_channel_file.hpp"
#include "djmd5_file.hpp"
#include "band_channelgen.hpp"
#include "repeaterbook_json.hpp"

//using leapus::csv;
using namespace std::string_literals;
using namespace leapus;
using namespace leapus::string;
using namespace leapus::hamconf;
using namespace leapus::configuration;

leapus::State leapus::state;

size_t State::channel_count() const{ return m_channels.size(); }
const channel_map_type &State::channels() const{ return m_channels; }

void State::channels( channel_map_type &&channels ){
    m_channels = std::move(channels);

    //m_channel_callsign_index.clear();
    //for(auto &p : m_channels){
    //    m_channel_callsign_index.insert( std::pair( p.second. ) );
    //}
}

static void append_channels(ordinal_t &maxi, const std::vector<channel> &v, channel_map_type &m){
    ordinal_t i;
    for(auto const &c : v){
        i=c.ordinal.value();
        if(m.find(i) != m.end())
            print_info("W: replacing channel ordinal #"s + std::to_string(i));

        m[i]=c;
        maxi=std::max(maxi,i);
    }
}

static void bandchangen(ordinal_t &maxi, std::string prefix, freq_t start, freq_t end, freq_t width, channel_map_type &dest, power_t power){
    band_channelgen gen(maxi+1, prefix, start, end, width, power);
    append_channels(maxi, gen.channels(), dest);

    /*
    for( auto &c : gen.channels() ){
        auto i=c.ordinal.value();
        dest[i]=std::move(c);
        maxi=std::max(maxi,i);
    }
    */
}

int main( int argc, const char *argv[]){

    //auto x=fixed_point("123.001",5);
    //std::cout << "TETS: " << fixed_to_string(x,5) << std::endl;
    try{
        leapus::state.config.parse(argc, argv);
    }
    catch(const config_exception &ex){
        leapus::state.config.usage();
        print_info("Error: "s + ex.what());
        return -1;
    }

    //
    // Import here
    //
    channel_map_type channels;
    auto &maxi=state.config.max_ordinal;
    if(!leapus::state.config.chirp_channels_in.empty()){
        print_info( "Loading CHIRP channel data from: "s + leapus::state.config.chirp_channels_in.string() );
        chirp_channel_importer import(leapus::state.config.chirp_channels_in);
        channels = std::move(import.channels());
        //print_info( mkstring<>() << "Imported CHIRP channels: " << channel_map.size() );
        print_info( "Imported CHIRP channels: " + std::to_string( channels.size() ) );
        maxi=std::max(maxi, import.max_ordinal());
    }

    if(!leapus::state.config.repeaterbook_json_in.empty()){
        rb_json_import import(maxi+1, leapus::state.config.repeaterbook_json_in);
        append_channels(maxi, import.channels(), channels);
    }

    //
    // Do Mexican stuff 
    //
    if(state.config.mexico){
        //Generate channels to fill the unlicensed public bands
        //All of these are 12.5kHz bandwidth, 40W max permissible, voice/data, no repeaters allowed
        //No interfacing with telephone services allowed.
        bandchangen(maxi, "MEX", 153012500, 153237500, 12500, channels, 40000);
        bandchangen(maxi, "MEX", 159012500, 159200000, 12500, channels, 40000);
        bandchangen(maxi, "MEX", 163012500, 163237500, 12500, channels, 40000);
        bandchangen(maxi, "MEX", 450262500, 450487500, 12500, channels, 40000);
        bandchangen(maxi, "MEX", 455262500, 455487500, 12500, channels, 40000);

        //There's also these, but they are limited to 500mW, and the closest
        //MD5 power setting is "Small" for 200mW.
        bandchangen(maxi, "MEX", 462556250, 462568750, 12500, channels, 500);
        bandchangen(maxi, "MEX", 462581250, 462593750, 12500, channels, 500);
    }

    //Merge in optional radioid.net repeater data

    //
    //Export here
    //
    djmd5_channel_exporter djmd5_exporter(leapus::state.config.stdout_path);
    djmd5_exporter.write( channels );

    //DEBUG
    //for( auto &chan : import.channels()){
    //    std::cout << "Channel: " << chan.first << std::endl;
    //}
    return 0;
}

