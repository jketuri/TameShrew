
#include "PhotoIndex.hpp"

using namespace common;

ostream& operator<<(ostream &out, const PhotoEntry &photoEntry)
{
    out << "PhotoEntry{filePath=" << photoEntry.filePath
        << ",timeModified=" << ctime(&photoEntry.timeModified) << ",deltaTime=" << photoEntry.deltaTime
        << ",coordinates=" << photoEntry.coordinates << "}";
    return out;
}

ostream& operator<<(ostream &out, const PhotoIndex &photoIndex)
{
    for (PHOTOENTRIES::const_iterator aPhotoEntryPair = photoIndex.photoEntries.begin(); aPhotoEntryPair != photoIndex.photoEntries.end(); aPhotoEntryPair++) {
        PhotoEntry &photoEntry = *aPhotoEntryPair->second;
        out << photoEntry << endl;
    }
    return out;
}

static time_t dateTime, dateTimeOriginal, dateTimeDigitized;

static time_t makeTime(char *value) {
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    value[4] = '\0';
    value[7] = '\0';
    value[10] = '\0';
    value[13] = '\0';
    value[16] = '\0';
    tm.tm_year = atoi((const char *)value) - 1900;
    tm.tm_mon = atoi((const char *)value + 5) - 1;
    tm.tm_mday = atoi((const char *)value + 8);
    tm.tm_hour = atoi((const char *)value + 11);
    tm.tm_min = atoi((const char *)value + 14);
    tm.tm_sec = atoi((const char *)value + 17);
    return mktime(&tm);
}

static void get_entry(ExifEntry *entry, void *data) {
    switch (entry->tag) {
    case EXIF_TAG_DATE_TIME:
        dateTime = makeTime((char *)entry->data);
        break;
    case EXIF_TAG_DATE_TIME_ORIGINAL:
        dateTimeOriginal = makeTime((char *)entry->data);
        break;
    case EXIF_TAG_DATE_TIME_DIGITIZED:
        dateTimeDigitized = makeTime((char *)entry->data);
        break;
    default: {
        PhotoEntry *photoEntry = (PhotoEntry *)data;
        photoEntry->exifEntries.insert(photoEntry->exifEntries.end(), entry);
    }
    }
}

static void show_ifd(ExifContent *content, void *data) {
    exif_content_foreach_entry(content, get_entry, data);
}

PhotoEntry::PhotoEntry()
{
}

PhotoIndex::PhotoIndex(string &dirPath)
{
    list(dirPath);
}

void PhotoIndex::list(string &dirPath)
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
        if (*fileName == '.') {
            continue;
        }
        string pathName = dirPath + "\\" + fileName;
        if (finddata.attrib & _A_SUBDIR) {
            list(pathName);
            continue;
        }
#else
        dirent = readdir(dir);
        if (!dirent) {
            break;
        }
        fileName = dirent->d_name;
        if (*fileName == '.') {
            continue;
        }
        string pathName = dirPath + "/" + fileName;
        struct stat st;
        if (stat(pathName.c_str(), &st)) throw new string("stat failed");
        if (st.st_mode & S_IFDIR) {
            list(pathName);
            continue;
        }
#endif
        ExifLoader *exifLoader = exif_loader_new();
        exif_loader_write_file(exifLoader, pathName.c_str());
        ExifData *exifData = exif_loader_get_data(exifLoader);
        exif_loader_unref(exifLoader);
        dateTime = (time_t)0L;
        dateTimeOriginal = (time_t)0L;
        dateTimeDigitized = (time_t)0L;
        PhotoEntry &photoEntry = *new PhotoEntry();
        exif_data_foreach_content(exifData, show_ifd, &photoEntry);
        time_t time = dateTime ? dateTime : dateTimeOriginal ? dateTimeOriginal : dateTimeDigitized;
#ifdef WIN32
        if (!time) time = finddata.time_write;
#else
        if (!time) time = st.st_mtime;
#endif
        photoEntry.fileName = fileName;
        photoEntry.filePath = pathName;
        photoEntry.timeModified = time;
        photoEntry.deltaTime = -1L;
        photoEntries.insert(pair<const time_t, class PhotoEntry *>(photoEntry.timeModified, &photoEntry));
#ifdef WIN32
        if (_findnext(h, &finddata) != 0) {
            break;
        }
#endif
    }
}

void PhotoIndex::assign(time_t time, Coordinates &coordinates)
{
    for (PHOTOENTRIES::const_iterator aPhotoEntryPair = photoEntries.lower_bound(time); aPhotoEntryPair != photoEntries.end(); aPhotoEntryPair++) {
        PhotoEntry &photoEntry = *aPhotoEntryPair->second;
        time_t deltaTime = abs((long)(photoEntry.timeModified - time));
        if (photoEntry.deltaTime == -1L || deltaTime < photoEntry.deltaTime) {
            photoEntry.deltaTime = deltaTime;
            photoEntry.coordinates = coordinates;
        }
    }
}
