/*
*
* Application global stuff
*
*/

#include <iostream>
#include "config.hpp"
#include "channel.hpp"
namespace leapus{

/*
std::ostream &conout;
std::istream &conin;
std::ostream &err;
*/

//std::ostream &conout = std::cout;
//std::istream &conin = std::cin;
//std::ostream &err = std::cerr;

extern std::ostream &conout;
extern std::istream &conin;
extern std::ostream &err;


//Application state
class State{
    leapus::hamconf::channel_map_type m_channels;

public:
    leapus::configuration::Config config;

    //Assign the channel dictionary and rebuild the index-by-callsign
    void channels(leapus::hamconf::channel_map_type &&channels);
    size_t channel_count() const;

    const leapus::hamconf::channel_map_type &channels() const;

    //Retrieve a channel by ordinal, creating the entry if it does not exist
    leapus::hamconf::channel &channel(leapus::hamconf::ordinal_t i);

    //Match a channel by searching for the channel name to contain a given call sign,
    //and requiring the RX and TX frequencies to match.
};

extern State state;

void print_info(const std::string &v);

}
