#pragma once

#include <cassert>
#include <stdexcept>
#include <utility>
#include <string>
#include <sstream>
#include <iomanip>
#include "string.hpp"

namespace leapus{

//Exceptions in the handling of fixed-point numbers
class fixed_point_exception:public std::runtime_error{
public:
    using runtime_error::runtime_error;
};

class fixed_point_parse_error:public fixed_point_exception{
public:
    using fixed_point_exception::fixed_point_exception;
};

template<typename T>
class fixed_point{
    using value_type=T;
    int m_dec;
    value_type m_scale, m_value;

    //Integral powers of ten
    static value_type pow10(value_type v, int p){
        if(p>=0){
            for(int i=0;i<p;++i)
                v*=10;
        }
        else{
            for(int i=0;i>p;--i)
                v/=10;
        }
        return v;
    }

    static int log10(value_type v){
        int r=0;
        while(v>=10){
            ++r;
            v/=10;
        }
        return r;
    }

    //Ok. Want to avoid parsing complexities like the leading sign and any whitespace rules we don't feel like researching.
    //So, we will parse twice with and without the decimal point to calculate the mantissa and exponent.
    //We assume there are no thousand separators and the decimal is the first period or comma.
    static std::pair<value_type, int> parse(const std::string &str) try{

        value_type a=leapus::string::to_integral<value_type>(str);
        std::string str2;

        //Find the decimal
        std::string::size_type dpos=str.find_first_of('.');
        if(dpos==str.npos)
            dpos=str.find_first_of(',');

        if(dpos==str.npos){
            return { a, 0 };
        }

        str2=str;
        str2.erase(dpos, (std::string::size_type)1);
        value_type b=leapus::string::to_integral<value_type>(str2);
        int x=log10(a), y=log10(b);
        return { b, y-x };
    }
    catch(const std::range_error &x){
        throw fixed_point_parse_error(std::string(x.what()) + ": string: " + str);
    }
    catch(const std::invalid_argument &x){
        throw fixed_point_parse_error(std::string(x.what()) + ": string: " + str);
    }

public:
    //v is mantissa, dec is number of decimal
    fixed_point(value_type v=0, int dec=0):
        m_dec(dec),
        m_scale(pow10(1,dec)),
        m_value(v){
            
            assert(m_scale >= 1);
    }

    fixed_point(const std::string &str):
        fixed_point( parse(str).first, parse(str).second ){
    }

    //Returns l and r, but with the fewer-decimal place one rescaled to match the other
    static std::pair<fixed_point, fixed_point> like_terms(const fixed_point &l, const fixed_point &r){
        if(l.m_dec < r.m_dec){
            auto p=like_terms(r,l);
            return {p.second, p.first};
        }

        return { l, { pow10(r.m_value, l.m_dec - r.m_dec), l.m_dec } };
    }

    //Return the number parts to the left and right of the decimal as integers
    //The right is zero padded or multiplied to match the number of decimal places
    std::pair<value_type, value_type> parts(int dec) const{
        auto l=m_value / m_scale;
        auto r=pow10(m_value % m_scale, dec - m_dec);
        return {l,r};
    }

    std::string to_string(int dec ){
        std::stringstream ss;
        auto p=parts(dec);
        ss << p.first << "." << std::setw(dec) << std::setfill('0') << p.second;
        return ss.str();
    }

    fixed_point operator+(const fixed_point &rhs) const {
        auto pr=like_terms(*this, rhs);
        return { pr.first.m_value + pr.second.m_value, pr.first.m_dec };
    }

    fixed_point operator-(const fixed_point &rhs) const {
        auto pr=like_terms(*this, rhs);
        return { pr.first.m_value-pr.second.m_value, pr.first.m_dec };
    }
};

/*
template<typename T>
class fixed_point{
    using value_type=T;
    int m_dec;
    value_type m_scale,m_l,m_r;

    static value_type pow10(int v, int p){
        if(p>=0){
            for(int i=0;i<p;++i)
                v*=10;
        }
        else{
            for(int i=0;i>p;--i)
                v/=10;
        }
        return v;
    }

    static int log10(value_type v){
        int r=0;
        while(v>=10){
            ++r;
            v/=10;
        }
        return r;
    }

    static std::pair<value_type, value_type> parse(const std::string &v, int dec){
        auto p=v.find_first_of('.');
        if(p == std::string::npos)
            p=v.find_first_of(',');
        
        if(p == std::string::npos)
            return {leapus::string::to_integral<value_type>(v), 0};
        else{
            return std::make_pair( leapus::string::to_integral<value_type>( v.substr(0,p) ),
                leapus::string::to_integral<value_type>( v.substr(p+1) ) );
        }
    }

public:
    fixed_point(value_type v=0, value_type d=0, int dec=0):
        m_dec(dec),
        m_scale(pow10(1,dec)),
        m_value(v*m_scale+d){}

    fixed_point(const std::string &str, int dec):
        fixed_point( parse(str, dec).first, parse(str, dec).second, dec ){}

    std::string to_string(int dec) const{
        //auto d=pow(m_dec,dec-m_dec);
        std::stringstream ss;
        ss << m_value/m_scale << "." << std::setfill('0') << std::setw(dec) << m_value%m_scale;
    }

    std::string to_string() const{
        return to_string(m_dec);
    }
};
*/

}
