#include <iomanip>
#include <sstream>
#include "string.hpp"
#include "field.hpp"

using namespace leapus::hamconf;

freq_field::freq_field(freq_t freq, int dec):
    field({freq,dec}){}

std::string freq_field::to_string(int dec) const{
    return value().to_string(dec);
}

std::string onoff_field::to_string() const{
    return value() ? "On" : "Off";
}
