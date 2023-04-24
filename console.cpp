#include <iostream>

#include "console.hpp"

std::ostream &leapus::conout = std::cout;
std::istream &leapus::conin = std::cin;
std::ostream &leapus::err = std::cerr;

void leapus::print_info(const std::string &v){
    leapus::err << v << std::endl;
}
