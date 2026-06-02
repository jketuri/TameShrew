
#include "LiveView.hpp"

#define KML_PUSHPIN_URL "http://demo.pelicanmapping.com/icons/pushpin_yellow.png"

#define VP_DURATION 4.5 // time to fly to a viewpoint

//------------------------------------------------------------------------

namespace
{

    // toggles a node in response to a control event (checkbox)
    class ToggleNodeHandler : public ControlEventHandler
    {
        observer_ptr<Node> _node;

        virtual void onValueChanged( class Control* control, bool value )
        {
            ref_ptr<Node> safeNode = _node.get();
            if ( safeNode.valid() ) {
                safeNode->setNodeMask( value ? ~0 : 0 );
            }
        }

    public:

        ToggleNodeHandler(Node* node) : _node(node) { }
    };

    // flies to a viewpoint in response to control event (click)
    class ClickViewpointHandler : public ControlEventHandler
    {
        LiveView &liveView;
        Viewpoint _vp;

        virtual void onClick( class Control* control )
        {
            liveView.earthManipulator->setViewpoint( _vp, VP_DURATION );
        }

    public:

        ClickViewpointHandler(LiveView &liveView, const Viewpoint &vp) : liveView(liveView), _vp(vp) {
        }
    };

}

//------------------------------------------------------------------------

/**
 * Creates UI controls that show the map coordinates under the mouse
 */
class MouseCoordsControlFactory
{
    LiveView &liveView;

public:
    MouseCoordsControlFactory(LiveView &liveView) : liveView(liveView) {
    }

    Control* create(MapNode*         mapNode,
                    osgViewer::View* view     ) const
    {
        // readout for coordinates under the mouse
        LabelControl* readout = new LabelControl();
        readout->setBackColor( Color(Color::Black, 0.8) );
        readout->setHorizAlign( Control::ALIGN_RIGHT );
        readout->setVertAlign( Control::ALIGN_BOTTOM );

        Formatter* formatter = 
            liveView.useMGRS ? (Formatter*)new MGRSFormatter(MGRSFormatter::PRECISION_1M, 0L, MGRSFormatter::USE_SPACES) :
            liveView.useDMS  ? (Formatter*)new LatLongFormatter(LatLongFormatter::FORMAT_DEGREES_MINUTES_SECONDS) :
            liveView.useDD   ? (Formatter*)new LatLongFormatter(LatLongFormatter::FORMAT_DECIMAL_DEGREES) :
            0L;

        MouseCoordsTool* mcTool = new MouseCoordsTool( mapNode );
        mcTool->addCallback( new MouseCoordsLabelCallback(readout, formatter) );
        view->addEventHandler( mcTool );

        return readout;
    }

};

//------------------------------------------------------------------------

namespace
{

    class SkySliderHandler : public ControlEventHandler
    {
        LiveView &liveView;

        virtual void onValueChanged( class Control* control, float value )
        {
            liveView.sky->setDateTime( 2011, 3, 6, value );
        }

    public:

        SkySliderHandler(LiveView &liveView) : liveView(liveView)
        {
        }
    };

}

/**
 * Creates a set of controls for manipulating the Sky model.
 */
class SkyControlFactory
{
    LiveView &liveView;

public:
    SkyControlFactory(LiveView &liveView) : liveView(liveView) {
    }

    Control *create(SkyNode*         sky,
                    osgViewer::View* view) const
    {
        HBox* skyBox = new HBox();
        skyBox->setChildVertAlign( Control::ALIGN_CENTER );
        skyBox->setChildSpacing( 10 );
        skyBox->setHorizFill( true );

        skyBox->addControl( new LabelControl("Time: ", 16) );

        HSliderControl* skySlider = skyBox->addControl(new HSliderControl( 0.0f, 24.0f, 18.0f ));
        skySlider->setBackColor( Color::Gray );
        skySlider->setHeight( 12 );
        skySlider->setHorizFill( true, 200 );
        skySlider->addEventHandler( new SkySliderHandler(liveView) );

        return skyBox;
    }

};

//------------------------------------------------------------------------

namespace
{

    class ChangeSeaLevel : public ControlEventHandler
    {
        LiveView &liveView;

        virtual void onValueChanged( class Control* control, float value )
        {
            liveView.ocean->options().seaLevel() = value;
            liveView.ocean->dirty();
        }

    public:

        ChangeSeaLevel(LiveView &liveView) : liveView(liveView)
        {
        }

    };

    class ChangeLowFeather : public ControlEventHandler
    {
        LiveView &liveView;

        virtual void onValueChanged( class Control* control, float value )
        {
            liveView.ocean->options().lowFeatherOffset() = value;
            liveView.ocean->dirty();
        }

    public:

        ChangeLowFeather(LiveView &liveView) : liveView(liveView)
        {
        }

    };

    class ChangeHighFeather : public ControlEventHandler
    {
        LiveView &liveView;

        virtual void onValueChanged( class Control* control, float value )
        {
            liveView.ocean->options().highFeatherOffset() = value;
            liveView.ocean->dirty();
        }

    public:

        ChangeHighFeather(LiveView &liveView) : liveView(liveView)
        {
        }

    };

}

/**
 * Creates a set of controls for manipulating the Ocean surface model.
 */
class OceanControlFactory
{
    LiveView &liveView;

public:
    OceanControlFactory(LiveView &liveView) : liveView(liveView) {
    }

    Control *create(OceanSurfaceNode* ocean,
                    osgViewer::View*  view   ) const
    {
        VBox* main = new VBox();

        HBox* oceanBox1 = main->addControl(new HBox());
        oceanBox1->setChildVertAlign( Control::ALIGN_CENTER );
        oceanBox1->setChildSpacing( 10 );
        oceanBox1->setHorizFill( true );

        oceanBox1->addControl( new LabelControl("Sea Level: ", 16) );

        HSliderControl* mslSlider = oceanBox1->addControl(new HSliderControl( -250.0f, 250.0f, 0.0f ));
        mslSlider->setBackColor( Color::Gray );
        mslSlider->setHeight( 12 );
        mslSlider->setHorizFill( true, 200 );
        mslSlider->addEventHandler( new ChangeSeaLevel(liveView) );

        HBox* oceanBox2 = main->addControl(new HBox());
        oceanBox2->setChildVertAlign( Control::ALIGN_CENTER );
        oceanBox2->setChildSpacing( 10 );
        oceanBox2->setHorizFill( true );

        oceanBox2->addControl( new LabelControl("Low Feather: ", 16) );

        HSliderControl* lfSlider = oceanBox2->addControl(new HSliderControl( -1000.0, 250.0f, -100.0f ));
        lfSlider->setBackColor( Color::Gray );
        lfSlider->setHeight( 12 );
        lfSlider->setHorizFill( true, 200 );
        lfSlider->addEventHandler( new ChangeLowFeather(liveView) );

        HBox* oceanBox3 = main->addControl(new HBox());
        oceanBox3->setChildVertAlign( Control::ALIGN_CENTER );
        oceanBox3->setChildSpacing( 10 );
        oceanBox3->setHorizFill( true );

        oceanBox3->addControl( new LabelControl("High Feather: ", 16) );

        HSliderControl* hfSlider = oceanBox3->addControl(new HSliderControl( -500.0f, 500.0f, -10.0f ));
        hfSlider->setBackColor( Color::Gray );
        hfSlider->setHeight( 12 );
        hfSlider->setHorizFill( true, 200 );
        hfSlider->addEventHandler( new ChangeHighFeather(liveView) );

        return main;
    }

};

//------------------------------------------------------------------------

namespace
{
    class AnnoControlBuilder : public osg::NodeVisitor
    {
        LiveView &liveView;

    public:
        AnnoControlBuilder(LiveView &liveView)
            : liveView(liveView), osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {
            _grid = new osgEarth::Util::Controls::Grid();
            _grid->setHorizFill( true );
            _grid->setAbsorbEvents( true );
            _grid->setPadding( 5 );
            _grid->setBackColor( Color(Color::Black,0.5) );
        }

        void apply( osg::Node& node )
        {
            AnnotationData* data = dynamic_cast<AnnotationData*>( node.getUserData() );
            if ( data ) {
                ControlVector row;
                CheckBoxControl* cb = new CheckBoxControl( node.getNodeMask() != 0, new ToggleNodeHandler( &node ) );
                cb->setSize( 12, 12 );
                row.push_back( cb );
                std::string name = trim(data->getName());
                if ( name.empty() ) name = "<unnamed>";
                LabelControl* label = new LabelControl( name, 14.0f );
                unsigned relDepth = osg::clampAbove(3u, (unsigned int)this->getNodePath().size());
                label->setMargin(Gutter(0,0,0,(relDepth-3)*20));
                if ( data->getViewpoint() )
                    {
                        label->addEventHandler( new ClickViewpointHandler(liveView, *data->getViewpoint()) );
                        label->setActiveColor( Color::Blue );
                    }
                row.push_back( label );
                _grid->addControls( row );
            }
            traverse(node);
        }

        osgEarth::Util::Controls::Grid*             _grid;
    };
}

/**
 * Creates a UI Control reflecting all the named Annotations found in a
 * scene graph.
 */
class AnnotationGraphControlFactory
{
    LiveView &liveView;

public:
    AnnotationGraphControlFactory(LiveView &liveView) : liveView(liveView) {
    }

    Control*
    create(osg::Node*       graph,
           osgViewer::View* view) const
    {
        AnnoControlBuilder builder( liveView );
        if ( graph ) {
            graph->accept( builder );
        }

        return builder._grid;
    }

};

/**
 * Visitor that builds a UI control for a loaded KML file.
 */
class KMLUIBuilder : public NodeVisitor
{
    LiveView &liveView;
    ControlCanvas* _canvas;
    osgEarth::Util::Grid* _grid;

    void apply( Node& node )
    {
        AnnotationData* data = dynamic_cast<AnnotationData*>( node.getUserData() );
        if ( data ) {
            ControlVector row;
            CheckBoxControl* cb = new CheckBoxControl( node.getNodeMask() != 0, new ToggleNodeHandler( &node ) );
            cb->setSize( 12, 12 );
            row.push_back( cb );
            string name = data->getName().empty() ? "<unnamed>" : data->getName();
            LabelControl* label = new LabelControl( name, 14.0f );
            label->setMargin(Gutter(0,0,0,(this->getNodePath().size()-3)*20));
            if ( data->getViewpoint() ) {
                label->addEventHandler( new ClickViewpointHandler(liveView, *data->getViewpoint()) );
                label->setActiveColor( Color::Blue );
            }
            row.push_back( label );
            _grid->addControls( row );
        }
        traverse(node);
    }

public:

    KMLUIBuilder( LiveView &liveView, ControlCanvas* canvas ) : liveView(liveView), NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN), _canvas(canvas)
    {
        _grid = new osgEarth::Util::Grid();
        _grid->setAbsorbEvents( true );
        _grid->setPadding( 5 );
        _grid->setVertAlign( Control::ALIGN_TOP );
        _grid->setHorizAlign( Control::ALIGN_LEFT );
        _grid->setBackColor( Color(Color::Black,0.5) );
        _canvas->addControl( _grid );
    }

};

//------------------------------------------------------------------------

namespace
{
    /**
     * Handler that dumps the current viewpoint out to the console.
     */
    class ViewpointHandler : public GUIEventHandler
    {
        LiveView &liveView;
        vector<Viewpoint> _viewpoints;

        bool handle( const GUIEventAdapter& ea, GUIActionAdapter& aa )
        {
            if ( ea.getEventType() == ea.KEYDOWN ) {
                int index = (int)ea.getKey() - (int)'1';
                if ( index >= 0 && index < (int)_viewpoints.size() ) {
                    liveView.earthManipulator->setViewpoint( _viewpoints[index], VP_DURATION );
                }
                else if ( ea.getKey() == 'v' ) {
                    XmlDocument xml( liveView.earthManipulator->getViewpoint().getConfig() );
                    xml.store( cout );
                    cout << endl;
                }
                else if ( ea.getKey() == '?' ) {
                    liveView.controlPanel->setVisible( !liveView.controlPanel->visible() );
                }
                aa.requestRedraw();
            }
            return false;
        }

    public:
        ViewpointHandler(LiveView &liveView, const vector<osgEarth::Viewpoint>& viewpoints)
            : liveView(liveView), _viewpoints(viewpoints) { }
    };

}

/**
 * Creates a UI Control with a list of clickable viewpoints.
 */
class ViewpointControlFactory
{
public:
    LiveView &liveView;

    ViewpointControlFactory(LiveView &liveView) : liveView(liveView) {
    }

    Control*
    create(const std::vector<Viewpoint>& viewpoints,
           osgViewer::View*              view) const
    {
        osgEarth::Util::Controls::Grid* grid = 0L;

        if ( viewpoints.size() > 0 ) {
            // the viewpoint container:
            grid = new osgEarth::Util::Controls::Grid();
            grid->setChildSpacing( 0 );
            grid->setChildVertAlign( Control::ALIGN_CENTER );

            for( unsigned i = 0; i < viewpoints.size(); ++i ) {
                const Viewpoint& vp = viewpoints[i];
                Control* num = new LabelControl(Stringify() << (i+1), 16.0f, osg::Vec4f(1,1,0,1));
                num->setPadding( 4 );
                grid->setControl( 0, i, num );

                Control* vpc = new LabelControl(vp.getName().empty() ? "<no name>" : vp.getName(), 16.0f);
                vpc->setPadding( 4 );
                vpc->setHorizFill( true );
                vpc->setActiveColor( Color::Blue );
                vpc->addEventHandler( new ClickViewpointHandler(liveView, vp));
                grid->setControl( 1, i, vpc );
            }

            view->addEventHandler( new ViewpointHandler(liveView, viewpoints) );
        }

        return grid;
    }

};

//------------------------------------------------------------------------

namespace
{

    class ActionHandler : public ControlEventHandler
    {
        LiveView &liveView;
        const enum LiveView::Action _action;

        virtual void onClick( class Control* control )
        {
            switch (_action) {
            case LiveView::action_normal_zoom_level:
                liveView.normalZoomLevel();
                break;
            case LiveView::action_change_tether_mode:
                liveView.changeTetherMode();
                break;
            case LiveView::action_change_view:
                liveView.changeView();
                break;
            case LiveView::action_change_video_view:
                liveView.changeVideoView();
                break;
            case LiveView::action_write_video_snapshots:
                liveView.writeVideoSnapshots();
                break;
            case LiveView::action_record_pause:
                liveView.record();
                break;
            case LiveView::action_play_pause:
                liveView.play();
                break;
            case LiveView::action_stop:
                liveView.stop();
                break;
            case LiveView::action_cue:
                liveView.cue();
                break;
            case LiveView::action_rewind:
                liveView.rewind();
                break;
            }
        }

    public:

        ActionHandler(LiveView &liveView, const LiveView::Action action) : liveView(liveView), _action(action) {
        }
    };

    class ActionEventHandler : public GUIEventHandler
    {
        LiveView &liveView;

        bool handle( const GUIEventAdapter& ea, GUIActionAdapter& aa )
        {
            if ( ea.getEventType() == ea.PUSH && liveView.videoNumber > 0 ) {
                float left = (ea.getXnormalized() + 1.0f) / 2.0f * (float)liveView.playbackVideos[liveView.videoNumber]->s() / 4.0f,
                    right = left + ((float)liveView.playbackVideos[liveView.videoNumber]->s() / 2.0f),
                    bottom = (ea.getYnormalized() + 1.0f) / 2.0f * (float)liveView.playbackVideos[liveView.videoNumber]->t() / 4.0f,
                    top = bottom + ((float)liveView.playbackVideos[liveView.videoNumber]->t() / 2.0f);
                cout << "left=" << left << ",right=" << right << ",bottom=" << bottom << ",top=" << top << endl;
                VideoLayer* imageBackground = (VideoLayer *)liveView.playbackVideoModelSwitch->getChild(liveView.videoNumber);
                imageBackground->setProjectionMatrixAsOrtho2D(left, right, bottom, top);
                aa.requestRedraw();
            }
            return false;
        }

    public:
        ActionEventHandler(LiveView &liveView)
            : liveView(liveView) { }
    };

}

class ActionControlFactory
{
public:
    LiveView &liveView;

    ActionControlFactory(LiveView &liveView) : liveView(liveView) {
    }

    Control*
    create(const LiveView::Action* actions,
           const unsigned actionCount,
           osgViewer::View*              view) const
    {
        osgEarth::Util::Controls::Grid* grid = NULL;

        // the action container:
        grid = new osgEarth::Util::Controls::Grid();
        grid->setChildSpacing( 0 );
        grid->setChildVertAlign( Control::ALIGN_CENTER );

        for( unsigned i = 0; i < actionCount; ++i ) {
            Control* key = new LabelControl(LiveView::actionKeys[i], 16.0f, osg::Vec4f(1,1,0,1));
            key->setPadding( 4 );
            grid->setControl( 0, i, key );

            Control* bc = new LabelControl(LiveView::actionNames[i], 16.0f);
            bc->setPadding( 4 );
            bc->setHorizFill( true );
            bc->setActiveColor( Color::Blue );
            bc->addEventHandler( new ActionHandler(liveView, actions[i]));
            grid->setControl( 1, i, bc );
        }

        view->addEventHandler( new ActionEventHandler(liveView) );

        return grid;
    }

};

//------------------------------------------------------------------------

VideoCallback::VideoCallback(LiveView &liveView, Video *video, int videoIndex) : liveView(liveView), video(video), videoIndex(videoIndex), videoListStream(NULL), imageVesselCoordinatesUpdated(false)
{
}

void VideoCallback::operator()(Node *node, NodeVisitor *nodeVisitor)
{
    if (imageVesselCoordinatesUpdated) {
        video->update(nodeVisitor);
        time_t timer = 0;
        if (liveView.garminSeconds != 0.0) {
            timer = (time_t)liveView.garminTime + (time_t)liveView.garminSeconds;
        } else {
            time(&timer);
        }
        struct tm *tm = gmtime(&timer);
        stringstream nameStream;
        nameStream << (1900 + tm->tm_year) << "-" << (tm->tm_mon + 1) << "-" << tm->tm_mday << "_" << tm->tm_hour << "_" << tm->tm_min << "_" << tm->tm_sec << "_lat_" << imageVesselCoordinates.y << "_lon_" << imageVesselCoordinates.x;
        string name = nameStream.str();
        Support::findAndReplace(name, ".", "-");
        imageVesselCoordinatesUpdated = false;
        liveView.writeVideoImage(video, name + "." + IMAGE_FILE_TYPE, videoIndex);
    }
    this->NodeCallback::operator()(node, nodeVisitor);
}

EventHandler::EventHandler(LiveView &liveView) : liveView(liveView)
{
}

bool EventHandler::handle(const GUIEventAdapter &eventAdapter, GUIActionAdapter &actionAdapter) {
    (void)&actionAdapter;
    switch (eventAdapter.getEventType()) {
    case GUIEventAdapter::KEYDOWN: {
        switch (eventAdapter.getKey()) {
        case 'a':
            liveView.normalZoomLevel();
            break;
        case 'd':
            liveView.changeTetherMode();
            break;
        case 'g':
            liveView.changeVideoView();
            break;
        case 'o':
            liveView.record();
            break;
        case 'n':
            liveView.writeVideoSnapshots();
            break;
        case 'p':
            liveView.play();
            break;
        case 'q':
            liveView.stop();
            break;
        case 'r':
            liveView.changeView();
            break;
        }
        default:
            break;
    }
    }
    return false;
}

class LiveViewGarmin : public Garmin
{
    LiveView &liveView;

protected:

    void pvtData(D800_Pvt_Data_Type &pvtData)
    {
#ifdef DEBUG
        cout << "pvtData alt=" << pvtData.alt << ", epe=" << pvtData.epe << ", eph=" << pvtData.eph << ", epv=" << pvtData.epv << ", fix=" << pvtData.fix << ", tow=" << pvtData.tow << ", lon=" << pvtData.posn.lon << ", lat=" << pvtData.posn.lat << ", east=" << pvtData.east << ", north=" << pvtData.north << ", up=" << pvtData.up << ", msl_hght=" << pvtData.msl_hght << ", leap_scnds=" << pvtData.leap_scnds << ", wn_days=" << pvtData.wn_days << endl;
#endif
        if (pvtData.fix == DIM_2D || pvtData.fix == DIM_3D) {
            liveView.gpsVesselCoordinates.x = 180.0 * (pvtData.posn.lon / Support::PI);
            liveView.gpsVesselCoordinates.y = 180.0 * (pvtData.posn.lat / Support::PI);
            if (pvtData.fix == DIM_3D) {
                liveView.gpsVesselCoordinates.z = pvtData.alt + pvtData.msl_hght;
            }
            double velocityMetersInSecond = sqrt(pvtData.east * pvtData.east + pvtData.north * pvtData.north);
            liveView.vesselVelocity = velocityMetersInSecond * 60.0 * 60.0 / 1000.0;
            liveView.garminSeconds = (double)pvtData.wn_days * 24.0 * 60.0 * 60.0 + pvtData.tow - (double)pvtData.leap_scnds;
            liveView.relocateVessel(false);
        }
    }

public:

    LiveViewGarmin(LiveView &liveView) : liveView(liveView)
    {
    }

};

GpsListenerThread::GpsListenerThread(LiveView &liveView) : liveView(liveView)
{
}

GpsListenerThread::~GpsListenerThread()
{
}

void GpsListenerThread::run()
{
    try {
        cout << "GpsListenerThread beginning" << endl;
        iostream gpsStream(liveView.gpsBuffer);
        if (liveView.garmin) {
            LiveViewGarmin liveViewGarmin(liveView);
            liveViewGarmin.readInputOutputStream(gpsStream, NULL);
        } else {
            liveView.nmea.readInputStream(gpsStream, NULL);
        }
        cout << "GpsListenerThread ending" << endl;
    }
    catch (string *message) {
        cout << "GpsListenerThread: " << *message << endl;
        delete message;
        return;
    }
}

GGA::GGA(LiveView &liveView) : liveView(liveView)
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
    liveView.gpsVesselCoordinates.x = parseLongitude(dataFields[3], dataFields[4]);
    liveView.gpsVesselCoordinates.y = parseLatitude(dataFields[1], dataFields[2]);
    liveView.gpsVesselCoordinates.z = atof(dataFields[8].c_str());
    if (liveView.relocateVessel(false) && liveView.recordNMEA) {
        *liveView.nmeaOutStream << sentence << endl;
    }
#ifdef DEBUG
    cout << "GGA done" << endl;
#endif
}

RMC::RMC(LiveView &liveView) : liveView(liveView)
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
    liveView.gpsVesselCoordinates.x = parseLongitude(dataFields[4], dataFields[5]);
    liveView.gpsVesselCoordinates.y = parseLatitude(dataFields[2], dataFields[3]);
    liveView.vesselVelocity = liveView.groundSpeedInKmh ? atof(dataFields[6].c_str()) :
        liveView.knotsToKmh(atof(dataFields[6].c_str()));
    if (!liveView.countedAngle) {
        double trackAngle = PI / 2.0 - Support::toRadians(atof(dataFields[7].c_str()));
        if (trackAngle < 0.0) {
            trackAngle += 2.0 * PI;
        }
        liveView.lastTrackAngle = liveView.trackAngle;
        liveView.trackAngle = trackAngle;
    }
    string dateOfFix = dataFields[8];
#ifdef DEBUG
    cout << "RMC: liveView.vesselVelocity=" << liveView.vesselVelocity << endl;
#endif
    if (liveView.relocateVessel(false) && liveView.recordNMEA) {
        *liveView.nmeaOutStream << sentence << endl;
    }
}

VTG::VTG(LiveView &liveView) : liveView(liveView)
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
    liveView.vesselVelocity = liveView.groundSpeedInKmh ? atof(dataFields[6].c_str()) : liveView.knotsToKmh(atof(dataFields[6].c_str()));
#ifdef DEBUG
    cout << "VTG: liveView.vesselVelocity=" << liveView.vesselVelocity << endl;
#endif
}

LiveViewPlaybackVideo::LiveViewPlaybackVideo(LiveView &liveView) : liveView(liveView)
{
}

bool LiveViewPlaybackVideo::relocateVessel(const Coordinates &coordinates)
{
    liveView.gpsVesselCoordinates = coordinates;
    return liveView.relocateVessel(false);
}

VesselUpdateCallback::VesselUpdateCallback(LiveView &liveView) : liveView(liveView)
{
}

void VesselUpdateCallback::operator()(Node *node, NodeVisitor *nodeVisitor)
{
    liveView.relocateVessel(true);
    traverse(node, nodeVisitor);
}

const char *LiveView::actionNames[] = {"Normal zoom level", "Change tether mode", "Change view", "Change video view", "Write video snapshots", "Record/Pause", "Play/Pause", "Stop", "Cue", "Rewind"};
const char *LiveView::actionKeys[] = {"a", "d", "r", "g", "n", "o", "p", "q", "u", "i"};

void LiveView::normalZoomLevel()
{
    if (videoNumber > 0) {
        VideoLayer* imageBackground = (VideoLayer *)playbackVideoModelSwitch->getChild(videoNumber);
        imageBackground->setProjectionMatrixAsOrtho2D(0.0f, (float)playbackVideos[videoNumber]->s(), 0.0f, (float)playbackVideos[videoNumber]->t());
    }
}

void LiveView::changeTetherMode()
{
    if (earthManipulator->getTetherNode() != NULL) {
        earthManipulator->setTetherNode(NULL);
    } else {
        earthManipulator->setTetherNode(vesselTransform.get());
    }
}

void LiveView::changeView()
{
    if (viewNumber != 0) {
        modelSwitch->setValue(viewNumber, false);
    }
    if (viewNumber < 2) {
        viewNumber++;
    }
    else {
        viewNumber = 1;
    }
    modelSwitch->setValue(viewNumber, true);
}

void LiveView::changeVideoView()
{
    if (videoNumber < numberVideos) {
        videoNumber++;
    }
    else {
        videoNumber = 0;
    }
    if (modelSwitch->getValue(2)) {
        playbackVideoModelSwitch->setSingleChildOn(videoNumber);
    } else {
        videoModelSwitch->setSingleChildOn(videoNumber);
    }
}

void LiveView::writeVideoSnapshots()
{
    for (unsigned int videoIndex = 0; videoIndex < numberVideos; videoIndex++) {
        videoCallbacks[videoIndex]->imageVesselCoordinatesUpdated = true;
    }
}

void LiveView::record()
{
    recordVideoImages ^= true;
}

void LiveView::play()
{
    if (viewNumber == 2) {
        for (unsigned int videoIndex = 0; videoIndex < numberVideos; videoIndex++) {
            if (playbackVideos[videoIndex]->getStatus() == Video::PLAYING) {
                playbackVideos[videoIndex]->pause();
            } else {
                playbackVideos[videoIndex]->play();
            }
        }
    }
}

void LiveView::stop()
{
    if (viewNumber == 2) {
        for (unsigned int videoIndex = 0; videoIndex < numberVideos; videoIndex++) {
            playbackVideos[videoIndex]->rewind();
        }
    }
}

void LiveView::cue()
{
}

void LiveView::rewind()
{
}

void LiveView::addTexturedQuad(osgART::VideoGeode& geode,
                               float width, float height,
                               float x, float y)
{

	float maxU(1.0);
	float maxV(1.0);
	
	if (geode.getTextureMode() == osgART::VideoGeode::USE_TEXTURE_RECTANGLE) 
        {
            maxU = width;
            maxV = height;
        } else 
        {
            maxU = width / (float)osgART::nextPowerOfTwo((unsigned int)width);
            maxV = height / (float)osgART::nextPowerOfTwo((unsigned int)height);
        }
	
	osg::Geometry* geometry = new osg::Geometry();
		
	osg::Vec3Array* vcoords = new osg::Vec3Array();
	geometry->setVertexArray(vcoords);

	osg::Vec2Array* tcoords = new osg::Vec2Array();
	geometry->setTexCoordArray(0, tcoords);

	vcoords->push_back(osg::Vec3(x, y, 0.0f));
	vcoords->push_back(osg::Vec3(x + width, y, 0.0f));
	vcoords->push_back(osg::Vec3(x + width, y + height, 0.0f));
	vcoords->push_back(osg::Vec3(x,  y + height, 0.0f));

	tcoords->push_back(osg::Vec2(0.0f, maxV));
	tcoords->push_back(osg::Vec2(maxU, maxV));
	tcoords->push_back(osg::Vec2(maxU, 0.0f));
	tcoords->push_back(osg::Vec2(0.0f, 0.0f));

	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	geode.addDrawable(geometry);
}

Group* LiveView::createImageBackground(Image *video) {
    return createImageBackground(video, NULL, 0.0, 0.0);
}

Group* LiveView::createImageBackground(Image *video, VideoLayer *layer,
                                       float x, float y) {
    if (layer == NULL) {
        layer = new VideoLayer();
        layer->setSize(*video);
    }
#ifdef WIN32
    VideoGeode *_geode = new VideoGeode(VideoGeode::USE_TEXTURE_RECTANGLE, video);
#else
    VideoGeode *_geode = new VideoGeode(VideoGeode::USE_TEXTURE_2D, video);
#endif
    addTexturedQuad(*_geode, video->s(), video->t(), x, y);
    layer->addChild(_geode);
    return layer;
}

void LiveView::setupVideo() {
	// preload the video and tracker
    _video_id = PluginManager::instance()->load("osgart_video_artoolkit2");
#ifdef DEBUG
    cout << "loaded osgart video with id " << _video_id << endl;
#endif
	//int _tracker_id = PluginManager::instance()->load("osgart_tracker_sstt");
    _tracker_id = PluginManager::instance()->load("osgart_tracker_artoolkit2");
#ifdef DEBUG
    cout << "loaded osgart tracker with id " << _tracker_id << endl;
#endif
    videoCallbacks = new VIDEO_CALLBACK[numberVideos];
    memset(videoCallbacks, 0, numberVideos * sizeof(VIDEO_CALLBACK));
    playbackVideos = new VIDEO[numberVideos];
    memset(playbackVideos, 0, numberVideos * sizeof(VIDEO));
}

int LiveView::addVideo(osg::Group *group, const string &usbDevice, int videoIndex) {
	// Load a video plugin.
#ifdef DEBUG
    cout << "adding osgart video " << videoIndex << " with id " << _video_id << endl;
#endif
	ref_ptr<Video> video
		= dynamic_cast<Video *>(PluginManager::instance()->get(_video_id));
#ifdef DEBUG
    cout << "added osgart video " << videoIndex << endl;
#endif
    if (!video.valid()) {
		osgEarth::notify(FATAL) << "Failed to open video plugin!" << endl;
        return 1;
    }
    video = dynamic_cast<Video *>(video->clone(CopyOp::DEEP_COPY_ALL));
	// check if an instance of the video stream could be started
	if (!video.valid()) {   
		// Without video an AR application can not work. Quit if none found.
		osgEarth::notify(FATAL) << "Could not initialize video plugin!" << endl;
        return 1;
	}
	VideoConfiguration *videoConfiguration = video->getVideoConfiguration();
    string deviceconfig
        = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<dsvl_input>"
        "<camera show_format_dialog=\"false\" friendly_name=\"" + usbDevice + "\">"
        "<pixel_format>"
        "<RGB32 flip_h=\"false\" flip_v=\"true\"/>"
        "</pixel_format>"
        "</camera>"
        "</dsvl_input>";
    videoConfiguration->deviceconfig = deviceconfig;
	// Open the video. This will not yet start the video stream but will
	// get information about the format of the video which is essential
	// for the connected tracker
	video->open();
#ifdef DEBUG
    cout << "opened osgart video " << videoIndex << endl;
#endif
    if (videoIndex == 0) {
        monitorVideoLayer = new VideoLayer();
        monitorVideoLayer->setSize(video->s() * 3, video->t() * 3);
        monitorVideoLayer->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
        videoModelSwitch->addChild(monitorVideoLayer.get());

        playbackMonitorVideoLayer = new VideoLayer();
        playbackMonitorVideoLayer->setSize(video->s() * 3, video->t() * 3);
        playbackMonitorVideoLayer->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
        playbackVideoModelSwitch->addChild(playbackMonitorVideoLayer.get());
    }
    if (videoIndex == 0 && useTracker) {
        ref_ptr<MatrixTransform> arTransform = new MatrixTransform();
        ref_ptr<Tracker> tracker = 
            dynamic_cast<Tracker *>(PluginManager::instance()->get(_tracker_id));
#ifdef DEBUG
        cout << "opened osgart tracker" << endl;
#endif
        if (!tracker.valid()) {
            // Without tracker an AR application can not work. Quit if none found.
            osgEarth::notify(FATAL) << "Could not initialize tracker plugin!" << endl;
            return 1;
        }
        // get the tracker calibration object
        ref_ptr<Calibration> calibration = tracker->getOrCreateCalibration();
        // load a calibration file
        if (!calibration->load(string("data/camera_para.dat"))) {
            // the calibration file was non-existing or couldn't be loaded
            osgEarth::notify(FATAL) << "Non existing or incompatible calibration file" << endl;
            return 1;
        }
        // set the image source for the tracker
        tracker->setImage(video.get());
        TrackerCallback::addOrSet(group, tracker.get());
        ref_ptr<Marker> marker = tracker->addMarker("single;data/patt.hiro;80;0;0");
#ifdef DEBUG
        cout << "added osgart marker" << endl;
#endif
        //ref_ptr<Marker> marker = tracker->addMarker("peugeot_logo.jpg;8;7.6");
        if (!marker.valid()) {
            // Without marker an AR application can not work. Quit if none found.
            osgEarth::notify(FATAL) << "Could not add marker!" << endl;
            return 1;
        }
        marker->setActive(true);
        attachDefaultEventCallbacks(arTransform.get(), marker.get());
        arTransform->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");
        ref_ptr<Group> videoBackground = createImageBackground(video.get());
        videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
        ref_ptr<Camera> camera = calibration->createCamera();
        camera->addChild(arTransform.get());
        camera->addChild(videoBackground.get());
        videoModelSwitch->addChild(camera.get());
    } else {
        ref_ptr<Group> videoBackground = createImageBackground(video.get());
        videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
        videoModelSwitch->addChild(videoBackground.get());
    }
    createImageBackground(video.get(), monitorVideoLayer.get(),
                          videoIndex % 2 == 0 ? 0.0 : video->s() * 2,
                          videoIndex / 2 == 0 ? 0.0 : video->t() * 2);
    VIDEO_CALLBACK videoCallback
        = new VideoCallback(*this, video.get(), videoIndex);
    videoCallbacks[videoIndex] = videoCallback;
    addEventCallback(group, videoCallback);
    stringstream dirStream;
    dirStream << dataDirectory << FILE_SEPARATOR << "video_" << videoIndex;
    string dir = dirStream.str();
    Support::makeDirs(dir);
    cout << "video write directory: " << dir << endl;
	ref_ptr<Video> playbackVideo = new LiveViewPlaybackVideo(*this);
    playbackVideos[videoIndex] = playbackVideo.get();
	VideoConfiguration *playbackVideoConfiguration
        = playbackVideo->getVideoConfiguration();
    playbackVideoConfiguration->deviceconfig = dir;
    playbackVideo->open();
    ref_ptr<Group> videoBackground = createImageBackground(playbackVideo.get());
    videoBackground->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
    playbackVideoModelSwitch->addChild(videoBackground.get());
    createImageBackground(playbackVideo.get(), playbackMonitorVideoLayer.get(),
                          videoIndex % 2 == 0 ? 0.0 : playbackVideo->s() * 2,
                          videoIndex / 2 == 0 ? 0.0 : playbackVideo->t() * 2);
	video->start();
    return 0;
}

void LiveView::openVideoListStreams()
{
    for (unsigned int videoIndex = 0; videoIndex < numberVideos; videoIndex++) {
        stringstream nameStream;
        nameStream << dataDirectory << FILE_SEPARATOR << "video_" << videoIndex << ".txt";
        videoCallbacks[videoIndex]->videoListStream = new osgDB::ofstream(nameStream.str().c_str(), ios::app);
    }
}

void LiveView::closeVideoListStreams()
{
    for (unsigned int videoIndex = 0; videoIndex < numberVideos; videoIndex++) {
        if (videoCallbacks[videoIndex]->videoListStream != NULL) {
            videoCallbacks[videoIndex]->videoListStream->close();
            delete videoCallbacks[videoIndex]->videoListStream;
            videoCallbacks[videoIndex]->videoListStream = NULL;
        }
    }
}

LiveView::LiveView() : nmea(183), nmeaOutStream(NULL), videoCallbacks(NULL), earthManipulator(NULL), controlPanel(NULL), ocean(NULL), sky(NULL), objectPlacer(NULL), darkBlue(NULL), gpsListenerThread(NULL), countedAngle(false), disableAudio(false), disableGPS(false), fixedNorth(false), fullZoneCircle(false), garmin(false), groundSpeedInKmh(false), recordNMEA(false), sceneryMode(false), showTracks(false), showingTracks(false), stopped(false), testing(false), useMGRS(false), useDMS(false), useDD(false), useTracker(false), recordVideoImages(false), _video_id(0), _tracker_id(0), angleSeconds(0.0), lastTrackAngle(0.0), vesselAngle(0.0), vesselVelocity(0.0), vesselZoneRadius(0.0), trackAngle(0.0), trackTimeRate(0.0), trackToleranceSecs(0.0), videoNumber(0), viewNumber(0), numberVideos(0), usbPacketSize(0L), idVendor(0), idProduct(0), gpsBuffer(NULL), garminSeconds(0.0) {
    memset(&angleTime, 0, sizeof angleTime);
    memset(&gpsTime, 0, sizeof gpsTime);
    struct tm tm;
    memset(&tm, 0, sizeof tm);
    tm.tm_mday = 31;
    tm.tm_mon = 11;
    tm.tm_year = 89;
    garminTime = mktime(&tm) - _timezone;
}

LiveView::~LiveView() {
    raise(SIGINT);
    int error = 0;
    if (gpsListenerThread && gpsListenerThread->cancel() ||
        gpsListenerThread && gpsListenerThread->join()) {
        error = errno;
    }
    closeVideoListStreams();
    if (nmeaOutStream != NULL) {
        nmeaOutStream->close();
        nmeaOutStream = NULL;
    }
    delete videoCallbacks;
    delete playbackVideos;
    if (error != 0) {
        throw Support::makeMessage("OpenThreads", strerror(error));
    }
}

void LiveView::readProperties()
{
    common::Properties properties;
    properties.load("LiveView.properties");
    for (common::Properties::iterator property = properties.begin();
         property != properties.end(); property++) {
        const string &name = property->first;
        string &value = *property->second;
        if (name == "NMEADirectory") {
            NMEADirectory = Support::toPhysicalPath(value);
        } else if (name == "altitude") {
            gpsVesselCoordinates.z = atof(value.c_str());
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
        } else if (name == "fixedNorth") {
            fixedNorth = Support::isTrueOrFalse(value);
        } else if (name == "fontName") {
            fontName = Support::toPhysicalPath(value);
        } else if (name == "fullZoneCircle") {
            fullZoneCircle = Support::isTrueOrFalse(value);
        } else if (name == "garmin") {
            garmin = Support::isTrueOrFalse(value);
        } else if (name == "groundSpeedInKmh") {
            groundSpeedInKmh = Support::isTrueOrFalse(value);
        } else if (name == "gpsUsbDeviceGuid") {
            gpsUsbDeviceGuid = value;
        } else if (name == "idProduct") {
            idProduct = atoi(value.c_str());
        } else if (name == "idVendor") {
            idVendor = atoi(value.c_str());
        } else if (name == "latitude") {
            gpsVesselCoordinates.y = atof(value.c_str());
        } else if (name == "longitude") {
            gpsVesselCoordinates.x = atof(value.c_str());
        } else if (name == "numberVideos") {
            numberVideos = atoi(value.c_str());
        } else if (name == "recordNMEA") {
            recordNMEA = Support::isTrueOrFalse(value);
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
        } else if (name == "vesselModel") {
            vesselModel = value;
        } else if (name == "recordVideoImages") {
            recordVideoImages = Support::isTrueOrFalse(value);
        } else {
            throw new string("Unknown property " + name);
        }
    }
}

void LiveView::readNMEA(const string &filePath) {
    cout << "readNMEA " << filePath << endl;
    osgDB::ifstream in(filePath.c_str());
    if (!in.is_open()) {
        throw new string("NMEA file " + filePath + " not found");
    }
    nmea.readInputStream(in, &stopped);
    in.close();
}

void LiveView::extrapolateVesselCoordinates()
{
    if (gpsTime.time == 0L || vesselVelocity == 0) {
        return;
    }
    timeb currentTime;
    ftime(&currentTime);
    double seconds = toSeconds(currentTime, gpsTime);
    //  if (seconds < 1.0 / 24.0) return;
    double distance = vesselVelocity * (seconds / 60.0 / 60.0) * 1000.0;
    /*
      #ifdef DEBUG
      cout << "seconds=" << seconds << endl;
      cout << "distance=" << distance << endl;
      cout << "trackAngle=" << trackAngle << endl;
      #endif
    */
    Coordinates coordinatesRadians;
    GeoMath::destination(DegreesToRadians(gpsVesselCoordinates.y), DegreesToRadians(gpsVesselCoordinates.x),
                         trackAngle, distance, coordinatesRadians.y, coordinatesRadians.x);
    vesselCoordinates.x = RadiansToDegrees(coordinatesRadians.x);
    vesselCoordinates.y = RadiansToDegrees(coordinatesRadians.y);
    /*
      vesselCoordinates.x = gpsVesselCoordinates.x + cos(trackAngle) * distance;
      vesselCoordinates.y = gpsVesselCoordinates.y + sin(trackAngle) * distance;
      // vesselCoordinates.z = gpsVesselCoordinates.z;
      */
}

bool LiveView::relocateVessel(bool extraPolate)
{
    if (lastVesselCoordinates == gpsVesselCoordinates) {
        if (extraPolate) {
            extrapolateVesselCoordinates();
        }
        else {
            return false;
        }
    } else {
        if (gpsTime.time != 0L) {
            for (unsigned int videoIndex = 0; videoIndex < numberVideos; videoIndex++) {
                if (!videoCallbacks[videoIndex]->imageVesselCoordinatesUpdated) {
                    if (videoCallbacks[videoIndex]->imageVesselCoordinates != gpsVesselCoordinates) {
                        videoCallbacks[videoIndex]->imageVesselCoordinates = gpsVesselCoordinates;
                        if (recordVideoImages) {
                            videoCallbacks[videoIndex]->imageVesselCoordinatesUpdated = true;
                        }
                    }
                }
            }
        } else {
            Viewpoint viewpoint(gpsVesselCoordinates.x, gpsVesselCoordinates.y, gpsVesselCoordinates.z, 0.0, -89.0, 1344.1420285);
            earthManipulator->setViewpoint(viewpoint, VP_DURATION );
        }
        ftime(&gpsTime);
        lastVesselCoordinates = gpsVesselCoordinates;
        vesselCoordinates = gpsVesselCoordinates;
        setVesselAngle();
        vesselAngleTransform->setMatrix(Matrix::rotate(-vesselAngle, 0.0f, 0.0f, 1.0f));
        //vesselAngleTransform->setMatrix(Matrix::rotate(vesselAngle - PI / 2.0f, 0.0f, 0.0f, 1.0f));
    }
    Matrixd matrix;
    objectPlacer->createPlacerMatrix(vesselCoordinates.y, vesselCoordinates.x, vesselCoordinates.z, matrix);
    vesselTransform->setMatrix(matrix);
    return true;
}

void LiveView::setVesselAngle()
{
    timeb currentTime;
    ftime(&currentTime);
    if (angleTime.time > 0L && angleSeconds > 0.0) {
        double seconds = toSeconds(currentTime, angleTime),
            factor = seconds / angleSeconds;
        if (factor > 1.0) {
            factor = 1.0;
        }
        vesselAngle = lastTrackAngle + (trackAngle - lastTrackAngle) * factor;
    } else {
        vesselAngle = trackAngle;
    }
    if (angleVesselCoordinates != gpsVesselCoordinates) {
        if (countedAngle) {
            lastTrackAngle = trackAngle;
            //trackAngle = gpsVesselCoordinates.angle(angleVesselCoordinates);
            trackAngle = osgEarth::GeoMath::bearing(DegreesToRadians(angleVesselCoordinates.y),
                                                    DegreesToRadians(angleVesselCoordinates.x),
                                                    DegreesToRadians(gpsVesselCoordinates.y),
                                                    DegreesToRadians(gpsVesselCoordinates.x));
        }
        if (angleTime.time > 0L) {
            angleSeconds = toSeconds(currentTime, angleTime);
        }
        angleTime = currentTime;
        angleVesselCoordinates = gpsVesselCoordinates;
    }
}

void LiveView::writeVideoImage(Video *video, string name, int videoIndex)
{
    stringstream nameStream;
    nameStream << dataDirectory << FILE_SEPARATOR << "video_" << videoIndex << FILE_SEPARATOR << name;
    video->getMutex().lock();
    writeImageFile(*video, nameStream.str());
    video->getMutex().unlock();
    *videoCallbacks[videoIndex]->videoListStream << name << endl;
}

void LiveView::flipVideoImages()
{
    for (unsigned int videoIndex = 0; videoIndex < numberVideos; videoIndex++) {
        stringstream nameStream;
        nameStream << dataDirectory << FILE_SEPARATOR << "video_" << videoIndex;
        flipVideoImages(nameStream.str());
    }
}

void LiveView::flipVideoImages(const string &dirPath)
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
            flipVideoImages(pathName);
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
        if (stat(pathName.c_str(), &st)) {
            throw new string("stat failed");
        }
        if (st.st_mode & S_IFDIR) {
            flipVideoImages(pathName);
            continue;
        }
#endif
        flipVideoImage(pathName);
#ifdef WIN32
        if (_findnext(h, &finddata) != 0) {
            break;
        }
#endif
    }
}

void LiveView::flipVideoImage(const string &filePath) {
    cout << "Flipping video image " << filePath << endl;
    ref_ptr<Image> videoImage = readImageFile(filePath);
    if (!videoImage) {
        return;
    }
    videoImage->flipVertical();
    stringstream nameStream;
    writeImageFile(*videoImage, filePath);
}

void LiveView::setupKeySwitchMatrixManipulator(Viewer &viewer, ArgumentParser &arguments)
{
    // set up the camera manipulators.
    earthManipulator = new EarthManipulator();
    earthManipulator->getSettings()->setArcViewpointTransitions( true );
	/*
      KeySwitchMatrixManipulator *keyswitchManipulator = new KeySwitchMatrixManipulator;
      keyswitchManipulator->addMatrixManipulator(GUIEventAdapter::KEY_F1, "Earth", earthManipulator);
      keyswitchManipulator->addMatrixManipulator(GUIEventAdapter::KEY_F2, "Trackball", new TrackballManipulator());
      keyswitchManipulator->addMatrixManipulator(GUIEventAdapter::KEY_F3, "Flight", new FlightManipulator());
      keyswitchManipulator->addMatrixManipulator(GUIEventAdapter::KEY_F4, "Drive", new DriveManipulator());
      keyswitchManipulator->addMatrixManipulator(GUIEventAdapter::KEY_F5, "Terrain", new TerrainManipulator());
      {
      string pathfile;
      char keyForAnimationPath = '5';
      while (arguments.read("-p", pathfile)) {
      AnimationPathManipulator* apm = new AnimationPathManipulator(pathfile);
      if (apm || !apm->valid()) {
      unsigned int num = keyswitchManipulator->getNumMatrixManipulators();
      keyswitchManipulator->addMatrixManipulator(keyForAnimationPath, "Path", apm);
      keyswitchManipulator->selectMatrixManipulator(num);
      ++keyForAnimationPath;
      }
      }
      }
      keyswitchManipulator->selectMatrixManipulator(0);
      viewer.setCameraManipulator(keyswitchManipulator);
	*/
    viewer.setCameraManipulator(earthManipulator);
}

void LiveView::setupApplicationUsage(ArgumentParser &arguments)
{
    ApplicationUsage *applicationUsage = arguments.getApplicationUsage();
    applicationUsage->setApplicationName(arguments.getApplicationName());
    applicationUsage->setDescription(arguments.getApplicationName() + " is the standard OpenSceneGraph example which loads and visualises 3d models.");
    applicationUsage->setCommandLineUsage(arguments.getApplicationName() + " [options] filename ...");
    applicationUsage->addCommandLineOption("--animateSky", "animates the sun across the sky");
    applicationUsage->addCommandLineOption("--autoclip", "activates the auto clip-plane handler");
    applicationUsage->addCommandLineOption("--image <filename>", "Load an image and render it on a quad");
    applicationUsage->addCommandLineOption("--dem <filename>", "Load an image/DEM and render it on a HeightField");
    applicationUsage->addCommandLineOption("--flipVideoImages", "flips all video images vertically and exits");
    applicationUsage->addCommandLineOption("--kml <kml file>", "Reads in a KML file.");
    applicationUsage->addCommandLineOption("--login <url> <username> <password>", "Provide authentication information for http file access.");
    applicationUsage->addCommandLineOption("--numberVideos <number of cameras>", "Provide number of video cameras.");
    applicationUsage->addCommandLineOption("--sky", "activates the atmospheric model");
    applicationUsage->addCommandLineOption("--ocean", "activates the ocean surface model");
    applicationUsage->addCommandLineOption("--dms", "format coordinates as degrees/minutes/seconds");
    applicationUsage->addCommandLineOption("--mgrs", "format coordinates as MGRS");
    applicationUsage->addKeyboardMouseBinding("d", "Change camera to vessel tether mode");
    applicationUsage->addKeyboardMouseBinding("g", "Change video camera view");
    applicationUsage->addKeyboardMouseBinding("i", "Rewind");
    applicationUsage->addKeyboardMouseBinding("n", "Write video snapshot images");
    applicationUsage->addKeyboardMouseBinding("o", "Record/Pause video images");
    applicationUsage->addKeyboardMouseBinding("p", "Play/Pause video images");
    applicationUsage->addKeyboardMouseBinding("q", "Stop play/record video images");
    applicationUsage->addKeyboardMouseBinding("r", "Change view");
    applicationUsage->addKeyboardMouseBinding("u", "Cue");
    applicationUsage->addKeyboardMouseBinding("v", "Dump viewport");
    applicationUsage->addKeyboardMouseBinding("?", "Toggle control panel");
}

void LiveView::setupNMEA()
{
    nmea.nmeaSentences.insert(NMEASENTENCES::value_type(string("GPGGA"), new GGA(*this)));
    nmea.nmeaSentences.insert(NMEASENTENCES::value_type(string("GPRMC"), new RMC(*this)));
    nmea.nmeaSentences.insert(NMEASENTENCES::value_type(string("GPVTG"), new VTG(*this)));
}

Node *LiveView::getVessel()
{
    ref_ptr<Geometry> vesselGeometry = new Geometry();
    ref_ptr<Vec3Array> vesselVertexArray = new Vec3Array(28);
    float x1 = 300.0f, x2 = 200.0f, x3 = 30.0f, y1 = 400.0f, y2 = 300.0f, y3 = 30.0f, z = 30.0f;
    (*vesselVertexArray)[0].set(0.0f, y1, 0.0f);
    (*vesselVertexArray)[1].set(-x1, 0.0f, 0.0f);
    (*vesselVertexArray)[2].set(-x2, 0.0f, -z);
    (*vesselVertexArray)[3].set(0.0f, y2, -z);
    (*vesselVertexArray)[4].set(-x1, 0.0f, 0.0f);
    (*vesselVertexArray)[5].set(-x1, -y1, 0.0f);
    (*vesselVertexArray)[6].set(-x2, -y2, -z);
    (*vesselVertexArray)[7].set(-x2, 0.0f, -z);
    (*vesselVertexArray)[8].set(-x1, -y1, 0.0f);
    (*vesselVertexArray)[9].set(x1, -y1, 0.0f);
    (*vesselVertexArray)[10].set(x2, -y2, -z);
    (*vesselVertexArray)[11].set(-x2, -y2, -z);
    (*vesselVertexArray)[12].set(x1, -y1, 0.0f);
    (*vesselVertexArray)[13].set(x1, 0.0f, 0.0f);
    (*vesselVertexArray)[14].set(x2, 0.0f, -z);
    (*vesselVertexArray)[15].set(x2, -y2, -z);
    (*vesselVertexArray)[16].set(x1, 0.0f, 0.0f);
    (*vesselVertexArray)[17].set(0.0f, y1, 0.0f);
    (*vesselVertexArray)[18].set(0.0f, y2, -z);
    (*vesselVertexArray)[19].set(x2, 0.0f, -z);
    (*vesselVertexArray)[20].set(-x2, y3, -z);
    (*vesselVertexArray)[21].set(x2, y3, -z);
    (*vesselVertexArray)[22].set(-x2, -y3, -z);
    (*vesselVertexArray)[23].set(x2, -y3, -z);
    (*vesselVertexArray)[24].set(-x3, -y2, -z);
    (*vesselVertexArray)[25].set(-x3, y2, -z);
    (*vesselVertexArray)[26].set(x3, -y2, -z);
    (*vesselVertexArray)[27].set(x3, y2, -z);
    normalArray = new Vec3Array(1);
    (*normalArray)[0].set(0.0f, 0.0f, 1.0f);
    vesselGeometry->setVertexArray(vesselVertexArray.get());
    vesselGeometry->setNormalArray(normalArray.get());
    vesselGeometry->setNormalBinding(Geometry::BIND_OVERALL);
    ref_ptr<PrimitiveSet> primitiveSet = new DrawArrays(PrimitiveSet::POLYGON, 0, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new DrawArrays(PrimitiveSet::POLYGON, 4, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new DrawArrays(PrimitiveSet::POLYGON, 8, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new DrawArrays(PrimitiveSet::POLYGON, 12, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new DrawArrays(PrimitiveSet::POLYGON, 16, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new DrawArrays(PrimitiveSet::QUAD_STRIP, 20, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    primitiveSet = new DrawArrays(PrimitiveSet::QUAD_STRIP, 24, 4);
    vesselGeometry->addPrimitiveSet(primitiveSet.get());
    darkBlue = new Vec4(0.0f, 0.0f, 0.3f, 1.0f);
    ref_ptr<Vec4Array> vesselColors = new Vec4Array(1);
    (*vesselColors)[0] = *darkBlue;
    vesselGeometry->setColorArray(vesselColors.get());
    vesselGeometry->setColorBinding(Geometry::BIND_OVERALL);
    Geode *vessel = new Geode();
    vessel->addDrawable(vesselGeometry.get());
    return vessel;
}

void LiveView::setupVessel()
{
    osg::ref_ptr<osg::Node> vessel;
    if (!vesselModel.empty()) {
        vessel = readNodeFile(dataDirectory + FILE_SEPARATOR + vesselModel);
    } else {
        vessel = getVessel();
    }
    vesselTransform = new MatrixTransform();
    vesselAngleTransform = new MatrixTransform();
    vesselAngleTransform->addChild(vessel.get());
    vesselTransform->addChild(vesselAngleTransform.get());
    ref_ptr<VesselUpdateCallback> vesselUpdateCallback = new VesselUpdateCallback(*this);
    vesselTransform->setUpdateCallback(vesselUpdateCallback.get());
}

//------------------------------------------------------------------------

int LiveView::run(int argc, char **argv)
{
    ArgumentParser arguments(&argc, argv);
    setupApplicationUsage(arguments);
    unsigned int helpType = arguments.readHelpType();
    if (helpType != 0) {
        arguments.getApplicationUsage()->write(cout, helpType);
        return 1;
    }
    // any option left unread are converted into errors to write out later.
    arguments.reportRemainingOptionsAsUnrecognized();
    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors()) {
        arguments.writeErrorMessages(cout);
        return 1;
    }
    if (arguments.argc() <= 1) {
        arguments.getApplicationUsage()->write(cout, ApplicationUsage::COMMAND_LINE_OPTION);
        return 1;
    }
    readProperties();
    setupNMEA();
    DisplaySettings::instance()->setMinimumNumStencilBits( 8 );
    Viewer viewer(arguments);

    useMGRS       = arguments.read("--mgrs");
    useDMS        = arguments.read("--dms");
    useDD         = arguments.read("--dd");
    bool useSky        = arguments.read("--sky");
    bool animateSky = arguments.read("--animateSky");
    if (animateSky) {
        useSky = true;
    }
    bool useOcean      = arguments.read("--ocean");
    bool useCoords     = arguments.read("--coords") || useMGRS || useDMS || useDD;
    bool useOrtho      = arguments.read("--ortho");
    bool useAutoClip   = arguments.read("--autoclip");
    bool doVideoImageFlip = arguments.read("--flipVideoImages");
    string kmlFile;
    arguments.read( "--kml", kmlFile );
    string url, username, password;
    if (arguments.read("--login", url, username, password)) {
        if (!osgDB::Registry::instance()->getAuthenticationMap()) {
            osgDB::Registry::instance()->setAuthenticationMap(new AuthenticationMap);
            osgDB::Registry::instance()->getAuthenticationMap()->addAuthenticationDetails(url, new AuthenticationDetails(username, password));
        }
    }
    arguments.read( "--numberVideos", numberVideos );
    if (doVideoImageFlip) {
        recordVideoImages = false;
        flipVideoImages();
        return 0;
    }

    // load the .earth file from the command line.
    ref_ptr<Node> earthNode = readNodeFiles( arguments );
    if (!earthNode.valid()) {
        cout << "Unable to load earth model." << endl;
        arguments.getApplicationUsage()->write(cout, ApplicationUsage::NO_HELP);
        return 1;
    }

    // install our default manipulator:
    setupKeySwitchMatrixManipulator(viewer, arguments);

    ref_ptr<Group> earthGroup = new Group();
    earthGroup->addChild( earthNode );

    ref_ptr<osgEarth::MapNode> mapNode = osgEarth::MapNode::findMapNode( earthNode );
    if ( !mapNode.valid() ) {
        cout << "Loaded scene graph does not contain a MapNode - aborting" << endl;
        return 0L;
    }
    // install a canvas for any UI controls we plan to create:
    ControlCanvas* canvas = ControlCanvas::get(&viewer, false);

    Container* mainContainer = canvas->addControl( new VBox() );
    mainContainer->setBackColor( Color(Color::Black, 0.8) );
    mainContainer->setHorizAlign( Control::ALIGN_LEFT );
    mainContainer->setVertAlign( Control::ALIGN_BOTTOM );

    // look for external data:
    const Config& externals = mapNode->externalConfig();

    const Config& skyConf         = externals.child("sky");
    const Config& oceanConf       = externals.child("ocean");
    const Config& annoConf        = externals.child("annotations");
    const Config& declutterConf   = externals.child("decluttering");
    Config        viewpointsConf  = externals.child("viewpoints");

    // Loading a viewpoint list from the earth file:
    if ( !viewpointsConf.empty() ) {
        // read in viewpoints, if any
        vector<Viewpoint> viewpoints;
        const ConfigSet& children = viewpointsConf.children();
        if ( children.size() > 0 )
            {
                for( ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i ) {
                    viewpoints.push_back( Viewpoint(*i) );
                }
            }
        if ( viewpoints.size() > 0 )
            {
                Control* viewpointControl = ViewpointControlFactory(*this).create(viewpoints, &viewer);
                if ( viewpointControl ) {
                    mainContainer->addControl( viewpointControl );
                }
            }
    }

    // Adding a sky model:
    if ( useSky || !skyConf.empty() ) {
        double hours = skyConf.value( "hours", 12.0 );
        sky = new SkyNode( mapNode->getMap() );
        sky->setDateTime( 2011, 3, 6, hours );
        sky->attach( &viewer );
        earthGroup->addChild( sky );
        Control* skyControl = SkyControlFactory(*this).create(sky, &viewer);
        if ( skyControl ) {
            mainContainer->addControl( skyControl );
        }
    }

    // Adding an ocean model:
    if ( useOcean || !oceanConf.empty() ) {
        ocean = new OceanSurfaceNode( mapNode, oceanConf);
        if ( ocean ) {
            earthGroup->addChild( ocean );
            Control* oceanControl = OceanControlFactory(*this).create(ocean, &viewer);
            if ( oceanControl ) {
                mainContainer->addControl( oceanControl );
            }
        }
    }

    // Load a KML file if specified
    if ( !kmlFile.empty() ) {
        KMLOptions kmlOptions;
        kmlOptions.declutter() = true;
        kmlOptions.defaultIconImage() = URI(KML_PUSHPIN_URL).getImage();

        Node* kml = KML::load( URI(kmlFile), mapNode, kmlOptions );
        if ( kml )
            {
                Control* annotationGraphControl = AnnotationGraphControlFactory(*this).create(kml, &viewer);
                if ( annotationGraphControl )
                    {
                        annotationGraphControl->setVertAlign( Control::ALIGN_TOP );
                        canvas->addControl( annotationGraphControl);
                    }

                earthGroup->addChild( kml );

            }
    }

    // Annotations in the map node externals:
    if ( !annoConf.empty() ) {
        Group* annotations = 0L;
        AnnotationRegistry::instance()->create( mapNode, annoConf, annotations );
        if ( annotations )
            {
                earthGroup->addChild( annotations );
            }
    }

    // Configure the de-cluttering engine for labels and annotations:
    if ( !declutterConf.empty() ) {
        Decluttering::setOptions( DeclutteringOptions(declutterConf) );
    }
    
    // Configure the mouse coordinate readout:
    if ( useCoords ) { 
        Control* mouseCoords = MouseCoordsControlFactory(*this).create(mapNode, &viewer);
        canvas->addControl(mouseCoords);
    }

    // Configure for an ortho camera:
    if ( useOrtho ) {
        earthManipulator->getSettings()->setCameraProjection( EarthManipulator::PROJ_ORTHOGRAPHIC );
    }

    if (useAutoClip ) {
        viewer.getCamera()->addCullCallback( new AutoClipPlaneCullCallback(mapNode) );
    }

    earthGroup->addChild( canvas );
    // osgEarth benefits from pre-compilation of GL objects in the pager. In newer versions of
    // OSG, this activates OSG's IncrementalCompileOpeartion in order to avoid frame breaks.
    viewer.getDatabasePager()->setDoPreCompile( true );
    setupVessel();

    earthGroup->addChild(vesselTransform.get());

    modelSwitch = new Switch();
    modelSwitch->addChild(earthGroup.get());

    videoModelSwitch = new Switch();
    playbackVideoModelSwitch = new Switch();

    vector<string> videoDevices = USBFinder::findUSBDevices("Image");
    numberVideos = numberVideos == 0 ? videoDevices.size() : min(numberVideos, videoDevices.size());
    setupVideo();

    ref_ptr<Group> sceneGroup = new Group();
    for (unsigned int videoIndex = 0; videoIndex < numberVideos; videoIndex++) {
        if (addVideo(sceneGroup.get(), videoDevices[videoIndex], videoIndex) != 0) {
            cout << "failed opening video " << videoIndex << endl;
            return 0;
        }
    }
    Container* controlContainer = canvas->addControl( new VBox() );

    controlContainer->setBackColor( Color(Color::Black, 0.8) );
    controlContainer->setHorizAlign( Control::ALIGN_RIGHT );
    controlContainer->setVertAlign( Control::ALIGN_BOTTOM );

    Action actions[] = {action_change_tether_mode, action_change_view, action_change_video_view, action_write_video_snapshots, action_record_pause, action_play_pause, action_stop, action_cue, action_rewind};
    Control *actionControl = ActionControlFactory(*this).create(actions, sizeof actions / sizeof(Action), &viewer);
    controlContainer->addControl(actionControl);

    videoModelSwitch->addChild(canvas);
    playbackVideoModelSwitch->addChild(canvas);

    videoModelSwitch->setSingleChildOn(0);
    modelSwitch->addChild(videoModelSwitch.get());
    playbackVideoModelSwitch->setSingleChildOn(0);
    modelSwitch->addChild(playbackVideoModelSwitch.get());
    modelSwitch->setSingleChildOn(0);
    sceneGroup->addChild(modelSwitch.get());

    viewer.setSceneData(sceneGroup);

    viewer.addEventHandler(new EventHandler(*this));

    // add some stock OSG handlers:
    viewer.addEventHandler(new HelpHandler(arguments.getApplicationUsage()));
    viewer.addEventHandler(new RecordCameraPathHandler);
    viewer.addEventHandler(new ScreenCaptureHandler);
    viewer.addEventHandler(new StatsHandler());
    viewer.addEventHandler(new WindowSizeHandler());
    viewer.addEventHandler(new ThreadingHandler());
    viewer.addEventHandler(new LODScaleHandler());
    viewer.addEventHandler(new StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

    objectPlacer = new ObjectPlacer(earthNode.get());
    relocateVessel(false);
    if (recordNMEA) {
        nmeaOutStream = new osgDB::ofstream((dataDirectory + FILE_SEPARATOR + "nmea.txt").c_str());
    }
    if (!disableGPS) {
        earthManipulator->setTetherNode(vesselTransform.get());
        if (!serialPortName.empty()) {
            cout << "Reading GPS from serial port" << endl;
            SerialBuffer *serialBuffer = new SerialBuffer(serialPortName);
            serialBuffer->setSerialPortParameters(4800, 8, SerialBuffer::StopBits1, SerialBuffer::ParityNone, SerialBuffer::FlowControlNone);
            gpsBuffer = serialBuffer;
        } else if (idVendor || !gpsUsbDeviceGuid.empty()) {
#ifdef USE_WIN_USB
            cout << "Reading GPS from USB GUID=" << gpsUsbDeviceGuid << endl;
            gpsBuffer = new USBBuffer(gpsUsbDeviceGuid, true, false, false, true);
#else
            cout << "Reading GPS from USB idVendor=" << idVendor << ", idProduct=" << idProduct << endl;
            gpsBuffer = new USBBuffer(idVendor, idProduct, true, false, false);
#endif
        } else {
            cout << "Reading GPS from socket" << endl;
            gpsBuffer = new SocketBuffer(socketHost, socketPort);
        }
        cout << "Starting GPS listener thread" << endl;
        gpsListenerThread = new GpsListenerThread(*this);
        int rv = gpsListenerThread->start();
        if (rv) {
            throw Support::makeMessage("OpenThreads", strerror(rv == -1 ? errno : rv));
        }
        cout << "GPS listener thread started" << endl;
    }
    openVideoListStreams();
    viewer.realize();
    return viewer.run();
}

int main(int argc, char **argv)
{
    int returnValue = 0;
    LiveView *liveView = NULL;
    osgEarth::setNotifyLevel(ALWAYS);
    try {
        liveView = new LiveView();
        returnValue = liveView->run(argc, argv);
    } catch (string *message) {
        cout << *message << endl;
        delete message;
    }
    if (liveView) {
        delete liveView;
    }
    return returnValue;
}
