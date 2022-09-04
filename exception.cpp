#include <cerrno>
#include <iostream>
#include "exception.hpp"

using namespace std::string_literals;
using namespace leapus::exception;

posix_exception::posix_exception(const std::string &wha):
    std::system_error( errno, std::generic_category(),  wha){
}

static void print_exception_impl( std::ostream &stream, const std::exception &ex, int n = 0){
        
    stream << "Exception (" << n <<  "): " << ex.what() << std::endl;
    try{
        std::rethrow_if_nested(ex);
    }
    catch(const std::exception &ex){
        print_exception_impl( stream, ex, n+1 );
    }            
    catch(...){
        stream << " Unknown exception (not derived from std::exception)" << std::endl;
    }
}

void leapus::exception::print_exception( std::ostream &stream, const std::exception &ex ){
    stream << "--- Exception trace start ---" << std::endl;
    print_exception_impl(stream, ex);    
    stream << "--- Exception trace end ---" << std::endl;
}

void leapus::exception::print_warning(const std::string &str){
    std::cerr << "WARNING: "s + str << std::endl;    
}

std::ostream &leapus::exception::operator<<( std::ostream &stream,  const std::exception &ex ){
    print_exception(stream, ex);
    return stream;
}

IOError::IOError( const std::string &msg ):
	std::runtime_error(msg){

}
