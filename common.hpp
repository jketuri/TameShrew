
#ifndef _js_common_hpp_
#define _js_common_hpp_

#define USE_OSG

#include <string.h>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <sstream>
#include <streambuf>
#include <string>
#ifdef WIN32
#include <io.h>
#include <direct.h>
//#include <winsock2.h>
#include <windows.h>
#define FILE_SEPARATOR "\\"
#define IMAGE_FILE_TYPE "bmp"
#else
extern "C"
{
#include <dirent.h>
}
#define FILE_SEPARATOR "/"
#define IMAGE_FILE_TYPE "png"
#define _access access
#endif
#ifdef USE_OSG
#include <osgDB/fstream>
#endif

#ifndef DEBUG
#define DEBUG
#endif
#ifndef DEBUG_MAP
//#define DEBUG_MAP 0
#endif
#ifndef DEBUG_NMEA
#define DEBUG_NMEA
#endif
#ifndef DEBUG_USB
#define DEBUG_USB
#endif

using namespace std;

#define LENGTH(array) (sizeof(array) / sizeof(*array))

namespace common {

class StringTokenizer
{

private:

    string tokens;
    string delimChars;
    string quoteChars;
    const char escapeChar;
    string token;
	bool hasQuoteChars;
    bool returnDelims;
    bool hasToken;
    int pos;

public:

    StringTokenizer(string tokens, string delimChars, bool returnDelims);
    StringTokenizer(string tokens, string delimChars, string quoteChars, char escapeChar, bool returnDelims);
    ~StringTokenizer();
    bool hasMoreTokens();
    string nextToken();

};

class Properties : public map<string, string *, less<string> >
{

public:

    ~Properties();
    void load(const string &filePath);
    const string *getProperty(const string &name);

};

class Support
{

public:

    static bool isTrueOrFalse(const string &value);
    static string trim(const string &value);
    static double toRadians(double degrees);
    static string toPhysicalPath(string &path);
    static string unescape(const string &value, char escapeChar);
    static string unquote(const string &value);
    static string& findAndReplace(string &value, const string &findValue, const string &replaceValue);
    static void makeDirs(string &dir);
#ifdef WIN32
    static void throwRuntimeError(long lastError);
#endif

    static const double PI;

    static inline string *makeMessage(const char *name, const char *text) {
        string *message = new string();
        (*message).append(name).append(": ").append(text);
        return message;
    }

};

}

#endif
