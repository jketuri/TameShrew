
#include "USBStream.hpp"

#ifdef USE_WIN_USB
#define IOCTL_ASYNC_IN        CTL_CODE (FILE_DEVICE_UNKNOWN, 0x850, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_USB_PACKET_SIZE CTL_CODE (FILE_DEVICE_UNKNOWN, 0x851, METHOD_BUFFERED, FILE_ANY_ACCESS)

//#define DEBUG_USBBuffer

using namespace common;

USBBuffer::USBBuffer(string &deviceGUID, bool findIn, bool findOut, bool findInterruptIn, bool usePacketSize) : usbHandle(INVALID_HANDLE_VALUE), usbPacketSize(0L), inEndpoint(0), outEndpoint(0), interruptInEndpoint(0), buffer(NULL), count(0), bufferAllocated(false), fullRead(false)
{
    GUID classGUID;
    memset(&classGUID, 0, sizeof classGUID);
    cout << "USBBuffer scanning deviceGUID=" << deviceGUID << endl;
    DWORD data[8];
    sscanf(deviceGUID.c_str(),
           "%8lx-%4hx-%4hx-%2x%2x-%2hx%2hx%2hx%2hx%2hx%2hx\n",
           &classGUID.Data1, &classGUID.Data2, &classGUID.Data3,
           &data[0], &data[1], &data[2], &data[3],
           &data[4], &data[5], &data[6], &data[7]);
    for (DWORD dataIndex = 0; dataIndex < 8; dataIndex++) {
        classGUID.Data4[dataIndex] = data[dataIndex];
    }
#ifdef DEBUG_USB
    printf("deviceGUID=%8lx-%4hx-%4hx-%2x%2x-%2x%2x%2x%2x%2x%2x\n",
           classGUID.Data1, classGUID.Data2, classGUID.Data3,
           classGUID.Data4[0], classGUID.Data4[1], classGUID.Data4[2],
           classGUID.Data4[3], classGUID.Data4[4], classGUID.Data4[5],
           classGUID.Data4[6], classGUID.Data4[7]);
    cout << "listing devices" << endl;
#endif
    // Get information about all the installed devices for the specified
    // device interface class.
    HDEVINFO hDeviceInfo = SetupDiGetClassDevs(&classGUID, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDeviceInfo == INVALID_HANDLE_VALUE) {
        Support::throwRuntimeError(GetLastError());
    }
#ifdef DEBUG_USBBuffer
        cout << "SetupDiGetClassDevs passed" << endl;
#endif
    SP_DEVINFO_DATA devInfoData;
    memset(&devInfoData, 0, sizeof devInfoData);
    devInfoData.cbSize = sizeof devInfoData;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    memset(&deviceInterfaceData, 0, sizeof deviceInterfaceData);
    deviceInterfaceData.cbSize = sizeof deviceInterfaceData;
    for (DWORD index = 0;; index++) {
        if (!SetupDiEnumDeviceInfo(hDeviceInfo, index, &devInfoData)) {
            DWORD error = GetLastError();
            if (error == ERROR_NO_MORE_ITEMS) {
#ifdef DEBUG_USBBuffer
                cout << "No more items" << endl;
#endif
                break;
            }
            Support::throwRuntimeError(error);
        }
#ifdef DEBUG_USBBuffer
        cout << "SetupDiEnumDeviceInfo for " << index << " passed" << endl;
#endif
        if (!SetupDiEnumDeviceInterfaces(hDeviceInfo, &devInfoData, &classGUID, 0, &deviceInterfaceData)) {
            Support::throwRuntimeError(GetLastError());
        }
#ifdef DEBUG_USBBuffer
            cout << "SetupDiEnumDeviceInterfaces for " << index << " passed" << endl;
#endif
        DWORD requiredSize = 0;
        if (!SetupDiGetDeviceInterfaceDetail(hDeviceInfo, &deviceInterfaceData, NULL, 0, &requiredSize, NULL)) {
            DWORD error = GetLastError();
            if (error != ERROR_INSUFFICIENT_BUFFER) {
                Support::throwRuntimeError(GetLastError());
            }
        }
#ifdef DEBUG_USBBuffer
        cout << "SetupDiGetDeviceInterfaceDetail for " << index << " query passed, requiredSize=" << requiredSize << endl;
#endif
        PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, requiredSize);
        if (!pInterfaceDetailData) {
            Support::throwRuntimeError(GetLastError());
        }
        pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        if (!SetupDiGetDeviceInterfaceDetail(hDeviceInfo, &deviceInterfaceData, pInterfaceDetailData, requiredSize, NULL, &devInfoData)) {
            Support::throwRuntimeError(GetLastError());
        }
#ifdef DEBUG_USBBuffer
        cout << "SetupDiGetDeviceInterfaceDetail for " << index << " passed" << endl;
        cout << "DevicePath " << index << "=" << pInterfaceDetailData->DevicePath << endl;
#endif
        usbHandle = CreateFile(pInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD error = 0;
        if (usbHandle == INVALID_HANDLE_VALUE) {
            error = GetLastError();
        }
        LocalFree(pInterfaceDetailData);
        if (usbHandle == INVALID_HANDLE_VALUE) {
            Support::throwRuntimeError(error);
        }
        break;
    }
    SetupDiDestroyDeviceInfoList(hDeviceInfo);
    if (usePacketSize) {
        DWORD bytesReturned = 0;
        DeviceIoControl(usbHandle, IOCTL_USB_PACKET_SIZE, 0, 0,
                        &usbPacketSize, sizeof usbPacketSize, &bytesReturned,
                        NULL);
#ifdef DEBUG_USB
        cout << "usbPacketSize=" << usbPacketSize << endl;
#endif
    }
#ifdef DEBUG_USB
    cout << "USBBuffer ready" << endl;
#endif
}
#else
USBBuffer::USBBuffer(uint16_t idVendor, uint16_t idProduct, bool findIn, bool findOut, bool findInterruptIn) : context(NULL), deviceHandle(NULL), inEndpoint(0), outEndpoint(0), interruptInEndpoint(0), buffer(NULL), count(0), bufferAllocated(false)
{
    int returnValue = libusb_init(&context);
#ifdef DEBUG_USB
    libusb_set_debug(context, 3);
#endif
    if (returnValue < 0) {
        throw Support::makeMessage("USBStream init", strerror(errno));
    }
	libusb_device **devices = NULL;
	ssize_t deviceCount = libusb_get_device_list(context, &devices);
    if (deviceCount < 0) {
        throw Support::makeMessage("USBStream get_device_list", strerror(errno));
    }
    int deviceIndex;
    struct libusb_device_descriptor deviceDescriptor;
    cout << "usb deviceCount:" << deviceCount << endl;
    for (deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
        returnValue = libusb_get_device_descriptor(devices[deviceIndex], &deviceDescriptor);
        if (returnValue < 0) {
            throw Support::makeMessage("USBStream get_device_descriptor", strerror(errno));
        }
        deviceHandle = libusb_open_device_with_vid_pid(NULL, deviceDescriptor.idVendor, deviceDescriptor.idProduct);
        if (!deviceHandle) {
            continue;
        }
        unsigned char manufacturer[1024];
        returnValue = libusb_get_string_descriptor_ascii(deviceHandle, deviceDescriptor.iManufacturer, manufacturer, 1024);
        if (returnValue < 0) {
            manufacturer[0] = '\0';
        }
        unsigned char product[1024];
        returnValue = libusb_get_string_descriptor_ascii(deviceHandle, deviceDescriptor.iProduct, product, 1024);
        if (returnValue < 0) {
            product[0] = '\0';
        }
        libusb_config_descriptor *configDescriptor;
        libusb_get_config_descriptor(devices[deviceIndex], 0, &configDescriptor);
        cout << "idVendor=" << deviceDescriptor.idVendor << ", idProduct=" << deviceDescriptor.idProduct << ", number configurations=" << (int)deviceDescriptor.bNumConfigurations << ", device class=" << (int)deviceDescriptor.bDeviceClass << ", manufacturer=" << manufacturer << ", product=" << product << ", number interfaces=" << (int)configDescriptor->bNumInterfaces << ", number alternate settings=" << (int)configDescriptor->interface->num_altsetting << ", numEndpoints=" << (int)configDescriptor->interface->altsetting->bNumEndpoints << endl;
        for (int endpointNum = 0; endpointNum < (int)configDescriptor->interface->altsetting->bNumEndpoints; endpointNum++) {
            unsigned char endpointAddress = configDescriptor->interface->altsetting->endpoint[endpointNum].bEndpointAddress,
                endpointNumber = endpointAddress & LIBUSB_ENDPOINT_ADDRESS_MASK,
                endpointDirection = configDescriptor->interface->altsetting->endpoint[endpointNum].bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK,
                transferType = configDescriptor->interface->altsetting->endpoint[endpointNum].bmAttributes & LIBUSB_TRANSFER_TYPE_MASK;
            cout << "endpoint number=" << (int)endpointNumber << ", direction=" << (endpointDirection == LIBUSB_ENDPOINT_IN ? "in" : "out") << ", transferType=" << (transferType == LIBUSB_TRANSFER_TYPE_CONTROL ? "control" : transferType == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS ? "isochronous" : transferType == LIBUSB_TRANSFER_TYPE_BULK ? "bulk" : transferType == LIBUSB_TRANSFER_TYPE_INTERRUPT ? "interrupt" : "unknown") << endl;
            if (transferType == LIBUSB_TRANSFER_TYPE_BULK) {
                if (endpointDirection == LIBUSB_ENDPOINT_IN) {
                    inEndpoint = endpointAddress;
                } else {
                    outEndpoint = endpointAddress;
                }
            } else if (findInterruptIn
                       && transferType == LIBUSB_TRANSFER_TYPE_INTERRUPT) {
                if (endpointDirection == LIBUSB_ENDPOINT_IN) {
                    interruptInEndpoint = endpointAddress;
                }
            }
        }
        libusb_free_config_descriptor(configDescriptor);
        if (idVendor == deviceDescriptor.idVendor && idProduct == deviceDescriptor.idProduct) {
            break;
        }
        libusb_close(deviceHandle);
        deviceHandle = NULL;
    }
    if (deviceIndex == deviceCount) {
        throw Support::makeMessage("USBStream cannot find device", "");
    }
    if (findIn && !inEndpoint) {
        throw Support::makeMessage("USBStream cannot find input", "");
    }
    if (findOut && !outEndpoint) {
        throw Support::makeMessage("USBStream cannot find output", "");
    }
    if (findInterruptIn && !interruptInEndpoint) {
        throw Support::makeMessage("USBStream cannot find interrupt input", "");
    }
	libusb_free_device_list(devices, 1);
    if (libusb_kernel_driver_active(deviceHandle, 0)) {
        returnValue = libusb_detach_kernel_driver(deviceHandle, 0);
        if (returnValue < 0) {
            throw Support::makeMessage("USBStream libusb_detach_kernel_driver", strerror(errno));
        }
    }
    returnValue = libusb_claim_interface(deviceHandle, 0);
    if (returnValue < 0) {
        throw Support::makeMessage("USBStream claim_interface", strerror(errno));
    }
}
#endif

USBBuffer::~USBBuffer()
{
#ifdef USE_WIN_USB
    if (usbHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(usbHandle);
    }
#else
    setbuf(NULL, 0);
    if (deviceHandle) {
        libusb_release_interface(deviceHandle, 0);
        libusb_close(deviceHandle);
    }
    if (context) {
        libusb_exit(context);
    }
#endif
}

int USBBuffer::overflow(int c)
{
#ifdef DEBUG_USB
    cout << "overflow" << endl;
#endif
    if (!base() && !allocate()) {
        return -1;
    }
    char *ep = base() + (blen() >> 1);
    if (!pbase()) {
        setp(base(), ep);
    }
#ifdef USE_WIN_USB
    DWORD numberOfBytesToWrite = pptr() - pbase(), numberOfBytesWritten = 0;
#else
    int numberOfBytesToWrite = pptr() - pbase(), numberOfBytesWritten = 0;
#endif
    while (numberOfBytesToWrite > 0) {
#ifdef DEBUG_USB
        cout << "bulk write " << numberOfBytesToWrite << endl;
#endif
#ifdef USE_WIN_USB
        if (!WriteFile(usbHandle, (LPCVOID)pbase(), numberOfBytesToWrite, &numberOfBytesWritten, NULL)) {
            Support::throwRuntimeError(GetLastError());
        }
        if (usbPacketSize != 0 && numberOfBytesToWrite % usbPacketSize == 0) {
            DWORD zeroNumberOfBytesWritten = 0;
            if (!WriteFile(usbHandle, NULL, 0, &zeroNumberOfBytesWritten, NULL)) {
                Support::throwRuntimeError(GetLastError());
            }
        }
#else
        int returnValue = libusb_bulk_transfer(deviceHandle, outEndpoint, (unsigned char *)pbase(), numberOfBytesToWrite, &numberOfBytesWritten, 0);
        if (returnValue < 0) {
            throw Support::makeMessage("USBStream bulk_transfer", strerror(errno));
        }
#endif
#ifdef DEBUG_USB
        cout << "bulk write numberOfBytesWritten=" << numberOfBytesWritten << endl;
#endif
        if ((numberOfBytesToWrite -= numberOfBytesWritten) < 0) {
            break;
        }
        setp(pbase() + numberOfBytesWritten, ep);
    }
    setp(base(), ep);
    return c == -1 ? 0 : sputc(c);
}

int USBBuffer::underflow()
{
#ifdef DEBUG_USB
    cout << "underflow" << endl;
#endif
    if (!base() && !allocate() || gptr() && gptr() < egptr()) {
        return -1;
    }
    char *gp = base() + (blen() >> 1);
#ifdef USE_WIN_USB
    DWORD numberOfBytesToRead = fullRead ? blen() >> 1 : usbPacketSize, numberOfBytesRead = 0;
#else
    int numberOfBytesToRead = blen() >> 1, numberOfBytesRead = 0;
#endif
#ifdef DEBUG_USB
    cout << "bulk " << (interruptInEndpoint ? "interrupt " : "") << "read numberOfBytesToRead=" << numberOfBytesToRead << ", fullRead=" << fullRead << endl;
#endif
#ifdef USE_WIN_USB
    for (;;) {
        if (fullRead) {
#ifdef DEBUG_USB
            cout << "ReadFile" << endl;
#endif
            if (!ReadFile(usbHandle, (LPVOID)gp, numberOfBytesToRead, &numberOfBytesRead, NULL)) {
                Support::throwRuntimeError(GetLastError());
            }
#ifdef DEBUG_USB
            cout << "ReadFile numberOfBytesRead=" << numberOfBytesRead << endl;
#endif
            if (numberOfBytesRead == 0) {
                fullRead = false;
            }
            else {
                break;
            }
        } else {
#ifdef DEBUG_USB
            cout << "DeviceIoControl" << endl;
#endif
            DeviceIoControl(usbHandle, IOCTL_ASYNC_IN, 0, 0, gp, numberOfBytesToRead, &numberOfBytesRead, NULL);
            break;
        }
    }
#else
    int returnValue = interruptInEndpoint ? libusb_interrupt_transfer(deviceHandle, interruptInEndpoint, (unsigned char *)gp, numberOfBytesToRead, &numberOfBytesRead, 0) : libusb_bulk_transfer(deviceHandle, inEndpoint, (unsigned char *)gp, numberOfBytesToRead, &numberOfBytesRead, 0);
    if (returnValue < 0) {
        throw Support::makeMessage("USBStream bulk_transfer", strerror(errno));
    }
#endif
#ifdef DEBUG_USB
    cout << "bulk read numberOfBytesRead=" << numberOfBytesRead << endl;
#endif
    setg(gp, gp, gp + numberOfBytesRead);
    return numberOfBytesRead > 0 ? (*gp & 0xff) : -1;
}

int USBBuffer::sync()
{
#ifdef DEBUG_USB
    cout << "sync" << endl;
#endif
    return overflow(-1);
}

streambuf *USBBuffer::setbuf(char *buffer, streamsize count)
{
#ifdef DEBUG_USB
    cout << "setbuf count=" << count << endl;
#endif
    if (bufferAllocated) {
        if (this->buffer) {
            delete this->buffer;
            this->buffer = NULL;
        }
        this->count = 0;
        bufferAllocated = false;
    }
    this->buffer = buffer;
    this->count = count;
    if (buffer != NULL) {
        char *ep = base() + (blen() >> 1);
        setp(base(), ep);
        setg(ep, ep, ep);
    }
    return this;
}
