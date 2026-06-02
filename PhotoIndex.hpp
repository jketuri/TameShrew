
#ifndef _js_PhotoIndex_hpp_
#define _js_PhotoIndex_hpp_

#if defined(_WIN32) && !defined(__CYGWIN__)
#define USE_WIN32
#endif

#include <math.h>
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
#include <list>
#include <map>
#include <string>
#include <string.h>
#include <time.h>
#include <libexif/exif-loader.h>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Referenced>
#include "Datum.hpp"
#include "common.hpp"

using namespace common;
using namespace std;

class PhotoEntry : public osg::Referenced
{

public:

    PhotoEntry();

    string fileName;
    string filePath;
    time_t timeModified;
    time_t deltaTime;
    Coordinates coordinates;
    list<ExifEntry *> exifEntries;
    osg::ref_ptr<osg::MatrixTransform> photoPinTransform;
    osg::ref_ptr<osg::MatrixTransform> photoTransform;

};

typedef multimap<const time_t, class PhotoEntry *> PHOTOENTRIES;

class PhotoIndex
{

public:

    PHOTOENTRIES photoEntries;

    PhotoIndex(string &dirPath);
    void list(string &dirPath);
    void assign(time_t time, Coordinates &coordinates);

};

ostream& operator<<(ostream &out, const PhotoEntry &photoEntry);
ostream& operator<<(ostream &out, const PhotoIndex &photoIndex);

#endif
