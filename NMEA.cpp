
#include "NMEA.hpp"

using namespace common;

ostream& operator<<(ostream &out, DATAFIELDS dataFields)
{
    for (DATAFIELDS::iterator dataFieldIterator = dataFields.begin();
         dataFieldIterator != dataFields.end();
         dataFieldIterator++) out << (string)*dataFieldIterator << " ";
    out << endl;
    return out;
}

NMEASentence::~NMEASentence()
{
}

void NMEASentence::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

double NMEASentence::parseLatitude(string &latitude, string &hemisphere)
{
    if (latitude.length() < 3 || hemisphere.length() < 1) {
        throw new string("Invalid latitude " + latitude);
    }
    double decimal = atof(latitude.substr(0, 2).c_str()) + atof(latitude.substr(2).c_str()) / 60.0;
    return toupper(hemisphere[0]) == 'S' ? -decimal : decimal;
}

double NMEASentence::parseLongitude(string &longitude, string &hemisphere)
{
    if (longitude.length() < 4 || hemisphere.length() < 1) {
        throw new string("Invalid longitude " + longitude);
    }
    double decimal = atof(longitude.substr(0, 3).c_str()) + atof(longitude.substr(3).c_str()) / 60.0;
    return toupper(hemisphere[0]) == 'W' ? -decimal : decimal;
}

NMEA::NMEA(int version) : version(version), outStream(NULL)
{
    (void)&version;
}

void NMEA::simpleDataInvalid(int simpleData)
{
    (void)&simpleData;
}

void NMEA::simpleCrossTrackError(int crossTrackError)
{
    (void)&crossTrackError;
}

void NMEA::complexDataInvalid(string &complexData)
{
    (void)&complexData;
}

void NMEA::wrongChecksum(string &complexData)
{
#ifdef DEBUG_NMEA
    cout << "wrong checksum " << complexData << endl;
#endif
}

void NMEA::complexData(char crossTrackErrorUnits, double crossTrackErrorValue, char crossTrackErrorPosition,
                       char trueOrMagneticBearing, int bearingToNextWayPoint, string presentLatitude, string presentLongitude, char status)
{
    (void)&crossTrackErrorUnits;
    (void)&crossTrackErrorValue;
    (void)&crossTrackErrorPosition;
    (void)&trueOrMagneticBearing;
    (void)&bearingToNextWayPoint;
    (void)&presentLatitude;
    (void)&presentLongitude;
    (void)&status;
}

void NMEA::sentenceDataInvalid(string &sentence)
{
#ifdef DEBUG_NMEA
    cout << "sentence data invalid: '" << sentence << "'" << endl;
#endif
}

void NMEA::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
#ifdef DEBUG_NMEA
    //cout << "id=" << id << ",dataFields=" << dataFields << endl;
#endif
}

int NMEA::readOldByte(istream &in)
{
    int c;
    for (in >> c; c != -1;)
        if ((c & 0x80) == 0) {
            // bit 7 is 0, simple data format
            if ((c & 0x40) == 0) {
                // bit 6 is 0, invalid data
                simpleDataInvalid(c);
                continue;
            }
            // bits 0 - 5 give the cross-track error
            simpleCrossTrackError(c & 0x3f);
            continue;
        }
        else return c;
    return -1;
}

void NMEA::readOldInputStream(istream &in, bool *stopped)
{
    for (;;) {
        int c = readOldByte(in);
        if (c == -1 || stopped && *stopped) {
            break;
        }
        if (c != '$') {
            continue;
        }
#if WIN32
        basic_ostringstream<char> buffer;
#else
        ostringstream buffer;
#endif
        buffer << (char)c;
        while ((c = readOldByte(in)) != -1 && c != 0x83) buffer << (char)c;
        string data = buffer.str();
        if (data.length() < 36 || data[1] != 'M' && data[2] != 'P') {
            complexDataInvalid(data);
            if (c == -1) break;
            continue;
        }
        char crossTrackErrorUnits = data[3];
        if (crossTrackErrorUnits != 'K' &&  // kilometres
            crossTrackErrorUnits != 'N' &&  // nautical miles
            crossTrackErrorUnits != 'U') {  // microseconds
            complexDataInvalid(data);
            if (c == -1) {
                break;
            }
            continue;
        }
        double crossTrackErrorValue = atof(data.substr(4, 4).c_str());
        char crossTrackErrorPosition = data[8];
        if (crossTrackErrorPosition != 'L' || crossTrackErrorPosition != 'R') {
            complexDataInvalid(data);
            if (c == -1) {
                break;
            }
            continue;
        }
        char trueOrMagneticBearing = data[9];
        if (trueOrMagneticBearing != 'T' || trueOrMagneticBearing != 'M') {
            complexDataInvalid(data);
            if (c == -1) {
                break;
            }
            continue;
        }
        int bearingToNextWayPoint = atol(data.substr(10, 3).c_str());
        string presentLatitude = data.substr(13, 10),
            presentLongitude = data.substr(23, 11);
        char status = (char)data[34];
        complexData(crossTrackErrorUnits, crossTrackErrorValue, crossTrackErrorPosition,
                    trueOrMagneticBearing, bearingToNextWayPoint, presentLatitude, presentLongitude, status);
        if (c == -1) {
            break;
        }
    }
}

void NMEA::readNewInputStream(istream &in, bool *stopped)
{
#ifdef DEBUG_NMEA
    cout << "read new input stream" << endl;
#endif
    string sentence;
    while (getline(in, sentence)) {
        if (stopped && *stopped) {
            break;
        }
        int length = sentence.length();
        if (length > 0 && sentence.at(length - 1) == '\r')
            sentence.resize(length - 1);
#ifdef DEBUG_NMEA
        cout << "sentence='" << sentence << "'" << endl;
#endif
        if (sentence == "") {
            continue;
        }
        StringTokenizer st = StringTokenizer(sentence, ",*$", true);
        if (!st.hasMoreTokens() || st.nextToken() != "$" || !st.hasMoreTokens()) {
            sentenceDataInvalid(sentence);
            continue;
        }
        string id = st.nextToken();
        if (id.length() < 5 || string(",*$").find(id[0]) != (unsigned)-1) {
            sentenceDataInvalid(sentence);
            continue;
        }
        bool error = false;
        DATAFIELDS dataFields;
        for (string lastToken(""), token; st.hasMoreTokens(); lastToken = token) {
            token = st.nextToken();
            if (token == ",") {
                if (lastToken != ",") {
                    continue;
                }
                dataFields.insert(dataFields.end(), "");
                continue;
            }
            if (token == "*") {
                if (!st.hasMoreTokens()) {
                    break;
                }
                char *p;
                int checkSum = strtol(st.nextToken().c_str(), &p, 16), countedCheckSum = 0;
                for (int i = (int)sentence.find_last_of('*') - 1; i > 0; i--) countedCheckSum ^= sentence[i];
                if (checkSum != countedCheckSum) {
                    wrongChecksum(sentence);
                    error = true;
                    break;
                }
                continue;
            }
            if (token == "$") {
                sentenceDataInvalid(sentence);
                error = true;
                break;
            }
            dataFields.insert(dataFields.end(), token);
        }
        if (error) {
            continue;
        }
        if (outStream) {
            *outStream << sentence << endl;
        }
        NMEASENTENCES::iterator nmeaSentenceIterator = nmeaSentences.find(id);
        if (nmeaSentenceIterator != nmeaSentences.end()) {
            NMEASentence *nmeaSentence = (NMEASentence *)nmeaSentenceIterator->second;
            nmeaSentence->sentenceData(id, dataFields, sentence);
        }
        else {
            sentenceData(id, dataFields, sentence);
        }
    }
    if (outStream) {
        outStream->close();
    }
}

void NMEA::readInputStream(istream &in, bool *stopped)
{
    if (version <= 182) {
        readOldInputStream(in, stopped);
    } else {
        readNewInputStream(in, stopped);
    }
}
