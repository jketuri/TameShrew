
#ifndef _js_LiveView_hpp
#define _js_LiveView_hpp

#include <OpenThreads/Condition>
#include <OpenThreads/Mutex>
#include <OpenThreads/Thread>
#include <osg/CoordinateSystemNode>
#include <osg/Group>
#include <osg/Image>
#include <osg/ImageUtils>
#include <osg/NodeCallback>
#include <osg/Switch>
#include <osgART/Calibration>
#include <osgART/Foundation>
#include <osgART/GeometryUtils>
#include <osgART/MarkerCallback>
#include <osgART/PluginManager>
#include <osgART/TransformFilterCallback>
#include <osgART/Utils>
#include <osgART/VideoGeode>
#include <osgART/VideoLayer>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgEarth/GeoMath>
#include <osgEarth/MapNode>
#include <osgEarth/XmlUtils>
#include <osgEarth/Viewpoint>
#include <osgEarthAnnotation/AnnotationRegistry>
#include <osgEarthAnnotation/AnnotationData>
#include <osgEarthAnnotation/Decluttering>
#include <osgEarthDrivers/kml/KML>
#include <osgEarthDrivers/ocean_surface/OceanSurface>
#include <osgEarthSymbology/Color>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/LatLongFormatter>
#include <osgEarthUtil/MGRSFormatter>
#include <osgEarthUtil/MouseCoordsTool>
#include <osgEarthUtil/ObjectPlacer>
#include <osgEarthUtil/SkyNode>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgText/Text>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <iostream>
#include <vector>

#include "Datum.hpp"
#include "Garmin.hpp"
#include "NMEA.hpp"
#include "PlaybackVideo.hpp"
#include "SerialStream.hpp"
#include "SocketStream.hpp"
#include "USBFinder.hpp"
#include "USBStream.hpp"

using namespace common;
using namespace osg;
using namespace osgART;
using namespace osgGA;
using namespace osgDB;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;
using namespace osgEarth::Symbology;
using namespace osgEarth::Drivers;
using namespace osgEarth::Annotation;
using namespace osgViewer;
using namespace std;

class ImageData;
class LiveView;
class TrackData;
class VideoCallback;

typedef class Video *VIDEO;
typedef class VideoCallback *VIDEO_CALLBACK;

ostream& operator<<(ostream &out, const TrackData &trackData);

class ImageData
{

public:

    Video *video;
    string name;

};

class VideoCallback : public NodeCallback
{

public:

    VideoCallback(LiveView &liveView, Video *video, int videoIndex);

    virtual void operator()(Node *node, NodeVisitor *nodeVisitor);

    LiveView &liveView;
    Video *video;
    int videoIndex;
    osgDB::ofstream *videoListStream;
    Coordinates imageVesselCoordinates;
    bool imageVesselCoordinatesUpdated;

};

class EventHandler : public GUIEventHandler
{

public:
    LiveView &liveView;

    EventHandler(LiveView &liveView);

    bool handle(const GUIEventAdapter &eventAdapter, GUIActionAdapter &actionAdapter);

};

class GpsListenerThread : public OpenThreads::Thread
{

public:

    GpsListenerThread(LiveView &liveView);
    virtual ~GpsListenerThread();
    virtual void run();

private:

    LiveView &liveView;

};

class VesselUpdateCallback : public NodeCallback
{

public:

    LiveView &liveView;

    VesselUpdateCallback(LiveView &liveView);

    virtual void operator()(Node *node, NodeVisitor *nodeVisitor);

};

class LiveView
{

public:
    static enum Action {action_normal_zoom_level, action_change_tether_mode, action_change_view, action_change_video_view, action_write_video_snapshots, action_record_pause, action_play_pause, action_stop, action_cue, action_rewind};
    static const char *actionNames[];
    static const char *actionKeys[];

    NMEA nmea;
    osgDB::ofstream *nmeaOutStream;
    VIDEO *playbackVideos;
    VIDEO_CALLBACK *videoCallbacks;
    EarthManipulator *earthManipulator;
    Control *controlPanel;
    OceanSurfaceNode* ocean;
    SkyNode *sky;
    ObjectPlacer *objectPlacer;
    ref_ptr<Switch> modelSwitch;
    ref_ptr<Switch> videoModelSwitch;
    ref_ptr<Switch> playbackVideoModelSwitch;
    ref_ptr<Vec3Array> normalArray;
    ref_ptr<MatrixTransform> vesselTransform;
    ref_ptr<MatrixTransform> vesselAngleTransform;
    ref_ptr<KeySwitchMatrixManipulator> keyswitchManipulator;
    ref_ptr<VideoLayer> monitorVideoLayer;
    ref_ptr<VideoLayer> playbackMonitorVideoLayer;
    Vec4 *darkBlue;
    GpsListenerThread *gpsListenerThread;
    Coordinates angleVesselCoordinates;
    Coordinates gpsVesselCoordinates;
    Coordinates lastVesselCoordinates;
    Coordinates vesselCoordinates;
    bool countedAngle;
    bool disableAudio;
    bool disableGPS;
    bool fixedNorth;
    bool fullZoneCircle;
    bool garmin;
    bool groundSpeedInKmh;
    bool recordNMEA;
    bool sceneryMode;
    bool showTracks;
    bool showingTracks;
    bool stopped;
    bool testing;
    bool useMGRS;
    bool useDMS;
    bool useDD;
    bool useTracker;
    bool recordVideoImages;
    int socketPort;
    int _video_id;
    int _tracker_id;
    double angleSeconds;
    double lastTrackAngle;
    double vesselAngle;
    double vesselVelocity;
    double vesselZoneRadius;
    double trackAngle;
    double trackTimeRate;
    double trackToleranceSecs;
    string NMEADirectory;
    string dataDirectory;
    string databaseDirectory;
    string fontName;
    string gpsUsbDeviceGuid;
    string serialPortName;
    string socketHost;
    string vesselModel;
    unsigned int videoNumber;
    unsigned int viewNumber;
    unsigned int numberVideos;
    unsigned long usbPacketSize;
    uint16_t idVendor;
    uint16_t idProduct;
    streambuf *gpsBuffer;
    double garminSeconds;
    time_t garminTime;
    timeb angleTime;
    timeb gpsTime;

    void normalZoomLevel();
    void changeTetherMode();
    void changeView();
    void changeVideoView();
    void writeVideoSnapshots();
    void record();
    void play();
    void stop();
    void cue();
    void rewind();
    void addTexturedQuad(osgART::VideoGeode& geode,
                         float width, float height,
                         float x, float y);
    Group* createImageBackground(Image *video);
    Group* createImageBackground(Image *video, VideoLayer *layer,
                                 float x, float y);
    void setupVideo();
    int addVideo(osg::Group *group, const string &usbDevice, int videoIndex);
    void readProperties();
    void readNMEA(const string &filePath);
    void extrapolateVesselCoordinates();
    bool relocateVessel(bool extraPolate);
    void setVesselAngle();
    void writeVideoImage(Video *video, string name, int videoIndex);
    void flipVideoImages();
    void flipVideoImages(const string &dirPath);
    void flipVideoImage(const string &filePath);
    int run(int argc, char **argv);
    void setupKeySwitchMatrixManipulator(Viewer &viewer, ArgumentParser &arguments);
    void setupApplicationUsage(ArgumentParser &arguments);
    void setupNMEA();
    Node *getVessel();
    void setupVessel();
    void openVideoListStreams();
    void closeVideoListStreams();
    LiveView();
    ~LiveView();

    static inline double knotsToKmh(double knots)
    {
        return knots * 1.8532;
    }

    static inline double toSeconds(timeb &time1, timeb &time2)
    {
      return (double)time1.time - (double)time2.time +
        (double)(time1.millitm - time2.millitm) / 1000.0;
    }

};

class GGA : public NMEASentence
{

public:

    LiveView &liveView;

    GGA(LiveView &liveView);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class RMC : public NMEASentence
{

public:

    LiveView &liveView;

    RMC(LiveView &liveView);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class VTG : public NMEASentence
{

public:

    LiveView &liveView;

    VTG(LiveView &liveView);

    void sentenceData(string &id, DATAFIELDS &dataFields, string &sentence);

};

class LiveViewPlaybackVideo : public PlaybackVideo
{

public:

    LiveView &liveView;

    LiveViewPlaybackVideo(LiveView &liveView);

    bool relocateVessel(const Coordinates &coordinates);

};
#endif
