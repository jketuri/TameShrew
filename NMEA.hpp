
#ifndef _js_NMEA_hpp_
#define _js_NMEA_hpp_

#include <stdlib.h>
#include <iostream>
#include <iosfwd>
#include <map>
#include <sstream>
#include <streambuf>
#include <string>
#include <utility>
#include <vector>
#include "common.hpp"

// old NMEA complex data status byte flags
#define MANUAL_CYCLE_CLOCK   (1 << 0)
#define LOW_SNR              (1 << 1)
#define CYCLE_JUMP           (1 << 2)
#define BLINK                (1 << 3)
#define ARRIVAL_ALARM        (1 << 4)
#define DISCONTINUITY_OF_TDS (1 << 5)

typedef vector<string> DATAFIELDS;

class NMEASentence
{

public:

    virtual ~NMEASentence();

    virtual void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);
    double parseLatitude(string &latitude, string &hemisphere);
    double parseLongitude(string &longitude, string &hemisphere);

};

typedef map<string, NMEASentence *, less<string>, allocator<pair<const string, NMEASentence *> > > NMEASENTENCES;

class NMEA
{

public:

    int version;
    NMEASENTENCES nmeaSentences;
    ofstream *outStream;

    NMEA(int version);

    void simpleDataInvalid(int simpleData);

    void simpleCrossTrackError(int crossTrackError);

    void complexDataInvalid(string &complexData);

    void wrongChecksum(string &complexData);

    void complexData(char crossTrackErrorUnits, double crossTrackErrorValue, char crossTrackErrorPosition,
                     char trueOrMagneticBearing, int bearingToNextWayPoint, string presentLatitude, string presentLongitude, char status);

    void sentenceDataInvalid(string &sentence);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

    int readOldByte(istream &in);

    void readOldInputStream(istream &in, bool *stopped);

    void readNewInputStream(istream &in, bool *stopped);

    void readInputStream(istream &in, bool *stopped);

};

ostream& operator<<(ostream &out, DATAFIELDS dataFields);

#endif
