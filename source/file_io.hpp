#ifndef FILE_IO_HPP
#define FILE_IO_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <pthread.h>
#include <sys/file.h>

#include <string>

namespace file_io {

std::string read_file(FILE* fp);
void write_file(FILE* fp, const char* out);
void flush_and_close_file(FILE* fp);

}

#endif
