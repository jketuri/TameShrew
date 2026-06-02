
#ifndef _js_SerialStream_hpp_
#define _js_SerialStream_hpp_

#if defined(_WIN32) && !defined(__CYGWIN__)
#define USE_WIN32
#endif

#ifdef USE_WIN32
//#include <winsock2.h>
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#endif
#include <iostream>
#include <streambuf>
#include "common.hpp"

class SerialBuffer : public streambuf
{

public:

    enum FlowControl {
        FlowControlNone = 0, FlowControlRTSCTS_In = 1, FlowControlRTSCTS_Out = 1 << 1, FlowControlXONXOFF_In = 1 << 2, FlowControlXONXOFF_Out = 1 << 3
    };
    enum Parity {
        ParityNone, ParityOdd, ParityEven, ParityMark, ParitySpace
    };
    enum StopBits {
        StopBits1, StopBits1_5, StopBits2
    };

    SerialBuffer(string portName);
    ~SerialBuffer();
    SerialBuffer& setSerialPortParameters(int baudRate, int dataBits, StopBits stopBits, Parity parity, FlowControl flowControl);

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
    HANDLE serialPortHandle;
#else
    int serialPortHandle;
#endif
    char *buffer;
    streamsize count;
    bool bufferAllocated;

};

#endif
