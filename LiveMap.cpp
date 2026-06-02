
#include "LiveMap.hpp"

static struct PORTAUDIOSAWTOOTHDATA
{
    float left_phase;
    float right_phase;
    int phaseCount;
    bool mutePeriod;
} portAudioSawToothData;

#ifdef WIN32
locale LiveMap::loc;
const ctype<char> &LiveMap::facet = use_facet<ctype<char> >(loc);
#endif
const string LiveMap::emptyString = "";

ostream& operator<<(ostream &out, const MapTexture &mapTexture)
{
    out << "MapTexture{";
    for (unsigned i = 0; i < LENGTH(mapTexture.vertexCoordinatesArray); i++)
        out << "vertexCoordinatesArray[" << i << "]=" <<
            mapTexture.vertexCoordinatesArray[i] << ",";
    out << "mapTextureData=";
    if (mapTexture.mapTextureData) {
        out << *mapTexture.mapTextureData;
    }
    else {
        out << "<NONE>";
    }
    out << ",mapEntry=";
    if (mapTexture.mapEntry) {
        out << *mapTexture.mapEntry;
    }
    else {
        out << "<NONE>";
    }
    out << ",sumOfDistances=" << mapTexture.sumOfDistances <<
        ",verticalRasterIndex=" << mapTexture.verticalRasterIndex <<
        ",horizontalRasterIndex=" << mapTexture.horizontalRasterIndex <<
        ",width=" << mapTexture.width <<
        ",height=" << mapTexture.height <<
        ",raster=" << mapTexture.raster << "}";
    return out;
}

ostream& operator<<(ostream &out, const MapTextureData &mapTextureData)
{
    out << "MapTextureData{";
    out << "map=" << mapTextureData.map.get() <<
        ",stateSet=" << mapTextureData.stateSet.get() << "}";
    return out;
}

ostream& operator<<(ostream &out, const MAPTEXTURES &mapTextures)
{
    out << "MAPTEXTURES{";
    MAPTEXTURES::const_iterator aMapTexture;
    for (aMapTexture = mapTextures.begin();
         aMapTexture != mapTextures.end();
         aMapTexture++)
        out << **aMapTexture << endl;
    out << "}";
    return out;
}

ostream& operator<<(ostream &out, const ObstacleData &obstacleData)
{
    out << "ObstacleData{id=" << obstacleData.id <<
        ",position=" << obstacleData.position <<
        ",meanSeaLevel=" << obstacleData.meanSeaLevel <<
        ",type=" << obstacleData.type << "}";
    return out;
}

ostream& operator<<(ostream &out, const ObstacleGroup &obstacleGroup)
{
    out << "ObstacleGroup{minPosition=" << obstacleGroup.minPosition <<
        ",maxPosition=" << obstacleGroup.maxPosition <<
        ",group.numChildren=" << obstacleGroup.group->getNumChildren() << "}";
    return out;
}

ostream& operator<<(ostream &out, const OBSTACLEGROUPS &obstacleGroups)
{
    out << "OBSTACLEGROUPS{";
    OBSTACLEGROUPS::const_iterator aObstacleGroup;
    for (aObstacleGroup = obstacleGroups.begin();
         aObstacleGroup != obstacleGroups.end();
         aObstacleGroup++)
        out << **aObstacleGroup << endl;
    out << "}";
    return out;
}

ostream& operator<<(ostream &out, const TrackData &trackData)
{
    out << "TrackData{time=" << trackData.time <<
        ",coordinates=" << trackData.coordinates <<
        ",nextTrackData=" << trackData.nextTrackData.get() << "}";
    return out;
}

/*
  ContextCallback::ContextCallback(LiveMap &liveMap) : liveMap(liveMap)
  {
  }

  void ContextCallback::operator()(osgViewer::OsgCameraGroup& cameraGroup, osgViewer::OsgSceneHandler& sceneHandler, const Producer::RenderSurface &renderSurface) {
  (void)&cameraGroup;
  (void)&renderSurface;
  if (!liveMap.mapTextureSize)
  liveMap.mapTextureSize = osg::Texture2D::getExtensions(0, true)->maxTextureSize() / 4;
  sceneHandler.init();
  }
*/

RealizeOperation::RealizeOperation(LiveMap &liveMap, osgViewer::Viewer &viewer) : Operation("RealizeOperation", false), liveMap(liveMap), viewer(viewer)
{
}

void RealizeOperation::operator()(osg::Object *) {
    if (!liveMap.mapTextureSize) {
//        liveMap.mapTextureSize = osg::Texture2D::getExtensions(0, true)->maxTextureSize() / 4;
//        liveMap.mapTextureSize = osg::Texture2D::getState()->extensions->maxTextureSize / 4;
//        const osg::GLExtensions* extensions = osg::Texture2D::state.get<osg::GLExtensions>();
//        liveMap.mapTextureSize = extensions->maxTextureSize / 4;
    }
    liveMap.initialize();
    /*
      EventHandlers &eventHandlers = viewer.getEventHandlers();
      for (EventHandlers::const_iterator eventHandler = eventHandlers.begin();
      eventHandler != eventHandlers.end(); eventHandler++) {
      }
    */
}

PickHandler::PickHandler(LiveMap &liveMap) : liveMap(liveMap)
{
}

bool PickHandler::handle(const osgGA::GUIEventAdapter &eventAdapter, osgGA::GUIActionAdapter &actionAdapter)
{
    (void)&actionAdapter;
    bool hasLayout = false;
    PhotoEntry *photoEntry = NULL;
    osg::MatrixTransform *matrixTransform = NULL;
    Coordinates position(eventAdapter.getXnormalized(), eventAdapter.getYnormalized(), 0.0);
    switch (eventAdapter.getEventType()) {
    case osgGA::GUIEventAdapter::FRAME: {
        if (liveMap.savingPhotoShots) {
            if (liveMap.photoEntryPair != liveMap.photoIndex->photoEntries.begin()) {
                if (liveMap.savingImage) {
                    rename("saved_image.jpg", (liveMap.photoShotDirectory + FILE_SEPARATOR + liveMap.photoEntry->fileName).c_str());
                    liveMap.savingImage = false;
                    char buffer[128], value[128];
                    ofstream outStream((liveMap.photoShotDirectory + FILE_SEPARATOR + liveMap.photoEntry->fileName.substr(0, liveMap.photoEntry->fileName.find(".", 0) + 1) + "txt").c_str());
                    sprintf(buffer, "%-20.20s: %s", "Filename", liveMap.photoEntry->fileName.c_str());
                    outStream << buffer << endl;
                    sprintf(buffer, "%-20.20s: %lf %c", "Latitude", liveMap.photoEntry->coordinates.y, liveMap.photoEntry->coordinates.y >= 0.0 ? 'N' : 'S');
                    outStream << buffer << endl;
                    sprintf(buffer, "%-20.20s: %lf %c", "Longitude", liveMap.photoEntry->coordinates.x, liveMap.photoEntry->coordinates.x >= 0.0 ? 'E' : 'W');
                    outStream << buffer << endl;
                    list<ExifEntry *>::iterator exifEntryItem = liveMap.photoEntry->exifEntries.begin();
                    while (exifEntryItem != liveMap.photoEntry->exifEntries.end()) {
                        ExifEntry *exifEntry = *exifEntryItem++;
                        exif_entry_get_value(exifEntry, value, 59);
                        string valueString(value);
                        valueString = Support::trim(valueString);
                        if (valueString == "") {
                            continue;
                        }
                        sprintf(buffer, "%-20.20s: %-58.58s", exif_tag_get_title(exifEntry->tag), valueString.c_str());
                        outStream << buffer << endl;
                    }
                    outStream << endl;
                    outStream.close();
                } else if (!liveMap.relocatingVessel) {
                    liveMap.screenCaptureHandler->captureNextFrame(*liveMap.viewer);
                    /*
                      ((osgViewer::ViewerEventHandler *)liveMap.viewer->getEventHandlerList().back().get())->setWriteImageOnNextFrame(true);
                    */
                    liveMap.savingImage = true;
                }
            }
            if (!liveMap.relocatingVessel && !liveMap.savingImage) {
                if (liveMap.photoEntryPair != liveMap.photoIndex->photoEntries.end()) {
                    if (liveMap.photoGroupSwitch->getValue(0)) {
                        liveMap.photoGroupSwitch->setValue(0, false);
                        hasLayout = true;
                    }
                    photoEntry = liveMap.photoEntryPair->second;
                    matrixTransform = liveMap.showPhotoEntry(photoEntry, position);
                    liveMap.showPhoto(photoEntry, NULL, matrixTransform, hasLayout);
                    hasLayout = false;
                    liveMap.photoEntryPair++;
                } else {
                    if (!liveMap.photoGroupSwitch->getValue(0)) {
                        liveMap.photoGroupSwitch->setValue(0, true);
                    }
                    if (liveMap.singlePhotoSwitch->getValue(0)) {
                        liveMap.singlePhotoSwitch->setValue(0, false);
                    }
                    liveMap.savingPhotoShots = false;
                    if (liveMap.photoSwitch->getValue(0)) {
                        liveMap.photoSwitch->setValue(0, false);
                    }
                }
            }
        }
        osg::Matrix matrix = liveMap.manipulator->getMatrix();
        osg::Vec3 eye, center, up;
        matrix.getLookAt(eye, center, up);
        if (liveMap.cameraEye == eye) {
            break;
        }
        liveMap.cameraEye = eye;
        Coordinates position = liveMap.vesselPosition;
        if (liveMap.fixedNorth) {
            position.x -= eye.x();
            position.y -= eye.y();
        } else {
            Coordinates cameraPosition(-eye.x(), -eye.y(), 0.0);
            double angle = Coordinates::origo.angle(cameraPosition) + liveMap.trackAngle - osg::PI / 2.0 + osg::PI,
                distance = Coordinates::origo.clearance(cameraPosition);
            position.x += cos(angle) * distance;
            position.y += sin(angle) * distance;
        }
        liveMap.cameraPosition = position;
        liveMap.followCamera = true;
        liveMap.mapReaderCondition.broadcast();
        break;
    }
    case osgGA::GUIEventAdapter::KEYDOWN: {
        if (liveMap.photoIndex) {
            switch (eventAdapter.getKey()) {
            case osgGA::GUIEventAdapter::KEY_BackSpace:
                liveMap.stopped = true;
                break;
            case 'a':
                liveMap.photoEntryPair = liveMap.photoIndex->photoEntries.begin();
                liveMap.savingImage = false;
                liveMap.savingPhotoShots = true;
                break;
            case 'c':
            case 'd':
            case 'e': {
                if (eventAdapter.getKey() == 'c' || liveMap.showingTracks) {
                    liveMap.showingTracks = !liveMap.showingTracks;
                    TRACKSWITCHES::iterator trackGroupItem = liveMap.trackSwitches.begin();
                    while (trackGroupItem != liveMap.trackSwitches.end()) {
                        osg::Switch *trackSwitch = *trackGroupItem++;
                        if (trackSwitch->getValue(0) ^ liveMap.showingTracks)
                            trackSwitch->setValue(0, liveMap.showingTracks);
                    }
                    if (eventAdapter.getKey() == 'c') break;
                }
                if (liveMap.trackSwitch && liveMap.trackSwitch->getValue(0))
                    liveMap.trackSwitch->setValue(0, false);
                if (eventAdapter.getKey() == 'd') {
                    if (liveMap.trackSwitchItem == liveMap.trackSwitches.end())
                        liveMap.trackSwitchItem = liveMap.trackSwitches.begin();
                    if (liveMap.trackSwitchItem != liveMap.trackSwitches.end()) {
                        liveMap.trackSwitch = *liveMap.trackSwitchItem;
                        liveMap.trackSwitchItem++;
                    } else liveMap.trackSwitch = NULL;
                }
                if (eventAdapter.getKey() == 'e') {
                    if (liveMap.trackSwitchItem == liveMap.trackSwitches.begin())
                        liveMap.trackSwitchItem = liveMap.trackSwitches.end();
                    if (liveMap.trackSwitchItem != liveMap.trackSwitches.begin()) {
                        liveMap.trackSwitchItem--;
                        liveMap.trackSwitch = *liveMap.trackSwitchItem;
                    } else liveMap.trackSwitch = NULL;
                }
                if (liveMap.trackSwitch) {
                    TrackData *trackData = (TrackData *)liveMap.trackSwitch->getUserData();
                    liveMap.vesselCoordinates = trackData->coordinates;
                    liveMap.relocateVessel();
                    if (!liveMap.trackSwitch->getValue(0))
                        liveMap.trackSwitch->setValue(0, true);
                }
                break;
            }
            case 'n':
                if (liveMap.photoEntryPair == liveMap.photoIndex->photoEntries.end())
                    liveMap.photoEntryPair = liveMap.photoIndex->photoEntries.begin();
                if (liveMap.photoEntryPair != liveMap.photoIndex->photoEntries.end()) {
                    photoEntry = liveMap.photoEntryPair->second;
                    liveMap.photoEntryPair++;
                }
                break;
            case 'p':
                if (liveMap.photoEntryPair == liveMap.photoIndex->photoEntries.begin())
                    liveMap.photoEntryPair = liveMap.photoIndex->photoEntries.end();
                if (liveMap.photoEntryPair != liveMap.photoIndex->photoEntries.begin()) {
                    liveMap.photoEntryPair--;
                    photoEntry = liveMap.photoEntryPair->second;
                }
                break;
            case 'x':
                if (!liveMap.photoGroupSwitch->getValue(0))
                    liveMap.photoGroupSwitch->setValue(0, true);
                if (liveMap.singlePhotoSwitch->getValue(0))
                    liveMap.singlePhotoSwitch->setValue(0, false);
            default:
                break;
            }
        }
        if (photoEntry) {
            if (liveMap.photoGroupSwitch->getValue(0)) {
                liveMap.photoGroupSwitch->setValue(0, false);
                hasLayout = true;
            }
            matrixTransform = liveMap.showPhotoEntry(photoEntry, position);
        } else break;
    }
    case osgGA::GUIEventAdapter::PUSH: {
        bool hasPosition = false;
        if (!photoEntry) {
            bool searchTrack = (eventAdapter.getButtonMask() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) != 0;
            string searchName = searchTrack ? "Track" : "Photo";
            /*
            osgUtil::IntersectVisitor::HitList hlist;
            osgUtil::IntersectVisitor::HitList::iterator hitr;
            if (!liveMap.viewer->computeIntersections(eventAdapter.getX(), eventAdapter.getY(), hlist)) hitr = hlist.end();
            else for (intersection = .begin(); hitr != hlist.end(); hitr++)
                     if (hitr->_geode.valid() && !hitr->_geode->getName().empty() && hitr->_geode->getName() == searchName) break;
            if (hitr == hlist.end()) {
                if (liveMap.photoSwitch->getValue(0))
                    liveMap.photoSwitch->setValue(0, false);
                break;
            }
            */
            osgUtil::LineSegmentIntersector::Intersections intersections;
            osgUtil::LineSegmentIntersector::Intersections::iterator intersection;
            if (!liveMap.viewer->computeIntersections(eventAdapter.getX(), eventAdapter.getY(), intersections)) {
                intersection = intersections.end();
            } else {
                for (intersection = intersections.begin(); intersection != intersections.end(); intersection++) {
                    if (intersection->drawable.valid() && !intersection->drawable->getName().empty() && intersection->drawable->getName() == searchName) {
                        break;
                    }
                }
            }
            if (intersection == intersections.end()) {
                if (liveMap.photoSwitch->getValue(0)) {
                    liveMap.photoSwitch->setValue(0, false);
                }
                break;
            }
            osg::NodePath nodePath = intersection->nodePath;
            if (searchTrack) {
                /*
                  for (osg::NodePath::iterator node = nodePath.begin();
                  node != nodePath.end(); ++node)
                  cout << "node=" << *node << "className=" << (*node)->className() << endl;
                */
                osg::Switch &trackSwitch = *(osg::Switch *)nodePath.at(nodePath.size() - 3);
                TrackData *trackData = (TrackData *)trackSwitch.getUserData();
                if (!trackData) {
                    break;
                }
                if (trackSwitch.getValue(0)) {
                    trackSwitch.setValue(0, false);
                }
                if (liveMap.showTrackSwitch->getValue(0)) {
                    liveMap.showTrackSwitch->setValue(0, false);
                }
                liveMap.showTrackGroup->removeChild(0, liveMap.showTrackGroup->getNumChildren());
                liveMap.trackData = trackData;
                if (!liveMap.showTrackSwitch->getValue(0)) {
                    liveMap.showTrackSwitch->setValue(0, true);
                }
                liveMap.trackShowCondition.broadcast();
                break;
            }
            matrixTransform = (osg::MatrixTransform *)nodePath.at(nodePath.size() - 2);
            photoEntry = (PhotoEntry *)matrixTransform->getUserData();
            position.x = (position.x + 1.0) / 2.0 * (double)liveMap.projectionWidth;
            position.y = (position.y + 1.0) / 2.0 * (double)liveMap.projectionHeight;
            hasLayout = hasPosition = true;
        }
        liveMap.showPhoto(photoEntry, hasPosition ? &position : NULL, matrixTransform, hasLayout);
        break;
    }
    default:
        break;
    }
    return false;
}

MapTexture *MapTexture::findIn(MAPTEXTURES &mapTextures)
{
    MAPTEXTURES::iterator aMapTexture;
    for (aMapTexture = mapTextures.begin();
         aMapTexture != mapTextures.end();
         aMapTexture++) {
        MapTexture &mapTexture = **aMapTexture;
        if (mapTexture == *this) {
            return &mapTexture;
        }
    }
    return NULL;
}

int ObstacleGroupData::findIndexIn(ObstacleGroupData *obstacleGroupDatas[])
{
    int obstacleGroupDataIndex;
    for (obstacleGroupDataIndex = 0; obstacleGroupDataIndex < 4; obstacleGroupDataIndex++) {
        if (obstacleGroupDatas[obstacleGroupDataIndex] == this) {
            return obstacleGroupDataIndex;
        }
    }
    return -1;
}

RootUpdateCallback::RootUpdateCallback(LiveMap &liveMap) : liveMap(liveMap)
{
}

void RootUpdateCallback::operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor)
{
    liveMap.updateRoot(node);
    traverse(node, nodeVisitor);
}

MapUpdateCallback::MapUpdateCallback(LiveMap &liveMap) : liveMap(liveMap)
{
}

void MapUpdateCallback::operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor)
{
    liveMap.updateMap(node);
    traverse(node, nodeVisitor);
}

ObstacleGroupUpdateCallback::ObstacleGroupUpdateCallback(LiveMap &liveMap) : liveMap(liveMap)
{
}

void ObstacleGroupUpdateCallback::operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor)
{
    liveMap.updateObstacleGroup(node);
    traverse(node, nodeVisitor);
}

VesselUpdateCallback::VesselUpdateCallback(LiveMap &liveMap) : liveMap(liveMap)
{
}

void VesselUpdateCallback::operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor)
{
    liveMap.updateVessel(node);
    traverse(node, nodeVisitor);
}

VesselZoneUpdateCallback::VesselZoneUpdateCallback(LiveMap &liveMap) : liveMap(liveMap)
{
}

void VesselZoneUpdateCallback::operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor)
{
    liveMap.updateVesselZone(node);
    traverse(node, nodeVisitor);
}

ObstacleUpdateCallback::ObstacleUpdateCallback(LiveMap &liveMap) : liveMap(liveMap)
{
}

void ObstacleUpdateCallback::operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor)
{
    liveMap.updateObstacle(node);
    traverse(node, nodeVisitor);
}

LineObstacleUpdateCallback::LineObstacleUpdateCallback(LiveMap &liveMap) : liveMap(liveMap)
{
}

void LineObstacleUpdateCallback::operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor)
{
    liveMap.updateLineObstacle(node);
    traverse(node, nodeVisitor);
}

InfoUpdateCallback::InfoUpdateCallback(LiveMap &liveMap) : liveMap(liveMap)
{
}

void InfoUpdateCallback::operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor)
{
    liveMap.updateInfo(node);
    traverse(node, nodeVisitor);
}

TrackUpdateCallback::TrackUpdateCallback(LiveMap &liveMap) : liveMap(liveMap)
{
}

void TrackUpdateCallback::operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor)
{
    liveMap.updateTrack(node);
    traverse(node, nodeVisitor);
}

// Waypoint Arrival Time
void AAM::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

// GPS Almanac Data
void ALM::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

// Autopilot Sentence "B"
void APB::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

// Bearing & Distance to Waypoint - Dead Reckoning
void BEC::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

BOD::BOD(LiveMap &liveMap) : liveMap(liveMap)
{
}

// Bearing - Origin to Destination
void BOD::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&sentence;
#ifdef DEBUG
    cout << "BOD" << endl;
    cout << "size=" << dataFields.size() << endl;
    cout << "dataFields=" << dataFields << endl;
#endif
    (void)&id;
    if (dataFields.size() < 6 ||
        dataFields[0].length() < 1 ||
        dataFields[2].length() < 1) {
        return;
    }
    /*
      double trueBearing = atof(dataFields[0].c_str()),
      magneticBearing = atof(dataFields[2].c_str());
      string destinationWaypointID = dataFields[4],
      originWaypointID = dataFields[5];
    */
}

// Bearing & Distance to Waypoint - Great Circle
void BWC::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
#ifdef DEBUG
    cout << "BWC" << endl;
#endif
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

// Bearing & Distance to Waypoint - Rhumb Line
void BWR::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
#ifdef DEBUG
    cout << "BWR" << endl;
#endif
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

GGA::GGA(LiveMap &liveMap) : liveMap(liveMap)
{
}

// Global Positioning System Fix Data
void GGA::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&sentence;
#ifdef DEBUG
    cout << "GGA" << endl;
#endif
    if (dataFields.size() < 9 ||
        dataFields[3].length() < 4 || dataFields[4].length() < 1 ||
        dataFields[1].length() < 3 || dataFields[2].length() < 1 ||
        dataFields[8].length() < 1) {
        return;
    }
    string timeOfFix = dataFields[0];
    liveMap.vesselCoordinates.x = parseLongitude(dataFields[3], dataFields[4]);
    liveMap.vesselCoordinates.y = parseLatitude(dataFields[1], dataFields[2]);
    liveMap.vesselCoordinates.z = atof(dataFields[8].c_str());
    if (liveMap.readingNMEA) {
        liveMap.readTrack(liveMap.timeOf(LiveMap::emptyString, timeOfFix));
    }
    else {
        if (liveMap.relocateVessel() && liveMap.recordNMEA) {
            *liveMap.nmeaOutStream << sentence << endl;
        }
    }
#ifdef DEBUG
    cout << "GGA done" << endl;
#endif
}

GLL::GLL(LiveMap &liveMap) : liveMap(liveMap)
{
}

// Geographic Position, Latitude and Longitude
void GLL::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&sentence;
#ifdef DEBUG
    cout << "GLL" << endl;
    cout << "size=" << dataFields.size() << endl;
    cout << "dataFields=" << dataFields << endl;
#endif
    if (dataFields.size() < 4 ||
        dataFields[2].length() < 4 || dataFields[3].length() < 1 ||
        dataFields[0].length() < 3 || dataFields[1].length() < 1) {
        return;
    }
    (void)&id;
    string timeOfFix = dataFields.size() > 4 ? dataFields[4] : "";
    liveMap.vesselCoordinates.x = parseLongitude(dataFields[2], dataFields[3]);
    liveMap.vesselCoordinates.y = parseLatitude(dataFields[0], dataFields[1]);
    char status = dataFields.size() > 5 && dataFields[5].length() > 0 ?
        toupper(dataFields[5][0]) : 0;
    (void)&status;
#ifdef DEBUG
    cout << "GLL vesselCoordinates=" << liveMap.vesselCoordinates << endl;
#endif
    if (liveMap.readingNMEA) {
        liveMap.readTrack(liveMap.timeOf(LiveMap::emptyString, timeOfFix));
    }
    else {
        if (liveMap.relocateVessel() && liveMap.recordNMEA) {
            *liveMap.nmeaOutStream << sentence << endl;
        }
    }
#ifdef DEBUG
    cout << "GLL done" << endl;
#endif
}

// GPS DOP and Active Satellites
void GSA::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

// GPS Satellites in View
void GSV::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

RMA::RMA(LiveMap &liveMap) : liveMap(liveMap)
{
}

// Recommended minimum navigation information
void RMA::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&sentence;
#ifdef DEBUG
    cout << "RMA" << endl;
    cout << "size=" << dataFields.size() << endl;
    cout << "dataFields=" << dataFields << endl;
#endif
    (void)&id;
    if (dataFields.size() < 12 ||
        dataFields[8].length() < 1) {
        return;
    }
}

RMB::RMB(LiveMap &liveMap) : liveMap(liveMap)
{
}

// Recommended minimum navigation information
void RMB::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&sentence;
#ifdef DEBUG
    cout << "RMB" << endl;
    cout << "size=" << dataFields.size() << endl;
    cout << "dataFields=" << dataFields << endl;
#endif
    if (dataFields.size() < 13 ||
        dataFields[10].length() < 1) {
        return;
    }
    string status = dataFields[0];
    if (status != "A") {
        return;
    }
    (void)&id;
    /*
      steerToCorrect = dataFields[2],
      originWaypointID = dataFields[3], destinationWaypointID = dataFields[4],
      arrivalAlarm = dataFields[12];
      double crossTrackError = atof(dataFields[1].c_str()),
      destinationWaypointLatitude = atof(dataFields[5].c_str()),
      destinationWaypointLongitude = atof(dataFields[7].c_str()),
      rangeToDestination = atof(dataFields[9].c_str()),
      velocityTowardsDestinationKmh = liveMap.knotsToKmh(atof(dataFields[11].c_str()));
    */
    double trueBearing = Support::toRadians(atof(dataFields[10].c_str()));
#ifdef DEBUG
    cout << "RMB bearing=" << trueBearing << endl;
#endif
}

RMC::RMC(LiveMap &liveMap) : liveMap(liveMap)
{
}

// Recommended Minimum Specific GPS/Transit Data
void RMC::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&sentence;
#ifdef DEBUG
    cout << "RMC" << endl;
    cout << "size=" << dataFields.size() << endl;
    cout << "dataFields=" << dataFields << endl;
#endif
    if (dataFields.size() < 9 ||
        dataFields[4].length() < 4 || dataFields[5].length() < 1 ||
        dataFields[2].length() < 3 || dataFields[3].length() < 1 ||
        dataFields[6].length() < 1) {
        return;
    }
    (void)&id;
    string timeOfFix = dataFields[0];
    liveMap.vesselCoordinates.x = parseLongitude(dataFields[4], dataFields[5]);
    liveMap.vesselCoordinates.y = parseLatitude(dataFields[2], dataFields[3]);
    liveMap.vesselVelocity = liveMap.groundSpeedInKmh ? atof(dataFields[6].c_str()) :
        liveMap.knotsToKmh(atof(dataFields[6].c_str()));
    if (!liveMap.countedAngle) {
        double trackAngle = osg::PI / 2.0 - Support::toRadians(atof(dataFields[7].c_str())) - liveMap.gridConvergence;
        if (trackAngle < 0.0) {
            trackAngle += 2.0 * osg::PI;
        }
        liveMap.lastTrackAngle = liveMap.trackAngle;
        liveMap.trackAngle = trackAngle;
    }
    string dateOfFix = dataFields[8];
#ifdef DEBUG
    cout << "RMC: liveMap.vesselVelocity=" << liveMap.vesselVelocity << endl;
#endif
    if (liveMap.readingNMEA) {
        liveMap.readTrack(liveMap.timeOf(dateOfFix, timeOfFix));
    }
    else {
        if (liveMap.relocateVessel() && liveMap.recordNMEA)
            *liveMap.nmeaOutStream << sentence << endl;
    }
}

// Waypoints in active route
void RTE::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

VTG::VTG(LiveMap &liveMap) : liveMap(liveMap)
{
}

// Track Made Good and Ground Speed
void VTG::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&sentence;
#ifdef DEBUG
    cout << "VTG" << endl;
    cout << "size=" << dataFields.size() << endl;
    cout << "dataFields=" << dataFields << endl;
#endif
    if (dataFields.size() < 8 ||
        dataFields[0].length() < 1 ||
        dataFields[2].length() < 1 ||
        dataFields[6].length() < 1) {
        return;
    }
    (void)&id;
    double trueBearing = Support::toRadians(atof(dataFields[0].c_str())),
        magneticTrackMadeGood = atof(dataFields[2].c_str());
    (void)&trueBearing;
    (void)&magneticTrackMadeGood;
    liveMap.vesselVelocity = liveMap.groundSpeedInKmh ? atof(dataFields[6].c_str()) : liveMap.knotsToKmh(atof(dataFields[6].c_str()));
#ifdef DEBUG
    cout << "VTG: liveMap.vesselVelocity=" << liveMap.vesselVelocity << endl;
#endif
}

// Waypoint location
void WPL::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

// Time & Date
void ZDA::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

// Garmin
void PGRME::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

// Currently active horizontal datum
void PGRMM::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&id;
    (void)&dataFields;
    (void)&sentence;
}

PGRMZ::PGRMZ(LiveMap &liveMap) : liveMap(liveMap)
{
}

void PGRMZ::sentenceData(string &id, DATAFIELDS &dataFields, string &sentence)
{
    (void)&sentence;
#ifdef DEBUG
    cout << "PGRMZ" << endl;
    cout << "size=" << dataFields.size() << endl;
    cout << "dataFields=" << dataFields << endl;
#endif
    if (dataFields.size() < 2) {
        return;
    }
    (void)&id;
    liveMap.vesselCoordinates.z = liveMap.feetToMeters(atof(dataFields[0].c_str()));
}

static int portAudioSawToothCallback(const void *inputBuffer, void *outputBuffer,
                                     unsigned long frameCount,
                                     const PaStreamCallbackTimeInfo *timeInfo,
                                     PaStreamCallbackFlags statusFlags,
                                     void *userData)
{
    /* Cast data passed through stream to our structure. */
    PORTAUDIOSAWTOOTHDATA *data = (PORTAUDIOSAWTOOTHDATA *)userData;
    float *out = (float*)outputBuffer;
    /* Prevent unused variable warnings. */
    (void)timeInfo;
    (void)statusFlags;
    (void)inputBuffer;
    unsigned long i;
    for (i = 0; i < frameCount; i++) {
        if (data->mutePeriod) {
            *out++ = 0.0;
            *out++ = 0.0;
        }
        else {
            *out++ = data->left_phase;  /* left */
            *out++ = data->right_phase;  /* right */
        }
        /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
        data->left_phase += 0.01f;
        /* When signal reaches top, drop back down. */
        if (data->left_phase >= 1.0f) {
            data->left_phase -= 2.0f;
            data->phaseCount++;
        }
        /* higher pitch so we can distinguish left and right. */
        data->right_phase += 0.03f;
        if (data->right_phase >= 1.0f) {
            data->right_phase -= 2.0f;
        }
        if (data->phaseCount > 100) {
            data->phaseCount = 0;
            data->mutePeriod = !data->mutePeriod;
        }
    }
    return 0;
}

DatabaseGenerateThread::DatabaseGenerateThread(LiveMap &liveMap) : liveMap(liveMap)
{
}

DatabaseGenerateThread::~DatabaseGenerateThread()
{
}

void DatabaseGenerateThread::run()
{
    try {
        for (MAPENTRIES::const_iterator aMapEntry = liveMap.mapIndex->mapEntries[0].begin();
             aMapEntry != liveMap.mapIndex->mapEntries[0].end();
             aMapEntry++) {
            MapEntry &mapEntry = **aMapEntry;
            cout << "[" << '0' << "] " << mapEntry << endl;
            liveMap.readMaps(&mapEntry);
        }
        liveMap.viewer->setDone(true);
    }
    catch (string *message) {
        cout << *message << endl;
        delete message;
        return;
    }
}

GpsListenerThread::GpsListenerThread(LiveMap &liveMap) : liveMap(liveMap)
{
}

GpsListenerThread::~GpsListenerThread()
{
}

void GpsListenerThread::run()
{
    try {
        iostream nmeaStream(liveMap.nmeaBuffer);
        liveMap.nmea.readInputStream(nmeaStream, NULL);
    }
    catch (string *message) {
        cout << *message << endl;
        delete message;
        return;
    }
}

MapReaderThread::MapReaderThread(LiveMap &liveMap) : liveMap(liveMap)
{
}

MapReaderThread::~MapReaderThread()
{
}

void MapReaderThread::run()
{
    try {
        for (;;) {
            /*
              struct timespec timespec;
              timeb currentTime;
              ftime(&currentTime);
              timespec.tv_sec = delay / 1000L + currentTime.time;
              timespec.tv_nsec = (delay % 1000L + currentTime.millitm) * 1000000L;
            */
            liveMap.mapReaderCondition.wait(&liveMap.mapReaderMutex);
            liveMap.readMaps(NULL);
            YieldCurrentThread();
        }
    }
    catch (string *message) {
        cout << *message << endl;
        delete message;
        return;
    }
}

TrackShowThread::TrackShowThread(LiveMap &liveMap) : liveMap(liveMap)
{
}

TrackShowThread::~TrackShowThread()
{
}

void TrackShowThread::run()
{
    try {
        for (;;) {
            liveMap.trackShowCondition.wait(&liveMap.trackShowMutex);
            TrackData *trackData = liveMap.trackData;
            cout << "filePath=" << *trackData->filePath << endl;
            while (trackData) {
                if (liveMap.stopped) {
                    liveMap.stopped = false;
                    break;
                }
                liveMap.followTrack(trackData);
                trackData = trackData->nextTrackData.get();
            }
            liveMap.trackData = NULL;
            YieldCurrentThread();
        }
    }
    catch (string *message) {
        cout << *message << endl;
        delete message;
        return;
    }
}

LiveMap::LiveMap(osgViewer::Viewer *viewer) : nmea(183), mapIndex(NULL), photoIndex(NULL), lastMapEntry(NULL), nmeaOutStream(NULL), nmeaBuffer(NULL), raster(NULL), elevationRaster(NULL), photoEntry(NULL), layout(LiveMap::FULL), trackData(NULL), trackGroup(NULL), trackSwitch(NULL), lastObstacleData(NULL), firstObstacleData(NULL), previousObstacleData(NULL), firstTrackData(NULL), previousTrackData(NULL), obstaclePivotPosition(NULL), allObstaclesInView(false), audioSignalStreamActive(false), cameraSet(false), countedAngle(false), disableAudio(false), disableGPS(false), disableMaps(false), disableObstacles(false), fixedNorth(false), followCamera(false), fullZoneCircle(false), generateDatabase(false), groundSpeedInKmh(false), hideMapsOnObstacles(false), inHazardZone(false), inSafetyZone(false), orthographicProjection(false), readingNMEA(false), recordNMEA(false), relocatingVessel(false), savingImage(false), savingPhotoShots(false), sceneryMode(false), showTracks(false), showingTracks(false), stopped(false), testing(false), useMapScales(false), elevationMap(false), photoInfoVisible(false), hasLastTrackVesselPosition(false), hasLastTrackVertexArray(false), angleSeconds(0.0), trackAngle(osg::PI / 2.0), lastTrackAngle(trackAngle), gridConvergence(0.0), hazardClearance(0.0), hazardLevel(0.0), vesselAngle(0.0), vesselVelocity(0.0), vesselZoneRadius(0.0), safetyZoneRadius(0.0), safetyHeight(100.0), safetyMarginHours(10.0 / 60.0 / 60.0), hazardZoneHours(1.0 / 60.0), verticalScale(1.0), trackTimeRate(1.0), trackToleranceSecs(10.0), mapSizePixels(0), mapTextureAddCount(4), mapTextureCount(36), mapTextureSize(1024), obstacleGroupDataNumber(0), scaleIndex(0), lastScaleIndex(0), vesselPositionNumber(0L), gpsListenerThread(NULL), mapReaderThread(NULL), trackShowThread(NULL)
{
    this->viewer = viewer;
    readProperties();
}

void LiveMap::initialize()
{
    memset(&angleTime, 0, sizeof angleTime);
    screenCaptureHandler = new osgViewer::ScreenCaptureHandler();
    scaleRamp = verticalScale * 10000.0;
    nmea.nmeaSentences.insert(NMEASENTENCES::value_type(string("GPBOD"), new BOD(*this)));
    nmea.nmeaSentences.insert(NMEASENTENCES::value_type(string("GPGGA"), new GGA(*this)));
    //nmea.nmeaSentences.insert(NMEASENTENCES::value_type(string("GPGLL"), new GLL(liveMap)));
    nmea.nmeaSentences.insert(NMEASENTENCES::value_type(string("GPRMB"), new RMB(*this)));
    nmea.nmeaSentences.insert(NMEASENTENCES::value_type(string("GPRMC"), new RMC(*this)));
    nmea.nmeaSentences.insert(NMEASENTENCES::value_type(string("GPVTG"), new VTG(*this)));
    //nmea.nmeaSentences.insert(NMEASENTENCES::value_type(string("PGRMZ"), new PGRMZ(liveMap)));
    font = osgText::readFontFile(fontName);
    mapIndex = new MapIndex(mapDirectory);
    portAudioSawToothData.left_phase = portAudioSawToothData.right_phase = 0.0;
    portAudioSawToothData.phaseCount = 0;
    portAudioSawToothData.mutePeriod = false;
    if (!disableAudio) {
        PaError paError = Pa_OpenDefaultStream(&audioSignalStream, 0, 2, paFloat32, 44100, 256, portAudioSawToothCallback, &portAudioSawToothData);
        if (paError != paNoError) {
            throw Support::makeMessage("PortAudio", Pa_GetErrorText(paError));
        }
    }
    mapTextures = new MAPTEXTURES();
    obstacleDatas = new OBSTACLEDATAS();
    lineObstacleDatas = new LINEOBSTACLEDATAS();
    obstacleGroups = new OBSTACLEGROUPS();
    black = new osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    white = new osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    red = new osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f);
    green = new osg::Vec4(0.0f, 1.0f, 0.5f, 1.0f);
    blue = new osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f);
    gray = new osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f);
    orange = new osg::Vec4(1.0f, 0.5f, 0.0f, 1.0f);
    darkBlue = new osg::Vec4(0.0f, 0.0f, 0.3f, 1.0f);
    whiteArray = new osg::Vec4ubArray(1);
    (*whiteArray)[0].set(255, 255, 255, 255);
    /*
    viewer->setClearColor(*white);
    */
    lastTrackVertexArray = new osg::Vec3Array(2);
    normalArray = new osg::Vec3Array(1);
    (*normalArray)[0].set(0.0f, 0.0f, 1.0f);
    osg::ref_ptr<osg::Group> root = new osg::Group();
    osg::ref_ptr<osg::MatrixTransform> rootTransform = new osg::MatrixTransform();
    osg::ref_ptr<RootUpdateCallback> rootUpdateCallback = new RootUpdateCallback(*this);
    rootTransform->setUpdateCallback(rootUpdateCallback.get());
    obstacleGroupUpdateCallback = new ObstacleGroupUpdateCallback(*this);
    int i;
    for (i = 0; i < 4; i++) obstacleGroupDatas[i] = NULL;
    root->addChild(rootTransform.get());
    obstacleUpdateCallback = new ObstacleUpdateCallback(*this);
    lineObstacleUpdateCallback = new LineObstacleUpdateCallback(*this);
    osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    osg::ref_ptr<osg::AlphaFunc> alphaFunc = new osg::AlphaFunc();
    alphaFunc->setFunction(osg::AlphaFunc::GREATER, 0.0f);
    stateSet->setAttributeAndModes(alphaFunc.get(), osg::StateAttribute::ON);
    root->setStateSet(stateSet.get());
    osg::ref_ptr<osg::Geometry> vesselGeometry = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> vesselVertexArray = new osg::Vec3Array(28);

    float x1 = 300.0f, x2 = 200.0f, x3 = 30.0f, y1 = 400.0f, y2 = 300.0f, y3 = 30.0f, z = 30.0f;

    (*vesselVertexArray)[0].set(0.0f, 0.0f, y1);
    (*vesselVertexArray)[1].set(-x1, 0.0f, 0.0f);
    (*vesselVertexArray)[2].set(-x2, -z, 0.0f);
    (*vesselVertexArray)[3].set(0.0f, -z, y2);

    (*vesselVertexArray)[4].set(-x1, 0.0f, 0.0f);
    (*vesselVertexArray)[5].set(-x1, 0.0f, -y1);
    (*vesselVertexArray)[6].set(-x2, -z, -y2);
    (*vesselVertexArray)[7].set(-x2, -z, 0.0f);

    (*vesselVertexArray)[8].set(-x1, 0.0f, -y1);
    (*vesselVertexArray)[9].set(x1, 0.0f, -y1);
    (*vesselVertexArray)[10].set(x2, -z, -y2);
    (*vesselVertexArray)[11].set(-x2, -z, -y2);

    (*vesselVertexArray)[12].set(x1, 0.0f, -y1);
    (*vesselVertexArray)[13].set(x1, 0.0f, 0.0f);
    (*vesselVertexArray)[14].set(x2, -z, 0.0f);
    (*vesselVertexArray)[15].set(x2, -z, -y2);

    (*vesselVertexArray)[16].set(x1, 0.0f, 0.0f);
    (*vesselVertexArray)[17].set(0.0f, 0.0f, y1);
    (*vesselVertexArray)[18].set(0.0f, -z, y2);
    (*vesselVertexArray)[19].set(x2, -z, 0.0f);

    (*vesselVertexArray)[20].set(-x2, -z, y3);
    (*vesselVertexArray)[21].set(x2, -z, y3);
    (*vesselVertexArray)[22].set(-x2, -z, -y3);
    (*vesselVertexArray)[23].set(x2, -z, -y3);

    (*vesselVertexArray)[24].set(-x3, -z, -y2);
    (*vesselVertexArray)[25].set(-x3, -z, y2);
    (*vesselVertexArray)[26].set(x3, -z, -y2);
    (*vesselVertexArray)[27].set(x3, -z, y2);

    vesselGeometry->setVertexArray(vesselVertexArray.get());
    vesselGeometry->setNormalArray(normalArray.get());
    vesselGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
    osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 4, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 8, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 12, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 16, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 20, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 24, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    osg::ref_ptr<osg::Vec4Array> vesselColors = new osg::Vec4Array(1);
    (*vesselColors)[0] = *darkBlue;
    vesselGeometry->setColorArray(vesselColors.get());
    vesselGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    osg::ref_ptr<osg::Geode> vessel = new osg::Geode();
    vessel->addDrawable(vesselGeometry.get());
    osg::ref_ptr<osg::MatrixTransform> vesselTransform = new osg::MatrixTransform();
    osg::ref_ptr<VesselUpdateCallback> vesselUpdateCallback = new VesselUpdateCallback(*this);
    vesselTransform->setUpdateCallback(vesselUpdateCallback.get());
    vesselTransform->addChild(vessel.get());
    root->addChild(vesselTransform.get());

    osg::Vec3 origo(0.0f, 0.0f, 0.0f), center(0.0f, 0.0f, 0.5f);
    osg::ref_ptr<osgSim::SphereSegment> vesselZone = new osgSim::SphereSegment(origo, 1.0f, 0.0f, fullZoneCircle ? 2 * osg::PI : osg::PI, -0.01, 0.01, 1);
    vesselZone->setDrawMask(osgSim::SphereSegment::SIDES);
    vesselZone->setSideColor(*red);

    osg::ref_ptr<osg::MatrixTransform> vesselZoneTransform = new osg::MatrixTransform();
    vesselZoneTransform->setMatrix(osg::Matrix::rotate(osg::PI / 2.0, 1.0, 0.0, 0.0) * osg::Matrix::translate(0.0f, -1.0, 0.0f));
    vesselZoneTransform->addChild(vesselZone.get());
    osg::ref_ptr<osg::Switch> vesselZoneSwitch = new osg::Switch();
    osg::ref_ptr<VesselZoneUpdateCallback> vesselZoneUpdateCallback = new VesselZoneUpdateCallback(*this);
    vesselZoneSwitch->setUpdateCallback(vesselZoneUpdateCallback.get());
    vesselZoneSwitch->addChild(vesselZoneTransform.get());
    vesselZoneSwitch->setValue(0, false);
    root->addChild(vesselZoneSwitch.get());

    osg::ref_ptr<osg::Box> photoBox = new osg::Box(center, 200.0f, 200.0f, 1.0f);
    osg::ref_ptr<osg::ShapeDrawable> photoDrawable = new osg::ShapeDrawable(photoBox.get());
    photoDrawable->setColor(*red);
    photoDrawable->setName("Photo");
    photo = new osg::Geode();
    photo->addDrawable(photoDrawable.get());

    osg::ref_ptr<osg::Cylinder> photoPinCylinder = new osg::Cylinder(center, 30.0f, 1.0f);
    osg::ref_ptr<osg::ShapeDrawable> photoPinDrawable = new osg::ShapeDrawable(photoPinCylinder.get());
    photoPinDrawable->setColor(*red);
    photoPin = new osg::Geode();
    photoPin->addDrawable(photoPinDrawable.get());

    osg::ref_ptr<osg::Cylinder> cylinder = new osg::Cylinder(center, 100.0f, 1.0f);
    osg::ref_ptr<osg::ShapeDrawable> blackObstacleDrawable = new osg::ShapeDrawable(cylinder.get());
    blackObstacleDrawable->setColor(*black);
    blackObstacle = new osg::Geode();
    blackObstacle->addDrawable(blackObstacleDrawable.get());

    osg::ref_ptr<osg::Cone> cone = new osg::Cone(center, 100.0f, 1.0f);
    osg::ref_ptr<osg::ShapeDrawable> greenObstacleDrawable = new osg::ShapeDrawable(cone.get());
    greenObstacleDrawable->setColor(*green);
    greenObstacle = new osg::Geode();
    greenObstacle->addDrawable(greenObstacleDrawable.get());

    osg::ref_ptr<osg::Box> box = new osg::Box(center, 200.0f, 200.0f, 1.0f);
    osg::ref_ptr<osg::ShapeDrawable> blueObstacleDrawable = new osg::ShapeDrawable(box.get());
    blueObstacleDrawable->setColor(*blue);
    blueObstacle = new osg::Geode();
    blueObstacle->addDrawable(blueObstacleDrawable.get());

    osg::ref_ptr<osg::ShapeDrawable> redObstacleDrawable = new osg::ShapeDrawable(cylinder.get());
    redObstacleDrawable->setColor(*red);
    redObstacle = new osg::Geode();
    redObstacle->addDrawable(redObstacleDrawable.get());

    blackObstacleSequence = new osg::Sequence();
    blackObstacleSequence->addChild(blackObstacle.get());
    blackObstacleSequence->addChild(redObstacle.get());
    blackObstacleSequence->setInterval(osg::Sequence::LOOP, 0, -1);
    blackObstacleSequence->setTime(0, 0.5f);
    blackObstacleSequence->setTime(1, 0.5f);
    blackObstacleSequence->setDuration(0.5f, -1);
    blackObstacleSequence->setMode(osg::Sequence::START);

    greenObstacleSequence = new osg::Sequence();
    greenObstacleSequence->addChild(greenObstacle.get());
    greenObstacleSequence->addChild(redObstacle.get());
    greenObstacleSequence->setInterval(osg::Sequence::LOOP, 0, -1);
    greenObstacleSequence->setTime(0, 0.5f);
    greenObstacleSequence->setTime(1, 0.5f);
    greenObstacleSequence->setDuration(0.5f, -1);
    greenObstacleSequence->setMode(osg::Sequence::START);

    blueObstacleSequence = new osg::Sequence();
    blueObstacleSequence->addChild(blueObstacle.get());
    blueObstacleSequence->addChild(redObstacle.get());
    blueObstacleSequence->setInterval(osg::Sequence::LOOP, 0, -1);
    blueObstacleSequence->setTime(0, 0.5f);
    blueObstacleSequence->setTime(1, 0.5f);
    blueObstacleSequence->setDuration(0.5f, -1);
    blueObstacleSequence->setMode(osg::Sequence::START);

    cout << "readObstacles" << endl;
    if (!disableObstacles) {
        if (obstacleFileName.length()) {
            readObstacles(*rootTransform.get());
        }
        if (mpowerObstacleFileName.length()) {
            readMpowerObstacles(*rootTransform.get());
        }
    }

    osg::ref_ptr<TrackUpdateCallback> trackUpdateCallback;
    trackUpdateCallback = new TrackUpdateCallback(*this);
    showTrackSwitch = new osg::Switch();
    showTrackSwitch->setUpdateCallback(trackUpdateCallback.get());
    showTrackGroup = new osg::Group();
    showTrackSwitch->addChild(showTrackGroup.get());
    showTrackSwitch->setValue(0, false);
    rootTransform->addChild(showTrackSwitch.get());

    if (photoDirectory.length()) {
        singlePhotoGroup = new osg::Group();
        singlePhotoSwitch = new osg::Switch();
        singlePhotoSwitch->addChild(singlePhotoGroup.get());
        singlePhotoSwitch->setValue(0, false);
        rootTransform->addChild(singlePhotoSwitch.get());
        photoIndex = new PhotoIndex(photoDirectory);
        photoEntryPair = photoIndex->photoEntries.begin();
        if (NMEADirectory.length()) {
            readNMEAs(NMEADirectory, trackUpdateCallback.get(), *rootTransform.get());
        }
        if (GPTDirectory.length()) {
            readGPTs(GPTDirectory, trackUpdateCallback.get(), *rootTransform.get(), false);
        }
        if (GPTNotesDirectory.length()) {
            readGPTs(GPTNotesDirectory, trackUpdateCallback.get(), *rootTransform.get(), true);
        }
        hasLastTrackVesselPosition = hasLastTrackVertexArray = false;
        readPhotos(*rootTransform.get());
    }
    trackSwitchItem = trackSwitches.begin();
    trackGroup = showTrackGroup.get();

    manipulator = new Manipulator();
    /*
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keySwitchMatrixManipulator = viewer->getKeySwitchMatrixManipulator();
    keySwitchMatrixManipulator->addNumberedMatrixManipulator(manipulator.get());
    keySwitchMatrixManipulator->selectMatrixManipulator(keySwitchMatrixManipulator->getNumMatrixManipulators() - 1);
    */
    viewer->setCameraManipulator(manipulator.get());

    /*
      viewer->setRealizeCallback(new RealizeOperation(*this));
    */
    //mapTextureSize=0;
    //viewer->realize();
    if (mapSizePixels) {
        mapTextureCount = mapSizePixels / mapTextureSize;
        mapTextureCount *= mapTextureCount;
    }
    mapGroup = new osg::Group();
    osg::ref_ptr<MapUpdateCallback> mapUpdateCallback = new MapUpdateCallback(*this);
    mapTextureDatas = new MapTextureData *[mapTextureCount];
    for (i = 0; i < mapTextureCount; i++) {
        osg::ref_ptr<osg::Switch> mapSwitch = new osg::Switch();
        mapSwitch->setUpdateCallback(mapUpdateCallback.get());
        osg::ref_ptr<osg::Group> mapGroup = new osg::Group();
        mapSwitch->addChild(mapGroup.get());
        mapSwitch->setValue(0, false);
        osg::ref_ptr<MapTextureData> mapTextureData = new MapTextureData();
        mapTextureDatas[i] = mapTextureData.get();
        mapSwitch->setUserData(mapTextureData.get());
        rootTransform->addChild(mapSwitch.get());
    }
    /*
    Producer::ref_ptr<Producer::CameraConfig> cameraConfig = viewer->getCameraConfig();
    Producer::ref_ptr<Producer::Camera> camera = cameraConfig->getCamera(0);
    camera->getProjectionRectangle(projectionX, projectionY, projectionWidth, projectionHeight);
    */
    osgViewer::Viewer::Cameras cameras;
    viewer->getCameras(cameras, false);
    osg::Camera *camera = cameras[0];
    osgViewer::Viewer::Windows windows;
    viewer->getWindows(windows);
    osgViewer::GraphicsWindow *window = windows[0];
    mapTextureSize = window->getState()->get<osg::GLExtensions>()->maxTextureSize / 4;
    window->getWindowRectangle(projectionX, projectionY, projectionWidth, projectionHeight);
    cout << "projectionX=" << projectionX << ",projectionY=" << projectionY << ",projectionWidth=" << projectionWidth << ",projectionHeight=" << projectionHeight << endl;
    camera->setClearColor(*white);
    osg::ref_ptr<osg::Geode> info = new osg::Geode();
    float characterSize = 20.0f;
    unsigned extent = projectionWidth / 4;
    osg::ref_ptr<osgText::Text> text = new osgText::Text();
    text->setFont(font.get());
    text->setColor(*black);
    text->setCharacterSize(characterSize);
    text->setPosition(osg::Vec3(0.0f, projectionHeight - characterSize, 0.0f));
    text->setLayout(osgText::Text::LEFT_TO_RIGHT);
    text->setText("");
    info->addDrawable(text.get());
    text = new osgText::Text();
    text->setFont(font.get());
    text->setColor(*green);
    text->setCharacterSize(characterSize);
    text->setPosition(osg::Vec3((float)extent, projectionHeight - characterSize, 0.0f));
    text->setLayout(osgText::Text::LEFT_TO_RIGHT);
    text->setText("");
    info->addDrawable(text.get());
    text = new osgText::Text();
    text->setFont(font.get());
    text->setColor(*orange);
    text->setCharacterSize(characterSize);
    text->setPosition(osg::Vec3((float)(extent * 2), projectionHeight - characterSize, 0.0f));
    text->setLayout(osgText::Text::LEFT_TO_RIGHT);
    text->setText("");
    info->addDrawable(text.get());
    text = new osgText::Text();
    text->setFont(font.get());
    text->setColor(*red);
    text->setCharacterSize(characterSize);
    text->setPosition(osg::Vec3((float)(extent * 3), projectionHeight - characterSize, 0.0f));
    text->setLayout(osgText::Text::LEFT_TO_RIGHT);
    text->setText("");
    info->addDrawable(text.get());
    text = new osgText::Text();
    text->setFont(font.get());
    text->setColor(*red);
    text->setCharacterSize(characterSize);
    text->setPosition(osg::Vec3((float)(extent * 2) + characterSize, projectionHeight - projectionHeight / 2 - characterSize, 0.0f));
    text->setLayout(osgText::Text::LEFT_TO_RIGHT);
    text->setText("");
    info->addDrawable(text.get());
    osg::ref_ptr<osg::MatrixTransform> infoTransform = new osg::MatrixTransform();
    infoTransform->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    infoTransform->setMatrix(osg::Matrix::identity());
    infoTransform->addChild(info.get());
    osg::ref_ptr<osg::StateSet> infoStateSet = new osg::StateSet();
    infoStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    infoStateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
    infoStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    infoTransform->setStateSet(infoStateSet.get());
    osg::ref_ptr<osg::Projection> infoProjection = new osg::Projection();
    infoProjection->setMatrix(osg::Matrix::ortho2D((float)projectionX, (float)projectionWidth, (float)projectionY, (float)projectionHeight));
    infoProjection->addChild(infoTransform.get());
    osg::ref_ptr<osg::Switch> infoSwitch = new osg::Switch();
    osg::ref_ptr<InfoUpdateCallback> infoUpdateCallback = new InfoUpdateCallback(*this);
    infoSwitch->setUpdateCallback(infoUpdateCallback.get());
    infoSwitch->addChild(infoProjection.get());
    infoSwitch->setValue(0, false);
    root->addChild(infoSwitch.get());
    osg::ref_ptr<osg::Projection> photoProjection = new osg::Projection();
    photoProjection->setMatrix(osg::Matrix::ortho2D((float)projectionX, (float)projectionWidth, (float)projectionY, (float)projectionHeight));
    photoSwitch = new osg::Switch();
    photoSwitch->addChild(photoProjection.get());
    photoSwitch->setValue(0, false);
    root->addChild(photoSwitch.get());
    osgUtil::Optimizer optimizer;
    optimizer.optimize(root.get());
    /*
    viewer->getEventHandlerList().push_front(new PickHandler(*this));
    */
    viewer->addEventHandler(new PickHandler(*this));
    viewer->setSceneData(root.get());
    //viewer->realize();
    cout << "mapTextureSize=" << mapTextureSize << endl;
    cout << "mapTextureCount=" << mapTextureCount << endl;
    mapReaderMutex.lock();
    trackShowMutex.lock();
    if (recordNMEA) {
        nmeaOutStream = new ofstream((dataDirectory + FILE_SEPARATOR + "nmea.txt").c_str());
    }
    if (generateDatabase) {
        databaseGenerateThread = new DatabaseGenerateThread(*this);
        int rv = databaseGenerateThread->start();
        if (rv) {
            throw Support::makeMessage("OpenThreads", strerror(rv == -1 ? errno : rv));
        }
        disableMaps = true;
        disableGPS = true;
    }
    if (!disableMaps) {
        mapReaderThread = new MapReaderThread(*this);
        int rv = mapReaderThread->start();
        if (rv) {
            throw Support::makeMessage("OpenThreads", strerror(rv == -1 ? errno : rv));
        }
    }
    if (!disableGPS) {
        if (!serialPortName.empty()) {
            SerialBuffer *serialBuffer = new SerialBuffer(serialPortName);
            serialBuffer->setSerialPortParameters(4800, 8, SerialBuffer::StopBits1, SerialBuffer::ParityNone, SerialBuffer::FlowControlNone);
            nmeaBuffer = serialBuffer;
        } else {
            nmeaBuffer = new SocketBuffer(socketHost, socketPort);
        }
        gpsListenerThread = new GpsListenerThread(*this);
        int rv = gpsListenerThread->start();
        if (rv) {
            throw Support::makeMessage("OpenThreads", strerror(rv == -1 ? errno : rv));
        }
    }
    trackShowThread = new TrackShowThread(*this);
    int rv = trackShowThread->start();
    if (rv) {
        throw Support::makeMessage("OpenThreads", strerror(rv == -1 ? errno : rv));
    }
    if (testing) {
        relocateVessel();
    }
}

LiveMap::~LiveMap() noexcept(false)
{
    raise(SIGINT);
    if (gpsListenerThread && gpsListenerThread->cancel() ||
        mapReaderThread && mapReaderThread->cancel() ||
        trackShowThread && trackShowThread->cancel() ||
        gpsListenerThread && gpsListenerThread->join() ||
        mapReaderThread && mapReaderThread->join() ||
        trackShowThread && trackShowThread->join()) {
        throw Support::makeMessage("OpenThreads", strerror(errno));
    }
    if (nmeaOutStream) {
        nmeaOutStream->close();
        nmeaOutStream = NULL;
    }
    delete gpsListenerThread;
    delete mapReaderThread;
    delete trackShowThread;
    if (raster) {
        delete raster;
    }
    if (!disableAudio) {
        if (audioSignalStreamActive) {
            Pa_StopStream(audioSignalStream);
        }
        PaError paError = Pa_CloseStream(audioSignalStream);
        if (paError != paNoError) {
            throw Support::makeMessage("PortAudio", Pa_GetErrorText(paError));
        }
    }
}

time_t LiveMap::timeOf(const string &dateOfFix, const string &timeOfFix)
{
    time_t timeNow;
    time(&timeNow);
    struct tm *tm = gmtime(&timeNow);
    if (dateOfFix.length()) {
        tm->tm_mday = atoi(dateOfFix.substr(0, 2).c_str());
        tm->tm_mon = atoi(dateOfFix.substr(2, 2).c_str()) - 1;
        tm->tm_year = 100 + atoi(dateOfFix.substr(4, 2).c_str());
    }
    tm->tm_hour = atoi(timeOfFix.substr(0, 2).c_str());
    tm->tm_min = atoi(timeOfFix.substr(2, 2).c_str());
    tm->tm_sec = atoi(timeOfFix.substr(4, 2).c_str());
    return mktime(tm);
}

double LiveMap::parseCoordinate(string &degrees, string &minutes)
{
    return atof(degrees.c_str()) + atof(minutes.c_str()) / 60.0;
}

void LiveMap::readProperties()
{
    Properties properties;
    properties.load("LiveMap.properties");
    for (Properties::iterator property = properties.begin();
         property != properties.end(); property++) {
        const string &name = property->first;
        string &value = *property->second;
        if (name == "GPTDirectory") {
            GPTDirectory = Support::toPhysicalPath(value);
        } else if (name == "GPTNotesDirectory") {
            GPTNotesDirectory = Support::toPhysicalPath(value);
        } else if (name == "NMEADirectory") {
            NMEADirectory = Support::toPhysicalPath(value);
        } else if (name == "allObstaclesInView") {
            allObstaclesInView = Support::isTrueOrFalse(value);
        } else if (name == "altitude") {
            vesselCoordinates.z = atof(value.c_str());
        } else if (name == "countedAngle") {
            countedAngle = Support::isTrueOrFalse(value);
        } else if (name == "dataDirectory") {
            dataDirectory = Support::toPhysicalPath(value);
        } else if (name == "databaseDirectory") {
            databaseDirectory = Support::toPhysicalPath(value);
        } else if (name == "disableAudio") {
            disableAudio = Support::isTrueOrFalse(value);
        } else if (name == "disableGPS") {
            disableGPS = Support::isTrueOrFalse(value);
        } else if (name == "disableMaps") {
            disableMaps = Support::isTrueOrFalse(value);
        } else if (name == "disableObstacles") {
            disableObstacles = Support::isTrueOrFalse(value);
        } else if (name == "elevationMap") {
            elevationMap = Support::isTrueOrFalse(value);
        } else if (name == "fixedNorth") {
            fixedNorth = Support::isTrueOrFalse(value);
        } else if (name == "fontName") {
            fontName = Support::toPhysicalPath(value);
        } else if (name == "fullZoneCircle") {
            fullZoneCircle = Support::isTrueOrFalse(value);
        } else if (name == "groundSpeedInKmh") {
            groundSpeedInKmh = Support::isTrueOrFalse(value);
        } else if (name == "hazardZoneMinutes") {
            hazardZoneHours = atof(value.c_str()) / 60.0;
        } else if (name == "hideMapsOnObstacles") {
            hideMapsOnObstacles = Support::isTrueOrFalse(value);
        } else if (name == "latitude") {
            vesselCoordinates.y = atof(value.c_str());
        } else if (name == "longitude") {
            vesselCoordinates.x = atof(value.c_str());
        } else if (name == "mapDirectory") {
            mapDirectory = Support::toPhysicalPath(value);
        } else if (name == "mapSizePixels") {
            mapSizePixels = atoi(value.c_str());
        } else if (name == "mpowerObstacleFileName") {
            mpowerObstacleFileName = value;
        } else if (name == "obstacleFileName") {
            obstacleFileName = value;
        } else if (name == "orthographicProjection") {
            orthographicProjection = Support::isTrueOrFalse(value);
        } else if (name == "photoDirectory") {
            photoDirectory = Support::toPhysicalPath(value);
        } else if (name == "photoShotDirectory") {
            photoShotDirectory = Support::toPhysicalPath(value);
        } else if (name == "recordNMEA") {
            recordNMEA = Support::isTrueOrFalse(value);
        } else if (name == "safetyHeightMeters") {
            safetyHeight = atof(value.c_str());
        } else if (name == "safetyMarginSeconds") {
            safetyMarginHours = atof(value.c_str()) / 60.0 / 60.0;
        } else if (name == "serialPortName") {
            serialPortName = value;
        } else if (name == "sceneryMode") {
            sceneryMode = Support::isTrueOrFalse(value);
        } else if (name == "showTracks") {
            showTracks = Support::isTrueOrFalse(value);
        } else if (name == "socketHost") {
            socketHost = value;
        } else if (name == "socketPort") {
            socketPort = atoi(value.c_str());
        } else if (name == "testing") {
            testing = Support::isTrueOrFalse(value);
        } else if (name == "trackTimeRate") {
            trackTimeRate = atof(value.c_str());
        } else if (name == "trackToleranceSecs") {
            trackToleranceSecs = atof(value.c_str());
        } else if (name == "useMapScales") {
            useMapScales = Support::isTrueOrFalse(value);
        } else if (name == "verticalScale") {
            verticalScale = atof(value.c_str());
        } else {
            throw new string("Unknown property " + name);
        }
    }
}

void LiveMap::readObstacles(osg::MatrixTransform &rootTransform)
{
    Coordinates coordinates;
    string fileName(dataDirectory + FILE_SEPARATOR + obstacleFileName);
    ifstream in(fileName.c_str());
    if (!in) {
        throw new string("Obstacle file " + fileName + " not found");
    }
    string line;
    getline(in, line);
    cout << "title=" << line << endl;
    for (;;) {
        if (!getline(in, line)) {
            break;
        }
        // if (line.length() == 0 || !isdigit(line[0] & 0xff)) continue;
        StringTokenizer st = StringTokenizer(line, ";", false);
        osg::ref_ptr<ObstacleData> obstacleData = new ObstacleData();
        /*
          obstacleData->id = Support::trim(st.nextToken());
          string nDegrees = Support::trim(st.nextToken()),
          nMinutes = Support::trim(st.nextToken()),
          eDegrees = Support::trim(st.nextToken()),
          eMinutes = Support::trim(st.nextToken());
          coordinates.y = parseCoordinate(nDegrees, nMinutes);
          coordinates.x = parseCoordinate(eDegrees, eMinutes);
          coordinates.z = 0.0f;
          mapIndex->convert(coordinates, obstacleData->position);
          obstacleData->meanSeaLevel = feetToMeters(atof(Support::trim(st.nextToken()).c_str()));
          obstacleData->type = Support::trim(st.nextToken());
          obstacleData->id = Support::trim(st.nextToken());
          coordinates.y = atof(st.nextToken().c_str());
          coordinates.x = atof(st.nextToken().c_str());
          coordinates.z = 0.0f;
          mapIndex->convert(coordinates, obstacleData->position);
          obstacleData->type = Support::trim(st.nextToken());
          obstacleData->meanSeaLevel = feetToMeters(atof(Support::trim(st.nextToken()).c_str()));
        */
        obstacleData->name = Support::trim(st.nextToken());
        obstacleData->id = Support::trim(st.nextToken());
        coordinates.y = atof(st.nextToken().c_str());
        coordinates.x = atof(st.nextToken().c_str());
        coordinates.z = 0.0f;
        mapIndex->kkjToYkj(coordinates, obstacleData->position);
        st.nextToken();
        obstacleData->meanSeaLevel = atof(Support::trim(st.nextToken()).c_str());
        st.nextToken();
        obstacleData->type = Support::trim(st.nextToken());
        readObstacle(rootTransform, obstacleData.get());
    }
    in.close();
    cout << "obstacleGroups=" << *obstacleGroups << endl;
}

void LiveMap::readMpowerObstacles(osg::MatrixTransform &rootTransform)
{
    Coordinates coordinates;
    string fileName(dataDirectory + FILE_SEPARATOR + mpowerObstacleFileName);
    ifstream in(fileName.c_str());
    if (!in) {
        throw new string("MPower obstacle file " + fileName + " not found");
    }
    string line;
    getline(in, line);
    cout << "title=" << line << endl;
    for (;;) {
        if (!getline(in, line)) {
            break;
        }
        if (line.length() == 0 || !isdigit(line[0] & 0xff)) {
            continue;
        }
        StringTokenizer st = StringTokenizer(line, ",", "\"", '\\', false);
        osg::ref_ptr<ObstacleData> obstacleData = new ObstacleData();
        // RowNum
        obstacleData->name = Support::trim(st.nextToken());
        // TrackRowNum
        obstacleData->id = Support::trim(st.nextToken());
        // Lat
        coordinates.y = atof(Support::trim(st.nextToken()).c_str());
        // Lon
        coordinates.x = atof(Support::trim(st.nextToken()).c_str());
        coordinates.z = 0.0f;
        obstacleData->type = "Voimajohtopylv�s";
        mapIndex->convert(coordinates, obstacleData->position);
        obstacleData->meanSeaLevel = 50;
        readObstacle(rootTransform, obstacleData.get());
    }
    in.close();
    cout << "obstacleGroups=" << *obstacleGroups << endl;
}

void LiveMap::readTrack(time_t trackTime) {
    osg::ref_ptr<TrackData> trackData = new TrackData();
    trackData->filePath = NULL;
    if (!firstTrackData) {
        firstTrackData = trackData;
    }
    if (previousTrackData.get()) {
        previousTrackData->nextTrackData = trackData;
    }
    previousTrackData = trackData;
    trackData->time = trackTime;
    trackData->coordinates = vesselCoordinates;
    photoIndex->assign(trackTime, vesselCoordinates);
    if (showTracks) {
        Coordinates position;
        mapIndex->convert(vesselCoordinates, position);
        if (hasLastTrackVesselPosition) {
            showTrack(*trackGroup, lastTrackVesselPosition, position);
        } else {
            hasLastTrackVesselPosition = true;
        }
        lastTrackVesselPosition = position;
    }
}

void LiveMap::followTrack(TrackData *trackData) {
    time_t timeNow, trackTime = trackData->time;
    time(&timeNow);
    if (!trackTimeOffset) {
        timeOffset = timeNow;
        trackTimeOffset = trackTime;
    }
    double timeDelta = difftime(timeNow, timeOffset),
        trackDelta = difftime(trackTime, trackTimeOffset);
    if (fabs(difftime(trackTime, lastTrackTime)) > trackToleranceSecs) {
        hasLastTrackVesselPosition = hasLastTrackVertexArray = false;
    }
    lastTrackTime = trackTime;
    timeDelta *= trackTimeRate;
    if (trackDelta > timeDelta) {
        unsigned int millis = (unsigned int)(((double)trackDelta - timeDelta) * 1000.0 + .5);
        if (millis > 0L) {
            OpenThreads::Thread::microSleep(millis * 1000);
        }
    }
    OpenThreads::Thread::YieldCurrentThread();
    vesselCoordinates = trackData->coordinates;
    relocateVessel();
}

bool LiveMap::relocateVessel()
{
    if (lastVesselCoordinates == vesselCoordinates) {
        relocatingVessel = false;
        return false;
    }
    followCamera = false;
    ftime(&gpsTime);
    lastVesselCoordinates = vesselCoordinates;
    gridConvergence = mapIndex->convert(vesselCoordinates, vesselPosition);
    double clearance = gpsVesselPosition.clearance(vesselPosition);
    if (clearance > 1000.0) {
        hasLastTrackVesselPosition = hasLastTrackVertexArray = false;
    }
    gpsVesselPosition = vesselPosition;
    if (sceneryMode) {
        gpsVesselPosition.z = vesselCoordinates.z;
    }
    vesselZoneRadius = 2.0 * vesselVelocity * hazardZoneHours * 1000.0;
    safetyZoneRadius = 2.0 * vesselVelocity * (hazardZoneHours + safetyMarginHours) * 1000.0;
    double distance = 2.0 * vesselVelocity * safetyMarginHours * 1000.0;
    safetyPosition.x = vesselPosition.x + cos(trackAngle) * distance;
    safetyPosition.y = vesselPosition.y + sin(trackAngle) * distance;
    safetyPosition.z = vesselPosition.z;
    mapReaderCondition.broadcast();
    if (allObstaclesInView) {
        return true;
    }
    ObstacleGroup *obstacleGroups[4];
    bool found[4];
    double delta = 40000.0;
    int i, obstacleGroupNumber = 0;
    for (i = 0; i < 4; i++) found[i] = false;
    for (i = 0; i < 4; i++) {
        Coordinates position;
        switch (i) {
        case 0:
            position.x = vesselPosition.x - delta;
            position.y = vesselPosition.y - delta;
            break;
        case 1:
            position.x = vesselPosition.x + delta;
            position.y = vesselPosition.y - delta;
            break;
        case 2:
            position.x = vesselPosition.x - delta;
            position.y = vesselPosition.y + delta;
            break;
        case 3:
            position.x = vesselPosition.x + delta;
            position.y = vesselPosition.y + delta;
            break;
        }
        position.z = 0.0;
        ObstacleGroup *obstacleGroup = search(position);
        if (!obstacleGroup) {
            continue;
        }
        int obstacleGroupDataIndex = obstacleGroup->obstacleGroupData->findIndexIn(obstacleGroupDatas);
        if (obstacleGroupDataIndex != -1) {
            found[obstacleGroupDataIndex] = true;
        }
        else {
            obstacleGroups[obstacleGroupNumber++] = obstacleGroup;
        }
    }
    int obstacleGroupIndex = 0;
    for (i = 0; i < 4 && obstacleGroupIndex < obstacleGroupNumber; i++) {
        if (!obstacleGroupDatas[i]) {
            obstacleGroupDataNumber++;
        } else if (found[i] || obstacleGroupDataNumber + obstacleGroupIndex < 4) {
            continue;
        }
        if (obstacleGroupDatas[i]) {
            obstacleGroupDatas[i]->inView = false;
        }
        obstacleGroupDatas[i] = obstacleGroups[obstacleGroupIndex]->obstacleGroupData;
        obstacleGroupDatas[i]->inView = true;
        obstacleGroupIndex++;
    }
    //cout << "vesselPosition=" << vesselPosition << endl;
    return true;
}

/*
  static tileContigRoutine contigRoutine;

  static void tiffTileContigRoutine(TIFFRGBAImage *image, uint32 *raster, uint32 col, uint32 row, uint32 npix, uint32 nrow, int32 fromskew, int32 toskew, unsigned char *buf)
  {
  (*contigRoutine)(image, raster, col, row, npix, nrow, fromskew, toskew, buf);
  OpenThreads::Thread::YieldCurrentThread();
  }

  static tileSeparateRoutine separateRoutine;

  static void tiffTileSeparateRoutine(TIFFRGBAImage *image, uint32 *raster, uint32 col, uint32 row, uint32 npix, uint32 nrow, int32 fromskew, int32 toskew, unsigned char *red, unsigned char *green, unsigned char *blue, unsigned char *alpha)
  {
  (*separateRoutine)(image, raster, col, row, npix, nrow, fromskew, toskew, red, green, blue, alpha);
  OpenThreads::Thread::YieldCurrentThread();
  }
*/

void LiveMap::readMaps(MapEntry *selectedMapEntry) {
#ifdef DEBUG_MAP
    cout << "readMaps" << endl;
#endif
    double delta = 40000.0;
    if (useMapScales) {
        osg::Matrix matrix = manipulator->getMatrix();
        osg::Vec3 eye, center, up;
        matrix.getLookAt(eye, center, up);
#ifdef DEBUG_MAP
        cout << "eye=" << &eye << ",center=" << &center << ",up=" << &up << endl;
#endif
        if (eye.z() >= -scaleRamp) {
            delta = 5000.0;
            scaleIndex = 1;
        } else {
            delta = 40000.0;
            scaleIndex = 0;
        }
    }
    Coordinates referencePosition = followCamera ? cameraPosition : vesselPosition;
#ifdef DEBUG_MAP
    cout << "scaleIndex=" << scaleIndex << endl;
#endif
    Coordinates viewMin, viewMax;
    viewMin.x = referencePosition.x - delta;
    viewMin.y = referencePosition.y - delta;
    viewMin.z = 0.0;
    viewMax.x = referencePosition.x + delta;
    viewMax.y = referencePosition.y + delta;
    viewMax.z = 0.0;
#ifdef DEBUG_MAP
    cout << "viewMin=" << viewMin << endl;
    cout << "viewMax=" << viewMax << endl;
#endif
    Coordinates vertices[4];
    vertices[0].x = viewMin.x;
    vertices[0].y = viewMin.y;
    vertices[0].z = viewMin.z;
    vertices[1].x = viewMax.x;
    vertices[1].y = viewMin.y;
    vertices[1].z = viewMin.z;
    vertices[2].x = viewMax.x;
    vertices[2].y = viewMax.y;
    vertices[2].z = viewMin.z;
    vertices[3].x = viewMin.x;
    vertices[3].y = viewMax.y;
    vertices[3].z = viewMin.z;
    MAPTEXTURES mapTextureSet;
    int i, numberOfMaps = 0, numberOfRasters = 0;
    for (unsigned vertexIndex = 0; vertexIndex < LENGTH(vertices); vertexIndex++) {
        const MapEntry *mapEntry = selectedMapEntry ? selectedMapEntry : mapIndex->search(vertices[vertexIndex], scaleIndex);
        if (!mapEntry) {
            continue;
        }
#ifdef DEBUG_MAP
        cout << "mapEntry=" << *mapEntry << endl;
#endif
        numberOfMaps++;
        GLint width = (GLint)powerOf2((double)mapEntry->imageWidth),
            height = (GLint)powerOf2((double)mapEntry->imageHeight);
        double xRatio = (double)width / (double)mapEntry->imageWidth,
            yRatio = (double)height / (double)mapEntry->imageHeight;
        int numberOfHorizontalRasters = 1;
        while (width > mapTextureSize) {
            numberOfHorizontalRasters <<= 1;
            width >>= 1;
        }
        int numberOfVerticalRasters = 1;
        while (height > mapTextureSize) {
            numberOfVerticalRasters <<= 1;
            height >>= 1;
        }
        numberOfRasters += numberOfHorizontalRasters * numberOfVerticalRasters;
#ifdef DEBUG_MAP
        cout << "width=" << width << endl;
        cout << "height=" << height << endl;
        cout << "numberOfHorizontalRasters=" << numberOfHorizontalRasters << endl;
        cout << "numberOfVerticalRasters=" << numberOfVerticalRasters << endl;
#endif
        double xSize = (mapEntry->coordinates2.x - mapEntry->coordinates1.x) / (double)numberOfHorizontalRasters * xRatio,
            ySize = (mapEntry->coordinates1.y - mapEntry->coordinates2.y) / (double)numberOfVerticalRasters * yRatio;
        for (int verticalRasterIndex = 0; verticalRasterIndex < numberOfVerticalRasters; verticalRasterIndex++) {
            for (int horizontalRasterIndex = 0; horizontalRasterIndex < numberOfHorizontalRasters; horizontalRasterIndex++) {
#ifdef DEBUG_MAP
                cout << "verticalRasterIndex=" << verticalRasterIndex << endl;
                cout << "horizontalRasterIndex=" << horizontalRasterIndex << endl;
#endif
                MapTexture &mapTexture = *new MapTexture();
                mapTexture.mapEntry = mapEntry;
                mapTexture.xRatio = xRatio;
                mapTexture.yRatio = yRatio;
                mapTexture.verticalRasterIndex = verticalRasterIndex;
                mapTexture.horizontalRasterIndex = horizontalRasterIndex;
                mapTexture.width = width;
                mapTexture.height = height;
                mapTexture.vertexCoordinatesArray[0].x = mapEntry->coordinates1.x + (double)horizontalRasterIndex * xSize;
                mapTexture.vertexCoordinatesArray[0].y = mapEntry->coordinates2.y + (double)verticalRasterIndex * ySize;
                mapTexture.vertexCoordinatesArray[1].x = mapTexture.vertexCoordinatesArray[0].x + xSize;
                mapTexture.vertexCoordinatesArray[1].y = mapTexture.vertexCoordinatesArray[0].y;
                mapTexture.vertexCoordinatesArray[2].x = mapTexture.vertexCoordinatesArray[1].x;
                mapTexture.vertexCoordinatesArray[2].y = mapTexture.vertexCoordinatesArray[1].y + ySize;
                mapTexture.vertexCoordinatesArray[3].x = mapTexture.vertexCoordinatesArray[0].x;
                mapTexture.vertexCoordinatesArray[3].y = mapTexture.vertexCoordinatesArray[2].y;
                mapTexture.sumOfDistances = 0.0;
                for (unsigned index = 0; index < LENGTH(mapTexture.vertexCoordinatesArray); index++)
                    mapTexture.sumOfDistances += referencePosition.clearance(mapTexture.vertexCoordinatesArray[index]);
                if (!mapTexture.findIn(mapTextureSet)) {
                    mapTextureSet.insert(mapTextureSet.end(), &mapTexture);
                }
                else {
                    delete &mapTexture;
                }
            }
        }
        if (selectedMapEntry) {
            break;
        }
    }
#ifdef DEBUG_MAP
    cout << "maxTextureSets=" << mapTextureSet.size() << endl;
    cout << "numberOfMaps=" << numberOfMaps << endl;
    cout << "numberOfRasters=" << numberOfRasters << endl;
#endif
    mapTextureSet.sort(mapTexturePredicate);
    MAPTEXTURES *newMapTextures = new MAPTEXTURES();
    MAPTEXTURES::iterator mapTextureItem = mapTextureSet.begin();
    for (i = 0; i < mapTextureCount && mapTextureItem != mapTextureSet.end(); i++) {
        MapTexture &mapTexture = **mapTextureItem;
        newMapTextures->insert(newMapTextures->end(), &mapTexture);
        mapTextureItem++;
    }
#ifdef DEBUG_MAP
    cout << "newMapTextures=" << *newMapTextures << endl;
#endif
    while (mapTextureItem != mapTextureSet.end()) {
        MapTexture &mapTexture = **mapTextureItem;
        delete &mapTexture;
        mapTextureItem++;
    }
    MAPTEXTURES::iterator aMapTexture;
#ifdef DEBUG_MAP
    cout << "mapTextures->size()=" << mapTextures->size() << endl;
    cout << "newMapTextures->size()=" << newMapTextures->size() << endl;
#endif
    /*
      for (aMapTexture = newMapTextures->begin();
      aMapTexture != newMapTextures->end();
      aMapTexture++) cout << "newMapTexture=" << **aMapTexture << endl;
      for (aMapTexture = mapTextures->begin();
      aMapTexture != mapTextures->end();
      aMapTexture++) cout << "mapTexture=" << **aMapTexture << endl;
    */
    MAPTEXTURES oldMapTextures;
    for (aMapTexture = mapTextures->begin();
         aMapTexture != mapTextures->end();
         aMapTexture++) {
        MapTexture &mapTexture = **aMapTexture;
        if (!mapTexture.findIn(*newMapTextures)) {
#ifdef DEBUG_MAP
            cout << "removeChild mapTexture=" << mapTexture << endl;
#endif
            if (!mapTexture.mapTextureData || mapTexture.mapTextureData->raster) {
                continue;
            }
            mapTexture.mapTextureData->raster = mapTexture.raster;
            mapTexture.mapTextureData->stateSet = NULL;
            mapTexture.mapTextureData->map = NULL;
        }
        else oldMapTextures.insert(oldMapTextures.end(), &mapTexture);
    }
#ifdef DEBUG_MAP
    cout << "searching new" << endl;
#endif
    MAPTEXTURES mapTexturesForAdding;
    for (aMapTexture = newMapTextures->begin();
         aMapTexture != newMapTextures->end();
         aMapTexture++) {
        MapTexture &mapTexture = **aMapTexture;
        MapTexture *theMapTexture = mapTexture.findIn(oldMapTextures);
#ifdef DEBUG_MAP
        cout << "theMapTexture=" << theMapTexture << endl;
#endif
        if (theMapTexture) {
            mapTexture.mapTextureData = theMapTexture->mapTextureData;
            mapTexture.raster = theMapTexture->raster;
        }
        else {
            mapTexturesForAdding.insert(mapTexturesForAdding.end(), &mapTexture);
        }
    }
    //mapTexturesForAdding.sort(mapTextureEntryPredicate);
#ifdef DEBUG_MAP
    cout << "mapTexturesForAdding.size()=" << mapTexturesForAdding.size() << endl;
#endif
    int mapTextureAddNumber = 0;
    MAPTEXTURES::iterator mapTextureForAdding;
    for (mapTextureForAdding = mapTexturesForAdding.begin();
         mapTextureForAdding != mapTexturesForAdding.end();
         mapTextureForAdding++) {
        MapTexture &mapTexture = **mapTextureForAdding;
#ifdef DEBUG_MAP
        cout << "add " << mapTexture << endl;
        cout << "filePath=" << mapTexture.mapEntry->filePath << endl;
#endif
        MapTextureData *mapTextureData = NULL;
        if (mapTextureAddNumber < mapTextureAddCount || vesselVelocity < 10.0) {
            for (i = 0; i < mapTextureCount; i++) {
                mapTextureData = (MapTextureData *)mapTextureDatas[i];
                if (mapTextureData->map.valid()) continue;
                break;
            }
        } else {
            i = mapTextureCount;
        }
        if (i == mapTextureCount) {
            MAPTEXTURES::iterator aMapTexture;
            for (aMapTexture = newMapTextures->begin();
                 aMapTexture != newMapTextures->end();
                 aMapTexture++)
                if (*aMapTexture == &mapTexture) {
                    delete *aMapTexture;
                    newMapTextures->erase(aMapTexture);
                    break;
                }
            continue;
        }
        if (!lastMapEntry || lastMapEntry != mapTexture.mapEntry) {
#ifdef DEBUG_MAP
            cout << "read tiff" << endl;
            cout << "raster=" << raster << endl;
#endif
            if (raster) {
                delete raster;
            }
            if (elevationRaster) {
                delete elevationRaster;
            }
            lastMapEntry = mapTexture.mapEntry;
            TIFF *tiff = TIFFOpen(mapTexture.mapEntry->filePath.c_str(), "r");
            size_t nPixels = mapTexture.mapEntry->imageWidth * mapTexture.mapEntry->imageHeight;
            raster = new uint32[nPixels];
            if (!raster) {
                throw new string("new raster failed");
            }
            /*
              char message[1024];
              TIFFRGBAImage image;
              if (!TIFFRGBAImageBegin(&image, tiff, 0, message))
              throw Support::makeMessage("TIFFlib", message);
              if (image.isContig) {
              contigRoutine = image.put.contig;
              image.put.contig = tiffTileContigRoutine;
              }
              else {
              separateRoutine = image.put.separate;
              image.put.separate = tiffTileSeparateRoutine;
              }
              if (!TIFFRGBAImageGet(&image, raster, mapTexture.mapEntry->imageWidth, mapTexture.mapEntry->imageHeight))
              throw new string("TIFFRGBAImageGet failed");
              TIFFRGBAImageEnd(&image);
            */
            if (!TIFFReadRGBAImage(tiff, mapTexture.mapEntry->imageWidth, mapTexture.mapEntry->imageHeight, raster, 0)) {
                throw new string("TIFFReadRGBAImage failed");
            }
            TIFFClose(tiff);
            if (elevationMap && mapTexture.mapEntry->elevationMapEntry) {
                tiff = TIFFOpen(mapTexture.mapEntry->elevationMapEntry->filePath.c_str(), "r");
                nPixels = mapTexture.mapEntry->elevationMapEntry->imageWidth * mapTexture.mapEntry->elevationMapEntry->imageHeight;
                elevationRaster = new uint16[nPixels];
                if (!elevationRaster) {
                    throw new string("new raster failed");
                }
                for (uint32 row = 0; row < mapTexture.mapEntry->elevationMapEntry->imageHeight; row++) {
                    TIFFReadScanline(tiff, elevationRaster + (row * mapTexture.mapEntry->elevationMapEntry->imageWidth), row);
                }
                TIFFClose(tiff);
            }
            else elevationRaster = NULL;
        }
        uint32 rasterSize = mapTexture.height * mapTexture.width;
        mapTexture.raster = new uint32[rasterSize];
        if (!mapTexture.raster) {
            throw new string("new raster failed");
        }
        //      _TIFFmemset(mapTexture.raster, 0xff, rasterSize * sizeof(uint32));
        uint32 i;
        for (i = 0; i < rasterSize; i++) {
            mapTexture.raster[i] = 0xffffff;
        }
        uint32 xOffset = mapTexture.horizontalRasterIndex * mapTexture.width,
            yOffset = mapTexture.verticalRasterIndex * mapTexture.height,
            width1 = min(mapTexture.width, mapTexture.mapEntry->imageWidth - xOffset),
            height1 = min(mapTexture.height, mapTexture.mapEntry->imageHeight - yOffset);
#ifdef DEBUG_MAP
        cout << "width1=" << width1 << endl;
        cout << "height1=" << height1 << endl;
#endif
        for (i = 0; i < height1; i++)
            _TIFFmemcpy(mapTexture.raster + i * mapTexture.width,
                        raster + (yOffset + i) * mapTexture.mapEntry->imageWidth + xOffset, width1 * sizeof(uint32));
        osg::ref_ptr<osg::Image> image = new osg::Image();
        image->setImage(mapTexture.width, mapTexture.height, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
                        (unsigned char *)mapTexture.raster, osg::Image::NO_DELETE);
#ifdef DEBUG_MAP
        cout << "image set" << endl;
#endif
        osg::ref_ptr<osg::Node> map = NULL;
        osg::ref_ptr<osg::Geode> mapGeode = new osg::Geode();
        if (elevationMap) {
            osg::ref_ptr<osg::Vec3Array> v3;
            osg::ref_ptr<osg::Vec2Array> t2;
            double horizontalSize = mapTexture.vertexCoordinatesArray[1].x - mapTexture.vertexCoordinatesArray[0].x + 1.0,
                verticalSize = mapTexture.vertexCoordinatesArray[2].y - mapTexture.vertexCoordinatesArray[1].y + 1.0;
            double interval = 100.0;
            uint32 horizontalCount = (uint32)(horizontalSize / interval + .5),
                verticalCount = (uint32)(verticalSize / interval + .5);
            double horizontalNumber = (double)(horizontalCount - 1),
                verticalNumber = (double)(verticalCount - 1);
            if (elevationRaster) {
                v3 = new osg::Vec3Array(horizontalCount * verticalCount);
                t2 = new osg::Vec2Array(horizontalCount * verticalCount);
                osg::Vec3 pos(0.0, 0.0, 0.0);
                osg::Vec2 tex(0.0, 0.0);
                int i = 0;
                for (uint32 verticalIndex = 0; verticalIndex < verticalCount; verticalIndex++) {
                    double yFactor = (double)verticalIndex / verticalNumber;
                    pos.z() = mapTexture.vertexCoordinatesArray[0].y + yFactor * verticalSize;
                    tex.y() = yFactor;
                    uint32 yOffset = (uint32)((double)(pos.z() - mapTexture.mapEntry->elevationMapEntry->coordinates2.y) / (double)(mapTexture.mapEntry->elevationMapEntry->coordinates1.y - mapTexture.mapEntry->elevationMapEntry->coordinates2.y) * (double)mapTexture.mapEntry->elevationMapEntry->imageHeight * mapTexture.yRatio + .5);
                    for (uint32 horizontalIndex = 0; horizontalIndex < horizontalCount; horizontalIndex++) {
                        double xFactor = (double)horizontalIndex / horizontalNumber;
                        pos.x() = mapTexture.vertexCoordinatesArray[0].x + xFactor * horizontalSize;
                        tex.x() = xFactor;
                        uint32 xOffset = (uint32)((double)(pos.x() - mapTexture.mapEntry->elevationMapEntry->coordinates1.x) / (double)(mapTexture.mapEntry->elevationMapEntry->coordinates2.x - mapTexture.mapEntry->elevationMapEntry->coordinates1.x) * (double)mapTexture.mapEntry->elevationMapEntry->imageWidth * mapTexture.xRatio + .5);
                        double elevation = yOffset < mapTexture.mapEntry->elevationMapEntry->imageHeight && xOffset < mapTexture.mapEntry->elevationMapEntry->imageWidth ? elevationToMeters((double)elevationRaster[yOffset * mapTexture.mapEntry->elevationMapEntry->imageWidth + xOffset]) * verticalScale : 0;
                        (*v3.get())[i].set(pos.x(), -elevation, pos.z());
                        //(*v3.get())[i].set(pos.x(), 0.0, pos.z());
                        (*t2.get())[i].set(tex.x(), tex.y());
                        i++;
                    }
                }
            } else {
#ifdef DEBUG_MAP
                cout << "horizontalSize=" << horizontalSize << endl;
                cout << "verticalSize=" << verticalSize << endl;
                cout << "horizontalCount=" << horizontalCount << endl;
                cout << "verticalCount=" << verticalCount << endl;
#endif
                /*
                  long height = 0L;
                  osg::ref_ptr<osg::Grid> grid = new osg::Grid();
                  grid->allocateGrid(horizontalCount, verticalCount);
                  grid->setXInterval(horizontalInterval);
                  grid->setYInterval(verticalInterval);
                  for (int verticalIndex = 0; verticalIndex < verticalCount; verticalIndex++)
                  for (int horizontalIndex = 0; horizontalIndex < horizontalCount; horizontalIndex++) {
                  if (rand() % 2 && height < 5000L) height += 10L;
                  else if (height > 10L) height -= 10L;
                  grid->setHeight(horizontalIndex, verticalIndex, (double)height);
                  }
                  osg::ref_ptr<osg::ShapeDrawable> ground = new osg::ShapeDrawable(grid.get());
                  mapGeode->addDrawable(ground.get());
                */
                v3 = new osg::Vec3Array(horizontalCount * verticalCount);
                t2 = new osg::Vec2Array(horizontalCount * verticalCount);
                osg::Vec3 pos(0.0, 0.0, 0.0);
                osg::Vec2 tex(0.0, 0.0);
                double radius = scaleIndex == 0 ? 5000.0 : 500.0,
                    sign = mapTexture.verticalRasterIndex % 2 == 0 ? mapTexture.horizontalRasterIndex % 2 == 0 ? 1.0 : -1.0 : mapTexture.horizontalRasterIndex % 2 == 0 ? -1.0 : 1.0;
                int i = 0;
                for (uint32 verticalIndex = 0; verticalIndex < verticalCount; verticalIndex++) {
                    double yFactor = (double)verticalIndex / verticalNumber;
                    pos.z() = mapTexture.vertexCoordinatesArray[0].y + yFactor * verticalSize;
                    tex.y() = yFactor;
                    double ySin = yFactor > 0.0 && yFactor < 1.0 ? fabs(sin(yFactor * osg::PI)) : 0.0;
                    ySin = sign * ySin * ySin * ySin * ySin;
                    for (uint32 horizontalIndex = 0; horizontalIndex < horizontalCount; horizontalIndex++) {
                        double xFactor = (double)horizontalIndex / horizontalNumber,
                            xSin = xFactor > 0.0 && xFactor < 1.0 ? fabs(sin(xFactor * osg::PI)) : 0.0;
                        pos.x() = mapTexture.vertexCoordinatesArray[0].x + xFactor * horizontalSize;
                        pos.y() = xSin * xSin * xSin * xSin * ySin * radius;
                        tex.x() = xFactor;
                        //cout << "pos.x=" << pos.x() << ",pos.y=" << pos.y() << ",pos.z=" << pos.z() << ",tex.x=" << tex.x() << ",tex.y=" << tex.y() << endl;
                        (*v3.get())[i].set(pos.x(), pos.y(), pos.z());
                        /*
                          radius += (double)(rand() % 20 - 10);
                          if (radius <= 0.0) radius = 0.0;
                          else if (radius >= 5000.0) radius = 5000.0;
                        */
                        (*t2.get())[i].set(tex.x(), tex.y());
                        i++;
                    }
                }
            }
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
            geometry->setVertexArray(v3.get());
            geometry->setColorArray(whiteArray);
            geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
            geometry->setTexCoordArray(0, t2.get());
            for (uint32 verticalIndex = 0; verticalIndex < verticalCount - 1; verticalIndex++) {
                osg::ref_ptr<osg::DrawElementsUShort> drawElements = new osg::DrawElementsUShort(GL_QUAD_STRIP, 2 * horizontalCount);
                geometry->addPrimitiveSet(drawElements.get());
                int i = 0;
                for (uint32 horizontalIndex = 0; horizontalIndex < horizontalCount; horizontalIndex++) {
                    (*drawElements.get())[i++] = (verticalIndex + 1) * horizontalCount + horizontalIndex;
                    (*drawElements.get())[i++] = verticalIndex * horizontalCount + horizontalIndex;
                }
            }
            osgUtil::SmoothingVisitor smoothingVisitor;
            smoothingVisitor.smooth(*geometry.get());
            mapGeode->addDrawable(geometry.get());
            /*
              osg::ref_ptr<osg::MatrixTransform> mapTransform = new osg::MatrixTransform();
              //mapTransform->setMatrix(osg::Matrix::rotate(osg::PI / 2.0, 1.0, 0.0, 0.0) * osg::Matrix::translate(mapTexture.vertexCoordinatesArray[0].x, 0.0, mapTexture.vertexCoordinatesArray[0].y));
              mapTransform->addChild(mapGeode.get());
              map = mapTransform.get();
            */
        } else {
            osg::ref_ptr<osg::Geometry> mapGeometry = new osg::Geometry();
            osg::ref_ptr<osg::Vec3Array> mapVertexArray = new osg::Vec3Array(4);
            for (i = 0; i < 4; i++)
                (*mapVertexArray)[i].set(mapTexture.vertexCoordinatesArray[i].x, 0.0f, mapTexture.vertexCoordinatesArray[i].y);
            /*
              {for (int i=0;i<4;i++) {
              cout << i << ".x=" << mapVertexArray[i].x() << endl;
              cout << i << ".y=" << mapVertexArray[i].y() << endl;
              cout << i << ".z=" << mapVertexArray[i].z() << endl;
              }}
            */
            mapGeometry->setVertexArray(mapVertexArray.get());
            mapGeometry->setColorArray(whiteArray);
            mapGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
            mapGeometry->setNormalArray(normalArray.get());
            mapGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
            osg::ref_ptr<osg::Vec2Array> texCoordArray = new osg::Vec2Array(4);
            (*texCoordArray)[0].set(0.0f, 0.0f);
            (*texCoordArray)[1].set(1.0f, 0.0f);
            (*texCoordArray)[2].set(1.0f, 1.0f);
            (*texCoordArray)[3].set(0.0f, 1.0f);
            mapGeometry->setTexCoordArray(0, texCoordArray.get());
            osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4);
            mapGeometry->addPrimitiveSet(primitiveSet.get());
            mapGeode->addDrawable(mapGeometry.get());
        }
        map = mapGeode.get();
        osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
        texture->setTextureSize(image->s(), image->t());
        texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_NEAREST);
        texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
        texture->setInternalFormatMode(osg::Texture::USE_S3TC_DXT3_COMPRESSION);
        texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
        texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
        texture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
        texture->setImage(image.get());
        osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();
        stateSet->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);
        mapTextureData->map = map.get();
        mapTextureData->stateSet = stateSet.get();
        mapTexture.mapTextureData = mapTextureData;
        mapTextureAddNumber++;
        if (generateDatabase) {
            mapGroup->setStateSet(stateSet.get());
            if (mapGroup->getNumChildren() > 0) {
                mapGroup->setChild(0, map.get());
            } else {
                mapGroup->addChild(map.get());
            }
            ostringstream fileName;
            fileName << databaseDirectory << FILE_SEPARATOR << mapTexture.vertexCoordinatesArray[0].x << '-' << mapTexture.vertexCoordinatesArray[0].y << ".osg";
            osgDB::writeNodeFile(*mapGroup.get(), fileName.str());
            vesselPosition.x = mapTexture.vertexCoordinatesArray[0].x + (mapTexture.vertexCoordinatesArray[1].x - mapTexture.vertexCoordinatesArray[0].x) / 2.0;
            vesselPosition.y = mapTexture.vertexCoordinatesArray[0].y + (mapTexture.vertexCoordinatesArray[2].y - mapTexture.vertexCoordinatesArray[1].y) / 2.0;
            gpsVesselPosition = vesselPosition;
        }
    }
    for (aMapTexture = mapTextures->begin();
         aMapTexture != mapTextures->end();
         aMapTexture++) {
        MapTexture &mapTexture = **aMapTexture;
        delete &mapTexture;
    }
    delete mapTextures;
    mapTextures = newMapTextures;
    relocatingVessel = false;
#ifdef DEBUG_MAP
    cout << "readMaps done" << endl;
#endif
}

void LiveMap::setVesselAngle()
{
    timeb currentTime;
    ftime(&currentTime);
    if (angleTime.time > 0L && angleSeconds > 0.0) {
        double seconds = (double)currentTime.time - (double)angleTime.time +
            (double)(currentTime.millitm - angleTime.millitm) / 1000.0,
            factor = seconds / angleSeconds;
        if (factor > 1.0) {
            factor = 1.0;
        }
        vesselAngle = lastTrackAngle + (trackAngle - lastTrackAngle) * factor;
    } else vesselAngle = trackAngle;
    if (angleVesselPosition != gpsVesselPosition) {
        if (countedAngle) {
            lastTrackAngle = trackAngle;
            trackAngle = gpsVesselPosition.angle(angleVesselPosition);
        }
        if (angleTime.time > 0L) {
            angleSeconds = (double)currentTime.time - (double)angleTime.time +
                (double)(currentTime.millitm - angleTime.millitm) / 1000.0;
        }
        angleTime = currentTime;
        angleVesselPosition = gpsVesselPosition;
    }
}

void LiveMap::extrapolateVesselPosition()
{
    timeb currentTime;
    ftime(&currentTime);
    double seconds = (double)currentTime.time - (double)gpsTime.time +
        (double)(currentTime.millitm - gpsTime.millitm) / 1000.0;
    //  if (seconds < 1.0 / 24.0) return;
    double distance = vesselVelocity * (seconds / 60.0 / 60.0) * 1000.0;
#ifdef DEBUG
    cout << "seconds=" << seconds << endl;
    cout << "distance=" << distance << endl;
    cout << "trackAngle=" << trackAngle << endl;
#endif
    vesselPosition.x = gpsVesselPosition.x + cos(trackAngle) * distance;
    vesselPosition.y = gpsVesselPosition.y + sin(trackAngle) * distance;
    // vesselPosition.z = gpsVesselPosition.z;
}

void LiveMap::showTrack(osg::Group &trackGroup, Coordinates vesselPosition1, Coordinates vesselPosition2) {
    double width = 50.0f,
        angle = -vesselPosition2.angle(vesselPosition1),
        xDelta = sin(angle) * width, yDelta = cos(angle) * width;
    osg::ref_ptr<osg::Vec3Array> lineVertexArray = new osg::Vec3Array(4);
    if (hasLastTrackVertexArray) {
        (*lineVertexArray)[0] = (*lastTrackVertexArray)[0];
        (*lineVertexArray)[1] = (*lastTrackVertexArray)[1];
    } else {
        (*lineVertexArray)[0].set(vesselPosition1.x - xDelta, -vesselPosition1.z * verticalScale, vesselPosition1.y - yDelta);
        (*lineVertexArray)[1].set(vesselPosition1.x + xDelta, -vesselPosition1.z * verticalScale, vesselPosition1.y + yDelta);
    }
    (*lineVertexArray)[2].set(vesselPosition2.x - xDelta, -vesselPosition2.z * verticalScale, vesselPosition2.y - yDelta);
    (*lineVertexArray)[3].set(vesselPosition2.x + xDelta, -vesselPosition2.z * verticalScale, vesselPosition2.y + yDelta);
    (*lastTrackVertexArray)[0] = (*lineVertexArray)[2];
    (*lastTrackVertexArray)[1] = (*lineVertexArray)[3];
    hasLastTrackVertexArray = true;
    osg::ref_ptr<osg::Geometry> trackGeometry = new osg::Geometry();
    trackGeometry->setVertexArray(lineVertexArray.get());
    trackGeometry->setNormalArray(normalArray.get());
    trackGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
    osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 0, 4);
    trackGeometry->addPrimitiveSet(primitiveSet.get());
    osg::ref_ptr<osg::Vec4Array> trackColors = new osg::Vec4Array(1);
    (*trackColors)[0] = *gray;
    trackGeometry->setColorArray(trackColors.get());
    trackGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    trackGeometry->setName("Track");
    osg::ref_ptr<osg::Geode> track = new osg::Geode();
    track->addDrawable(trackGeometry.get());
    trackGroup.addChild(track.get());
}

void LiveMap::updateRoot(osg::Node *node) {
    osg::MatrixTransform &rootTransform = *(osg::MatrixTransform *)node;
    if (gpsVesselPosition != lastRootVesselPosition) {
        lastRootVesselPosition = gpsVesselPosition;
    } else {
        extrapolateVesselPosition();
    }
    if (fixedNorth) {
        osg::Matrix matrix = rootTransform.getMatrix();
        matrix.setTrans(-vesselPosition.x, 0.0, -vesselPosition.y);
        rootTransform.setMatrix(matrix);
    } else {
        setVesselAngle();
        rootTransform.setMatrix(osg::Matrix::translate(-vesselPosition.x, 0.0, -vesselPosition.y) * osg::Matrix::rotate(-vesselAngle + osg::PI / 2.0f, 0.0f, -1.0f, 0.0f));
    }
    if (!cameraSet) {
        const osg::Vec3 eye(0.0, (-vesselPosition.z - (sceneryMode || useMapScales ? 1000.0 : 10000.0)) * verticalScale, sceneryMode ? -14000.0 : 0.0), center(0.0, 0.0, 0.0), up(0.0, 0.0, 1.0);
        manipulator->setCameraData(eye, center, up);
        cameraSet = true;
    }
}

void LiveMap::updateMap(osg::Node *node)
{
    osg::Switch &mapSwitch = *(osg::Switch *)node;
    osg::Group &mapGroup = *(osg::Group *)mapSwitch.getChild(0);
    MapTextureData &mapTextureData = *(MapTextureData *)mapSwitch.getUserData();
    if (!mapTextureData.map.valid()) {
        return;
    }
    //cout << "updateMap mapTextureData=" << mapTextureData << endl;
    if (hideMapsOnObstacles && inHazardZone && mapSwitch.getValue(0)) {
        mapSwitch.setValue(0, false);
    }
    bool update = false;
    if (mapGroup.getNumChildren() > 0) {
        osg::Geode &map = (osg::Geode &)*mapGroup.getChild(0);
        if (mapTextureData.map.get() != &map) {
            update = true;
        }
    }
    else update = true;
    if (update) {
        mapGroup.setStateSet(mapTextureData.stateSet.get());
        if (mapTextureData.raster) {
            delete mapTextureData.raster;
            mapTextureData.raster = NULL;
        }
        if (mapGroup.getNumChildren() > 0) {
            mapGroup.setChild(0, mapTextureData.map.get());
        }
        else {
            mapGroup.addChild(mapTextureData.map.get());
        }
    }
    if (hideMapsOnObstacles && !inHazardZone && !inSafetyZone && !mapSwitch.getValue(0)) {
        mapSwitch.setValue(0, true);
    }
}

void LiveMap::updateObstacleGroup(osg::Node *node)
{
    osg::Switch &obstacleGroupSwitch = *(osg::Switch *)node;
    ObstacleGroupData &obstacleGroupData = *(ObstacleGroupData *)obstacleGroupSwitch.getUserData();
    if (obstacleGroupData.inView) {
        obstacleGroupSwitch.setValue(0, true);
    } else {
        obstacleGroupSwitch.setValue(0, false);
    }
}

void LiveMap::updateVessel(osg::Node *node) {
    if (gpsVesselPosition == lastGpsVesselPosition) {
        extrapolateVesselPosition();
        if (scaleIndex == lastScaleIndex) {
            return;
        }
        lastScaleIndex = scaleIndex;
    }
    lastGpsVesselPosition = gpsVesselPosition;
    setVesselAngle();
    osg::MatrixTransform &vesselTransform = *(osg::MatrixTransform *)node;
    float scaleFactor = scaleIndex == 0 ? 1.0f : 0.1f;
    if (fixedNorth) {
        vesselTransform.setMatrix(osg::Matrix::scale(scaleFactor, 1.0f, scaleFactor) *
                                  osg::Matrix::rotate(vesselAngle - osg::PI / 2.0f, 0.0f, -1.0f, 0.0f) *
                                  osg::Matrix::translate(0.0f, -vesselCoordinates.z * verticalScale - 1.0, 0.0f));
    } else {
        vesselTransform.setMatrix(osg::Matrix::scale(scaleFactor, 1.0f, scaleFactor) *
                                  osg::Matrix::translate(0.0f, -vesselCoordinates.z * verticalScale - 1.0, 0.0f));
    }
}

void LiveMap::updateVesselZone(osg::Node *node)
{
    osg::Switch &vesselZoneSwitch = *(osg::Switch *)node;
    if (!inHazardZone) {
        if (!vesselZoneSwitch.getValue(0)) {
            return;
        }
        vesselZoneSwitch.setValue(0, false);
        return;
    }
    osg::MatrixTransform &vesselZoneTransform = *(osg::MatrixTransform *)vesselZoneSwitch.getChild(0);
#ifdef DEBUG
    cout << "vesselZoneRadius=" << vesselZoneRadius << endl;
#endif
    if (fixedNorth) {
        vesselZoneTransform.setMatrix(
                                      osg::Matrix::rotate(osg::PI / 2.0f, 1.0f, 0.0f, 0.0f) *
                                      osg::Matrix::rotate(vesselAngle - osg::PI / 2.0f, 0.0f, 1.0f, 0.0f) *
                                      osg::Matrix::scale(vesselZoneRadius, 1.0f, vesselZoneRadius) *
                                      osg::Matrix::translate(0.0f, -vesselCoordinates.z * verticalScale - 1.0, 0.0f));
    } else {
        vesselZoneTransform.setMatrix(
                                      osg::Matrix::rotate(osg::PI / 2.0f, 1.0f, 0.0f, 0.0f) *
                                      osg::Matrix::scale(vesselZoneRadius, 1.0f, vesselZoneRadius) *
                                      osg::Matrix::translate(0.0f, -vesselCoordinates.z * verticalScale - 1.0, 0.0f));
    }
    if (vesselZoneSwitch.getValue(0)) {
        return;
    }
    vesselZoneSwitch.setValue(0, true);
}

void LiveMap::checkObstacles(ObstacleData *obstacleData, LineObstacleData *lineObstacleData,
                             bool inHazardZone, bool inSafetyZone, double clearance, double level)
{
    bool inHazardZone0 = inHazardZone, inSafetyZone0 = inSafetyZone;
    OBSTACLEDATAS *newObstacleDatas = new OBSTACLEDATAS();
    OBSTACLEDATAS::iterator anObstacleData;
    for (anObstacleData = obstacleDatas->begin();
         anObstacleData != obstacleDatas->end();
         anObstacleData++) {
        ObstacleData *obstacleData1 = *anObstacleData;
        if (obstacleData1 == obstacleData) {
            continue;
        }
        double clearance1 = 0, level1;
        bool inSafetyZone1, inHazardZone1 = isInHazardZone(*obstacleData1, inSafetyZone1, clearance1, level1);
        if (inHazardZone1 || inSafetyZone1) {
            newObstacleDatas->insert(newObstacleDatas->end(), obstacleData1);
            if (clearance < 0.0 || clearance1 < clearance) {
                clearance = clearance1;
                level = level1;
            }
        }
        if (inHazardZone1) {
            inHazardZone0 = true;
        }
        if (inSafetyZone1) {
            inSafetyZone0 = true;
        }
    }
    if ((inHazardZone || inSafetyZone) && obstacleData) {
        newObstacleDatas->insert(newObstacleDatas->end(), obstacleData);
    }
    delete obstacleDatas;
    obstacleDatas = newObstacleDatas;
    LINEOBSTACLEDATAS *newLineObstacleDatas = new LINEOBSTACLEDATAS();
    LINEOBSTACLEDATAS::iterator aLineObstacleData;
    for (aLineObstacleData = lineObstacleDatas->begin();
         aLineObstacleData != lineObstacleDatas->end();
         aLineObstacleData++) {
        LineObstacleData *lineObstacleData1 = *aLineObstacleData;
        if (lineObstacleData1 == lineObstacleData) {
            continue;
        }
        double clearance1, level1;
        bool inSafetyZone1, inHazardZone1 = isInHazardZone(*lineObstacleData1, inSafetyZone1, clearance1, level1);
        if (inHazardZone1 || inSafetyZone1) {
            newLineObstacleDatas->insert(newLineObstacleDatas->end(), lineObstacleData1);
            if (clearance < 0.0 || clearance1 < clearance) {
                clearance = clearance1;
                level = level1;
            }
        }
        if (inHazardZone1) {
            inHazardZone0 = true;
        }
        if (inSafetyZone1) {
            inSafetyZone0 = true;
        }
    }
    if ((inHazardZone || inSafetyZone) && lineObstacleData) {
        newLineObstacleDatas->insert(newLineObstacleDatas->end(), lineObstacleData);
    }
    delete lineObstacleDatas;
    lineObstacleDatas = newLineObstacleDatas;
    this->inSafetyZone = inSafetyZone0;
    hazardClearance = clearance;
    hazardLevel = level;
    if (inHazardZone0) {
        this->inHazardZone = true;
        if (!disableAudio) {
            if (!audioSignalStreamActive) {
                Pa_StartStream(audioSignalStream);
                audioSignalStreamActive = true;
            }
        }
        return;
    }
    this->inHazardZone = false;
    if (!disableAudio) {
        if (audioSignalStreamActive) {
            Pa_StopStream(audioSignalStream);
            audioSignalStreamActive = false;
        }
    }
}

void LiveMap::updateObstacle(osg::Node *node)
{
    osg::MatrixTransform &obstacleTransform = *(osg::MatrixTransform *)node;
    ObstacleData &obstacleData = *(ObstacleData *)obstacleTransform.getUserData();
    double clearance = -1.0, level = -1.0;
    bool inSafetyZone, inHazardZone = isInHazardZone(obstacleData, inSafetyZone, clearance, level);
    checkObstacles(&obstacleData, NULL, inHazardZone, inSafetyZone, clearance, level);
    osg::Switch &obstacleSwitch = (osg::Switch &)*obstacleTransform.getChild(0);
    int childIndex = inHazardZone ? 1 : 0;
    if (obstacleSwitch.getValue(childIndex)) {
        return;
    }
    float scaleFactor = inHazardZone ? 2.0f : 1.0f;
    obstacleTransform.setMatrix(osg::Matrix::scale(scaleFactor, scaleFactor, obstacleData.meanSeaLevel * verticalScale) *
                                osg::Matrix::rotate(osg::PI / 2.0f, 1.0f, 0.0f, 0.0f) *
                                osg::Matrix::translate(obstacleData.position.x, 0.0f, obstacleData.position.y));
    obstacleSwitch.setSingleChildOn(childIndex);
}

void LiveMap::updateLineObstacle(osg::Node *node)
{
    osg::MatrixTransform &lineObstacleTransform = *(osg::MatrixTransform *)node;
    LineObstacleData &lineObstacleData = *(LineObstacleData *)lineObstacleTransform.getUserData();
    double clearance = -1.0, level = -1.0;
    bool inSafetyZone, inHazardZone = isInHazardZone(lineObstacleData, inSafetyZone, clearance, level);
    checkObstacles(NULL, &lineObstacleData, inHazardZone, inSafetyZone, clearance, level);
    osg::Switch &lineObstacleSwitch = (osg::Switch &)*lineObstacleTransform.getChild(0);
    int childIndex = inHazardZone ? 1 : 0;
    if (lineObstacleSwitch.getValue(childIndex)) {
        return;
    }
    float scaleFactor = inHazardZone ? 2.0f : 1.0f;
    lineObstacleTransform.setMatrix(osg::Matrix::scale(1.0f, 1.0f, scaleFactor) *
                                    osg::Matrix::rotate(lineObstacleData.angle, 0.0f, 1.0f, 0.0f) *
                                    osg::Matrix::translate(lineObstacleData.obstacleData1->position.x, 0.0f, lineObstacleData.obstacleData1->position.y));
    lineObstacleSwitch.setSingleChildOn(childIndex);
}

void LiveMap::updateInfo(osg::Node *node)
{
    osg::Switch &infoSwitch = *(osg::Switch *)node;
    bool photoVisible = photoSwitch->getValue(0);
    if (!inHazardZone) {
        if (!photoVisible) {
            if (infoSwitch.getValue(0)) {
                infoSwitch.setValue(0, false);
            }
            photoInfoVisible = false;
            return;
        } else if (photoInfoVisible) {
            return;
        }
    }
    osg::Projection &projection = (osg::Projection &)*infoSwitch.getChild(0);
    osg::MatrixTransform &infoTransform = *(osg::MatrixTransform *)projection.getChild(0);
    osg::Geode &info = (osg::Geode &)*infoTransform.getChild(0);
    osgText::Text &time = (osgText::Text &)*info.getDrawable(0),
        &altitude = (osgText::Text &)*info.getDrawable(1),
        &level = (osgText::Text &)*info.getDrawable(2),
        &difference = (osgText::Text &)*info.getDrawable(3),
        &difference1 = (osgText::Text &)*info.getDrawable(4);
    char value[128];
    if (inHazardZone) {
        double delaySeconds = vesselVelocity > 0.0 ? hazardClearance / (vesselVelocity * 1000.0) * 30.0 * 60.0 : 0.0,
            altitudeMeters = vesselCoordinates.z, differenceMeters = hazardLevel - altitudeMeters;
        sprintf(value, "%d sec", (int)(delaySeconds + 0.5));
        time.setText(value);
        sprintf(value, "%d m", (int)(altitudeMeters + 0.5));
        altitude.setText(value);
        sprintf(value, "%d m", (int)(hazardLevel + 0.5));
        level.setText(value);
        sprintf(value, "%d m", (int)(differenceMeters + 0.5));
        difference.setText(value);
        difference1.setText(value);
    } else {
        sprintf(value, "%lf %c, %lf %c, %s", photoEntry->coordinates.y, photoEntry->coordinates.y >= 0.0 ? 'N' : 'S', photoEntry->coordinates.x, photoEntry->coordinates.x >= 0.0 ? 'E' : 'W', photoEntry->fileName.c_str());
        time.setText(value);
        photoInfoVisible = true;
    }
    if (infoSwitch.getValue(0)) {
        return;
    }
    infoSwitch.setValue(0, true);
}

void LiveMap::updateTrack(osg::Node *node)
{
    osg::Switch &trackSwitch = *(osg::Switch *)node;
    osg::Group &trackGroup = *(osg::Group *)trackSwitch.getChild(0);
    if (!hasLastTrackVesselPosition) {
        lastTrackVesselPosition = vesselPosition;
        hasLastTrackVesselPosition = true;
        return;
    }
    if (vesselPosition == lastTrackVesselPosition) {
        return;
    }
    showTrack(trackGroup, lastTrackVesselPosition, vesselPosition);
    lastTrackVesselPosition = vesselPosition;
}

void LiveMap::readObstacle(osg::MatrixTransform &rootTransform, ObstacleData *obstacleData)
{
    //cout << "obstacleData=" << obstacleData << endl;
    bool isLineObstacle = obstacleData->type == "Voimajohtopylv�s";
    osg::ref_ptr<osg::MatrixTransform> obstacleTransform = NULL;
    if (!isLineObstacle) {
        obstacleTransform = new osg::MatrixTransform();
        obstacleTransform->setUpdateCallback(obstacleUpdateCallback.get());
        obstacleTransform->setMatrix(osg::Matrix::scale(1.0f, 1.0f, obstacleData->meanSeaLevel * verticalScale) *
                                     osg::Matrix::rotate(osg::PI / 2.0f, 1.0f, 0.0f, 0.0f) *
                                     osg::Matrix::translate(obstacleData->position.x, 0.0f, obstacleData->position.y));
        obstacleTransform->setUserData(obstacleData);
        osg::ref_ptr<osg::Switch> obstacleSwitch = new osg::Switch();
        if (obstacleData->type == "Puusto") {
            obstacleSwitch->addChild(greenObstacle.get());
            obstacleSwitch->addChild(greenObstacleSequence.get());
        }
        else if (obstacleData->type == "Rakennus") {
            obstacleSwitch->addChild(blueObstacle.get());
            obstacleSwitch->addChild(blueObstacleSequence.get());
        }
        else {
            obstacleSwitch->addChild(blackObstacle.get());
            obstacleSwitch->addChild(blackObstacleSequence.get());
        }
        obstacleSwitch->setSingleChildOn(0);
        obstacleTransform->addChild(obstacleSwitch.get());
        if (testing) {
            float characterSize = 160.0f;
            osg::ref_ptr<osgText::Text> text = new osgText::Text();
            text->setFont(font.get());
            text->setColor(*black);
            text->setCharacterSize(characterSize);
            text->setPosition(osg::Vec3(0.0f, 110.0f, 1.0f));
            text->setLayout(osgText::Text::LEFT_TO_RIGHT);
            char value[64];
            sprintf(value, "%d", (int)(obstacleData->meanSeaLevel + 0.5));
            text->setText(value);
            osg::ref_ptr<osg::Geode> note = new osg::Geode();
            note->addDrawable(text.get());
            obstacleTransform->addChild(note.get());
        }
    }
    ObstacleGroup *obstacleGroup = NULL;
    if (!allObstaclesInView) {
        obstacleGroup = search(obstacleData->position);
        if (!obstacleGroup) {
            obstacleGroup = new ObstacleGroup();
            obstacleGroup->group = new osg::Group();
            if (!obstaclePivotPosition) {
                obstaclePivotPosition = new Coordinates();
                *obstaclePivotPosition = obstacleData->position;
            }
            obstacleGroup->minPosition.x = obstaclePivotPosition->x + floor((obstacleData->position.x - obstaclePivotPosition->x) / 80000.0) * 80000.0;
            obstacleGroup->minPosition.y = obstaclePivotPosition->y + floor((obstacleData->position.y - obstaclePivotPosition->y) / 80000.0) * 80000.0;
            obstacleGroup->minPosition.z = 0.0;
            obstacleGroup->maxPosition.x = obstacleGroup->minPosition.x + 80000.0;
            obstacleGroup->maxPosition.y = obstacleGroup->minPosition.y + 80000.0;
            obstacleGroup->maxPosition.z = 0.0;
            osg::ref_ptr<ObstacleGroupData> obstacleGroupData = new ObstacleGroupData();
            obstacleGroup->obstacleGroupData = obstacleGroupData.get();
            obstacleGroups->insert(obstacleGroups->end(), obstacleGroup);
            osg::ref_ptr<osg::Switch> obstacleGroupSwitch = new osg::Switch();
            obstacleGroupSwitch->setValue(0, false);
            obstacleGroupSwitch->setUpdateCallback(obstacleGroupUpdateCallback.get());
            obstacleGroupSwitch->setUserData(obstacleGroupData.get());
            obstacleGroupSwitch->addChild(obstacleGroup->group.get());
            rootTransform.addChild(obstacleGroupSwitch.get());
        }
        if (!isLineObstacle) {
            obstacleGroup->group->addChild(obstacleTransform.get());
        }
    }
    else if (!isLineObstacle) {
        rootTransform.addChild(obstacleTransform.get());
    }
    if (!firstObstacleData) {
        firstObstacleData = obstacleData;
    }
    if (previousObstacleData.get()) {
        previousObstacleData->nextObstacleData = obstacleData;
    }
    previousObstacleData = obstacleData;
    if (!isLineObstacle) {
        return;
    }
    osg::ref_ptr<ObstacleData> recentObstacleData = lastObstacleData;
    if (!recentObstacleData.get()) {
        lastObstacleData = obstacleData;
        return;
    }
    lastObstacleData = NULL;
    double clearance = obstacleData->position.clearance(recentObstacleData->position);
    if (clearance > 3000.0) {
        return;
    }
    double angle = -obstacleData->position.angle(recentObstacleData->position);
    float width = 100.0f;
    osg::ref_ptr<osg::Vec3Array> lineVertexArray = new osg::Vec3Array(4);
    (*lineVertexArray)[0].set(0.0, -recentObstacleData->meanSeaLevel * verticalScale, -width);
    (*lineVertexArray)[1].set(0.0, -recentObstacleData->meanSeaLevel * verticalScale, width);
    (*lineVertexArray)[2].set(clearance, -obstacleData->meanSeaLevel * verticalScale, -width);
    (*lineVertexArray)[3].set(clearance, -obstacleData->meanSeaLevel * verticalScale, width);
    osg::ref_ptr<osg::Geometry> blackLineObstacleGeometry = new osg::Geometry();
    blackLineObstacleGeometry->setVertexArray(lineVertexArray.get());
    blackLineObstacleGeometry->setNormalArray(normalArray.get());
    blackLineObstacleGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
    osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 0, 4);
    blackLineObstacleGeometry->addPrimitiveSet(primitiveSet.get());
    osg::ref_ptr<osg::Vec4Array> blackLineObstacleColors = new osg::Vec4Array(1);
    // (*blackLineObstacleColors)[0] = *black;
    (*blackLineObstacleColors)[0] = *darkBlue;
    blackLineObstacleGeometry->setColorArray(blackLineObstacleColors.get());
    blackLineObstacleGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    osg::ref_ptr<osg::Geode> blackLineObstacle = new osg::Geode();
    blackLineObstacle->addDrawable(blackLineObstacleGeometry.get());
    osg::ref_ptr<osg::Geometry> redLineObstacleGeometry = new osg::Geometry();
    redLineObstacleGeometry->setVertexArray(lineVertexArray.get());
    redLineObstacleGeometry->setNormalArray(normalArray.get());
    redLineObstacleGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
    primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 0, 4);
    redLineObstacleGeometry->addPrimitiveSet(primitiveSet.get());
    osg::ref_ptr<osg::Vec4Array> redLineObstacleColors = new osg::Vec4Array(1);
    (*redLineObstacleColors)[0] = *red;
    redLineObstacleGeometry->setColorArray(redLineObstacleColors.get());
    redLineObstacleGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    osg::ref_ptr<osg::Geode> redLineObstacle = new osg::Geode();
    redLineObstacle->addDrawable(redLineObstacleGeometry.get());
    osg::ref_ptr<osg::Switch> lineObstacleSwitch = new osg::Switch();
    lineObstacleSwitch->addChild(blackLineObstacle.get());
    osg::ref_ptr<osg::Sequence> lineObstacleSequence = new osg::Sequence();
    lineObstacleSequence->addChild(blackLineObstacle.get());
    lineObstacleSequence->addChild(redLineObstacle.get());
    lineObstacleSequence->setInterval(osg::Sequence::LOOP, 0, -1);
    lineObstacleSequence->setTime(0, 0.5f);
    lineObstacleSequence->setTime(1, 0.5f);
    lineObstacleSequence->setDuration(0.5f, -1);
    lineObstacleSequence->setMode(osg::Sequence::START);
    lineObstacleSwitch->addChild(lineObstacleSequence.get());
    lineObstacleSwitch->setSingleChildOn(0);
    osg::ref_ptr<osg::MatrixTransform> lineObstacleTransform = new osg::MatrixTransform();
    lineObstacleTransform->setMatrix(
                                     osg::Matrix::rotate(angle, 0.0, 1.0, 0.0) *
                                     osg::Matrix::translate(recentObstacleData->position.x, 0.0f, recentObstacleData->position.y));
    osg::ref_ptr<LineObstacleData> lineObstacleData = new LineObstacleData(recentObstacleData.get(), obstacleData, angle);
    lineObstacleTransform->setUserData(lineObstacleData.get());
    lineObstacleTransform->setUpdateCallback(lineObstacleUpdateCallback.get());
    lineObstacleTransform->addChild(lineObstacleSwitch.get());
    if (testing) {
        osg::ref_ptr<osg::MatrixTransform> noteTransform = new osg::MatrixTransform();
        noteTransform->setMatrix(
                                 osg::Matrix::rotate(osg::PI / 2.0f, 1.0, 0.0, 0.0));
        float characterSize = 160.0f;
        osg::ref_ptr<osgText::Text> text = new osgText::Text();
        text->setFont(font.get());
        text->setColor(*black);
        text->setCharacterSize(characterSize);
        text->setPosition(osg::Vec3(0.0f, 110.0f, 1.0f));
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);
        char value[64];
        sprintf(value, "%d", (int)(recentObstacleData->meanSeaLevel + 0.5));
        text->setText(value);
        osg::ref_ptr<osg::Geode> note = new osg::Geode();
        note->addDrawable(text.get());
        noteTransform->addChild(note.get());
        lineObstacleTransform->addChild(noteTransform.get());
    }
    if (allObstaclesInView) {
        rootTransform.addChild(lineObstacleTransform.get());
    }
    else {
        obstacleGroup->group->addChild(lineObstacleTransform.get());
    }
    lastObstacleData = obstacleData;
    //cout << "done" << endl;
}

ObstacleGroup *LiveMap::search(const Coordinates &position)
{
    for (OBSTACLEGROUPS::const_iterator anObstacleGroup = obstacleGroups->begin();
         anObstacleGroup != obstacleGroups->end();
         anObstacleGroup++) {
        ObstacleGroup &obstacleGroup = **anObstacleGroup;
        if (position.x >= obstacleGroup.minPosition.x && position.x < obstacleGroup.maxPosition.x &&
            position.y >= obstacleGroup.minPosition.y && position.y < obstacleGroup.maxPosition.y) {
            return &obstacleGroup;
        }
    }
    return NULL;
}

void LiveMap::readPhotos(osg::MatrixTransform &rootTransform) {
    osg::ref_ptr<osg::Group> photoGroup = new osg::Group();
    PHOTOENTRIES photoEntries = photoIndex->photoEntries;
    for (PHOTOENTRIES::const_iterator aPhotoEntryPair = photoEntries.begin(); aPhotoEntryPair != photoEntries.end(); aPhotoEntryPair++) {
        PhotoEntry &photoEntry = *aPhotoEntryPair->second;
        readPhoto(photoEntry, *photoGroup.get());
    }
    photoGroupSwitch = new osg::Switch();
    photoGroupSwitch->addChild(photoGroup.get());
    photoGroupSwitch->setValue(0, true);
    rootTransform.addChild(photoGroupSwitch.get());
    cout << "photoIndex=" << *photoIndex << endl;
}

void LiveMap::readPhoto(PhotoEntry &photoEntry, osg::Group &photoGroup) {
    Coordinates position;
    mapIndex->convert(photoEntry.coordinates, position);
    cout << "readPhoto position=" << position << ",vesselPosition=" << vesselPosition << endl;
    osg::ref_ptr<osg::MatrixTransform> photoPinTransform = new osg::MatrixTransform();
    photoPinTransform->setMatrix(osg::Matrix::scale(1.0f, 1.0f, 10000.0f) *
                                 osg::Matrix::rotate(osg::PI / 2.0f, 1.0f, 0.0f, 0.0f) *
                                 osg::Matrix::translate(position.x, 0.0f, position.y));
    photoPinTransform->addChild(photoPin.get());
    photoGroup.addChild(photoPinTransform.get());
    osg::ref_ptr<osg::MatrixTransform> photoTransform = new osg::MatrixTransform();
    photoTransform->setMatrix(osg::Matrix::rotate(osg::PI / 2.0f, 1.0f, 0.0f, 0.0f) *
                              osg::Matrix::translate(position.x, -10000.0f, position.y));
    photoEntry.photoPinTransform = photoPinTransform;
    photoEntry.photoTransform = photoTransform;
    photoTransform->setUserData(&photoEntry);
    photoTransform->addChild(photo.get());
    photoGroup.addChild(photoTransform.get());
}

bool LiveMap::isInHazardZone(ObstacleData &obstacleData, bool &inSafetyZone, double &clearance, double &level)
{
    level = 0.0;
    inSafetyZone = false;
    if (!fullZoneCircle) {
        double angle = vesselPosition.angle(obstacleData.position);
        if (cos(angle - trackAngle) > 0.0) {
            return false;
        }
    }
    level = obstacleData.meanSeaLevel + safetyHeight;
    if (vesselCoordinates.z > level) {
        return false;
    }
    double safetyClearance = safetyPosition.clearance(obstacleData.position),
        angle = safetyPosition.angle(obstacleData.position);
    inSafetyZone = safetyClearance <= safetyZoneRadius &&
        fabs(sin(angle - trackAngle)) * safetyClearance <= vesselZoneRadius;
    clearance = vesselPosition.clearance(obstacleData.position);
    return clearance <= vesselZoneRadius;
}

bool LiveMap::isInHazardZone(LineObstacleData &lineObstacleData, bool &inSafetyZone, double &clearance, double &level)
{
    level = 0.0;
    inSafetyZone = false;
    Coordinates nearestCoordinates;
    clearance = vesselPosition.clearance(lineObstacleData.obstacleData1->position, lineObstacleData.obstacleData2->position, &nearestCoordinates);
    if (!fullZoneCircle) {
        double angle = vesselPosition.angle(nearestCoordinates);
        if (cos(angle - trackAngle) > 0.0) {
            return false;
        }
    }
    Coordinates nearestSafetyCoordinates;
    double safetyClearance = safetyPosition.clearance(lineObstacleData.obstacleData1->position, lineObstacleData.obstacleData2->position, &nearestSafetyCoordinates), angle = safetyPosition.angle(nearestSafetyCoordinates);
    inSafetyZone = safetyClearance <= safetyZoneRadius &&
        fabs(sin(angle - trackAngle)) * safetyClearance <= vesselZoneRadius;
    Coordinates position, coordinates1, coordinates2;
    coordinates1.x = lineObstacleData.obstacleData1->position.x;
    coordinates1.y = lineObstacleData.obstacleData1->meanSeaLevel;
    coordinates1.z = 0.0;
    coordinates2.x = lineObstacleData.obstacleData2->position.x;
    coordinates2.y = lineObstacleData.obstacleData2->meanSeaLevel;
    coordinates2.z = 0.0;
    if (inSafetyZone) {
        position.x = nearestSafetyCoordinates.x;
        position.y = 0.0;
        position.z = 0.0;
        level = position.clearance(coordinates1, coordinates2) + safetyHeight;
        inSafetyZone = vesselCoordinates.z <= level;
    }
    if (clearance > vesselZoneRadius) {
        return false;
    }
    position.x = nearestCoordinates.x;
    position.y = 0.0;
    position.z = 0.0;
    level = position.clearance(coordinates1, coordinates2) + safetyHeight;
    return vesselCoordinates.z <= level;
}

METHODDEF(void) jpegErrorExit(j_common_ptr jpegDecompress)
{
    (*jpegDecompress->err->output_message)(jpegDecompress);
    throw jpegDecompress;
}

osg::MatrixTransform *LiveMap::showPhotoEntry(PhotoEntry *photoEntry, Coordinates &position)
{
    osg::MatrixTransform *matrixTransform = photoEntry->photoTransform.get();
    vesselCoordinates = photoEntry->coordinates;
    relocatingVessel = true;
    relocateVessel();
    if (singlePhotoSwitch->getValue(0)) {
        singlePhotoSwitch->setValue(0, false);
    }
    if (singlePhotoGroup->getNumChildren() > 0) {
        singlePhotoGroup->setChild(0, photoEntry->photoTransform.get());
    } else {
        singlePhotoGroup->addChild(photoEntry->photoTransform.get());
    }
    if (singlePhotoGroup->getNumChildren() > 1) {
        singlePhotoGroup->setChild(1, photoEntry->photoPinTransform.get());
    } else {
        singlePhotoGroup->addChild(photoEntry->photoPinTransform.get());
    }
    if (!singlePhotoSwitch->getValue(0)) {
        singlePhotoSwitch->setValue(0, true);
    }
    osg::Matrix matrix = manipulator->getMatrix();
    osg::Vec3 eye, center, up;
    matrix.getLookAt(eye, center, up);
    osg::Vec3 location;
    center.set(center.x(), center.y(), 0.0);
    /*
    viewer->getSceneHandlerList().at(0)->getSceneView()->projectObjectIntoWindow(center, location);
    */
    position.x = location.x();
    position.y = location.y();
    return matrixTransform;
}

void LiveMap::showPhoto(PhotoEntry *photoEntry, Coordinates *position, osg::MatrixTransform *matrixTransform, bool hasLayout)
{
    LiveMap::Layout layout;
    if (hasLayout) {
        osg::Vec3 trans = matrixTransform->getMatrix().getTrans();
        Coordinates photoPosition;
        photoPosition.x = trans.x();
        photoPosition.y = trans.z();
        double angle = cameraPosition.angle(photoPosition);
        if (fixedNorth) {
            angle -= osg::PI;
        }
        else {
            angle -= trackAngle + osg::PI / 2.0;
        }
        angle = fmod(angle, 2.0 * osg::PI);
        if (angle < 0.0) {
            angle += 2.0 * osg::PI;
        }
        if (angle >= 0.0 && angle < osg::PI / 2.0) {
            layout = LiveMap::LOWER_LEFT;
        } else if (angle >= osg::PI / 2.0 && angle < osg::PI) {
            layout = LiveMap::LOWER_RIGHT;
        } else if (angle >= osg::PI && angle < osg::PI + osg::PI / 2.0) {
            layout = LiveMap::UPPER_RIGHT;
        } else {
            layout = LiveMap::UPPER_LEFT;
        }
    } else {
        layout = this->layout;
    }
    if (photoEntry != this->photoEntry || layout != this->layout) {
        photoSwitch->setValue(0, false);
        readJPEG(photoEntry->filePath, layout, position);
        this->photoEntry = photoEntry;
        this->layout = layout;
        photoInfoVisible = false;
    }
    if (!photoSwitch->getValue(0)) {
        photoSwitch->setValue(0, true);
    }
}

void LiveMap::readJPEG(const string &filePath, Layout &layout, Coordinates *position)
{
    uint32 width, height, width1, height1, nComponents;
    // osg::ref_ptr<osg::Image> image = osgDB::readImageFile(filePath);
    FILE *file = fopen(filePath.c_str(), "rb");
    if (!file) {
        throw new string("File " + filePath + " not found");
    }
    struct jpeg_decompress_struct jpegDecompress;
    struct jpeg_error_mgr jpegErrorMgr;
    jpegDecompress.err = jpeg_std_error(&jpegErrorMgr);
    jpegErrorMgr.error_exit = jpegErrorExit;
    unsigned char *raster;
    try {
        jpeg_create_decompress(&jpegDecompress);
        jpeg_stdio_src(&jpegDecompress, file);
        jpeg_read_header(&jpegDecompress, TRUE);
        jpeg_start_decompress(&jpegDecompress);
        double factor = 1.0;
        unsigned width0 = projectionWidth, height0 = projectionHeight;
        if (layout > FULL) {
            width0 >>= 1;
            height0 >>= 1;
        }
        if (jpegDecompress.output_width > width0 ||
            jpegDecompress.output_height > height0) {
            if (jpegDecompress.output_width - width0 > jpegDecompress.output_height - height0) {
                factor = (double)width0 / (double)jpegDecompress.output_width;
            } else {
                factor = (double)height0 / (double)jpegDecompress.output_height;
            }
        }
        width1 = (unsigned)((double)jpegDecompress.output_width * factor + .5);
        height1 = (unsigned)((double)jpegDecompress.output_height * factor + .5);
        width = (uint32)powerOf2((double)width1 * 2.0);
        height = (uint32)powerOf2((double)height1 * 2.0);
        cout << "width=" << width << ",height=" << height << ",width1=" << width1 << ",height1=" << height1 << ",output_width=" << jpegDecompress.output_width << ",output_height=" << jpegDecompress.output_height << endl;
        nComponents = jpegDecompress.output_components % 2 ? jpegDecompress.output_components + 1 : jpegDecompress.output_components;
        raster = new unsigned char[width * height * nComponents];
        int row_stride = jpegDecompress.output_width * jpegDecompress.output_components;
        JSAMPARRAY sampleArray = (*jpegDecompress.mem->alloc_sarray)((j_common_ptr)&jpegDecompress, JPOOL_IMAGE, row_stride, 1);
        while (jpegDecompress.output_scanline < jpegDecompress.output_height) {
            jpeg_read_scanlines(&jpegDecompress, sampleArray, 1);
            unsigned i, sampleIndex = 0;
            int j, rasterOffset = (int)((double)jpegDecompress.output_scanline / (double)jpegDecompress.output_height * height1 + .5) * width * nComponents;
            for (i = 0; i < jpegDecompress.output_width; i++) {
                int rasterOffset1 = rasterOffset + (int)((double)i / (double)jpegDecompress.output_width * (double)width1 + .5) * nComponents;
                for (j = 0; j < jpegDecompress.output_components; j++)
                    raster[rasterOffset1 + j] = (*sampleArray)[sampleIndex++];
                raster[rasterOffset1 + nComponents - 1] = 0xff;
            }
            int rasterOffset1 = rasterOffset + width1 * nComponents;
            for (i = width1; i < width; i++) {
                for (j = 0; j < jpegDecompress.output_components; j++)
                    raster[rasterOffset1 + j] = 0xff;
                raster[rasterOffset1 + nComponents - 1] = 0x00;
                rasterOffset1 += nComponents;
            }
        }
        int rasterOffset = height1 * width * nComponents;
        unsigned scanline = height1;
        while (scanline < height) {
            unsigned i;
            int j;
            for (i = 0; i < width; i++) {
                for (j = 0; j < jpegDecompress.output_components; j++)
                    raster[rasterOffset + j] = 0xff;
                raster[rasterOffset + nComponents - 1] = 0x00;
                rasterOffset += nComponents;
            }
            scanline++;
        }
        jpeg_finish_decompress(&jpegDecompress);
    } catch (j_common_ptr) {
        jpeg_destroy_decompress(&jpegDecompress);
        throw new string("File " + filePath + " cannot be read");
    }
    jpeg_destroy_decompress(&jpegDecompress);
    osg::ref_ptr<osg::Image> image = new osg::Image();
    image->setImage(width, height, 1, GL_RGBA, nComponents == 1 ? GL_LUMINANCE : nComponents == 2 ? GL_LUMINANCE_ALPHA : nComponents == 3 ? GL_RGB : nComponents == 4 ? GL_RGBA : (GLenum)-1, GL_UNSIGNED_BYTE, raster, osg::Image::USE_NEW_DELETE);
    osg::Projection &photoProjection = (osg::Projection &)*photoSwitch->getChild(0);
    osg::ref_ptr<osg::Geometry> photoGeometry = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> photoVertexArray = new osg::Vec3Array(4);
    double x, y, x1, y1;
    switch (layout) {
    case FULL:
    case CENTER:
        x1 = x = (double)(projectionWidth - width1) / 2.0;
        y1 = y = (double)projectionHeight - (double)(projectionHeight - height1) / 2.0;
        break;
    case UPPER_LEFT:
        x = 0.0;
        y = (double)projectionHeight;
        x1 = (double)width1 / 2.0;
        y1 = (double)projectionHeight - (double)height1;
        break;
    case UPPER_RIGHT:
        x = (double)(projectionWidth - width1);
        y = (double)projectionHeight;
        x1 = (double)projectionWidth - (double)width1 / 2.0;
        y1 = (double)projectionHeight - (double)height1;
        break;
    case LOWER_LEFT:
        x = 0.0;
        y = (double)height1;
        x1 = (double)width1 / 2.0;
        y1 = y;
        break;
    default:
        x = (double)(projectionWidth - width1);
        y = (double)height1;
        x1 = (double)projectionWidth - (double)width1 / 2.0;
        y1 = y;
        break;
    }
    (*photoVertexArray)[0].set(x, y, 0.0);
    (*photoVertexArray)[1].set((*photoVertexArray)[0].x() + width, (*photoVertexArray)[0].y(), 0.0);
    (*photoVertexArray)[2].set((*photoVertexArray)[1].x(), (*photoVertexArray)[0].y() - height, 0.0);
    (*photoVertexArray)[3].set((*photoVertexArray)[0].x(), (*photoVertexArray)[2].y(), 0.0);
    photoGeometry->setVertexArray(photoVertexArray.get());
    photoGeometry->setNormalArray(normalArray.get());
    photoGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
    osg::ref_ptr<osg::Vec2Array> texCoordArray = new osg::Vec2Array(4);
    (*texCoordArray)[0].set(0.0f, 0.0f);
    (*texCoordArray)[1].set(1.0f, 0.0f);
    (*texCoordArray)[2].set(1.0f, 1.0f);
    (*texCoordArray)[3].set(0.0f, 1.0f);
    osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4);
    photoGeometry->addPrimitiveSet(primitiveSet.get());
    osg::ref_ptr<osg::Vec4Array> photoColors = new osg::Vec4Array(1);
    (*photoColors)[0] = *white;
    photoGeometry->setColorArray(photoColors.get());
    photoGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    photoGeometry->setTexCoordArray(0, texCoordArray.get());
    osg::ref_ptr<osg::Geode> photoGeode = new osg::Geode();
    photoGeode->addDrawable(photoGeometry.get());
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
    texture->setTextureSize(image->s(), image->t());
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_NEAREST);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    texture->setInternalFormatMode(osg::Texture::USE_S3TC_DXT3_COMPRESSION);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    texture->setImage(image.get());
    osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    stateSet->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);
    osg::ref_ptr<osg::MatrixTransform> photoTransform = new osg::MatrixTransform();
    photoTransform->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    photoTransform->setMatrix(osg::Matrix::identity());
    photoTransform->setStateSet(stateSet.get());
    photoTransform->addChild(photoGeode.get());
    if (photoProjection.getNumChildren() > 0) {
        photoProjection.setChild(0, photoTransform.get());
    }
    else {
        photoProjection.addChild(photoTransform.get());
    }
    if (position) {
        Coordinates photoPosition(x1, y1, 0.0);
        double angle = position->angle(photoPosition), xDelta = sin(angle) * 2.0, yDelta = cos(angle) * 2.0;
        osg::ref_ptr<osg::Vec3Array> lineVertexArray = new osg::Vec3Array(4);
        (*lineVertexArray)[0].set(position->x - xDelta, position->y + yDelta, 0.0);
        (*lineVertexArray)[1].set(photoPosition.x - xDelta, photoPosition.y + yDelta, 0.0);
        (*lineVertexArray)[2].set(photoPosition.x + xDelta, photoPosition.y - yDelta, 0.0);
        (*lineVertexArray)[3].set(position->x + xDelta, position->y - yDelta, 0.0);
        osg::ref_ptr<osg::Geometry> lineGeometry = new osg::Geometry();
        lineGeometry->setVertexArray(lineVertexArray.get());
        lineGeometry->setNormalArray(normalArray.get());
        lineGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
        primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4);
        lineGeometry->addPrimitiveSet(primitiveSet.get());
        osg::ref_ptr<osg::Vec4Array> lineColors = new osg::Vec4Array(1);
        (*lineColors)[0] = *black;
        lineGeometry->setColorArray(lineColors.get());
        lineGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
        osg::ref_ptr<osg::Geode> line = new osg::Geode();
        line->addDrawable(lineGeometry.get());
        osg::ref_ptr<osg::MatrixTransform> lineTransform = new osg::MatrixTransform();
        lineTransform->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        lineTransform->setMatrix(osg::Matrix::identity());
        lineTransform->addChild(line.get());
        if (photoProjection.getNumChildren() > 1) {
            photoProjection.setChild(1, lineTransform.get());
        } else {
            photoProjection.addChild(lineTransform.get());
        }
    } else if (photoProjection.getNumChildren() > 1) {
        photoProjection.removeChild(1);
    }
    photoSwitch->setValue(0, true);
}

osg::ref_ptr<TrackData> LiveMap::readNMEA(const string &filePath) {
    cout << "readNMEA " << filePath << endl;
    firstTrackData = NULL;
    previousTrackData = NULL;
    this->trackGroup = trackGroup;
    hasLastTrackVesselPosition = hasLastTrackVertexArray = false;
    lastTrackTime = trackTimeOffset = 0L;
    readingNMEA = true;
    ifstream in(filePath.c_str());
    if (!in.is_open()) {
        throw new string("NMEA file " + filePath + " not found");
    }
    nmea.readInputStream(in, &stopped);
    in.close();
    readingNMEA = false;
    firstTrackData->filePath = new string(filePath);
    return firstTrackData;
}

void LiveMap::readNMEAs(const string &dirPath, TrackUpdateCallback *trackUpdateCallback, osg::MatrixTransform &rootTransform)
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
        if (*fileName == '.') continue;
        string pathName = dirPath + "\\" + fileName;
        if (finddata.attrib & _A_SUBDIR) {
            readNMEAs(pathName, trackUpdateCallback, rootTransform);
            continue;
        }
#else
        dirent = readdir(dir);
        if (!dirent) break;
        fileName = dirent->d_name;
        if (*fileName == '.') continue;
        string pathName = dirPath + "/" + fileName;
        struct stat st;
        if (stat(pathName.c_str(), &st)) throw new string("stat failed");
        if (st.st_mode & S_IFDIR) {
            readNMEAs(pathName, trackUpdateCallback, rootTransform);
            continue;
        }
#endif
        osg::ref_ptr<osg::Switch> trackSwitch = new osg::Switch();
        trackSwitch->setUpdateCallback(trackUpdateCallback);
        osg::ref_ptr<osg::Group> trackGroup = new osg::Group();
        trackSwitch->addChild(trackGroup.get());
        trackSwitch->setValue(0, false);
        this->trackGroup = trackGroup.get();
        osg::ref_ptr<TrackData> trackData = readNMEA(pathName);
        trackSwitch->setUserData(trackData.get());
        rootTransform.addChild(trackSwitch.get());
        trackSwitches.insert(trackSwitches.end(), trackSwitch.get());
#ifdef WIN32
        if (_findnext(h, &finddata) != 0) {
            break;
        }
#endif
    }
}

osg::ref_ptr<TrackData> LiveMap::readGPT(const string &filePath, bool trackNotes) {
    cout << "readGPT " << filePath << endl;
    firstTrackData = NULL;
    previousTrackData = NULL;
    bool currentCountedAngle = countedAngle;
    countedAngle = true;
    Coordinates coordinates, position;
    hasLastTrackVesselPosition = hasLastTrackVertexArray = false;
    lastTrackTime = trackTimeOffset = 0L;
    ifstream in(filePath.c_str());
    if (!in) return NULL;
    string line;
    getline(in, line);
    cout << "GPT title=" << line << endl;
    for (;;) {
        if (!getline(in, line)) break;
        StringTokenizer st = StringTokenizer(line, ",", "\"", '\\', false);
        if (!st.hasMoreTokens()) continue;
        // RowNum
        string rowNum = Support::trim(Support::unquote(st.nextToken()));
        if (!rowNum.length() || !isdigit(rowNum.at(0))) continue;
        // TrackRowNum
        if (trackNotes)
            st.nextToken();
        // Lat
        coordinates.y = atof(Support::trim(Support::unquote(st.nextToken())).c_str());
        // Lon
        coordinates.x = atof(Support::trim(Support::unquote(st.nextToken())).c_str());
        coordinates.z = 2000.0;
        // Speed/ApproxHdg
        st.nextToken();
        // GpsDate
        string gpsDate = Support::trim(Support::unquote(st.nextToken())),
            gpsTime = Support::trim(Support::unquote(st.nextToken()));
        if (gpsDate.at(0) == '"') gpsDate = gpsDate.substr(1, gpsDate.length() - 2);
        if (gpsTime.at(0) == '"') gpsTime = gpsTime.substr(1, gpsTime.length() - 2);
        time_t trackTime = timeOf(gpsDate, gpsTime);
#ifdef DEBUG
        cout << coordinates << ",gpsDate=" << gpsDate << ",gpsTime=" << gpsTime << endl;
#endif
        vesselCoordinates = coordinates;
        readTrack(trackTime);
    }
    in.close();
    countedAngle = currentCountedAngle;
    firstTrackData->filePath = new string(filePath);
    return firstTrackData;
}

void LiveMap::readGPTs(const string &dirPath, TrackUpdateCallback *trackUpdateCallback, osg::MatrixTransform &rootTransform, bool trackNotes)
{
#ifdef WIN32
    struct _finddata_t finddata;
    intptr_t h = _findfirst((char *)(dirPath + "\\*.*").c_str(), &finddata);
    do
#else
        DIR *dir;
    struct dirent *dirent;
    dir = opendir(dirPath.c_str());
    if (!dir) throw new string("Directory " + dirPath + " not found");
    for (;;)
#endif
        {
            const char *fileName;
#ifdef WIN32
            fileName = finddata.name;
            if (*fileName == '.') continue;
            string pathName = dirPath + "\\" + fileName;
            if (finddata.attrib & _A_SUBDIR) {
                readGPTs(pathName, trackUpdateCallback, rootTransform, trackNotes);
                continue;
            }
#else
            dirent = readdir(dir);
            if (!dirent) break;
            fileName = dirent->d_name;
            if (*fileName == '.') continue;
            string pathName = dirPath + "/" + fileName;
            struct stat st;
            if (stat(pathName.c_str(), &st)) throw new string("stat failed");
            if (st.st_mode & S_IFDIR) {
                readGPTs(pathName, trackUpdateCallback, rootTransform, trackNotes);
                continue;
            }
#endif
            if (!trackNotes) {
                osg::ref_ptr<osg::Switch> trackSwitch = new osg::Switch();
                trackSwitch->setUpdateCallback(trackUpdateCallback);
                osg::ref_ptr<osg::Group> trackGroup = new osg::Group();
                trackSwitch->addChild(trackGroup.get());
                trackSwitch->setValue(0, false);
                showTracks = true;
                this->trackGroup = trackGroup.get();
                osg::ref_ptr<TrackData> trackData = readGPT(pathName, false);
                trackSwitch->setUserData(trackData.get());
                rootTransform.addChild(trackSwitch.get());
                trackSwitches.insert(trackSwitches.end(), trackSwitch.get());
            } else {
                showTracks = false;
                this->trackGroup = NULL;
                readGPT(pathName, trackNotes);
            }
        }
#ifdef WIN32
    while (_findnext(h, &finddata) == 0);
#endif
}

int main(int argc, char **argv)
{
    try {
        srand(clock());
        OpenThreads::Thread::Init();
        osg::ArgumentParser arguments(&argc, argv);
        arguments.getApplicationUsage()->setDescription(arguments.getApplicationName());
        arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName() + " [options] [jpg-filename 0-5]...");
        arguments.getApplicationUsage()->addCommandLineOption("--generateDatabase", "Generate database from map images");
        arguments.getApplicationUsage()->addCommandLineOption("--saveSerial <filename>", "Save NMEA input from serial port to a file");
        arguments.getApplicationUsage()->addCommandLineOption("-?", "Display these instructions");
        osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer(arguments);
        //viewer->setUpViewer(osgViewer::Viewer::STANDARD_SETTINGS);
        viewer->getUsage(*arguments.getApplicationUsage());
        if (arguments.read("-?")) {
            arguments.getApplicationUsage()->write(cout);
            exit(0);
        }
        arguments.reportRemainingOptionsAsUnrecognized();
        if (arguments.errors()) {
            arguments.writeErrorMessages(cout);
            exit(1);
        }
        viewer->addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));
        //viewer->requestContinuousUpdate(true);
        viewer->realize();
        LiveMap *liveMap = new LiveMap(viewer.get());
        for (int pos = 1; pos < arguments.argc(); pos++)
            if (arguments.isOption(pos))
                if (!strcmp(arguments[pos], "--generateDatabase")) {
                    liveMap->generateDatabase = true;
                    pos++;
                } else if (!strcmp(arguments[pos], "--saveSerial")) {
                    liveMap->nmea.outStream = new ofstream(arguments[pos + 1]);
                    pos += 2;
                }
        if (!liveMap->disableAudio) {
            PaError paError = Pa_Initialize();
            if (paError != paNoError) throw Support::makeMessage("PortAudio", Pa_GetErrorText(paError));
        }
        viewer->setRealizeOperation(new RealizeOperation(*liveMap, *viewer));
        for (int pos = 1; pos < arguments.argc(); pos++)
            if (!arguments.isOption(pos)) {
                LiveMap::Layout layout;
                switch (atoi(arguments[pos + 1])) {
                case 0:
                    layout = LiveMap::FULL;
                    break;
                case 1:
                    layout = LiveMap::CENTER;
                    break;
                case 2:
                    layout = LiveMap::UPPER_LEFT;
                    break;
                case 3:
                    layout = LiveMap::UPPER_RIGHT;
                    break;
                case 4:
                    layout = LiveMap::LOWER_LEFT;
                    break;
                default:
                    layout = LiveMap::LOWER_RIGHT;
                    break;
                }
                string filePath(arguments[pos]);
                liveMap->readJPEG(filePath, layout, NULL);
                pos++;
            }
        /*
        while (!viewer->done()) {
            viewer->sync();
            viewer->update();
            viewer->frame();
        }
        viewer->sync();
        */
        viewer->run();
        if (!liveMap->disableAudio) {
            PaError paError = Pa_Terminate();
            if (paError != paNoError) throw Support::makeMessage("PortAudio", Pa_GetErrorText(paError));
        }
        delete liveMap;
        viewer = NULL;
    } catch (string *message) {
        cout << *message << endl;
        delete message;
        exit(-1);
    }
    return 0;
}
