
#ifndef _js_MapIndex_hpp_
#define _js_MapIndex_hpp_

#if defined(_WIN32) && !defined(__CYGWIN__)
#define USE_WIN32
#endif

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#include <dirent.h>
#ifndef __APPLE__
#include <sys/io.h>
#endif
#include <sys/stat.h>
#endif
#include <stdio.h>
#include <tiffio.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include "common.hpp"
#include "Datum.hpp"
#include "NMEA.hpp"

using namespace common;
using namespace std;

class MapEntry;

class MapEntry
{

public:

	MapEntry();
	MapEntry(const MapEntry &mapEntry);

	bool operator==(MapEntry mapEntry);

	string filePath;
	Coordinates coordinates1;
	Coordinates coordinates2;
	double x_scale;
	double y_scale;
	uint32 imageWidth;
	uint32 imageHeight;
    MapEntry *elevationMapEntry;

};

typedef vector<MapEntry *> MAPENTRIES;

class MapIndex
{

public:

    MAPENTRIES mapEntries[2];

    MapIndex(string &dirPath);
    const MapEntry *search(Coordinates &coordinates, int scaleIndex);
    double convert(const Coordinates &from, Coordinates &to);
    void kkjToYkj(Coordinates &kkj, Coordinates &ykj);
    void print(ostream out);

private:

    ReferenceEllipsoid *international1924;
    ReferenceEllipsoid *WGS84ReferenceEllipsoid;
    Datum *WGS84;
    Datum *european1950;
    Datum *european1979;
    Projection *transverseMercator;
    int projection;
    int datum;

    void generate(string &dirPath, int scaleIndex);

};

ostream& operator<<(ostream &out, const MapEntry &mapEntry);
ostream& operator<<(ostream &out, const MapIndex &mapIndex);

#endif
