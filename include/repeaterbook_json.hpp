#pragma once

#include <exception>
#include <vector>
#include <filesystem>
#include "jsoncpp/json/json.h"
#include "channel.hpp"

namespace leapus::hamconf{

class rb_json_exception:public std::runtime_error{
public:
    using std::runtime_error::runtime_error;
};

class json_format_exception:public rb_json_exception{
public:
    using rb_json_exception::rb_json_exception;
};

//Repeaterbook API json importer
class rb_json_import{
 public:   
    using collection_type = std::vector<channel>;
    
private:
    collection_type m_channels;
    leapus::hamconf::ordinal_t m_ordinal;

    void do_import(std::ifstream &stream);
    void do_summary() const;
    void handle_record(const Json::Value &obj, bool dig_pass=false);
    void record_print_info(const std::string &) const;
    bool squelch_tone_set(const Json::Value rec, const std::string &k, freq_field &dest);

public:
    rb_json_import(ordinal_t starti, const std::filesystem::path &path);
    const collection_type &channels() const;
};

}
