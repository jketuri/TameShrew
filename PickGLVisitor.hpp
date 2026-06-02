/* --------------------------------------------------------------------------
 *
 *	PickGLVisitor
 *
 * --------------------------------------------------------------------------
 *	
 *	prog:	max rheiner;mrn@paus.ch
 *	date:	1/10/2001	(m/d/y)
 *
 * --------------------------------------------------------------------------
 */

#ifndef _PICKGLVISITOR_H
#define _PICKGLVISITOR_H

#include <float.h>
#include <algorithm>
#include <locale>
#include <map>
#include <stack>
#include <vector>
#include <osg/gl>
#include <osg/Transform>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Node>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/DrawPixels>
#include <osgGA/TrackballManipulator>
#include <osg/NodeVisitor>
#include <osg/BoundingSphere>
#include <osg/BoundingBox>
#include <osg/Matrix>
#include <osg/Drawable>
#include <osg/StateSet>
#include <osg/State>
#include <osg/Camera>
#include <osg/Impostor>
#include <osg/MatrixTransform>
#include <osg/ShadowVolumeOccluder>
#include <osgUtil/CullVisitor>

using namespace osg;
using namespace osgUtil;
using namespace std;

namespace glPicking {

  class CullViewState : public osg::Referenced
  {
  public:

    CullViewState() : _clippingVolume(*new osg::ShadowVolumeOccluder)
    {
      _matrix = NULL;
      _inverse = NULL;
      _ratio2 = 0.002f*0.002f;
      _bbCornerNear = 8; // note this is an error value, valid range is 0..7 
      _bbCornerFar = 8;  // these error values are used to show a unset corner.
    }


    osg::Matrix *_matrix;
    osg::Matrix *_inverse;
    osg::Vec3    _eyePoint;
    osg::Vec3    _centerPoint;
    osg::Vec3    _lookVector;
    osg::Vec3    _upVector;
    unsigned int _bbCornerFar;
    unsigned int _bbCornerNear;
    float        _ratio2;

    osg::ShadowVolumeOccluder &_clippingVolume;

    enum 
      {
        NO_CULLING                  = 0x00,
        FRUSTUM_LEFT_CULLING        = 0x01,
        FRUSTUM_RIGHT_CULLING       = 0x02,
        FRUSTUM_BOTTOM_CULLING      = 0x04,
        FRUSTUM_TOP_CULLING         = 0x08,
        FRUSTUM_NEAR_CULLING        = 0x10,
        FRUSTUM_FAR_CULLING         = 0x20,
        VIEW_FRUSTUM_CULLING        = 0x3F,
        SMALL_FEATURE_CULLING       = 0x40,
        ENABLE_ALL_CULLING          = 0x7F
      };
        
    typedef unsigned int CullingMode;

    inline bool isCulled(const osg::BoundingSphere& sp, CullingMode& mode) const
    {
      if (!sp.valid()) return true;

      //            if (!_clippingVolume.contains(sp,mode)) return true;
      if (!_clippingVolume.contains(sp)) return true;

      if (mode&SMALL_FEATURE_CULLING)
        {
          osg::Vec3 delta(sp._center-_eyePoint);
          if (sp.radius2()<delta.length2()*_ratio2)
            {
              return true;
            }
        }
      return false;
    }
        
    inline bool isCulled(const osg::BoundingBox& bb, CullingMode mode) const
    {
      if (!bb.valid()) return true;

      //            return !_clippingVolume.contains(bb,mode);
      return !_clippingVolume.contains(bb);
    }

  protected:

    ~CullViewState() 
    {}

  };

  typedef std::stack< unsigned int >	IdStack;


  ///////////////////////////////////////////////////////////////////////////////
  // HitContainer
  class HitContainer
  {
  public:
    HitContainer():_node(NULL),_drawable(NULL){}
    HitContainer(const HitContainer& hit):_node(hit._node),_drawable(hit._drawable){}
    HitContainer(osg::Node* node)
    {
      _node=node;
      _drawable=NULL;
    }
    HitContainer(osg::Drawable* drawable)
    {
      _node=NULL;
      _drawable=drawable;
    }

    osg::Node*			 _node;
    osg::Drawable*       _drawable;
  };


  typedef std::vector<HitContainer>		HitContainerList;

  // HitContainer
  ///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  // Hit
  class Hit
  {
  public:
    Hit(const Hit& hit)
    {
      _hitObjectPaths=hit._hitObjectPaths;
		
      _zDepth1=hit._zDepth1;
      _zDepth2=hit._zDepth2;
    }
     
    Hit(HitContainerList hitObjectPaths,float zDepth1,float zDepth2)
    {
      _hitObjectPaths=hitObjectPaths;
		
      _zDepth1=zDepth1;
      _zDepth2=zDepth2;
    }

    float				_zDepth1;
    float				_zDepth2;

    HitContainerList	_hitObjectPaths;
  };

  typedef std::vector<Hit>		HitList;

  // Hit
  ///////////////////////////////////////////////////////////////////////////////



  class PickVisitor : public osg::NodeVisitor
  {
  public:

    PickVisitor();
    virtual ~PickVisitor();

    // modes to control how PickVisitor reports hits
    enum HitReportingMode 
      {
        ONLY_NEAREST_HIT,
        ALL_HITS,
        ONLY_FARTHEST_HIT
      };

    // 
    virtual bool		pick(osg::Viewport&	viewport,
                                     osg::Camera&	camera,
                                     osg::Node&		startNode,
                                     osg::Vec2&		pickPoint,
                                     osg::Vec2&		pickSize);

    /*
      virtual bool		pick(const osg::Viewport&	viewport,
      const osg::Matrix&		projection,
      const osg::Matrix&		modelView,
      const osg::Node&		startNode,
      const osg::Vec2&		pickPoint,
      const osg::Vec2&		pickSize);
    */

    virtual HitList&	getHitList(void);

    virtual void reset();

    virtual void apply(osg::Node&);
    virtual void apply(osg::Geode& node);
    virtual void apply(osg::Billboard& node);
    virtual void apply(osg::LightSource& node);

    virtual void apply(osg::Group& node);
    virtual void apply(osg::MatrixTransform& node);
    virtual void apply(osg::Projection& node);
    virtual void apply(osg::Switch& node);
    virtual void apply(osg::LOD& node);
    virtual void apply(osg::ClearNode& node);
    virtual void apply(osg::Impostor& node);

    void setView(osg::Camera&	cam,
                 osg::Viewport& viewport,
                 osg::Vec2&		pickPoint,
                 osg::Vec2&		pickSize);
    
    void pushCullViewState(const osg::Matrix* matrix=NULL);
    void popCullViewState();

    void pushStateSet(osg::State* state,const osg::StateSet* ss);
    void popStateSet(osg::State* state);

    void setLODBias(const float bias) { _LODBias = bias; }
    const float getLODBias() const    { return _LODBias; }
    
  protected:


    /** prevent unwanted copy construction.*/
    PickVisitor(const PickVisitor&):osg::NodeVisitor() {}

    /** prevent unwanted copy operator.*/
    PickVisitor& operator = (const PickVisitor&) { return *this; }
    
    ///////////////////////////////////////////////////////////////////////////
 
    inline osg::Matrix* getCurrentMatrix()
    {
      return _cvs->_matrix;
    }

    inline osg::Matrix* getInverseCurrentMatrix()
    {
      return _cvs->_inverse;
    }

    inline const osg::Vec3& getEyeLocal() const
    {
      return _cvs->_eyePoint;
    }

    inline const osg::Vec3& getCenterLocal() const
    {
      return _cvs->_centerPoint;
    }

    inline const osg::Vec3& getLookVectorLocal() const
    {
      return _cvs->_lookVector;
    }

    inline bool isCulled(const osg::BoundingSphere& sp,CullViewState::CullingMode& mode) const
    {
      return _cvs->isCulled(sp,mode);
    }

    inline const bool isCulled(const osg::BoundingBox& bb,const CullViewState::CullingMode mode) const
    {
      return _cvs->isCulled(bb,mode);
    }
	
    typedef std::vector< osg::ref_ptr<CullViewState> > CullViewStateStack;
    CullViewStateStack          _viewStateStack;
    osg::ref_ptr<CullViewState> _tvs;
    osg::ref_ptr<CullViewState> _cvs;
    
    std::vector<CullViewState::CullingMode> _cullingModeStack;
    

    // viewport x,y,width,height respectiveyly.
    osg::ref_ptr<osg::Viewport> _viewport;
    osg::ref_ptr<osg::Camera>	_camera;


    typedef std::vector<osg::Matrix *> MatrixList;
    //	MatrixList _reuseMatrixList;
    //	unsigned int _currentReuseMatrixIndex;
	
    inline osg::Matrix* createOrReuseMatrix()
    {
      /*
      // skip of any already reused matrix.
      while (_currentReuseMatrixIndex<_reuseMatrixList.size() && 
      _reuseMatrixList[_currentReuseMatrixIndex]->referenceCount()>1)
      {
      osg::notify(osg::NOTICE)<<"Warning:createOrReuseMatrix() skipping multiply refrenced entry."<< std::endl;
      ++_currentReuseMatrixIndex;
      }

      // if still within list, element must be singularily referenced
      // there return it to be reused.
      if (_currentReuseMatrixIndex<_reuseMatrixList.size())
      {
      osg::Matrix* matrix = _reuseMatrixList[_currentReuseMatrixIndex++].get();
      matrix->makeIdentity();
      return matrix;
      }
      */

      // otherwise need to create new matrix.
      osg::Matrix* matrix = new osg::Matrix();
      //            _reuseMatrixList.push_back(matrix);
      //            ++_currentReuseMatrixIndex;
      return matrix;
    }
	
    float   _LODBias;

    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    osg::ref_ptr<osg::State>        _state;
	

    ///////////////////////////////////////////////////////////////////////////
    // pickVar's
    typedef std::map<float,Hit>		HitReportList;

    HitReportingMode				_hitReportingMode;
    HitList							_hitList;

    GLuint							_pickBuffer[512];

    std::map<unsigned int,HitContainer>		_pickHashList;
    IdStack									_nameStack;

    void	pushPickData(osg::Node& node);
    void	pushPickData(osg::Geode& geode,osg::Drawable& drawable);
    void	popPickData(void);

    static long	getHashValue(const std::string& name,int id);
    static long	getHashValue(const std::string& name,int id,int idSec);

    osg::Matrix		_projectionMatrix;
    osg::Matrix		_modelViewMatrix;

    osg::Vec2		_pickPoint;
    osg::Vec2		_pickSize;

  };


};

using namespace glPicking;

#endif // _PICKGLVISITOR_H
