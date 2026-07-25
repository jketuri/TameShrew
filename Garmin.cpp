
#include "Garmin.hpp"
#include "USBStream.hpp"

Garmin::Garmin() : data(NULL), currentDataSize(0)
{
}

Garmin::~Garmin()
{
    if (data) {
        delete data;
        data = NULL;
    }
}

void Garmin::pvtData(D800_Pvt_Data_Type &pvtData)
{
}

void Garmin::readInputOutputStream(
                                   iostream &inout,
                                   bool *stopped)
{
#ifdef DEBUG_NMEA
    cout << "read input stream" << endl;
#endif
    memset(&packet, 0, sizeof packet);
    packet.mPacketType = (unsigned char)Pt_USBProtocolLayer;
    packet.mPacketId = (unsigned short)Pid_Start_Session;
    inout.write((const char *)&packet, sizeof packet);
    inout << flush;
    USBBuffer *usbBuffer = dynamic_cast<USBBuffer *>(inout.rdbuf());
    bool started = false;
    while (!inout.eof() && (!stopped || *stopped)) {
#ifdef DEBUG_NMEA
        cout << "reading packet" << endl;
#endif
        memset(&packet, 0, sizeof packet);
        inout.read((char *)&packet, sizeof packet);
#ifdef DEBUG_NMEA
        cout << "packetType=" << (unsigned)packet.mPacketType
             << ", packetId=" << packet.mPacketId
             << ", dataSize=" << packet.mDataSize << endl;
#endif
        if (packet.mPacketType == Pt_USBProtocolLayer
            && packet.mPacketId == Pid_Data_Available) {
            if (usbBuffer) {
                usbBuffer->setFullRead(true);
            }
#ifdef DEBUG_NMEA
            cout << "reading data packet" << endl;
#endif
        }
        if (packet.mDataSize > 0) {
            if (currentDataSize < packet.mDataSize) {
                currentDataSize = packet.mDataSize;
                data = new char[currentDataSize];
            }
            inout.read(data, packet.mDataSize);
        }
        if (packet.mPacketType == Pt_USBProtocolLayer) {
            if (packet.mPacketId == Pid_Session_Started) {
                started = true;
#ifdef DEBUG_NMEA
                cout << "started session" << endl;
#endif
                memset(&packet, 0, sizeof packet);
                packet.mPacketType = (unsigned char)Pt_ApplicationLayer;
                packet.mPacketId = (unsigned short)Pid_L001_Command_Data;
                Command_Id_Type commandIdType = Cmnd_A010_Start_Pvt_Data;
                packet.mDataSize = sizeof commandIdType;
#ifdef DEBUG_NMEA
                cout << "writing command Start Pvt Data size=" << packet.mDataSize << endl;
#endif
                inout.write((const char *)&packet, sizeof packet);
                inout.write((const char *)&commandIdType, sizeof commandIdType);
                inout << flush;
            }
            continue;
        }
        if (packet.mPacketId != Pid_L001_Pvt_Data) {
            continue;
        }
#ifdef DEBUG_NMEA
        cout << "received Pvt Data" << endl;
#endif
        D800_Pvt_Data_Type *pvtData = (D800_Pvt_Data_Type *)data;
        this->pvtData(*pvtData);
    }

}
