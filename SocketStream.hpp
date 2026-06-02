
#ifndef _js_SocketStream_hpp_
#define _js_SocketStream_hpp_

#if defined(_WIN32) && !defined(__CYGWIN__)
#define USE_WIN32
#endif

#ifdef USE_WIN32
//#include <winsock2.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#endif
#include <iostream>
#include <streambuf>
#include "common.hpp"

class SocketBuffer : public streambuf
{

public:

    SocketBuffer(string host, int port);
    ~SocketBuffer();

protected:

    int overflow(int c);
    int underflow();
    int sync();
    streambuf *setbuf(char *buffer, streamsize count);

    char *base()
    {
        return buffer;
    }

    streamsize blen()
    {
        return count;
    }

    int allocate()
    {
        count = 256;
        buffer = new char[count];
        bufferAllocated = true;
        return count;
    }

private:

#ifdef USE_WIN32
    SOCKET socketDescriptor;
#else
    int socketDescriptor;
#endif
    char *buffer;
    streamsize count;
    bool bufferAllocated;

};

#endif
