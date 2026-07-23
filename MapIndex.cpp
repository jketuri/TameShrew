
#include "MapIndex.hpp"

ostream& operator<<(ostream &out, const MapEntry &mapEntry)
{
    out << "MapEntry{filePath=" << mapEntry.filePath
        << ",coordinates1=" << mapEntry.coordinates1 << ",coordinates2=" << mapEntry.coordinates2
        << ",x_scale=" << mapEntry.x_scale << ",y_scale=" << mapEntry.y_scale
        << ",imageWidth=" << mapEntry.imageWidth << ",imageHeight=" << mapEntry.imageHeight;
    if (mapEntry.elevationMapEntry) {
        out << *mapEntry.elevationMapEntry;
    }
    out << "}";
    return out;
}

ostream& operator<<(ostream &out, const MapIndex &mapIndex)
{
    for (unsigned scaleIndex = 0; scaleIndex < LENGTH(mapIndex.mapEntries); scaleIndex++) {
        for (MAPENTRIES::const_iterator aMapEntry = mapIndex.mapEntries[scaleIndex].begin();
             aMapEntry != mapIndex.mapEntries[scaleIndex].end();
             aMapEntry++) {
            MapEntry &mapEntry = **aMapEntry;
            out << "[" << scaleIndex << "] " << mapEntry << endl;
        }
    }
    return out;
}

MapEntry::MapEntry() : x_scale(0.0), y_scale(0.0), imageWidth(0), imageHeight(0), elevationMapEntry((MapEntry *)NULL)
{
}

MapEntry::MapEntry(const MapEntry &mapEntry) : filePath(mapEntry.filePath), coordinates1(mapEntry.coordinates1), coordinates2(mapEntry.coordinates2), x_scale(mapEntry.x_scale), y_scale(mapEntry.y_scale), imageWidth(mapEntry.imageWidth), imageHeight(mapEntry.imageHeight), elevationMapEntry(mapEntry.elevationMapEntry)
{
}

bool MapEntry::operator==(MapEntry mapEntry)
{
    return filePath == mapEntry.filePath &&
        coordinates1.x == mapEntry.coordinates1.x &&
        coordinates1.y == mapEntry.coordinates1.y &&
        coordinates2.x == mapEntry.coordinates2.x &&
        coordinates2.y == mapEntry.coordinates2.y;
}

MapIndex::MapIndex(string &dirPath)
{
    international1924 = new ReferenceEllipsoid("International 1924", 6378388.0, 297.0);
    WGS84ReferenceEllipsoid = new ReferenceEllipsoid("WGS 84", 6378137.0, 298.25722);
    WGS84 = new Datum("WGS 84", WGS84ReferenceEllipsoid, 0.0, 0.0, 0.0);
    european1950 = new Datum("European 1950", international1924, -87.0, -98.0, -121.0);
    european1979 = new Datum("European 1979", international1924, -86.0, -98.0, -119.0);
    transverseMercator = new Projection("Transverse Mercator", international1924, Support::toRadians(27.0), Support::toRadians(0.0), 1.0, 3500000.0, 0.0);
    projection = -1;
    datum = -1;
    generate(dirPath, 0);
    for (MAPENTRIES::const_iterator aMapEntry = mapEntries[1].begin();
         aMapEntry != mapEntries[1].end();
         aMapEntry++) {
        MapEntry &mapEntry = **aMapEntry;
        const MapEntry *mapEntry0 = search(mapEntry.coordinates1, 0);
        if (!mapEntry0) continue;
        mapEntry.elevationMapEntry = mapEntry0->elevationMapEntry;
    }
    cout << "MapIndex=" << endl;
    cout << *this;
}

const MapEntry *MapIndex::search(Coordinates &coordinates, int scaleIndex)
{

#ifdef DEBUG_MAP
    cout << "MapIndex.search coordinates=" << coordinates << ",scaleIndex=" << scaleIndex << endl;
#endif
    for (MAPENTRIES::const_iterator aMapEntry = mapEntries[scaleIndex].begin();
         aMapEntry != mapEntries[scaleIndex].end();
         aMapEntry++) {
        MapEntry &mapEntry = **aMapEntry;
        if (coordinates.x >= mapEntry.coordinates1.x && coordinates.x <= mapEntry.coordinates2.x &&
            coordinates.y >= mapEntry.coordinates2.y && coordinates.y <= mapEntry.coordinates1.y)
            return &mapEntry;
    }
    return (MapEntry *)NULL;
}

/** Converts geodesical coordinates in vector 'from' given in decimal
    degrees from WGS84 Datum to European 1979 Datum and these coordinates in
    turn to Universal Transverse Mercator Coordinates in vector 'to'. Uses
    Finnish unified one zone grid with false easting of 3500000 in 27E.

    @param from latitude in x and longitude in y given in decimal degrees
    @param to easting in x and northing in y given in meters */
double MapIndex::convert(const Coordinates &from, Coordinates &to)
{
    Coordinates from0, to0;
    from0.x = Support::toRadians(from.x);
    from0.y = Support::toRadians(from.y);
    from0.z = from.z;
    WGS84->convert(from0, to0, *european1979);
    double gridConvergence = transverseMercator->forward(to0, to);
    to.z = from.z;
    return gridConvergence;
}

void MapIndex::kkjToYkj(Coordinates &kkj, Coordinates &ykj)
{
    ykj.x = 0.999183661 * kkj.x + 0.046153552 * kkj.y + 527475.4297;
    ykj.y = -0.046163336 * kkj.x + 0.999177089 * kkj.y + 124700.5625;
    ykj.z = 0.0;
}

void MapIndex::generate(string &dirPath, int scaleIndex)
{
#ifdef WIN32
    struct _finddata_t finddata;
    intptr_t h = _findfirst((char *)(dirPath + "\\*.*").c_str(), &finddata);
    if (h == -1) {
        throw Support::makeMessage(dirPath.c_str(), strerror(errno));
    }
#else
    DIR *dir;
    struct dirent *dirent;
    dir = opendir(dirPath.c_str());
    if (!dir) {
        throw new string("Directory " + dirPath + " not found");
    }
#endif
    for (;;) {
        const char *fileName;
#ifdef WIN32
        fileName = finddata.name;
        if (*fileName == '.' || !strcmp(fileName, "elevation")) {
            continue;
        }
        string pathName = dirPath + "\\" + fileName;
        if (finddata.attrib & _A_SUBDIR) {
            generate(pathName, scaleIndex + 1);
            continue;
        }
#else
        dirent = readdir(dir);
        if (!dirent) {
            break;
        }
        fileName = dirent->d_name;
        if (*fileName == '.' || !strcmp(fileName, "elevation")) {
            continue;
        }
        string pathName = dirPath + "/" + fileName;
        struct stat st;
        if (stat(pathName.c_str(), &st)) throw new string("stat failed");
        if (st.st_mode & S_IFDIR) {
            generate(pathName, scaleIndex + 1);
            continue;
        }
#endif
        bool lowerCase = false;
        if (pathName.length() < 4 ||
            pathName.substr(pathName.length() - 4).compare(".TAB") != 0 &&
            !(lowerCase = pathName.substr(pathName.length() - 4).compare(".tab") == 0) ||
            _access((pathName.substr(0, pathName.length() - 4) + (lowerCase ? ".tif" : ".TIF")).c_str(), 0) != 0) {
            continue;
        }
        MapEntry &mapEntry = *new MapEntry;
        bool foundCoordinate1 = false, foundCoordinate3 = false;
        int point_x1 = 0, point_y1 = 0, point_x2 = 0, point_y2 = 0;
        ifstream in(pathName.c_str());
        string line;
        for (;;) {
            if (!getline(in, line)) {
                break;
            }
            bool isCoordinate1 = line.find("Label \"Pt 1\"") != -1;
            if (isCoordinate1 || line.find("Label \"Pt 3\"") != -1) {
                int leftParen = (int)line.find('('), rightParen = (int)line.find(')', leftParen + 1);
                if (leftParen == -1 || rightParen == -1) break;
                StringTokenizer st(line.substr(leftParen + 1, rightParen - leftParen - 1), ",", false);
                leftParen = (int)line.find('(', rightParen + 1);
                rightParen = (int)line.find(')', leftParen + 1);
                if (leftParen == -1 || rightParen == -1) break;
                StringTokenizer st1(line.substr(leftParen + 1, rightParen - leftParen - 1), ",", false);
                if (isCoordinate1) {
                    mapEntry.coordinates1.x = atof(Support::trim(st.nextToken()).c_str());
                    mapEntry.coordinates1.y = atof(Support::trim(st.nextToken()).c_str());
                    point_x1 = atoi(Support::trim(st1.nextToken()).c_str());
                    point_y1 = atoi(Support::trim(st1.nextToken()).c_str());
                    foundCoordinate1 = true;
                }
                else {
                    mapEntry.coordinates2.x = atof(Support::trim(st.nextToken()).c_str());
                    mapEntry.coordinates2.y = atof(Support::trim(st.nextToken()).c_str());
                    point_x2 = atoi(Support::trim(st1.nextToken()).c_str());
                    point_y2 = atoi(Support::trim(st1.nextToken()).c_str());
                    foundCoordinate3 = true;
                }
            }
            else if (line.find("CoordSys Earth Projection") != -1) {
                StringTokenizer st(line, ",", false);
                string token = Support::trim(st.nextToken());
                int space = (int)token.rfind(' ');
                projection = atoi(token.substr(space + 1, token.length()).c_str());
                datum = atoi(Support::trim(st.nextToken()).c_str());
            }
        }
        in.close();
        if (!foundCoordinate1 || !foundCoordinate3) {
            cout << "Invalid .TAB-file " << fileName;
            cout << " in line " << line;
            delete &mapEntry;
            continue;
        }
        if (datum == 28) {
            Coordinates coordinates;
            european1950->convert(mapEntry.coordinates1, coordinates, *european1979);
            mapEntry.coordinates1 = coordinates;
            european1950->convert(mapEntry.coordinates2, coordinates, *european1979);
            mapEntry.coordinates2 = coordinates;
        }
        string name(fileName);
        mapEntry.filePath = string(dirPath + FILE_SEPARATOR + name.substr(0, name.length() - 4) + (lowerCase ? ".tif" : ".TIF"));
        TIFF *tiff = TIFFOpen(mapEntry.filePath.c_str(), "r");
        TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &mapEntry.imageWidth);
        TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &mapEntry.imageHeight);
        TIFFClose(tiff);
        string elevationDataFilePathBase = string(dirPath + FILE_SEPARATOR + "elevation" + FILE_SEPARATOR + name.substr(0, name.length() - 4));
        if ((lowerCase = !_access((elevationDataFilePathBase + ".tif").c_str(), 0)) || !_access((elevationDataFilePathBase + ".TIF").c_str(), 0)) {
            class MapEntry &elevationMapEntry = *new MapEntry();
            elevationMapEntry.filePath = elevationDataFilePathBase + (lowerCase ? ".tif" : ".TIF");
            elevationMapEntry.coordinates1 = mapEntry.coordinates1;
            elevationMapEntry.coordinates2 = mapEntry.coordinates2;
            elevationMapEntry.coordinates2 = mapEntry.coordinates2;
            elevationMapEntry.coordinates2 = mapEntry.coordinates2;
            TIFF *tiff = TIFFOpen(elevationMapEntry.filePath.c_str(), "r");
            TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &elevationMapEntry.imageWidth);
            TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &elevationMapEntry.imageHeight);
            TIFFClose(tiff);
            mapEntry.elevationMapEntry = &elevationMapEntry;
        }
        mapEntries[scaleIndex].insert(mapEntries[scaleIndex].end(), &mapEntry);
#ifdef WIN32
        if (_findnext(h, &finddata) != 0) {
            break;
        }
#endif
    }
}
