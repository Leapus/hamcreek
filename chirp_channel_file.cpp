#include <csv.h>
#include <cctype>
#include <string>
#include <iostream> //debug
#include <functional>
#include <algorithm>

#include "string.hpp"
#include "csv_exception.hpp"
#include "chirp_channel_file.hpp"

using namespace std::string_literals;
using namespace leapus::hamconf;
using namespace leapus::exception;

chirp_channel_importer::chirp_channel_importer( const std::filesystem::path &path ):
   m_max_ordinal(0){
   csv::CSVFile file(path, *this, std::ios_base::in);
   skip_row(); //Skip the header
   file.parse_all();
}

void chirp_channel_importer::field(const std::string &field){
   
   //Ignore unknown extra fields
   if(fieldno >= sizeof(field_handlers) / sizeof(field_handlers[0]))
      return;

   std::invoke( field_handlers[fieldno++], this, field);   
 }

void chirp_channel_importer::record(){
   channels()[tempchan.ordinal]=tempchan;
   m_max_ordinal=std::max(m_max_ordinal,tempchan.ordinal.value());
   tempchan = {};
   fieldno=0;
}

channel_map_type &chirp_channel_importer::channels(){
   return m_channel_map;
}

ordinal_t chirp_channel_importer::max_ordinal() const{
   return m_max_ordinal;
}

using cci = chirp_channel_importer;

void cci::ordinal(const std::string &v){
   tempchan.ordinal = std::stoi(v);
}

void cci::name(const std::string &v){
   tempchan.name = v;
}

//Channel frequencies have 5 decimal places, accurate to 10Hz
void cci::frequency(const std::string &v){
   tempchan.rx_mhz = freq_field{v};
   //tempchan.rx_freq = std::stod(v) * 1000000;
}

//Print a warning for a specific record
void cci::channel_warning(const std::string &str){
   print_warning( "CHIRP CSV channel #"s + std::to_string(tempchan.ordinal) + ": " + str );
}

void cci::duplex(const std::string &v){
   std::string v2 = leapus::string::toupper(v);
   
   //Not a repeater, so inhibit TX on carrier detect
   if(v.size() == 0 || v == "(NONE)"){
      m_duplex = None;
      tempchan.busy_lock = Busy;
   }
   else if(v == "+"){
      m_duplex = Positive;
      tempchan.busy_lock = Busy;
   }
   else if(v == "-"){
      m_duplex = Negative;
      tempchan.busy_lock = Busy;
   }
   else if(v == "SPLIT"){
      m_duplex = Split;
      tempchan.busy_lock = Busy;
   }
   else if(v == "OFF"){
      m_duplex = Off;
      tempchan.tx_prohibit = true;
   }
   else{
      m_duplex = None;
      tempchan.busy_lock = busy_lock_values::Off;
      channel_warning("duplex string unrecognized: "s + v);
   }
}


//Let's not bother storing as an offset since it's simpler to just track the absolute frequency
void cci::offset(const std::string &v){

   //Simplex, single-frequency
   if(m_duplex == None){
      tempchan.tx_mhz = tempchan.rx_mhz;
      return;
   }
   
   //auto offs = std::stod(v) * 1000000;
   //Have not seen offsets with more precision than 100kHz, but 5 decimal places is for consistency
   auto offs=freq_field::value_type(v);

   //Absolute tx freq given
   if(m_duplex == Split){
      tempchan.tx_mhz = offs;
      return;
   }

   //Relative offset in MHz
   if(m_duplex == Positive){
      tempchan.tx_mhz = tempchan.rx_mhz.value() + offs;
   }
   else if(m_duplex == Negative){
      tempchan.tx_mhz = tempchan.rx_mhz.value() - offs;
   }
   else{
      throw std::runtime_error("Oops. How did we get an unknown duplex value?");
   }
}

void cci::tone(const std::string &v){

   std::string v2 = leapus::string::toupper(v);

   //TX-only tone code
   if(v2 == "TONE"){
      //m_tone_mode = Tone;
      tempchan.tx_code_type = CTCSS;
      tempchan.rx_code_type = ToneNone;
   }
   else if(v2 == "TSQL"){
      //TX and RX-squelch tone
      tempchan.tx_code_type = tempchan.rx_code_type = CTCSS;
   }else if(v2 == "DTCS"){
      tempchan.tx_code_type = tempchan.rx_code_type = DCS;
   }
   else{
      //Empty or non-recognized tone mode defaults to none
      tempchan.tx_code_type = tempchan.rx_code_type = ToneNone;

      //Warning for unrecognized
      if(v.size() > 0){
         channel_warning("Unrecognized tone mode: "s + v);
      }
   }
}

//Tone string, minus the polarity suffix
/*
std::string cci::make_tone_string( const std::string &v ) const{
   //Zero the tone code unless it is enabled. DTCS comes from a different field
   if( m_tone_mode == ToneNone || m_tone_mode == DTCS ){
      return "Off";
   }
   else if( m_tone_mode == Tone || m_tone_mode == TSQL ){
      //CTCSS tones are written as-is
      return v;
   }
   else
      throw std::runtime_error("Unknown tone mode (how'd that haoppen?)");
}
*/

//CTCSS tones are usually expressed as Hz with tenth precision
void cci::rx_tone(const std::string &v){
   tempchan.ctcss_rx_code_hz = {v};
}

void cci::tx_tone(const std::string &v){
   tempchan.ctcss_tx_code_hz = {v};
}

void cci::dtcs_code(const std::string &v){
   tempchan.dts_code = std::stol(v);
}

void cci::dtcs_polarity(const std::string &v){
   std::string v2 = leapus::string::toupper(v);

   if(v2.size() > 2)
      print_warning("dtcs polarity field has extra characters");
   
   if(v2[0] == 'N'){
      tempchan.tx_code_polarity = true;
   }
   else if (v2[0] == 'P'){
      tempchan.tx_code_polarity = false;
   }
   else{
      tempchan.tx_code_polarity = true;
      print_warning("dtcs polarity not recognized, so defaulting to N/negative");
   }

   if(v2[1] != v2[0]){
      print_warning("dtcs polarity codes are not the same, and we only support one for both TX/RX. Default to: "s + v2[0]);
   }

}

/*
enum channel_mode_types{
    FM,
    NFM,
    WFM,
    AM,
    DV,     //D-STAR
    DIG,    //DMR
    P25
};
*/

void cci::mode(const std::string &v){
   std::string v2 = leapus::string::toupper(v);

   tempchan.type = analog;
   if(v2 == "FM"){
      tempchan.channel_mode = FM;
   }
   else if(v2 == "NFM"){
      tempchan.channel_mode = NFM;
   }
   else if(v2 == "WFM"){
      tempchan.channel_mode = WFM;
   }
   else if(v2 == "AM"){
      tempchan.channel_mode = AM;
   }
   else if(v2 == "DV"){
      tempchan.channel_mode = DV;
      tempchan.type = digital;
   }
   else if(v2 == "DIG"){
      tempchan.channel_mode = DIG;
      tempchan.type = digital;
   }
   else if(v2 == "P25"){
      tempchan.channel_mode = P25;
      tempchan.type = digital;
   }
   else{
      tempchan.channel_mode = FM;
      channel_warning("Unknown channel mode string: "s + v + ", defaulting to FM"s);
   }
}

//Don't know what this even does, so we ignore it
void cci::Tstep(const std::string &v){
}

void cci::comment(const std::string &v){
   tempchan.comment = v;
}
