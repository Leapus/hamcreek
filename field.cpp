#include <iomanip>
#include <sstream>
#include "string.hpp"
#include "field.hpp"

using namespace leapus::hamconf;

/*
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
*/

std::string freq_field::to_string(int dec) const{
    return value().to_string(dec);
}

/*
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
    return std::to_string(l) + "." + std::to_string(r); 
    * /
}
*/

std::string onoff_field::to_string() const{
    return value() ? "On" : "Off";
}

/*
//Positive integer exponents
ulong leapus::hamconf::intpow(ulong x, int p){

    ulong result=x;
    if(x==0)
        return 1;

    for(int i=0;i < p-1;++i){
        result *=x;
    }
    return result;
}
*/

/*
//Redundant
std::string leapus::hamconf::fixed_to_string(ulong x, int dec){

    std::stringstream ss;
    auto l = x / intpow(10,dec);
    auto r = x % intpow(10, dec);

    ss << l << "." << std::setw(dec) << std::setfill('0') << r;
    return ss.str();
    //return std::to_string(l) + "." + std::to_string(r);
}
*/

/*
ulong fixed_point(const std::string &str, int dec_dig){
    ulong result;
    ulong decnum;
    int dec, need_dec;

    if(dec_dig <= 0)
        return result;

    for(dec=0;dec<str.size();++dec){
        if(str[dec] == '.')
            break;
    }
   
    //No decimal part, so just scale the integral part to leave space for the decimal places
    if(dec >= (str.size()-1) )
        return result * intpow(10, dec_dig);

    auto decpart = str.substr(dec+1);
    if(decpart.size() > dec_dig){
        //If too many decimal places, truncate
        decpart.resize(dec_dig);
        decnum = std::stoul(decpart);
    }
    else if(decpart.size() < dec_dig){
            //If there are not enough decimal digits, multiply by powers of ten
            need_dec = dec_dig - decpart.size();
            decnum = std::stoul(decpart) * intpow(10,need_dec);
    }
    else
        decnum = std::stoul(decpart); //Have the expected number of decimal places, so just parse it to integer
    
    return std::stoul(str.substr(0,dec)) * intpow(10,dec_dig) + decnum;   
}
*/

