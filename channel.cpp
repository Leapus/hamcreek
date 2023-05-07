#include "channel.hpp"

using namespace leapus::hamconf;

//Some reasonable, or at least well-defined default values
leapus::hamconf::channel::channel():
    ordinal(0),
    name(),
    rx_mhz(),
    tx_mhz(),
    type( ChanTypeA ),
    tx_power(0),
    bandwidth_khz(bw_nfm,3),
    
    tx_code_type(ToneNone),
    ctcss_tx_code_hz(67),             //Lowest
    tx_code_polarity(true),         //Usually negative,
    rx_code_type(ToneNone),
    ctcss_rx_code_hz(67),
    rx_code_polarity(true),
    contact(),
    contact_call_type(group_call),
    radio_id(),
    busy_lock(Off),
    squelch_mode(SquelchCarrier),
    optional_signal(false),
    dtmf_id(1),                 //These three seem to default to "1"
    twotone_id(1),
    fivetone_id(1),
    ptt_id(false),
    color_code(1),              //IIRC 1 is very common
    slot(1),                    //Not sure what this means, but it defaults to 1 in CPS
    scan_list(),
    receive_group_list(),
    tx_prohibit(false),
    reverse(false),
    //simplex_tdma(false),
    tdma_adaptive(false),
    aes_encryption(aes_normal),
    digital_encryption(false),
    call_confirmation(false),
    talk_around(false),
    work_alone(false),
    custom_ctss_hz(),
    twotone_decode(0),              //Defaults to 0
    ranging(false),
    //through_mode(false),
    digiaprs_rx(false),
    analog_aprs_ptt(false),
    digital_aprs_ptt(false),
    aprs_report_type(aprsOff),
    digital_aprs_report_channel(1),     //CPS defaults to 1
    correct_mhz(),
    sms_confirmation(false),
    exclude_channel_from_roaming(0),
    dmr_type(DMRSimplex)
                                        //Seems like it should be boolean, but is oddly numeric. Defaults to 0.
                                        //Update: Solved this mystery. It's because these last two headings are swapped in the CPS output
                                        //Swapped the order of the fields here, too   
    {}
