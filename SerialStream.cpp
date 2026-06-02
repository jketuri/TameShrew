
#include "SerialStream.hpp"

using namespace common;

SerialBuffer::SerialBuffer(string portName) : serialPortHandle(0), buffer(NULL), count(0), bufferAllocated(false)
{
#ifdef WIN32
    serialPortHandle = CreateFile(portName.c_str(),
                                  GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    if (serialPortHandle == INVALID_HANDLE_VALUE) Support::throwRuntimeError(GetLastError());
#else
    serialPortHandle = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (serialPortHandle < 0) throw Support::makeMessage("SerialStream open", strerror(errno));
    fcntl(serialPortHandle, F_SETFL, 0);
#endif
}

SerialBuffer::~SerialBuffer()
{
    setbuf(NULL, 0);
#ifdef WIN32
    if (!CloseHandle(serialPortHandle)) Support::throwRuntimeError(GetLastError());
#else
    close(serialPortHandle);
#endif
}

SerialBuffer& SerialBuffer::setSerialPortParameters(int baudRate, int dataBits, StopBits stopBits, Parity parity, FlowControl flowControl)
{
#ifdef WIN32
    static const BYTE win32StopBits[] = {ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS},
        win32Parity[] = {NOPARITY, ODDPARITY, EVENPARITY, MARKPARITY, SPACEPARITY};
        DCB dcb;
        if (!GetCommState(serialPortHandle, &dcb)) Support::throwRuntimeError(GetLastError());
        dcb.BaudRate = (DWORD)baudRate;
        dcb.ByteSize = (BYTE)dataBits;
        dcb.StopBits = win32StopBits[stopBits];
        dcb.fParity = parity != ParityNone;
        dcb.Parity = win32Parity[parity];
        dcb.fOutxCtsFlow = FALSE;
        dcb.fOutxDsrFlow = FALSE;
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
        dcb.fDtrControl = DTR_CONTROL_DISABLE;
        dcb.fRtsControl = RTS_CONTROL_DISABLE;
        if (flowControl & FlowControlRTSCTS_In)
            dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
        if (flowControl & FlowControlRTSCTS_Out)
            dcb.fOutxCtsFlow = TRUE;
        if (flowControl & FlowControlXONXOFF_In)
            dcb.fInX = TRUE;
        if (flowControl & FlowControlXONXOFF_Out)
            dcb.fOutX = TRUE;
        if (!SetCommState(serialPortHandle, &dcb)) Support::throwRuntimeError(GetLastError());
        COMMTIMEOUTS commTimeouts;
        if (!GetCommTimeouts(serialPortHandle, &commTimeouts))
            Support::throwRuntimeError(GetLastError());
        commTimeouts.ReadIntervalTimeout = 0;
        commTimeouts.ReadTotalTimeoutMultiplier = 0;
        commTimeouts.ReadTotalTimeoutConstant = 0;
        if (!SetCommTimeouts(serialPortHandle, &commTimeouts))
            Support::throwRuntimeError(GetLastError());
#else
        static const int posixBaudRates[] = {0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400},
            posixBaudRateValues[] = {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400},
                posixDataBits[] = {5, 6, 7, 8}, posixDataBitsValues[] = {CS5, CS6, CS7, CS8};
                struct termios options;
                if (tcgetattr(serialPortHandle, &options) != 0)
                    throw Support::makeMessage("SerialStream tcgetattr", strerror(errno));
                /*
                options.c_cflag &= ~CBAUD;
                */
                bool baudRateSet = false;
                for (unsigned baudRateIndex = 0; baudRateIndex < LENGTH(posixBaudRates); baudRateIndex++)
                    if (baudRate == posixBaudRates[baudRateIndex]) {
                        if (cfsetispeed(&options, posixBaudRateValues[baudRateIndex]) != 0)
                            throw Support::makeMessage("SerialStream cfsetispeed", strerror(errno));
                        if (cfsetospeed(&options, posixBaudRateValues[baudRateIndex]) != 0)
                            throw Support::makeMessage("SerialStream cfsetospeed", strerror(errno));
                        baudRateSet = true;
                        break;
                    }
                if (!baudRateSet) throw new string("Unsupported baud rate");
                options.c_cflag &= ~CSIZE;
                bool dataBitsSet = false;
                for (unsigned dataBitsIndex = 0; dataBitsIndex < LENGTH(posixDataBits); dataBitsIndex++)
                    if (dataBits == posixDataBits[dataBitsIndex]) {
                        options.c_cflag |= posixDataBitsValues[dataBitsIndex];
                        dataBitsSet = true;
                        break;
                    }
                if (!dataBitsSet) throw new string("Unsupported data bits");
                switch (stopBits) {
                case StopBits1:
                    options.c_cflag &= ~CSTOPB;
                    break;
                case StopBits2:
                    options.c_cflag |= CSTOPB;
                    break;
                default:
                    throw new string("Unsupported stop bits");
                }
                switch (parity) {
                case ParityNone:
                    options.c_cflag &= ~PARENB;
                    options.c_iflag &= ~INPCK;
                    break;
                case ParityOdd:
                    options.c_cflag |= PARODD | PARENB;
                    options.c_iflag |= INPCK;
                    break;
                case ParityEven:
                    options.c_cflag |= PARENB;
                    options.c_cflag &= ~PARODD;
                    options.c_iflag |= INPCK;
                    break;
                default:
                    throw new string("Unsupported parity");
                }
                options.c_cflag &= ~CRTSCTS;
                options.c_iflag &= ~(IXON | IXOFF | IXANY);
                if (flowControl & FlowControlRTSCTS_Out)
                    options.c_cflag |= CRTSCTS;
                if (flowControl & FlowControlXONXOFF_In)
                    options.c_iflag |= IXON | IXOFF | IXANY;
                options.c_cflag |= CLOCAL | CREAD;
                options.c_lflag |= ~(ICANON | ECHO | ECHOE | ISIG);
                options.c_oflag &= ~OPOST;
                if (tcsetattr(serialPortHandle, TCSANOW, &options) != 0)
                    throw Support::makeMessage("SerialStream tcsetattr", strerror(errno));
#endif
                return *this;
}

int SerialBuffer::overflow(int c)
{
    if (!base() && !allocate()) return -1;
    char *ep = base() + (blen() >> 1);
    if (!pbase()) setp(base(), ep);
#ifdef WIN32
    DWORD numberOfBytesToWrite = (DWORD)(pptr() - pbase()), numberOfBytesWritten;
#else
    int numberOfBytesToWrite = pptr() - pbase(), numberOfBytesWritten;
#endif
    for (;;) {
#ifdef WIN32
        if (!WriteFile(serialPortHandle, pbase(), numberOfBytesToWrite, &numberOfBytesWritten, NULL)) return -1;
#else
        numberOfBytesWritten = write(serialPortHandle, pbase(), numberOfBytesToWrite);
        if (numberOfBytesWritten != numberOfBytesToWrite) return -1;
#endif
        if ((numberOfBytesToWrite -= numberOfBytesWritten) < 0) break;
        setp(pbase() + numberOfBytesWritten, ep);
    }
    setp(base(), ep);
    return c == -1 ? 0 : sputc(c);
}

int SerialBuffer::underflow()
{
    if (!base() && !allocate() || gptr() && gptr() < egptr()) return -1;
    int count = blen() >> 1;
    char *gp = base() + count;
#ifdef WIN32
    DWORD numberOfBytesRead = 0;
    if (!ReadFile(serialPortHandle, gp, count, &numberOfBytesRead, NULL)) return -1;
#else
    int numberOfBytesRead = 0;
    numberOfBytesRead = read(serialPortHandle, gp, count);
#endif
    setg(gp, gp, gp + numberOfBytesRead);
    return numberOfBytesRead > 0 ? (*gp & 0xff) : -1;
}

int SerialBuffer::sync()
{
    overflow(-1);
    setp(base(), base() + (blen() >> 1));
    setg(NULL, NULL, NULL);
    return -1;
}

streambuf *SerialBuffer::setbuf(char *buffer, streamsize count)
{
    if (bufferAllocated) {
        if (buffer) delete buffer;
        bufferAllocated = false;
        buffer = NULL;
        count = 0;
    }
    this->buffer = buffer;
    this->count = count;
    return this;
}
