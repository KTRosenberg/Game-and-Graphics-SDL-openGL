#include "file_io.hpp"

namespace file_io {

std::string read_file(FILE* fp) 
{
    errno = 0;

    std::string input = "";
    int status = 0;
    char byte = '\0';

    do {
        status = fread((void*)&byte, sizeof(char), 1, fp);

        if (status == 0) {
            break;
        } else if (status < 0 || errno > 0) {
            return "";
        }

        input += byte;
    } while(true);

    return input;
}

std::string open_and_read_file(const char* path, bool* is_valid)
{
    FILE* fd = fopen(path, "r");
    if (fd == NULL) {
        *is_valid = false;
        return "";
    }

    std::string out = read_file(fd);

    if (ferror(fd) == 0 || errno > 0) {
        *is_valid = true;
        flush_and_close_file(fd);
    } else {
        *is_valid = false;
    }

    return out;
}


void flush_and_close_file(FILE* fp) 
{
    fflush(fp);
    fclose(fp);
}

void write_file(FILE* fp, const char* out) 
{
    fwrite((void*)out, sizeof(char), strlen(out), fp);
}

}
