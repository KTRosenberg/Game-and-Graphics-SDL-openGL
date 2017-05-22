#ifndef FILE_HANDLE_RAII_HPP
#define FILE_HANDLE_RAII_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <pthread.h>
#include <sys/file.h>

#include <string>

/*
 * file class wrapper for FILE* streams
 */
class FileHandle {
private:
    // file descriptor
    FILE* fp;
    // valid marker
    bool _is_valid;
    // no non-argument construction
    FileHandle(void) = delete;

    // copy and assignment operations prohibited
    FileHandle(const FileHandle& other) = delete;
    FileHandle& operator=(FileHandle rhs) = delete;
public:
    // constructors for path string types
    FileHandle(const char* path, const char* access);
    FileHandle(const std::string& path, const char* access);

    // destructor
    ~FileHandle(void);

    int flock(int operation);
    
    // non-thread-safe writes
    void write(const char* out);
    void write(const std::string& out);

    // non-thread-safe read
    std::string read(void);

    // flushes the stream
    void flush(void);
    // retrieve the file descriptor
    FILE* getFd(void) const;
    // returns whether the file has been opened successfully
    bool is_valid(void);
};

#endif // FILE_HANDLE_RAII_HPP
