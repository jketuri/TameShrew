
#include "PlaybackVideo.hpp"

PlaybackVideo::PlaybackVideo() : Video(),
                                 startTimer((time_t)0),
                                 videoImageTimer((time_t)0),
                                 videoImageStartTimer((time_t)0),
                                 videoImageIndex(0)
{
    memset(&m_config, 0, sizeof m_config);
    memset(videoImage, 0, sizeof videoImage);
}

PlaybackVideo::PlaybackVideo(const PlaybackVideo &, 
                             const CopyOp& copyop/* = CopyOp::SHALLOW_COPY*/)
{       
}

PlaybackVideo::~PlaybackVideo() 
{
    this->close(false);
}

PlaybackVideo& 
PlaybackVideo::operator=(const PlaybackVideo &)
{
    return *this;
}

void
PlaybackVideo::open()
{
    if (!clear()) {
        return;
    }
    // report the actual image size
    notify() << std::dec << "PlaybackVideo::open() size of video " << 
        videoImage[0]->s() << " x " << videoImage[0]->t() << std::endl;
    // create an image that same size (packing set to 1)
    this->allocateImage(videoImage[0]->s(), videoImage[0]->t(), videoImage[0]->r(),
                        videoImage[0]->getPixelFormat(), videoImage[0]->getDataType(), 1);

    this->setDataVariance(Object::DYNAMIC);
}

void
PlaybackVideo::close(bool waitForThread)
{
    videoListStream->close();
}

void
PlaybackVideo::seek(double time)
{
    close(false);
    if (!clear()) {
        return;
    }
    while (videoImageTimer - videoImageStartTimer < time) {
        string name;
        if (!readVideoImageData(name)) {
            return;
        }
    }
    ImageStream::seek(time);
}

void
PlaybackVideo::play()
{
    if (getStatus() != PAUSED) {
        close(false);
        if (!clear()) {
            return;
        }
    }
    if (videoImage[videoImageIndex].valid()) {
        videoImageStartTimer = videoImageTimer;
        time(&startTimer);
    }
    ImageStream::play();
}

void
PlaybackVideo::pause()
{
    ImageStream::pause();
}

void
PlaybackVideo::rewind()
{
    pause();
    seek(0);
    ImageStream::rewind();
}

void
PlaybackVideo::update(NodeVisitor* nv)
{
    if (getStatus() == PLAYING && videoImage[videoImageIndex].valid()) {
        time_t timer;
        time(&timer);
        if (timer - startTimer >= videoImageTimer - videoImageStartTimer) {
            OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(this->getMutex());
            Timer t;
            this->setImage(videoImage[videoImageIndex]->s(), videoImage[videoImageIndex]->t(), videoImage[videoImageIndex]->r(),
                           videoImage[videoImageIndex]->getInternalTextureFormat(),
                           videoImage[videoImageIndex]->getPixelFormat(),
                           videoImage[videoImageIndex]->getDataType(),
                           videoImage[videoImageIndex]->data(), Image::NO_DELETE, 1);

            // hopefully report some interesting data
            if (nv) {
                    
                const FrameStamp *framestamp = nv->getFrameStamp();

                if (framestamp && _stats.valid()) {
                    _stats->setAttribute(framestamp->getFrameNumber(),
                                         "Capture time taken", t.time_m());
                }
            }
            relocateVessel(videoImageCoordinates);
            videoImageIndex = videoImageIndex == 0 ? 1 : 0;
            videoImage[videoImageIndex] = readVideoImage();
            if (!videoImage[videoImageIndex].valid()) {
                rewind();
            }
        }
    }
}

VideoConfiguration* 
PlaybackVideo::getVideoConfiguration() 
{
    return &m_config;
}

bool
PlaybackVideo::relocateVessel(const Coordinates &coordinates)
{
    return false;
}

void
PlaybackVideo::releaseImage() 
{
}

bool
PlaybackVideo::readVideoImageData(string &name)
{
    if (videoListStream->eof()
        || !getline(*videoListStream, name)) {
        return false;
    }
    struct tm videoImageTime;
    memset(&videoImageTime, 0, sizeof videoImageTime);
    memset(&videoImageCoordinates, 0, sizeof videoImageCoordinates);
    string::size_type offset = 0, index = name.find_first_of('-');
    videoImageTime.tm_year = atoi(name.substr(offset, index).c_str()) - 1900;
    offset = index + 1;
    index = name.find_first_of('-', offset);
    videoImageTime.tm_mon = atoi(name.substr(offset, index).c_str()) - 1;
    offset = index + 1;
    index = name.find_first_of('_', offset);
    videoImageTime.tm_mday = atoi(name.substr(offset, index).c_str());
    offset = index + 1;
    index = name.find_first_of('_', offset);
    videoImageTime.tm_hour = atoi(name.substr(offset, index).c_str());
    offset = index + 1;
    index = name.find_first_of('_', offset);
    videoImageTime.tm_min = atoi(name.substr(offset, index).c_str());
    offset = index + 1;
    index = name.find_first_of('_', offset);
    videoImageTime.tm_sec = atoi(name.substr(offset, index).c_str());
    offset = index + 5;
    index = name.find_first_of('_', offset);
    videoImageCoordinates.y = atof(name.substr(offset, index).c_str());
    offset = index + 5;
    index = name.find_first_of('.', offset);
    videoImageCoordinates.x = atof(name.substr(offset, index).c_str());
    videoImageTimer = mktime(&videoImageTime);
    return true;
}

Image *
PlaybackVideo::readVideoImage()
{
    string name;
    if (!readVideoImageData(name)) {
        return NULL;
    }
    stringstream nameStream;
    nameStream << m_config.deviceconfig << FILE_SEPARATOR << name;
    name = nameStream.str();
    return readImageFile(name);
}

bool
PlaybackVideo::clear()
{
    string videoListFilename = m_config.deviceconfig;
    videoListFilename.append(".txt");
    videoListStream = new osgDB::ifstream(videoListFilename.c_str(), ios::app);
    videoImageIndex = 0;

    // check if the video was successfully opened
    if (!videoListStream->is_open()) {
        return false;
    }
    videoImage[0] = readVideoImage();
    if (!videoImage[0].valid()) {
        return false;
    }
    return true;
}
