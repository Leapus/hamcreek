#include <string>
#include "csv.hpp"
#include "djmd5_file.hpp"
#include "lte_dictionary.hpp"

using namespace std::string_literals;
using namespace leapus;
using namespace leapus::hamconf;

lte_dictionary<power_t, std::string> power_settings = { {Small, "Small"}, {Low, "Low"}, {Middle, "Middle"}, {High, "High"}};

djmd5_channel_exporter::djmd5_channel_exporter(const std::filesystem::path &path):
    m_csv(path, *this, std::ios_base::out){
}

/*
void djmd5_channel_exporter::field(const std::string &field){
    //TODO: Unimplemented
 }

void djmd5_channel_exporter::record(){
    //TODO: Unimplemented
}
*/

void djmd5_channel_exporter::write_header(){
    //Happens to work conveniently to paste the literal CSV into an array initializer

    //Experimentally determined that the "Exclude channel from roaming" and "DMR Mode" headings appear to be swapped,
    //so, we're going to take a bit of license here, and output them correctly, since it's likely
    //that the CPS software doesn't even look at the heading contents anyway. It will save a lot of confusion
    //for anyone who is reading or editing the CSV file by hand.
    static constexpr  const char *headings[] =
        {"No.","Channel Name","Receive Frequency","Transmit Frequency","Channel Type","Transmit Power",
        "Band Width","CTCSS/DCS Decode","CTCSS/DCS Encode","Contact","Contact Call Type","Radio ID","Busy Lock/TX Permit",
        "Squelch Mode","Optional Signal","DTMF ID","2Tone ID","5Tone ID","PTT ID","Color Code","Slot","Scan List",
        "Receive Group List","TX Prohibit","Reverse","Simplex TDMA","TDMA Adaptive","AES Encryption",
        "Digital Encryption","Call Confirmation","Talk Around","Work Alone","Custom CTCSS","2TONE Decode","Ranging",
        "Through Mode","Digi APRS RX","Analog APRS PTT Mode","Digital APRS PTT Mode","APRS Report Type",
        "Digital APRS Report Channel","Correct Frequency[Hz]","SMS Confirmation","Exclude channel from roaming","DMR MODE"};
    
    //We write it again as fields using the API in order to be consistent, perhaps mostly with respect to delimiters
    bool first=true;
    for(auto h : headings){
        m_csv.write_field(h);
    }
    m_csv.end_record();
}

void djmd5_channel_exporter::write( const leapus::hamconf::channel_map_type &channels){
    write_header();

    for(const auto &r : channels){
        write_channel(r.second);
    }
}

std::string mk_tone_string(const freq_field &ctcss, const dts_field &dcstone, tone_code_types type, bool pol){

    std::stringstream ss;
    std::string tmp;
    switch(type){
        case ToneNone:
            return "Off";
            break;

        //These usually have a precision of 100kHz
        //Not sure why I wrote above comment. CTCSS codes are usually Hz with one decimal place for tenths
        case CTCSS:
            //return fixed_to_string(ctcss.value(),5);
            return ctcss.value().to_string(1);
            break;

        case DCS:
            //Without zero-padding to 3 digits, CPS will see a code of zero. Good job.
            ss.fill('0');
            ss.setf(std::ios_base::right, std::ios_base::adjustfield);
            ss.width(3);
            ss << dcstone.value();
            tmp= "D"s + ss.str();
            if(pol)
                return tmp + "N";
            else
                return tmp + "P";
            return tmp;
            break;

        default:
            throw std::runtime_error("Wound up with an invalid tone type somehow");
    }
}


//Return "None" if the string is empty
std::string noneifblank(const std::string &v){
    return v.empty() ? "None" : v;
}

void djmd5_channel_exporter::write_channel(const leapus::hamconf::channel &c){
    m_csv.write_field( std::to_string(c.ordinal) );
    m_csv.write_field( c.name );
    //m_csv.write_field( c.rx_freq.to_string(6) );
    //m_csv.write_field( c.tx_freq.to_string(6) );
    m_csv.write_field( c.rx_mhz.value().to_string(5) );
    m_csv.write_field( c.tx_mhz.value().to_string(5) );

    std::string typestr;
    switch(c.type){
        case ChanTypeA:
            typestr="A-Analog";
            break;

        case ChanTypeD:
            typestr="D-Digital";
            break;

        case ChanTypeADA:
            typestr="A+D TX A";
            break;
        
        case ChanTypeADD:
            typestr="D+A TX D";
            break;
        
        default:
            throw std::range_error("Invalid channel type number: " + std::to_string(c.type));

    }
    m_csv.write_field(typestr);

    power_t tx_power=c.tx_power;
    //Default to max power of 5W if none was specified
    if(tx_power == 0)
        tx_power = 5000;

    m_csv.write_field( power_settings[tx_power] );

    //bandwidth field
    switch(c.channel_mode){

        case WFM:
            //This model does have support to RX broadcast FM, but who knows if wideband will work here
            m_csv.write_field("100K");      
            channel_warning("Wideband 100K broadcast-style wide/WFM might not work in the Channel table");
            break;

        //Hopefully this is correct for typical analog FM
        case FM:
            m_csv.write_field("25K");
            break;

        //Digital modes are narrow, so this is the catch-all.
        case NFM:
        default:
            m_csv.write_field("12.5K");
            break;
    }

    m_csv.write_field( mk_tone_string(c.ctcss_rx_code_hz, c.dts_code, c.rx_code_type, c.rx_code_polarity) );
    m_csv.write_field( mk_tone_string(c.ctcss_tx_code_hz, c.dts_code, c.tx_code_type, c.tx_code_polarity) );

    //Ok, so this defaults to a default contact named "Contact 1" when empty or not defined.
    //TODO: Find out if this will work with the "None" convention, or blank. Might not, as firmware is super flaky.
    m_csv.write_field(  c.contact.value().empty() ? "Contact 1"s : c.contact.value() );

    //TODO
    //"Group Call" is a valid string. Make sure the other strings follow the same convention and are valid to the firmware.
    switch(c.contact_call_type){
        case group_call:
            m_csv.write_field("Group Call");
            break;

        case private_call:
            m_csv.write_field("Private Call");
            break;

        case all_call:
            m_csv.write_field("All Call");
            break;
    }

    m_csv.write_field(c.radio_id);

    auto tmplock = c.busy_lock;
    //This looked like it was going to be necessary, but it seems that the squelch level
    //is configurable, so it should be possible to keep it from sticking open and endlessly
    //locking transmit. Would be extra nice if you could set per-channel squelch level.
    /*
    if(c.squelch_mode == squelch_modes::Carrier){
        //Carrier squelch mode can be really spurious, where it holds the squelch open indefinitely,
        //so if you lock for Carrier, you will never be able to transmit. So, prohibit this condition.
        tmplock = busy_lock_values::Off;
    }
    */

    //busy lock
    switch(tmplock){
        case Off:
            m_csv.write_field("Off");
            break;

        case Always:
            m_csv.write_field("Always");
            break;

        case Repeater:
            m_csv.write_field("Repeater");
            break;

        case Busy:
            m_csv.write_field("Busy");
            break;
    }

    /*
    //TODO: Currently going to assume carrier, and that's probably the most failsafe method anyway
    m_csv.write_field("Carrier");   //Squelch mode
    */

   //If we are configured for an RX code, then rely on that code for squelch.
   //Seems like carrier-detect can be sketchy and lacking in selectivity.
   //It seems closer to an amplitude-detect, and can be tripped by noise.
    if(c.rx_code_type != tone_code_types::ToneNone)
        m_csv.write_field("CTCSS/DCS");
    else
        m_csv.write_field("Carrier");

    m_csv.write_field("Off");   //Optional signal
    m_csv.write_field( std::to_string( c.dtmf_id));
    m_csv.write_field( std::to_string(c.twotone_id) );
    m_csv.write_field( std::to_string(c.fivetone_id) );
    m_csv.write_field( std::to_string(c.ptt_id) );
    m_csv.write_field( std::to_string(c.color_code) );
    m_csv.write_field( std::to_string(c.slot) );

    m_csv.write_field( noneifblank(c.scan_list));
    m_csv.write_field( noneifblank(c.receive_group_list));
    
    m_csv.write_field( c.tx_prohibit.to_string() );
    m_csv.write_field( c.reverse.to_string() );

    //Decide a bunch of magic nonsense DMR-related fields in imitation of CPS' behavior
    leapus::hamconf::field<int> dmr_mode;
    onoff_field simplex_tdma, through_mode;

    switch(c.dmr_type){
        case DMRSimplex:
            dmr_mode=0;
            simplex_tdma=false;
            through_mode=true;
            break;
        case DMRRepeater:
            dmr_mode=0;
            simplex_tdma=false;
            through_mode=false;
            break;
        case DMRDoubleSlotSimplex:
            dmr_mode=2;
            simplex_tdma=true;
            through_mode=true;
            break;
        default:
            throw std::range_error("Unknown DMR Type: "s + std::to_string(c.dmr_type));
    }

    m_csv.write_field( simplex_tdma.to_string() );
    m_csv.write_field( c.tdma_adaptive.to_string() );

    switch( c.aes_encryption ){
        case aes_normal:
            m_csv.write_field("Normal Encryption"); //Known good string
            break;

        case aes_enhanced:
            m_csv.write_field("Enhanced Encryption"); //TODO: Currently assume this is the corresponding string
            break;
    }

    m_csv.write_field( c.digital_encryption.to_string() );
    m_csv.write_field( c.call_confirmation.to_string() );
    m_csv.write_field( c.talk_around.to_string() );
    m_csv.write_field( c.work_alone.to_string() );

    //This doesn't do anything right now. We default it to an arbitrary standard value.
    //if(c.custom_ctss.value() == 0)
        m_csv.write_field( "67.0" );
    //else
    //    m_csv.write_field( c.custom_ctss.to_string(1) );

    m_csv.write_field( std::to_string(c.twotone_decode) );
    m_csv.write_field( c.ranging.to_string() );
    m_csv.write_field( through_mode.to_string() );
    m_csv.write_field( c.digiaprs_rx.to_string() );
    m_csv.write_field( c.analog_aprs_ptt.to_string() );
    m_csv.write_field( c.digital_aprs_ptt.to_string() );

    //TODO. APRS report type. Assume off for now.
    m_csv.write_field( "Off" );
    m_csv.write_field( std::to_string(c.digital_aprs_report_channel) );
    m_csv.write_field( c.correct_mhz.value().to_string(5));
    m_csv.write_field( c.sms_confirmation.to_string() );

    //These two fields' corresponding headings appear to be reversed in CPS 1.09, so we swap them relative to how CPS labels them
    //(and we label the headings correctly, by the way)
    //CPS will undoubtedly reverse the heading labels again if you should export, but the values stay in the same column, order-wise
    m_csv.write_field( std::to_string(c.exclude_channel_from_roaming) );
    m_csv.write_field( std::to_string(dmr_mode) );

    //switch(c.squelch_mode){
    //}
    
    m_csv.end_record();
}
