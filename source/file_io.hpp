#ifndef FILE_IO_HPP
#define FILE_IO_HPP

#if !(UNITY_BUILD)
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <string>
#endif

namespace file_io {

std::string read_file(FILE* fp);
std::string open_and_read_file(const char* path, bool* is_valid);
void write_file(FILE* fp, const char* out);
void flush_and_close_file(FILE* fp);

}

#ifdef FILE_IO_IMPLEMENTATION
#undef FILE_IO_IMPLEMENTATION
#include "file_io.cpp"
#endif

#endif
