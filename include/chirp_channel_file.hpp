#pragma

/*
*  Represents a CHRIP CSV file defining the channel list. Each channel is a simplex or repeater frequency, each analog or digital.
*/

#include <csv.h>
#include <filesystem>
#include <tuple>

#include "radio_config.hpp"
#include "csv.hpp"

namespace leapus::hamconf{

//Parses a CHIRP channel CSV table so that the map of channel
//definitions keyed by ordinal can be retrieved from channels()
class chirp_channel_importer:public csv::CSVParser{

    using Th=chirp_channel_importer;

    channel_map_type m_channel_map;

    //Field handlers
    void ordinal(const std::string &);
    void name(const std::string &);
    void frequency(const std::string &);
    void duplex(const std::string &);
    void offset(const std::string &);
    void tone(const std::string &);
    void rx_tone(const std::string &);
    void tx_tone(const std::string &);
    void dtcs_code(const std::string &);
    void dtcs_polarity(const std::string &);
    void mode(const std::string &);
    void Tstep(const std::string &);
    void comment(const std::string &);

    static constexpr void (Th::*field_handlers[])(const std::string &) = 
        {&Th::ordinal, &Th::name, &Th::frequency, &Th::duplex, &Th::offset, &Th::tone, &Th::tx_tone,
        &Th::rx_tone, &Th::dtcs_code, &Th::dtcs_polarity, &Th::mode, &Th::Tstep, &Th::comment};

    int fieldno=0;
    channel tempchan;

    enum duplex_types{
        None,
        Split,
        Positive,
        Negative,
        Off
    } m_duplex;

    /*
    enum tone_modes{
        ToneNone,
        Tone,
        TSQL,
        DTCS
    }
    //} m_tone_mode;
    */

protected:
    void field(const std::string &) override;
    void record() override;
    void channel_warning(const std::string &);
    std::string make_tone_string(const std::string &) const;

public:
    chirp_channel_importer( const std::filesystem::path & );
    channel_map_type &channels();
};

}
