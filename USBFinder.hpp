
#ifndef _js_USBFinder_hpp_
#define _js_USBFinder_hpp_

#if defined(_WIN32) && !defined(__CYGWIN__)
#define USE_WIN32
#define USE_WIN_USB
#endif

#ifdef WIN32
//#include <winsock2.h>
#include <windows.h>
#endif
#ifdef USE_WIN_USB
#include <DEVPKEY.h>
#include <SetupAPI.h>
#else
#include <libusb-1.0/libusb.h>
#endif
#include <vector>

#include "common.hpp"

using namespace std;

class USBFinder
{
public:

    static vector<string> findUSBDevices(const string &forDeviceSetupClass);

private:
    static PCHAR getDeviceProperty(HDEVINFO hDeviceInfo, SP_DEVINFO_DATA devInfoData, DWORD property, DWORD &propertyRegDataType, DWORD &size);
};

#endif
