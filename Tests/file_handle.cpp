// FileHandle

#include "file_handle.hpp"

FileHandle::FileHandle(const char* path, const char* access)
    :   fp(nullptr),
        _is_valid(0)
{
    this->fp = fopen(path, access);
    this->_is_valid = (this->fp != nullptr);
}

FileHandle::FileHandle(const std::string& path, const char* access)
    :   fp(nullptr),
        _is_valid(0)
{
    this->fp = fopen(path.c_str(), access);
    this->_is_valid = (this->fp != nullptr);
}

FileHandle::~FileHandle(void)
{
    if (this->_is_valid) {
        fflush(this->fp);
        fclose(this->fp);
    }
}

int FileHandle::flock(int operation) 
{
    return ::flock(fileno(this->fp), operation);
}

void FileHandle::write(const char* out)
{
    fwrite((void*)out, sizeof(char), strlen(out), this->fp);
}

void FileHandle::write(const std::string& out)
{
    fwrite((void*)out.c_str(), sizeof(char), out.length(), this->fp);
}

std::string FileHandle::read(void)
{
    errno = 0;

    std::string input = "";
    int status = 0;
    char byte = '\0';
    do {
        status = fread((void*)&byte, sizeof(char), 1, this->fp);
        if (status == 0) {
            break;
        } else if (status < 0 || errno != 0) {
            return "";
        }

        input += byte;
    } while(true);

    return input;
}

bool FileHandle::is_valid(void)
{
    return this->_is_valid;
}

void FileHandle::flush(void)
{
    fflush(this->fp);
}

FILE* FileHandle::getFd(void) const
{
    return this->fp;
}

