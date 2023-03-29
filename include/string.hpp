#pragma once

/*
*
* String handling functions
*
*/

#include <string>
#include <ostream>
#include <sstream>

namespace leapus::string{
std::string toupper(const std::string &);

template<typename T>
T to_integral(const std::string &str);

template<>
int to_integral(const std::string &str);

template<>
long to_integral(const std::string &str);

template<>
unsigned long to_integral(const std::string &str);

//A temporary stringsream which is easy to insert into and converts to a string so that you can do f(mkstring() << "abcdefg" << endl)
template<typename CharT = char, class Traits=std::char_traits<CharT>, class Alloc=std::allocator<CharT>>
class mkstring{
    using stream_type = std::basic_stringstream<CharT, Traits, Alloc>;
    stream_type m_stream;

public:
    operator std::basic_string<CharT, Traits, Alloc>(){
        return m_stream.str();
    }

    template<typename T>
    mkstring &insert(T v){
        m_stream << v;
        return *this;   
    }
};

//Value insertion
template<typename T, typename CharT, class Traits, class Alloc>
auto &operator<<(mkstring<CharT, Traits, Alloc> &&obj, T v){
    obj.insert(v);
    return std::forward<mkstring<CharT, Traits, Alloc>>(obj);
}

//char array insertion
template<typename CharT, class Traits, class Alloc>
auto &operator<<(mkstring<CharT, Traits, Alloc> &&obj, const CharT *v){
    return obj.insert(v);
}

template<typename CharT, class Traits>
using manipulator_type = std::basic_ostream<CharT, Traits> &(&)(std::basic_ostream<CharT, Traits> &);


//manipulator insertion
template<typename T, typename CharT, class Traits, class Alloc>
auto &operator<<(mkstring<CharT, Traits, Alloc> &&obj, manipulator_type<CharT, Traits> &manip){
    return obj.insert(manip);
}

}

