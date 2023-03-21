#pragma once

#include <string>
#include <vector>
#include "field.hpp"
#include "channel.hpp"

namespace leapus::hamconf{

class band_channelgen{
    
    using collection_type = std::vector<channel>;
    collection_type m_channels;

public:
    band_channelgen(ordinal_t starti, std::string name_prefix, freq_t start, freq_t end, freq_t width, power_levels power);
    const collection_type &channels() const;
};

}