#pragma once
/*
*
* Handle libcsv exceptions
*
*/


#include <csv.h>
#include "exception.hpp"

namespace leapus::exception{

class csv_exception:public std::runtime_error{
public:
    csv_exception(::csv_parser &p);
    static void check(int result, ::csv_parser &p);
};

}