/*
It would be nice if you could define multiple VFO scan ranges for the Alinco MD5J, but you can't.
You get one range for VHF, one for UHF, and that's it for VFO ranges.
Governments may offer a bunch of open-use bands, but they can be scattered blocks, so if you're going
to scan them on the MD5 or otherwise even store the range inside the radio for reference, 
it seems like you're stuck generating a channel list to fill those band regions.
Then, you can scan those, and it's also easier to steer clear of the band edges and stay compliant.
*/

#include <string>
#include "channel.hpp"
#include "band_channelgen.hpp"

using namespace leapus::hamconf;

band_channelgen::band_channelgen(ordinal_t startn, std::string name_prefix, freq_t start, freq_t end, freq_t width, power_t power){

    freq_t hwidth=width/2;
    auto n=startn;

    for(freq_t f=start+hwidth; f+hwidth<=end;f+=width){
        channel chan;
        chan.ordinal=n++;
        chan.rx_mhz=fixed_point<freq_t>{f,6};
        chan.name=name_prefix + " " +  chan.rx_mhz.value().to_string(5);
        chan.tx_mhz=chan.rx_mhz;
        chan.bandwidth_khz={width,3};
        chan.type=ChanTypeA;
        chan.tx_power=power;
        chan.channel_mode=FM;
        chan.squelch_mode=SquelchCarrier;
        chan.scan_list=name_prefix;
        m_channels.push_back(chan);
    }
}

const band_channelgen::collection_type &band_channelgen::channels() const{
    return m_channels;
}
