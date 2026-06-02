#include <osg/Object>
#include <osg/Notify>
#include <osg/Timer>

#include <osgDB/FileUtils>

#include <iostream>
#include <iomanip>
#include <string>

#include <osgART/PluginManager>
#include <osgART/Video>
#include <osgART/VideoConfig>
#include <osgDB/ReadFile>
#include "Datum.hpp"
#include "common.hpp"

using namespace osg;
using namespace osgART;
using namespace osgDB;

class PlaybackVideo : public Video
{
public:        
    // Standard Services
    
    /** 
     * Default constructor. It creates a video source from a configuration string
     * pointing to video image directory.
     */
    PlaybackVideo();
    
    /** 
     * Copy constructor.
     *
     */
    PlaybackVideo(const PlaybackVideo &, 
                  const CopyOp& copyop = CopyOp::SHALLOW_COPY);
    
    /** 
     * Destructor.
     *
     */
    ~PlaybackVideo();   

    virtual Object* cloneType() const 
    { 
        return new PlaybackVideo(); 
    }
    
    virtual Object* clone(const CopyOp& copyop) const 
    { 
        return new PlaybackVideo(*this,copyop); 
    }
    
    virtual bool isSameKindAs(const Object* obj) const 
    {
        return dynamic_cast<const PlaybackVideo*>(obj) != 0; 
    }
    
    virtual const char* libraryName() const 
    {
        return "osgART"; 
    }
    
    virtual const char* className() const 
    {
        return "PlaybackVideo";
    }
    
    /** 
     * Affectation operator.
     *
     */
    PlaybackVideo& operator = (const PlaybackVideo &);
    
    /**
     * Open the video stream. Access the video stream (hardware or file) and get an handle on it.
     */
    void open();
    
    /**
     * Close the video stream. Terminates the connection with the video stream and clean handle.
     */
    void close(bool = true);

    void seek(double time);

    /**
     * Start the video stream grabbing. Start to get image from the video stream. In function of the 
     * implementation on different platform, this function can run a thread, signal or 
     * real-time function.
     */
    void play();
    
    /**
     * Stop the video stream grabbing. Stop to get image from the video stream. In function 
     * of the implementation on different platform, this function can stop a thread, signal or 
     * real-time function. 
     */
    void pause();
    
    void rewind();

    /**
     * Update the video stream grabbing. Try to get an image of the video instance, usable 
     * by your application.
     */
    void update(NodeVisitor* nv);
    

    /** 
     * Deallocate image memory. Deallocates any internal memory allocated by the instance of this
     * class.
     */      
    void releaseImage();
    
    virtual VideoConfiguration* getVideoConfiguration();

    virtual bool relocateVessel(const Coordinates &coordinates);

private:

    osgDB::ifstream *videoListStream;

    VideoConfiguration m_config;

    Coordinates videoImageCoordinates;
    time_t startTimer;
    time_t videoImageTimer;
    time_t videoImageStartTimer;
    ref_ptr<Image> videoImage[2];
    int videoImageIndex;

    bool
    readVideoImageData(string &name);
    Image *
    readVideoImage();
    bool
    clear();

};
