/* A field used in the internal data structures */

#pragma once
#include "fixed_point.hpp"

namespace leapus::hamconf{

using uint = unsigned int;
using ulong = unsigned long;

using ordinal_t = uint;
using freq_t = ulong;
using power_t = int; //In milliwatts

enum bandwidths_hz:freq_t{
    bw_nfm=12500,   //Narrow FM
    bw_fm=25000,    //Standard FM
    bw_wfm=10000    //Wide/broadcast FM
};

//Using only integer math, parse a decimal string 
//returning a certain number of fixed decimal digits, as an integer.
//We don't observe localization. The decimal point is the period character.
//We don't need signed numbers. Offsets can be negative, but their sign
//has its own field.
//ulong fixed_point(const std::string &str, int dec_dig);
//std::string fixed_to_string(ulong x, int dec);

//This device is always FM, and A/D is the only distinction
//It also does mixed A/D transmit/receive, so TODO to find the strings for that
enum channel_types{
    ChanTypeA,      //Analog
    ChanTypeD,      //Digital
    ChanTypeADA,    //Mixed A/D RX, A TX
    ChanTypeADD     //Mixed A/D RX, D TX
};

//These are the three DMR Types supported by the Alicno CPS programming software
//AFAIK double-slot always implies a simplex channel on that device, and we name it accordingly.
enum dmr_types{
    DMRSimplex,
    DMRRepeater,
    DMRDoubleSlotSimplex
};

//Changing this to instead remember the power level in milliwatts because this arrangement is just silly.
//Only when exporting do we convert to this sort of nonsense, like what the MD5-J firmware demands.
/*
//As named in the device UI
enum power_levels{
    Small,
    Low,
    Middle,
    High
};
*/

//TODO: Other values?
enum contact_call_types{
    private_call,
    group_call,
    all_call
};

//TODO: Make sure the latter two are valid strings
enum busy_lock_values{
    Off,
    Always,
    Repeater,
    Busy
};

//TODO: Other values
enum squelch_modes{
    SquelchCarrier,
    SquelchCTCSS_DCS
};

//TODO: Other values
enum encryption_types{
    normal
};

//TODO: Other values
enum aprs_report_types{
    aprsOff
};

enum tone_code_types{
    ToneNone,
    ToneCTCSS,
    ToneDCS
};


//There were three kinds of FM in here, denoting different bandwidths, but there's already a bandwidth field.
//So, that's redundant. We're only concerned with mode here, which implies modulation and possibly digital encoding.
//Where mode implies bandwidth, that's a device-specific consideration, so it goes there.
//It might also make sense to eventually separate encoding from modulation, too.
 enum channel_mode_types{
    FM,
    AM,
    DV,     //D-STAR
    DIG,    //DMR
    P25
};

//Seems to be a choice between "Normal" and "Enhanced"
//Does this thing TX on non-amateur ranges?
//(Yes, the DJ-MD5X indeed transmits successfully on the Mexican open bands, and probably others)
enum aes_types{
    aes_normal,
    aes_enhanced
};

template<typename T>
class field{
public:
    using value_type=T;

private:
    value_type m_value;

public:
    field(value_type v = value_type()):
        m_value(v){}

    operator value_type() const{ return m_value; }
    value_type value() const{ return m_value; }
    field &operator=(const value_type &v){
        m_value = v;
        return *this;
    } 
};

class dts_field:public field<int>{
public:
    using field<int>::field;
};

class freq_field:public field<fixed_point<freq_t>>{
public:
    using field::field;
    freq_field(freq_t v, int dec=0);
    std::string to_string(int decimal=3) const;
};

class bandwidth_field:public field<bandwidths_hz>{
public:
    using field::field;
};
class onoff_field:public field<bool>{
public:
    using field::field;
    std::string to_string() const;
};
class channel_type_field:public field<channel_types>{
public:
    using field::field;
};
class tx_power_field:public field<power_t>{
public:
    using field::field;
};
class contact_call_type_field:public field<contact_call_types>{
public:
    using field::field;
};
class busy_lock_field:public field<busy_lock_values>{
    using field::field;
};
class squelch_mode_field:public field<squelch_modes>{
public:
    using field::field;
};

//This field refers to an entry in some list, and is distinguished
//because when it is logically empty, it serializes to CPS CSV format as "None"
class reference_field:public field<std::string>{
public:
    using field::field;
};

class aprs_report_type_field:public field<aprs_report_types>{
public:
    using field::field;    
};

class dmr_type_field:public field<dmr_types>{
public:
    using field::field;
};

ulong intpow(ulong x, int p);

}
