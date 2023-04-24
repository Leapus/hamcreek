#pragma once
/*
*
* For processing MD-5J CSV exchange format, which the CPS uses to import and export
* for that device.
*
*/

#include <filesystem>
#include "field.hpp"
#include "channel.hpp"
#include "csv.hpp"

namespace leapus::hamconf{

enum djmd5_power_levels:power_t{
    Small=200,
    Low=1000,
    Middle=2500,
    High=5000
};

class djmd5_channel_exporter:public csv::CSVParser{
    leapus::csv::CSVFile m_csv;
    void write_header();
    void write_channel(const leapus::hamconf::channel &);

protected:
    inline void field(const std::string &) override{};
    inline void record() override{};
    inline void channel_warning(const std::string &){};

public:
    djmd5_channel_exporter(const std::filesystem::path &);
    void write( const leapus::hamconf::channel_map_type &channels);
};

}
