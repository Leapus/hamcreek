#include <fstream>
#include <jsoncpp/json/json.h>
#include "string.hpp"
#include "console.hpp"
#include "repeaterbook_json.hpp"

using namespace leapus;
using namespace leapus::hamconf;
using namespace std::string_literals;

const rb_json_import::collection_type &rb_json_import::channels() const{
    return m_channels;
}

//Production-build assert
static void ensure_present(const Json::Value &v, const std::string &msg){
    if(!v)
        throw json_format_exception("json value missing: "s + msg);
}

static std::string fetch_string(const Json::Value &v, const std::string &k){
    auto v2=v[k];
    ensure_present(v2, k );
    return v2.asString();
}

//Convert Yes/No to the same boolean which the entire rest of the universe uses
static bool yesno_bool(const std::string &v){
    auto tmp=string::toupper(v);
    if(tmp=="YES")
        return true;
    else if(tmp=="NO")
        return false;
    else
        throw std::range_error("Invalid yes/no string: "s + v);
}

static bool fetch_bool(const Json::Value &v, const std::string &k){
    return yesno_bool(fetch_string(v,k));
}

//Come up with a short, descriptive name for the channel based on whatever information is available.
//Requires the frequency to have been populated already so that it can be parsed and formatted in a consistent way
static std::string make_name(const Json::Value &v, channel &chan){

    //Obvious option is the callsign and frequency
    //The frequency is needed because there can be several repeaters under the same callsign
    auto freq=chan.rx_mhz.value().to_string(3);
    auto callsign=fetch_string(v, "Callsign");

    if(!callsign.empty()){
        return callsign + " " + freq;
    }
    
    //Limit city name so that it fits on the tiny screen
    //TODO: 8 characters is a wild guess
    auto city=fetch_string(v, "Nearest City").substr(0,6);
    if(!city.empty()){
        return city + " " + freq;
    }

    auto id=fetch_string(v, "Rptr ID");
    if(!id.empty()){
        return "ID"s + id + " " + freq;
    }

    //Despair for all we know is the frequency. Hopefully.
    return freq;
}

//If it's blank, leave it unset, at the default which channel object initializes to
static void optional_set(const Json::Value &v, const std::string &k, freq_field &freq){
    auto str=fetch_string(v,k);
    if(!str.empty()){
        freq={ str };
    }
}

void rb_json_import::record_print_info(const std::string &str) const{
    print_info("W: record #"s + std::to_string(m_ordinal) + " " + str + " ");
}

//Squelch tones can be either blank or "CSQ" to idicate no-tone
//They can also be "Restricted", which means you need to contact the operator to obtain the tones
bool rb_json_import::squelch_tone_set(const Json::Value rec, const std::string &k, freq_field &dest){
    auto str=rec[k].asString();

    if(str.empty())
        return false;

    auto upper = string::toupper(str);

    if(upper == "RESTRICTED"){
        record_print_info("has tone marked 'Restricted'");
        return false;
    }

    if(str != "CSQ")
        dest = {str};
    return true;
}

void rb_json_import::handle_record(const Json::Value &v, bool dig_pass) try{

    leapus::hamconf::channel c;

    c.ordinal = m_ordinal+1;
    c.rx_mhz = freq_field( fetch_string(v, "Frequency") );
    c.tx_mhz = freq_field( fetch_string(v, "Input Freq") );
    c.name = make_name(v,c);

    //I did see an example of a DCS tone represented in the "notes"
    //field, but not anywhere else, nor in the documentation, so the json
    //format doesn't appear to support DCS tones
    bool haztxtone = squelch_tone_set(v, "PL", c.ctcss_tx_code_hz);
    bool hazrxtone = squelch_tone_set(v, "TSQ", c.ctcss_rx_code_hz);

    if(haztxtone)
        c.tx_code_type=ToneCTCSS;
    
    if(hazrxtone)
        c.rx_code_type=ToneCTCSS;

    if(haztxtone || hazrxtone)
        c.squelch_mode=SquelchCTCSS_DCS;
    else
        c.squelch_mode=SquelchCarrier;

    bool dmr=fetch_bool(v,"DMR");
    bool analog=fetch_bool(v,"FM Analog");

    if(dmr || analog){
        c.channel_mode=FM;
    }

    //If this repeater supports both DMR and analog, then create two hybrid channels suffixed 'A' and 'D' for transmit mode
    auto tmpname=c.name.value();
    if(analog){
        if(analog && dmr){
            c.name=tmpname+"A";
            c.type=ChanTypeADA;
        }
        else
            c.type=ChanTypeA;

        m_channels.push_back(c);
        m_ordinal=c.ordinal;
    }

    if(dmr){
        if(analog && dmr){
            c.ordinal = m_ordinal+1;
            c.name=tmpname+"D";
            c.type=ChanTypeADD;
        }
        else{
            c.name=tmpname;
            c.type=ChanTypeD;
        }
        
        m_channels.push_back(c);
        m_ordinal=c.ordinal;
    }
}
catch(const fixed_point_parse_error &x){
    throw json_format_exception("fixed point pase error: "s + x.what());
}

void rb_json_import::do_import(std::ifstream &in){
    Json::Value root;
    in >> root;
    ensure_present(root, "No root element in json.");
    if(!root.isObject())
        throw json_format_exception("'root' element is not an object as expected");

    Json::Value count=root["count"];
    Json::Value results=root["results"];

    ensure_present(results, "No 'results' element in root of json. ");
    if(!results.isArray())
        throw json_format_exception("'results' element is not an array as expected");

    if(!count){
        print_info("W: No 'count' element in file");
    }
    else if(count.asInt() > results.size()){
        print_info("W: Fewer records were found than the 'count' element specifies so the file is short records");
    }
    else if(count.asInt() < results.size()){
        print_info("W: More records were found than the 'count' element specifies, so there may be junk in the file");
    }

    print_info("json API 'result' members parsed: "s + std::to_string(results.size()));

    int i=1;
    for(auto &v : results){
        try{
            handle_record(v);
        }
        catch(const std::exception &ex){
            print_info("Record #"s + std::to_string(i) + " skipped due to exception: " + ex.what());
        }
        catch(...){
            print_info("Record #"s + std::to_string(i) + " skipped due to unhandled exception");
        }
        ++i;
    }
}

void rb_json_import::do_summary() const{
    print_info("RB/json channels imported: "s + std::to_string(m_channels.size()));
    print_info("");
}

rb_json_import::rb_json_import(ordinal_t starti, const std::filesystem::path &path):
    m_ordinal(starti){

    try{
        try{
            std::ifstream in;
            print_info("Beginning RepeaterBook/json import from input: "s + path.string());
            in.open(path, std::ifstream::in);
            do_import(in);
        }
        catch(const json_format_exception &ex){
            throw rb_json_exception( "Unrecognized json content: "s + ex.what() );
        }
    }
    catch(const std::exception &ex){
        print_info( "Error during RepeaterBook/json import: "s +  ex.what());
    }
    do_summary();
}
