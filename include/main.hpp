/*
*
* Application global stuff
*
*/

#include <iostream>
namespace leapus{

/*
std::ostream &conout;
std::istream &conin;
std::ostream &err;
*/

std::ostream &conout = std::cout;
std::istream &conin = std::cin;
std::ostream &err = std::cerr;

void print_info(const std::string &v);

}
