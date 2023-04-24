#pragma once
/*
 Try to define how we interact with the console all in one place so that it's consistent
*/

#include <string>

namespace leapus{

extern std::ostream &conout;
extern std::istream &conin;
extern std::ostream &err;
void print_info(const std::string &v);

}