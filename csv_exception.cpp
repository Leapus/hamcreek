#include <string>
#include "csv_exception.hpp"

using namespace std::string_literals;
using namespace leapus::exception;

csv_exception::csv_exception(::csv_parser &parser):
    std::runtime_error( "libcsv error: "s + ::csv_strerror( ::csv_error(&parser) ) ){
}

void csv_exception::check(int result, ::csv_parser &parser){
    if(result != 0){
        auto err = ::csv_error(&parser);

        //Bizarrely necessary, as it seems that parsing returns a failure result on EOF
        //But the error code is success, for successful completion.
        if(err!=CSV_SUCCESS)
            throw csv_exception(parser);
    }
}
