/*
The aggregate of the radio configuration elements; contacts, channels, zones, etc.
*/

#pragma once
#include <map>
#include "channel.hpp"

namespace leapus::hamconf{

    class radio_config{

    public:
        channel_map_type channels;
    };

}
