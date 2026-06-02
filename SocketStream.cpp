
#include "SocketStream.hpp"

using namespace common;

SocketBuffer::SocketBuffer(string host, int port) : socketDescriptor(0), buffer(NULL), count(0), bufferAllocated(false)
{
#ifdef WIN32
    WSADATA wsaData;
    int wsaReturnCode = WSAStartup(MAKEWORD(1,1), &wsaData);
    if (wsaReturnCode != 0) Support::throwRuntimeError(WSAGetLastError());
    socketDescriptor = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, (LPWSAPROTOCOL_INFO)NULL, 0, 0);
    if (socketDescriptor == INVALID_SOCKET) Support::throwRuntimeError(GetLastError());
#else
    socketDescriptor = socket(PF_INET, SOCK_STREAM, 0);
    if (socketDescriptor < 0) throw Support::makeMessage("SocketStream open", strerror(errno));
    setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, "yes", 4);
#endif
    const char *host_str = host.c_str();
#ifdef WIN32
    int size = sizeof(SOCKADDR_IN);
    struct hostent *he;
    he = gethostbyname(host_str);
    if (!he) Support::throwRuntimeError(GetLastError());
    LPSOCKADDR_IN socketAddress = (LPSOCKADDR_IN)new char[size];
    memset(socketAddress, 0, size);
    memcpy(&socketAddress->sin_addr, *he->h_addr_list, 4);
    socketAddress->sin_family = AF_INET;
    socketAddress->sin_port = htons((short)port);
    int connectValue = connect(socketDescriptor, (const sockaddr *)socketAddress, size);
    if (connectValue == SOCKET_ERROR) Support::throwRuntimeError(GetLastError());
#else
    int size = sizeof(struct sockaddr_in);
    struct addrinfo *hints, *res;
    int length = strlen(host_str);
    struct addrinfo hints_alphabetic;
    struct addrinfo hints_numeric;
    memset(&hints_alphabetic, 0, sizeof(addrinfo));
    memset(&hints_numeric, 0, sizeof(addrinfo));
    hints_alphabetic.ai_family = AF_INET;
    hints_alphabetic.ai_protocol = IPPROTO_TCP;
    hints_alphabetic.ai_flags = AI_CANONNAME;
    hints_numeric.ai_family = AF_INET;
    hints_numeric.ai_protocol = IPPROTO_TCP;
    hints_numeric.ai_flags = AI_CANONNAME | AI_NUMERICHOST;
    if (length && isdigit(host_str[length - 1])) hints = &hints_numeric;
    else hints = &hints_alphabetic;
    int addrinfoValue = getaddrinfo(host_str, NULL, hints, &res);
    if (addrinfoValue) throw Support::makeMessage("SocketStream open", strerror(errno));
    struct sockaddr_in *socketAddress = (struct sockaddr_in *)new char[size];
    memset(socketAddress, 0, size);
    memcpy(&socketAddress->sin_addr, &((struct sockaddr_in *)res->ai_addr)->sin_addr, 4);
    socketAddress->sin_family = PF_INET;
    socketAddress->sin_port = htons((short)port);
    socketAddress->sin_len = size;
    freeaddrinfo(res);
    int connectValue = connect(socketDescriptor, (const struct sockaddr *)socketAddress, size);
    if (connectValue == -1) throw Support::makeMessage("SocketStream open", strerror(errno));
#endif
}

SocketBuffer::~SocketBuffer()
{
    setbuf(NULL, 0);
#ifdef WIN32
    closesocket(socketDescriptor);
    WSACleanup();
#else
    close(socketDescriptor);
#endif
}

int SocketBuffer::overflow(int c)
{
    if (!base() && !allocate()) return -1;
    char *ep = base() + (blen() >> 1);
    if (!pbase()) setp(base(), ep);
    int numberOfBytesToWrite = pptr() - pbase(), numberOfBytesWritten;
    for (;;) {
        numberOfBytesWritten = send(socketDescriptor, pbase(), numberOfBytesToWrite, 0);
        if (numberOfBytesWritten == -1) return -1;
        if ((numberOfBytesToWrite -= numberOfBytesWritten) < 0) break;
        setp(pbase() + numberOfBytesWritten, ep);
    }
    setp(base(), ep);
    return c == -1 ? 0 : sputc(c);
}

int SocketBuffer::underflow()
{
    if (!base() && !allocate() || gptr() && gptr() < egptr()) return -1;
    int count = blen() >> 1;
    char *gp = base() + count;
    int numberOfBytesRead = recv(socketDescriptor, (char *)gp, count, 0);
    setg(gp, gp, gp + numberOfBytesRead);
    return numberOfBytesRead > 0 ? (*gp & 0xff) : -1;
}

int SocketBuffer::sync()
{
    overflow(-1);
    setp(base(), base() + (blen() >> 1));
    setg(NULL, NULL, NULL);
    return -1;
}

streambuf *SocketBuffer::setbuf(char *buffer, streamsize count)
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
