#pragma once
/*
*
* Wrap some libcsv stuff, which is a C library
*
*/

#include <csv.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

namespace leapus::csv{

    class CSVParser{
        bool m_skip_row=false;
        ::csv_parser m_libcsv={0};
        static void field_callback(void *, size_t, void *);
        static void record_callback(int, void *);

    protected:
        //Inherit from this class and override these event handlers or callbacks
        virtual void field(const std::string &) = 0;
        virtual void record() = 0;
        ::csv_parser &libcsv();
        static CSVParser *cast_this(void *);
    public:
        CSVParser();
        void parse(const char *, size_t);
        void skip_row();
    };

    class CSVFile{
        static constexpr size_t block_size = 4096;  //typical system I/O size
        std::filebuf m_filebuf;
        CSVParser &m_parser;
        char m_buf[block_size];
        std::vector<char> m_outbuf;
        bool m_first_field_out=true;

    public:
        CSVFile(const std::filesystem::path &path, CSVParser &parser, std::ios_base::openmode mode);
        size_t parse();
        size_t parse_all();
        void write_field(const std::string &);
        void end_record();
    };
}
