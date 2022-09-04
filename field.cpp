#include <sstream>
#include "string.hpp"
#include "field.hpp"

using namespace leapus::hamconf;

freq_t leapus::hamconf::stringtohztenths( const std::string &v ){

    std::stringstream buf;
    char c;

    //find the decimal point, remove it, and truncate following the next character, then convert to integer
    for(size_t i=0;i<v.size();++i){
        c = v[i];
        if(c != '.'){
            buf.put(c);
        }
        else{
            if(i+1 < v.size())
                buf.put(v[i+1]);
            else
                buf.put('0');
            break;
        }
    }
    return std::stol( buf.str() );
}

std::string freq_field::to_string(int decimal) const{

    std::stringstream ss;

    if(decimal <= 0 || value() == 0)
        return std::to_string(value());

    auto tmp = std::to_string(value());
    ss << tmp.substr(0, tmp.size()-decimal) << "." << tmp.substr( tmp.size() - decimal );
    return ss.str();
    //double tmp=(double)value() / (10 * decimal);
    //ss.precision(decimal);
    //ss << tmp;
    //return ss.str();

    /*
    if(decimal <= 0)
        return std::to_string(value());

    freq_t m = decimal * 10;
    freq_t l = value() / m;
    freq_t r = value() %  m;
    return std::to_string(l) + "." + std::to_string(r);*/
}



std::string onoff_field::to_string() const{
    return value() ? "On" : "Off";
}
