#include <string>
#include <streambuf>
#include <vector>
#include "csv_exception.hpp"
#include "csv.hpp"

using namespace std::string_literals;
using namespace leapus::csv;
using namespace leapus::exception;

::csv_parser &CSVParser::libcsv(){
    return m_libcsv;
}

CSVParser *CSVParser::cast_this(void *user_data){
    return static_cast<CSVParser *>(user_data);
}

void CSVParser::field_callback(void *str, size_t len, void *user_data){
    auto that = CSVParser::cast_this(user_data); 
    if(that->m_skip_row) return;
    that->field(std::string((char *)str,len));
}

 void CSVParser::record_callback(int lastchar, void *user_data){
    auto that = CSVParser::cast_this(user_data);
    if(that->m_skip_row){
        that->m_skip_row = false;
        return;
    }
    that->record();
 }

CSVParser::CSVParser(){
     csv_exception::check(::csv_init(&m_libcsv, 0), m_libcsv);
}

void CSVParser::parse(const char *data, size_t sz){
    csv_exception::check( ::csv_parse(&m_libcsv, data, sz, field_callback, record_callback, this), m_libcsv );
}

void CSVParser::skip_row(){
    m_skip_row=true;
}

CSVFile::CSVFile(const std::filesystem::path &path, CSVParser &parser, std::ios_base::openmode mode):
    m_parser(parser){

    m_filebuf.open(path, mode);
    if(!m_filebuf.is_open())
        throw std::runtime_error("Failed opening CSV file: "s + path.string());    
}

size_t CSVFile::parse(){
    size_t sz = m_filebuf.sgetn(m_buf, block_size);
    m_parser.parse(m_buf, sz);
    return sz;    
}

size_t CSVFile::parse_all(){
    size_t sz, ct=0;

    do{
        sz=parse();
        ct+=sz;
    }while(sz > 0);
    return ct;
}


void CSVFile::write_field(const std::string &str){

    size_t sz = csv_write(m_outbuf.data(), m_outbuf.size(), str.data(), str.size());
    if(sz > m_outbuf.size()){
        m_outbuf.resize(sz);
        write_field(str);
    }
    else{
        //prepend a comma separator unless it's the first field in the record
        if(m_first_field_out){
            m_first_field_out = false;
        }
        else{
            m_filebuf.sputc(',');
        }
        m_filebuf.sputn(m_outbuf.data(), sz);
    }
}

//Seems like most radio apps are Windows products, and will likely expect CRLF
void CSVFile::end_record(){
    m_filebuf.sputc('\r');
    m_filebuf.sputc('\n');
    m_first_field_out=true; //remember to skip the leading comma
}
