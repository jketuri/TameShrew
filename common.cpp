
#include "common.hpp"

common::StringTokenizer::StringTokenizer(string tokens, string delimChars, bool returnDelims) : tokens(tokens), delimChars(delimChars), escapeChar(0), hasQuoteChars(false), returnDelims(returnDelims), hasToken(false), pos(0)
{
}

common::StringTokenizer::StringTokenizer(string tokens, string delimChars, string quoteChars, char escapeChar, bool returnDelims) : tokens(tokens), delimChars(delimChars), quoteChars(quoteChars), escapeChar(escapeChar), hasQuoteChars(true), returnDelims(returnDelims), hasToken(false), pos(0)
{
}

common::StringTokenizer::~StringTokenizer()
{
}

bool common::StringTokenizer::hasMoreTokens()
{
    if (hasToken) {
        return true;
    }
    if (pos >= (int)tokens.length()) {
        return hasToken = false;
    }
    int pos1 = (int)tokens.find_first_not_of(delimChars, pos);
    if (pos1 != -1 && (!returnDelims || pos1 == pos)) {
        pos = pos1;
        if (pos1 < (int)tokens.length()) {
            if (hasQuoteChars && quoteChars.find(tokens.at(pos1)) != string::npos) {
                for (;;) {
                    if (pos1 < (int)tokens.length() - 1) {
                        pos1 = (int)tokens.find_first_of(quoteChars, pos1 + 1);
                        if (pos1 > 0 && tokens.at(pos1 - 1) == escapeChar) {
                            pos1++;
                            continue;
                        } else if (pos1 != -1) {
                            pos1++;
                        }
                    } else {
                        pos1 = -1;
                    }
                    break;
                }
            } else {
                pos1 = (int)tokens.find_first_of(delimChars, pos);
            }
        } else {
            pos1 = -1;
        }
    }
    if (pos1 == -1) {
        if (pos < (int)tokens.length()) {
            token = tokens.substr(pos);
            pos = (int)tokens.length();
            return hasToken = true;
        }
        else {
            return hasToken = false;
        }
    }
    token = tokens.substr(pos, pos1 -= pos);
    if (escapeChar) {
        token = Support::unescape(token, escapeChar);
    }
    pos += pos1;
    return hasToken = true;
}

string common::StringTokenizer::nextToken()
{
    if (hasToken || hasMoreTokens()) {
        hasToken = false;
        return token;
    }
    throw new string("No next token");
}

common::Properties::~Properties()
{
    for (Properties::iterator properties = begin();
         properties != end(); properties++)
        delete (string *)properties->second;
}

void common::Properties::load(const string &filePath)
{
#ifdef USE_OSG
    osgDB::ifstream in(filePath.c_str());
#else
    ifstream in(filePath.c_str());
#endif
    if (!in.is_open()) {
        throw new string("Properties file " + filePath + " not found");
    }
    string line;
    for (;;) {
        if (!getline(in, line)) {
            break;
        }
        StringTokenizer tokens(line, "=#", true);
        if (tokens.hasMoreTokens()) {
            string token = Support::trim(tokens.nextToken());
            if (token == "=" || token == "#") {
                continue;
            }
            string name = token;
            if (tokens.hasMoreTokens()) {
                token = Support::trim(tokens.nextToken());
                if (token != "=") {
                    continue;
                }
                string value = Support::trim(tokens.nextToken());
                insert(value_type(name, new string(value)));
            }
        }
    }
    in.close();
}

const string *common::Properties::getProperty(const string &name)
{
    Properties::const_iterator values = find(name);
    return values != end() ? values->second : NULL;
}

bool common::Support::isTrueOrFalse(const string &value)
{
    if (value == "true") {
        return true;
    }
    if (value == "false") {
        return false;
    }
    throw new string("Value must be 'true' or 'false'");
}

const double common::Support::PI = 3.14159265358979323846;

#ifdef WIN32
void common::Support::throwRuntimeError(long lastError)
{
    LPTSTR buffer;
    BOOL value = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                               NULL, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT), (LPTSTR)&buffer, 0, NULL);
    if (!value) {
        char a[64];
        _itoa(lastError, a, 10);
        throw new string(a);
    }
    string *message = new string(buffer);
    LocalFree(buffer);
    throw message;
}
#endif

string common::Support::trim(const string &value)
{
    int firstNonWhite = (int)value.find_first_not_of("\t\n "),
        lastNonWhite = (int)value.find_last_not_of("\t\n ");
    if (firstNonWhite == -1) {
        return "";
    }
    return value.substr(firstNonWhite, lastNonWhite - firstNonWhite + 1);
}

double common::Support::toRadians(double degrees)
{
    return degrees / 180.0 * PI;
}

string common::Support::toPhysicalPath(string &path)
{
    char *home;
#if WIN32
    home = getenv("USERPROFILE");
    path = Support::findAndReplace(path, "/", FILE_SEPARATOR);
#else
    home = getenv("HOME");
    path = Support::findAndReplace(path, "\\", FILE_SEPARATOR);
#endif
    int endIndex = path.length();
    if (endIndex > 0 && path.at(endIndex - 1) == FILE_SEPARATOR[0]) {
        endIndex--;
    }
    string::size_type tilde = path.find("~");
    if (tilde != string::npos) {
        return path.substr(0, tilde) + home + path.substr(tilde + 1, endIndex);
    }
    return path.substr(0, endIndex);
}

string common::Support::unescape(const string &value, char escapeChar)
{
    char escapeChars[2];
    escapeChars[0] = escapeChar;
    escapeChars[1] = 0;
#if WIN32
    basic_ostringstream<char> buffer;
#else
    ostringstream buffer;
#endif
    int pos = 0, length = value.length();
    while (pos < length) {
        int escape = value.find_first_of(escapeChars, pos);
        if (escape != -1 && escape < length - 1) {
            buffer << value.substr(pos, escape);
            buffer << value.at(escape + 1);
            pos = escape + 2;
        } else {
            if (pos == 0) {
                return value;
            }
            buffer << value.substr(pos);
            break;
        }
    }
    return buffer.str();
}

string common::Support::unquote(const string &value)
{
    return value.length() > 1 && value.at(0) == '"' ?
        value.substr(1, value.length() - 2) : value;
}

string& common::Support::findAndReplace(string &value, const string &findValue, const string &replaceValue)
{
    size_t findValueLength = findValue.length(), found = 0;
    for (;;) {
        found = value.find(findValue, found > 0 ? found + findValueLength : 0);
        if (found == string::npos) {
            break;
        }
        value.replace(found, findValueLength, replaceValue);
    }
    return value;
}

void common::Support::makeDirs(string &dir)
{
    for (size_t separator = 0;; separator++) {
        separator = dir.find(FILE_SEPARATOR, separator);
        string name = separator != string::npos ? dir.substr(0, separator) : dir;
#ifdef WIN32
        _mkdir(name.c_str());
#else
        mkdir(name.c_str(), S_IRWXU);
#endif
        if (separator == string::npos) {
            break;
        }
    }
}
