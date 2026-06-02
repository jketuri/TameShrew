
#ifndef _js_USBStream_hpp_
#define _js_USBStream_hpp_

#if defined(_WIN32) && !defined(__CYGWIN__)
#define USE_WIN32
#define USE_WIN_USB
#endif

#ifdef WIN32
//#include <winsock2.h>
#include <windows.h>
#endif

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <streambuf>
#ifdef USE_WIN_USB
#include <SetupAPI.h>
typedef WORD uint16_t;
#else
#include <libusb-1.0/libusb.h>
#endif
#include "common.hpp"

class USBBuffer : public streambuf
{

public:

#ifdef USE_WIN_USB
    USBBuffer::USBBuffer(string &deviceGUID, bool findIn, bool findOut, bool findInterruptIn, bool usePacketSize);
#else
    USBBuffer(uint16_t idVendor, uint16_t idProduct, bool findIn, bool findOut, bool findInterruptIn);
#endif
    ~USBBuffer();

public:
    void setFullRead(bool fullRead)
    {
        this->fullRead = fullRead;
    }

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

#ifdef USE_WIN_USB
    HANDLE usbHandle;
    DWORD usbPacketSize;
#else
    libusb_context *context;
    libusb_device_handle *deviceHandle;
#endif
    unsigned char inEndpoint, outEndpoint, interruptInEndpoint;
    char *buffer;
    streamsize count;
    bool bufferAllocated;
    bool fullRead;

};

#endif
