/* Intermediate in-memory representation of a radio channel or repeater definition */

#pragma once
#include <map>
#include <tuple>
#include <string>

#include "field.hpp"
#include "member_enumerator.hpp"

namespace leapus::hamconf{

//Manage the collection of fields in a configuration object
template<typename T, typename Fields>
class conf_fields;

/*
template<typename T, typename ...FT>
class conf_fields<T, std::tuple<FT...>>{
    using tuple_type = std::tuple<FT...>;
    static constexpr uint field_count=std::tuple_size<tuple_type>::value;
};
*/

struct channel{

private:
    using C=channel;

public:
    channel();
    field<ordinal_t> ordinal;
    field<std::string> name;
    freq_field rx_mhz;
    freq_field tx_mhz;
    channel_type_field type;
    tx_power_field tx_power;
    //freq_field bandwidth; //Already implied by the modulation type

    //This is broken down into extra fields for ease of representation
    field<tone_code_types> tx_code_type;
    freq_field ctcss_tx_code_hz;  //Digital code or tone frequency
    field<bool> tx_code_polarity; //true for negative

    field<tone_code_types> rx_code_type;
    freq_field ctcss_rx_code_hz;
    field<bool> rx_code_polarity;

    dts_field dts_code;
    field<channel_mode_types> channel_mode;
    field<std::string> contact;
    field<contact_call_types> contact_call_type;
    field<std::string> radio_id;
    busy_lock_field busy_lock;
    squelch_mode_field squelch_mode;
    onoff_field optional_signal;
    field<uint> dtmf_id;
    field<uint> twotone_id;
    field<uint> fivetone_id;
    onoff_field ptt_id;
    field<uint> color_code;
    field<uint> slot;
    reference_field scan_list;
    reference_field receive_group_list;
    onoff_field tx_prohibit;
    onoff_field reverse;
    //onoff_field simplex_tdma;
    onoff_field tdma_adaptive;
    field<aes_types> aes_encryption;
    onoff_field digital_encryption;
    onoff_field call_confirmation;
    onoff_field talk_around;
    onoff_field work_alone;
    freq_field custom_ctss_hz;
    field<uint> twotone_decode;
    onoff_field ranging;

    //UPDATE: I decided the below MD5JX arrangement is demented and that the imposition of that kind of nonsense
    //belongs in the device-specific code for the device that demands it. Maybe it would make some sort of
    //sense if they'd bothered to document it, but they didn't. So, the intermediate format will be sane,
    //and then we convert to this Rube Goldberg shit when exporting.
    //
    //DMR mode is rather inscrutable and is expressed through a combination of the "DMR Mode",
    //"Simplex TDMA", and "Through Mode" fields.
    //Experimentally determined mappings between the CPS "DMR Mode" field and its CSV output:
    //
    //Simplex   DMR Mode=0, Simplex TDMA=Off, Through Mode=On
    //Repeater  DMR Mode=0, Simplex TDMA=Off, Through Mode=Off
    //Double-Slot DMR Mode=2,Simplex TDMA=On, Through Mode=On
    //
    // So, then "DMR Mode" is always zero, except for double-slot mode which sets it to 2.
    // "Simplex TDMA" seems to be redundant with DMR Mode. It turns on to enable Time Division Multiple Access
    // which is synonymous with double-slotting, so it's not clear why you need both, but whatever.
    // I have no idea what "Through Mode" means and it's maybe some commentary as to whether it's a peer-to-peer
    // connection, but then you would think double-slot implies a repeater as well, so who the hell knows.
    // Since "Simplex TDMA" comes on for double-slot, that might mean that double-slot is always simplex without a repeater.
    // This is the monkey-see-monkey-do territory of reverse-engineering. Oh well.

    //onoff_field through_mode;
    onoff_field digiaprs_rx;
    onoff_field analog_aprs_ptt;
    onoff_field digital_aprs_ptt;
    aprs_report_type_field aprs_report_type;
    field<int> digital_aprs_report_channel;
    freq_field correct_mhz;
    onoff_field sms_confirmation;
    onoff_field exclude_channel_from_roaming;
    //field<int> dmr_mode;
    dmr_type_field dmr_type;
    field<std::string> comment;

    //Probably don't need this if we're going to contemplate adding additional export formats
    //each with their own layout

    /*
    static constexpr auto fields = std::make_tuple(
        &C::ordinal,
        &C::name,
        &C::rx_freq,    
        &C::tx_freq,
        &C::type,
        &C::tx_power,
        &C::bandwidth,
        &C::ctcss_tx_code,
        &C::ctcss_rx_code,
        &C::dts_code,
        &C::channel_mode,
        &C::contact,
        &C::radio_id,
        &C::busy_lock,
        &C::squelch_mode,
        &C::optional_signal,
        &C::dtmf_id,
        &C::twotone_id,
        &C::fivetone_id,
        &C::ptt_id,
        &C::color_code,
        &C::slot,
        &C::scan_list,
        &C::receive_group_list,
        &C::tx_prohibit,
        &C::reverse,
        &C::simplex_tdma,
        &C::tdma_adaptive,
        &C::digital_encryption,
        &C::call_confirmation,
        &C::talk_around,
        &C::work_alone,
        &C::custom_ctss,
        &C::twotone_decode,
        &C::ranging,
        &C::through_mode,
        &C::digiaprs_rx,
        &C::analog_aprs_ptt,
        &C::digital_aprs_ptt,
        &C::aprs_report_type,
        &C::digital_aprs_report_channel,
        &C::correct_frequency,
        &C::sms_confirmation,
        &C::dmr_mode,
        &C::exclude_channel_from_roaming);
    */

    //inline auto enumerator(){ return make_member_enumerator(fields); }
};

using channel_map_type = std::map<uint, channel>;

}
