
#ifndef _js_LiveMap_hpp
#define _js_LiveMap_hpp

#ifdef WIN32
#define XMD_H
#endif

extern "C"
{
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <jpeglib.h>
#include <tiffio.h>
#include "portaudio.h"
}

#include <iostream>
#include <iterator>
#include <list>
#include <sstream>
#ifdef WIN32
#include <io.h>
#include <direct.h>
#include <locale>
#else
extern "C"
{
#include <dirent.h>
}
#endif
#include <set>
#include <OpenThreads/Condition>
#include <OpenThreads/Thread>
#include <OpenThreads/Mutex>
#include <osg/AlphaFunc>
#include <osg/DrawPixels>
#include <osg/GLExtensions>
#include <osg/Geometry>
#include <osg/LineSegment>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/NodeCallback>
#include <osg/Object>
#include <osg/Projection>
#include <osg/Sequence>
#include <osg/ShapeDrawable>
#include <osg/Switch>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/Viewport>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgSim/SphereSegment>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgText/Font>
#include <osgText/Text>
#include <osgUtil/IntersectVisitor>
#include <osgUtil/Optimizer>
#include <osgUtil/SceneView>
#include <osgUtil/SmoothingVisitor>
#include "Datum.hpp"
#include "MapIndex.hpp"
#include "PhotoIndex.hpp"
#include "NMEA.hpp"
#include "SerialStream.hpp"
#include "SocketStream.hpp"
#include "common.hpp"

#define BOTH_BUTTONS (osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)

using namespace common;
using namespace std;

class MapTexture;
class MapTextureData;
class ObstacleData;
class LineObstacleData;
class ObstacleGroup;
class ObstacleGroupData;
class TrackData;
class LiveMap;

typedef list<class MapTexture *> MAPTEXTURES;
typedef list<class ObstacleData *> OBSTACLEDATAS;
typedef list<class LineObstacleData *> LINEOBSTACLEDATAS;
typedef list<class ObstacleGroup *> OBSTACLEGROUPS;
typedef list<osg::Switch *> TRACKSWITCHES;

ostream& operator<<(ostream &out, const MapTexture &mapTexture);
ostream& operator<<(ostream &out, const MapTextureData &mapTextureData);
ostream& operator<<(ostream &out, const MAPTEXTURES &mapTextures);
ostream& operator<<(ostream &out, const ObstacleData &obstacleData);
ostream& operator<<(ostream &out, const ObstacleGroup &obstacleGroup);
ostream& operator<<(ostream &out, const OBSTACLEGROUPS &obstacleGroups);
ostream& operator<<(ostream &out, const TrackData &trackData);

class RealizeOperation : public osg::Operation
{

public:
    LiveMap &liveMap;
    osgViewer::Viewer &viewer;

    RealizeOperation(LiveMap &liveMap, osgViewer::Viewer &viewer);

    void operator()(osg::Object *);

};

/*
  class ContextCallback : public osgViewer::OsgCameraGroup::RealizeCallback
  {

  public:
  LiveMap &liveMap;

  ContextCallback(LiveMap &liveMap);

  void operator()(osgViewer::OsgCameraGroup& cameraGroup, osgViewer::OsgSceneHandler& sceneHandler, const Producer::RenderSurface &renderSurface);

  };
*/

class PickHandler : public osgGA::GUIEventHandler
{

public:
    LiveMap &liveMap;

    PickHandler(LiveMap &liveMap);

    bool handle(const osgGA::GUIEventAdapter &eventAdapter, osgGA::GUIActionAdapter &actionAdapter);

};

class Manipulator : public osgGA::TrackballManipulator
{

public:

    virtual void home(const osgGA::GUIEventAdapter &eventAdapter, osgGA::GUIActionAdapter &actionAdapter)
    {
        (void)&eventAdapter;
        (void)&actionAdapter;
        //computePosition(homeEye, homeCenter, homeUp);
    }

    void setCameraData(const osg::Vec3 &eye, const osg::Vec3 &center, const osg::Vec3 &up)
    {
        homeEye = eye;
        homeCenter = center;
        homeUp = up;
        //computePosition(eye, center, up);
    }

protected:
    osg::Vec3 homeEye, homeCenter, homeUp;

};

class MapTexture
{

public:

    Coordinates vertexCoordinatesArray[4];
    MapTextureData *mapTextureData;
    const MapEntry *mapEntry;
    double xRatio, yRatio;
    double sumOfDistances;
    int verticalRasterIndex;
    int horizontalRasterIndex;
    uint32 width, height;
    uint32 *raster;

    MapTexture() : mapTextureData(NULL), mapEntry(NULL), xRatio(0.0), yRatio(0.0), sumOfDistances(0.0), verticalRasterIndex(0), horizontalRasterIndex(0), width(0), height(0), raster(NULL)
    {
    }

    MapTexture(const MapTexture &mapTexture) : mapTextureData(mapTexture.mapTextureData), mapEntry(mapTexture.mapEntry), xRatio(mapTexture.xRatio), yRatio(mapTexture.yRatio), sumOfDistances(mapTexture.sumOfDistances), verticalRasterIndex(mapTexture.verticalRasterIndex), horizontalRasterIndex(mapTexture.horizontalRasterIndex), width(mapTexture.width), height(mapTexture.height), raster(mapTexture.raster)
    {
        for (unsigned i = 0; i < LENGTH(vertexCoordinatesArray); i++)
            vertexCoordinatesArray[i] = mapTexture.vertexCoordinatesArray[i];
    }

    bool operator==(MapTexture &mapTexture)
    {
        return mapEntry && mapTexture.mapEntry &&
            mapEntry->filePath == mapTexture.mapEntry->filePath &&
            verticalRasterIndex == mapTexture.verticalRasterIndex &&
            horizontalRasterIndex == mapTexture.horizontalRasterIndex;
    }

    MapTexture *findIn(MAPTEXTURES &mapTextures);

};

class MapTextureData : public osg::Referenced
{

public:

    osg::ref_ptr<osg::Node> map;
    osg::ref_ptr<osg::StateSet> stateSet;
    uint32 *raster;

    MapTextureData() : map(NULL), stateSet(NULL), raster(NULL)
    {
    }

    MapTextureData(const MapTextureData &mapTextureData) : osg::Referenced(), map(mapTextureData.map), stateSet(mapTextureData.stateSet), raster(mapTextureData.raster)
    {
    }

};

class ObstacleData : public osg::Referenced
{

public:

    string name;
    string id;
    Coordinates position;
    double meanSeaLevel;
    string type;
    osg::ref_ptr<ObstacleData> nextObstacleData;

    ObstacleData() : id(""), meanSeaLevel(0.0), type("")
    {
    }

    ObstacleData(const ObstacleData &obstacleData) : osg::Referenced(), id(obstacleData.id), position(obstacleData.position), meanSeaLevel(obstacleData.meanSeaLevel), type(obstacleData.type)
    {
    }

};

class LineObstacleData : public osg::Referenced
{

public:

    LineObstacleData(ObstacleData *obstacleData1, ObstacleData *obstacleData2, double angle) : obstacleData1(obstacleData1), obstacleData2(obstacleData2), angle(angle)
    {
    }

    osg::ref_ptr<ObstacleData> obstacleData1;
    osg::ref_ptr<ObstacleData> obstacleData2;
    double angle;

};

class ObstacleGroup
{

public:

    Coordinates minPosition, maxPosition;
    ObstacleGroupData *obstacleGroupData;
    osg::ref_ptr<osg::Group> group;

    ObstacleGroup() : obstacleGroupData(NULL), group(NULL)
    {
    }

};

class ObstacleGroupData : public osg::Referenced
{

public:

    bool inView;

    ObstacleGroupData() : inView(false)
    {
    }

    ObstacleGroupData(const ObstacleGroupData &obstacleGroupData) : osg::Referenced(),
                                                                    inView(obstacleGroupData.inView)
    {
    }

    int findIndexIn(ObstacleGroupData *obstacleGroupDatas[]);

};

class TrackData : public osg::Referenced
{

public:

    string *filePath;
    time_t time;
    Coordinates coordinates;
    osg::ref_ptr<TrackData> nextTrackData;

};

class RootUpdateCallback : public osg::NodeCallback
{

public:

    LiveMap &liveMap;

    RootUpdateCallback(LiveMap &liveMap);

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor);

};

class MapUpdateCallback : public osg::NodeCallback
{

public:

    LiveMap &liveMap;

    MapUpdateCallback(LiveMap &liveMap);

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor);

};

class ObstacleGroupUpdateCallback : public osg::NodeCallback
{

public:

    LiveMap &liveMap;

    ObstacleGroupUpdateCallback(LiveMap &liveMap);

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor);

};

class VesselUpdateCallback : public osg::NodeCallback
{

public:

    LiveMap &liveMap;

    VesselUpdateCallback(LiveMap &liveMap);

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor);

};

class VesselZoneUpdateCallback : public osg::NodeCallback
{

public:

    LiveMap &liveMap;

    VesselZoneUpdateCallback(LiveMap &liveMap);

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor);

};

class ObstacleUpdateCallback : public osg::NodeCallback
{

public:

    LiveMap &liveMap;

    ObstacleUpdateCallback(LiveMap &liveMap);

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor);

};

class LineObstacleUpdateCallback : public osg::NodeCallback
{

public:

    LiveMap &liveMap;

    LineObstacleUpdateCallback(LiveMap &liveMap);

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor);

};

class InfoUpdateCallback : public osg::NodeCallback
{

public:

    LiveMap &liveMap;

    InfoUpdateCallback(LiveMap &liveMap);

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor);

};

class TrackUpdateCallback : public osg::NodeCallback
{

public:

    LiveMap &liveMap;

    TrackUpdateCallback(LiveMap &liveMap);

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor);

};

class DatabaseGenerateThread : public OpenThreads::Thread
{

public:

    DatabaseGenerateThread(LiveMap &liveMap);
    virtual ~DatabaseGenerateThread();
    virtual void run();

private:

    LiveMap &liveMap;

};

class GpsListenerThread : public OpenThreads::Thread
{

public:

    GpsListenerThread(LiveMap &liveMap);
    virtual ~GpsListenerThread();
    virtual void run();

private:

    LiveMap &liveMap;

};

class MapReaderThread : public OpenThreads::Thread
{

public:

    MapReaderThread(LiveMap &liveMap);
    virtual ~MapReaderThread();
    virtual void run();

private:

    LiveMap &liveMap;

};

class TrackShowThread : public OpenThreads::Thread
{

public:

    TrackShowThread(LiveMap &liveMap);
    virtual ~TrackShowThread();
    virtual void run();

private:

    LiveMap &liveMap;

};

class LiveMap
{

public:

    enum Layout {
        FULL, CENTER, UPPER_LEFT, UPPER_RIGHT, LOWER_LEFT, LOWER_RIGHT
    };

    NMEA nmea;
    MapIndex *mapIndex;
    PhotoIndex *photoIndex;
    PHOTOENTRIES::const_iterator photoEntryPair;
    MAPTEXTURES *mapTextures;
    MapTextureData **mapTextureDatas;
    const MapEntry *lastMapEntry;
    ofstream *nmeaOutStream;
    streambuf *nmeaBuffer;
    uint32 *raster;
    uint16 *elevationRaster;
    PaStream *audioSignalStream;
    OBSTACLEDATAS *obstacleDatas;
    LINEOBSTACLEDATAS *lineObstacleDatas;
    OBSTACLEGROUPS *obstacleGroups;
    ObstacleGroupData *obstacleGroupDatas[4];
    PhotoEntry *photoEntry;
    Layout layout;
    TrackData *trackData;
    TRACKSWITCHES trackSwitches;
    TRACKSWITCHES::iterator trackSwitchItem;
    osg::Group *trackGroup;
    osg::Switch *trackSwitch;
    osg::ref_ptr<Manipulator> manipulator;
    osg::ref_ptr<osg::Geode> photo;
    osg::ref_ptr<osg::Geode> photoPin;
    osg::ref_ptr<osg::Geode> blackObstacle;
    osg::ref_ptr<osg::Geode> redObstacle;
    osg::ref_ptr<osg::Geode> greenObstacle;
    osg::ref_ptr<osg::Geode> blueObstacle;
    osg::ref_ptr<osg::Group> mapGroup;
    osg::ref_ptr<osg::Group> showTrackGroup;
    osg::ref_ptr<osg::Group> singlePhotoGroup;
    osg::ref_ptr<osg::NodeCallback> obstacleUpdateCallback;
    osg::ref_ptr<osg::NodeCallback> lineObstacleUpdateCallback;
    osg::ref_ptr<osg::NodeCallback> obstacleGroupUpdateCallback;
    osg::ref_ptr<osg::Sequence> blackObstacleSequence;
    osg::ref_ptr<osg::Sequence> greenObstacleSequence;
    osg::ref_ptr<osg::Sequence> blueObstacleSequence;
    osg::ref_ptr<osg::Switch> photoGroupSwitch;
    osg::ref_ptr<osg::Switch> photoSwitch;
    osg::ref_ptr<osg::Switch> showTrackSwitch;
    osg::ref_ptr<osg::Switch> singlePhotoSwitch;
    osg::ref_ptr<osg::Vec3Array> lastTrackVertexArray;
    osg::ref_ptr<osg::Vec3Array> normalArray;
    osg::ref_ptr<osgViewer::ScreenCaptureHandler> screenCaptureHandler;
    osg::ref_ptr<osgViewer::Viewer> viewer;
    osg::ref_ptr<osgText::Font> font;
    osg::ref_ptr<ObstacleData> lastObstacleData;
    osg::ref_ptr<ObstacleData> firstObstacleData;
    osg::ref_ptr<ObstacleData> previousObstacleData;
    osg::ref_ptr<TrackData> firstTrackData;
    osg::ref_ptr<TrackData> previousTrackData;
    osg::Vec4ubArray *whiteArray;
    osg::Vec4 *black;
    osg::Vec4 *white;
    osg::Vec4 *red;
    osg::Vec4 *green;
    osg::Vec4 *blue;
    osg::Vec4 *gray;
    osg::Vec4 *orange;
    osg::Vec4 *darkBlue;
    osg::Vec3 cameraEye;
    Coordinates *obstaclePivotPosition;
    Coordinates vesselCoordinates;
    Coordinates lastVesselCoordinates;
    Coordinates angleVesselPosition;
    Coordinates gpsVesselPosition;
    Coordinates lastGpsVesselPosition;
    Coordinates lastRootVesselPosition;
    Coordinates lastTrackVesselPosition;
    Coordinates safetyPosition;
    Coordinates vesselPosition;
    Coordinates cameraPosition;
    bool allObstaclesInView;
    bool audioSignalStreamActive;
    bool cameraSet;
    bool countedAngle;
    bool disableAudio;
    bool disableGPS;
    bool disableMaps;
    bool disableObstacles;
    bool fixedNorth;
    bool followCamera;
    bool fullZoneCircle;
    bool generateDatabase;
    bool groundSpeedInKmh;
    bool hideMapsOnObstacles;
    bool inHazardZone;
    bool inSafetyZone;
    bool orthographicProjection;
    bool readingNMEA;
    bool recordNMEA;
    bool relocatingVessel;
    bool savingImage;
    bool savingPhotoShots;
    bool sceneryMode;
    bool showTracks;
    bool showingTracks;
    bool stopped;
    bool testing;
    bool useMapScales;
    bool elevationMap;
    bool photoInfoVisible;
    bool hasLastTrackVesselPosition;
    bool hasLastTrackVertexArray;
    double angleSeconds;
    double trackAngle;
    double lastTrackAngle;
    double gridConvergence;
    double hazardClearance;
    double hazardLevel;
    double vesselAngle;
    double vesselVelocity;
    double vesselZoneRadius;
    double safetyZoneRadius;
    double safetyHeight;
    double safetyMarginHours;
    double hazardZoneHours;
    double verticalScale;
    double trackTimeRate;
    double trackToleranceSecs;
    double scaleRamp;
    int mapSizePixels;
    int mapTextureAddCount;
    int mapTextureCount;
    int mapTextureSize;
    int obstacleGroupDataNumber;
    int projectionWidth, projectionHeight;
    int projectionX, projectionY;
    int scaleIndex, lastScaleIndex;
    int socketPort;
    time_t lastTrackTime;
    time_t trackTimeOffset;
    time_t timeOffset;
    long vesselPositionNumber;
    string dataDirectory;
    string databaseDirectory;
    string fontName;
    string mapDirectory;
    string photoDirectory;
    string photoShotDirectory;
    string NMEADirectory;
    string GPTDirectory;
    string GPTNotesDirectory;
    string obstacleFileName;
    string mpowerObstacleFileName;
    string serialPortName;
    string socketHost;
    DatabaseGenerateThread *databaseGenerateThread;
    GpsListenerThread *gpsListenerThread;
    MapReaderThread *mapReaderThread;
    TrackShowThread *trackShowThread;
    OpenThreads::Mutex mapReaderMutex;
    OpenThreads::Mutex trackShowMutex;
    OpenThreads::Mutex trackTimeMutex;
    OpenThreads::Condition mapReaderCondition;
    OpenThreads::Condition trackShowCondition;
    timeb angleTime;
    timeb gpsTime;

    LiveMap(osgViewer::Viewer *viewer);
    ~LiveMap() noexcept(false);
    void initialize();
    void readTrack(time_t time);
    void followTrack(TrackData *trackData);
    bool relocateVessel();
    void readMaps(MapEntry *selectedMapEntry);
    void setVesselAngle();
    void extrapolateVesselPosition();
    void showTrack(osg::Group &trackGroup, Coordinates vesselPosition1, Coordinates vesselPosition2);
    void updateRoot(osg::Node *node);
    void updateMap(osg::Node *node);
    void updateObstacleGroup(osg::Node *node);
    void updateVessel(osg::Node *node);
    void updateVesselZone(osg::Node *node);
    void checkObstacles(ObstacleData *obstacleData, LineObstacleData *lineObstacleData, bool inHazardZone, bool inSafetyZone, double clearance, double level);
    void updateObstacle(osg::Node *node);
    void updateLineObstacle(osg::Node *node);
    void updateInfo(osg::Node *node);
    void updateTrack(osg::Node *node);
    time_t timeOf(const string &dateOfFix, const string &timeOfFix);
    double parseCoordinate(string &degrees, string &minutes);
    void readProperties();
    void readObstacles(osg::MatrixTransform &rootTransform);
    void readMpowerObstacles(osg::MatrixTransform &rootTransform);
    void readObstacle(osg::MatrixTransform &rootTransform, ObstacleData *obstacleData);
    ObstacleGroup *search(const Coordinates &position);
    void readPhotos(osg::MatrixTransform &rootTransform);
    void readPhoto(PhotoEntry &photoEntry, osg::Group &photoGroup);
    inline bool isInHazardZone(ObstacleData &obstacleData, bool &inSafetyZone, double &clearance, double &level);
    inline bool isInHazardZone(LineObstacleData &lineObstacleData, bool &inSafetyZone, double &clearance, double &level);
    osg::MatrixTransform *showPhotoEntry(PhotoEntry *photoEntry, Coordinates &position);
    void showPhoto(PhotoEntry *photoEntry, Coordinates *position, osg::MatrixTransform *matrixTransform, bool hasLayout);
    void readJPEG(const string &filePath, Layout &layout, Coordinates *point);
    osg::ref_ptr<TrackData> readNMEA(const string &filePath);
    void readNMEAs(const string &dirPath, TrackUpdateCallback *trackUpdateCallback, osg::MatrixTransform &rootTransform);
    osg::ref_ptr<TrackData> readGPT(const string &filePath, bool trackNotes);
    void readGPTs(const string &dirPath, TrackUpdateCallback *trackUpdateCallback, osg::MatrixTransform &rootTransform, bool trackNotes);

    static inline double elevationToMeters(double elevation)
    {
        return elevation * 0.1;
    }

    static inline double feetToMeters(double feet)
    {
        return feet * 0.30480370641306998293099244086808;
    }

    static inline double knotsToKmh(double knots)
    {
        return knots * 1.8532;
    }

    static inline double powerOf2(double value)
    {
        return pow(2.0, (double)(long)(log10(value) / log10(2.0) + 0.5));
    }

    static bool mapTexturePredicate(const MapTexture *mapTexture1, const MapTexture *mapTexture2)
    {
        return mapTexture1->sumOfDistances < mapTexture2->sumOfDistances;
    }

    static bool mapTextureEntryPredicate(const MapTexture *mapTexture1, const MapTexture *mapTexture2)
    {
        return mapTexture1->mapEntry->filePath.compare(mapTexture2->mapEntry->filePath) < 0;
    }

#ifdef WIN32
    static locale loc;
    static const ctype<char> &facet;
#endif
    static const string emptyString;

};

class AAM : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class ALM : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class APB : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class BEC : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class BOD : public NMEASentence
{

public:

    LiveMap &liveMap;

    BOD(LiveMap &liveMap);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class BWC : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class BWR : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class GGA : public NMEASentence
{

public:

    LiveMap &liveMap;

    GGA(LiveMap &liveMap);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class GLL : public NMEASentence
{

public:

    LiveMap &liveMap;

    GLL(LiveMap &liveMap);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class GSA : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class GSV : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class RMA : public NMEASentence
{

public:

    LiveMap &liveMap;

    RMA(LiveMap &liveMap);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class RMB : public NMEASentence
{

public:

    LiveMap &liveMap;

    RMB(LiveMap &liveMap);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class RMC : public NMEASentence
{

public:

    LiveMap &liveMap;

    RMC(LiveMap &liveMap);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class RTE : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class VTG : public NMEASentence
{

public:

    LiveMap &liveMap;

    VTG(LiveMap &liveMap);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class WPL : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class ZDA : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class PGRME : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class PGRMM : public NMEASentence
{

public:

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class PGRMZ : public NMEASentence
{

public:

    LiveMap &liveMap;

    PGRMZ(LiveMap &liveMap);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

#endif
