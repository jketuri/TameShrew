
#include "USBFinder.hpp"

//#define DEBUG_USBFinder

using namespace common;

#ifdef DEBUG_USBFinder
void WriteValue(HANDLE file, const char *value, DWORD size, CHAR endCharacter)
{
    DWORD written;
    WriteFile(file, value, size, &written, NULL);
    WriteFile(file, &endCharacter, 1, &written, NULL);
}
#endif

PCHAR USBFinder::getDeviceProperty(HDEVINFO hDeviceInfo, SP_DEVINFO_DATA devInfoData, DWORD property, DWORD &propertyRegDataType, DWORD &size)
{
    if (!SetupDiGetDeviceRegistryProperty(hDeviceInfo, &devInfoData,
                                          property,
                                          &propertyRegDataType,
                                          NULL, 0, &size)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_DATA) {
            return NULL;
        }
        if (error != ERROR_INSUFFICIENT_BUFFER) {
            SetupDiDestroyDeviceInfoList(hDeviceInfo);
            Support::throwRuntimeError(GetLastError());
        }
    }
    PCHAR value = new CHAR[size];
    if (!SetupDiGetDeviceRegistryProperty(hDeviceInfo, &devInfoData,
                                          property,
                                          &propertyRegDataType,
                                          (PBYTE)value, size, NULL)) {
        SetupDiDestroyDeviceInfoList(hDeviceInfo);
        Support::throwRuntimeError(GetLastError());
    }
    return value;
}

vector<string> USBFinder::findUSBDevices(const string &forDeviceSetupClass)
{
    HDEVINFO hDeviceInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (hDeviceInfo == INVALID_HANDLE_VALUE) {
        Support::throwRuntimeError(GetLastError());
    }
#ifdef DEBUG_USBFinder
    cout << "SetupDiGetClassDevs passed" << endl;
    HANDLE devices = CreateFile("devices.txt", GENERIC_WRITE,
                                0, NULL, CREATE_ALWAYS, 0, NULL);
#endif
    SP_DEVINFO_DATA devInfoData;
    memset(&devInfoData, 0, sizeof devInfoData);
    devInfoData.cbSize = sizeof devInfoData;
    vector<string> usbDevices;
    for (DWORD index = 0;; index++) {
        if (!SetupDiEnumDeviceInfo(hDeviceInfo, index, &devInfoData)) {
            DWORD error = GetLastError();
            if (error == ERROR_NO_MORE_ITEMS) {
#ifdef DEBUG_USBFinder
                cout << "No more items" << endl;
#endif
                break;
            }
            SetupDiDestroyDeviceInfoList(hDeviceInfo);
            Support::throwRuntimeError(error);
        }
#ifdef DEBUG_USBFinder
        cout << "SetupDiEnumDeviceInfo for " << index << " passed" << endl;
#endif
        DWORD propertyRegDataType;
        DWORD size = 0;
        PCHAR nameValue
            = getDeviceProperty(hDeviceInfo, devInfoData,
                                SPDRP_FRIENDLYNAME,
                                propertyRegDataType, size);
        if (nameValue == NULL) {
            nameValue = getDeviceProperty(hDeviceInfo, devInfoData,
                                          SPDRP_DEVICEDESC,
                                          propertyRegDataType, size);
            if (nameValue == NULL) {
                continue;
            }
        }
        string name(nameValue, 0, size);
        delete nameValue;
#ifdef DEBUG_USBFinder
        WriteValue(devices, name.c_str(), size, L' ');
#endif
        PCHAR deviceSetupClassValue
            = getDeviceProperty(hDeviceInfo, devInfoData,
                                SPDRP_CLASS,
                                propertyRegDataType, size);
        if (deviceSetupClassValue == NULL) {
            continue;
        }
        string deviceSetupClass(deviceSetupClassValue, 0, size);
        delete deviceSetupClassValue;
        if (!forDeviceSetupClass.empty()) {
            if (forDeviceSetupClass == deviceSetupClass) {
                usbDevices.push_back(name);
            }
        }
#ifdef DEBUG_USBFinder
        WriteValue(devices, deviceSetupClass.c_str(), size, L'\n');
#endif
    }
#ifdef DEBUG_USBFinder
    CloseHandle(devices);
#endif
    SetupDiDestroyDeviceInfoList(hDeviceInfo);
    return usbDevices;
}
