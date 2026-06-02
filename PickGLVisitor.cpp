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

#include "PickGLVisitor.hpp"

PickVisitor::PickVisitor()
{
  // overide the default node visitor mode.
  setTraversalMode(NodeVisitor::TRAVERSE_ACTIVE_CHILDREN);

  // note all subsequent _cullingModeStack code assumes that there
  // is a least this one value on the stack, therefore they never
  // check whether the stack is empty. This shouldn't be a problem
  // unless there is bug somewhere...
  _cullingModeStack.push_back(CullViewState::ENABLE_ALL_CULLING);

  _tvs = new CullViewState;
  _tvs->_eyePoint.set(0.0f,0.0f,1.0f);
  _tvs->_centerPoint.set(0.0f,0.0f,0.0f);
  _tvs->_lookVector.set(0.0f,0.0f,-1.0f);
  _tvs->_upVector.set(0.0f,1.0f,0.0f);

  _cvs = _tvs;

	
  _LODBias = 1.0f;

  _state = new osg::State;
	
  _hitReportingMode=ALL_HITS;
}


PickVisitor::~PickVisitor()
{
  reset();
}


void PickVisitor::reset()
{
  // remove all accept the first element of the stack.
  _cullingModeStack.erase(_cullingModeStack.begin()+1,_cullingModeStack.end());
}

void PickVisitor::
setView(osg::Camera&	camera,
        osg::Viewport&	viewport,
        osg::Vec2&		pickPoint,
        osg::Vec2&		pickSize)
{
  _camera = &camera;

  _pickPoint=pickPoint;
  _pickSize=pickSize;
	
  _viewport=&viewport;

  ///////////////////////////////////////////////////////////////////////////
  // Translate and scale the picked region to the entire viewport 
  // gluPickMatrix(GLdouble x, GLdouble y, GLdouble deltax, GLdouble deltay,GLint viewport[4])
  ///////////////////////////////////////////////////////////////////////////
  // glTranslatef((viewport[2] - 2 * (x - viewport[0])) / deltax,
  //				(viewport[3] - 2 * (y - viewport[1])) / deltay, 0);
  // glScalef(viewport[2] / deltax, viewport[3] / deltay, 1.0);
	
  _projectionMatrix = osg::Matrix::translate(( _viewport->width() - 2 * ( _pickPoint.x() - _viewport->x() )) / _pickSize.x(),
                                             ( _viewport->height() - 2 * ( _pickPoint.y() - _viewport->y() )) / _pickSize.y(),
                                             0);
  _projectionMatrix = osg::Matrix::scale(_viewport->width()/_pickSize.x(),
                                         _viewport->height()/_pickSize.y(),
                                         1.0) *
    _projectionMatrix;

  _projectionMatrix = _camera->getProjectionMatrix() * 
    _projectionMatrix;

  _modelViewMatrix=osg::Matrix(_camera->getModelViewMatrix());
}

void PickVisitor::pushCullViewState(const Matrix* matrix)
{
  osg::ref_ptr<CullViewState> nvs = new CullViewState;

  Matrix* inverse_world = NULL;

  if (matrix)
    {
      if (_cvs.valid() && _cvs->_matrix->valid())
        {
          nvs->_matrix = new Matrix;
          nvs->_matrix->mult(*matrix,*(_cvs->_matrix));
        }
      else
        {
          nvs->_matrix = new Matrix(*matrix);
        }

      inverse_world = new Matrix;
      inverse_world->invert(*(nvs->_matrix));
      nvs->_inverse = inverse_world;
    }
  else
    {
      if (_cvs.valid())
        {
          nvs->_matrix = _cvs->_matrix;
          nvs->_inverse = _cvs->_inverse;
          inverse_world = nvs->_inverse;

        }
      else
        {
          nvs->_matrix = NULL;
          nvs->_inverse = NULL;
          inverse_world = NULL;
        }
    }

  if (inverse_world)
    {
      nvs->_eyePoint = _tvs->_eyePoint*(*inverse_world);

      nvs->_centerPoint = _tvs->_centerPoint*(*inverse_world);

      nvs->_lookVector = nvs->_centerPoint - nvs->_eyePoint;
      nvs->_lookVector.normalize();

      Vec3 zero_transformed = Vec3(0.0f,0.0f,0.0f)*(*inverse_world);
      nvs->_upVector = (_tvs->_upVector)*(*inverse_world) - zero_transformed;
      nvs->_upVector.normalize();

      nvs->_clippingVolume = _tvs->_clippingVolume;
      nvs->_clippingVolume.transformProvidingInverse(*(nvs->_matrix));

    }
  else
    {
      nvs->_eyePoint = _tvs->_eyePoint;

      nvs->_lookVector = _tvs->_lookVector;

      nvs->_centerPoint = _tvs->_centerPoint;

      nvs->_upVector = _tvs->_upVector;

      nvs->_clippingVolume = _tvs->_clippingVolume;
    }


  nvs->_bbCornerFar = (nvs->_lookVector.x()>=0?1:0) |
    (nvs->_lookVector.y()>=0?2:0) |
    (nvs->_lookVector.z()>=0?4:0);

  nvs->_bbCornerNear = (~nvs->_bbCornerFar)&7;

  _cvs = nvs;

  _viewStateStack.push_back(nvs);

}


void PickVisitor::popCullViewState()
{
  // pop the top of the view stack and unref it.
  _viewStateStack.pop_back();

  // to new cvs and ref it.
  if (_viewStateStack.empty())
    {
      _cvs = _tvs;
    }
  else
    {
      _cvs = _viewStateStack.back().get();
    }

}


void PickVisitor::pushStateSet(osg::State* state,const osg::StateSet* ss)
{
  //	state->apply(ss);

  glPushAttrib(GL_ENABLE_BIT);

  // find cullFace
  osg::StateSet::ModeList::const_iterator	itr=ss->getModeList().find(GL_CULL_FACE);
  if(itr!=ss->getModeList().end())
    {
      if(itr->second==osg::StateAttribute::Values::OFF)
        glDisable(GL_CULL_FACE);
      else
        glEnable(GL_CULL_FACE);

    }

}

void PickVisitor::popStateSet(osg::State* state)
{
  glPopAttrib();
}

///////////////////////////////////////////////////////////////////////////////
// apply

void PickVisitor::apply(Node& node)
{
  CullViewState::CullingMode mode = _cullingModeStack.back();
  if(isCulled(node.getBound(),mode)) 
    return;

  // push the culling mode.
  _cullingModeStack.push_back(mode);
    
  // push the node's state.
  StateSet* node_state = node.getStateSet();
  if(node_state) 
    pushStateSet(_state.get(),node_state);

  if(!node.getName().empty())
    {
      pushPickData(node);
      traverse(node);
      popPickData();
    }
  else
    traverse(node);

  // pop the node's state off the geostate stack.    
  if(node_state) 
    popStateSet(_state.get());
    
  // pop the culling mode.
  _cullingModeStack.pop_back();
}


void PickVisitor::apply(Geode& node)
{
  // return if object's bounding sphere is culled.
  CullViewState::CullingMode mode = _cullingModeStack.back();
  //    if(isCulled(node.getBound(),mode)) 
  //		return;

  // push the node's state.
  StateSet* node_state = node.getStateSet();
  if(node_state) 
    pushStateSet(_state.get(),node_state);

  if(!node.getName().empty())
    pushPickData(node);

  Matrix* matrix=getCurrentMatrix();
  if(matrix)
    {
      glPushMatrix();
      glMultMatrixf((GLfloat*)matrix->ptr());
    }

  for(int i=0; i < (int)node.getNumDrawables(); ++i)
    {
      Drawable* drawable = node.getDrawable(i);
      const BoundingBox &bb =drawable->getBound();

      //       if(isCulled(bb,mode)) 
      //			continue;
        
      // push the geoset's state on the geostate stack.    
      StateSet* stateset = drawable->getStateSet();
        

      if(stateset) 
        pushStateSet(_state.get(),stateset);
        
      // draw directly to opengl
      if(!_nameStack.empty())
        {
          pushPickData(node,*drawable);
          glPushMatrix();
          drawable->draw(*_state);
          glPopMatrix();
          popPickData();
        }
      else
        drawable->draw(*_state);
		
      if(stateset) 
        popStateSet(_state.get());
    }

  if(matrix)
    glPopMatrix();

  if(!node.getName().empty())
    popPickData();

  // pop the node's state off the geostate stack.    
  if(node_state) 
    popStateSet(_state.get());

}


void PickVisitor::apply(Billboard& node)
{
  /*
  // return if object's bounding sphere is culled.
  CullViewState::CullingMode mode = _cullingModeStack.back();
  if (isCulled(node.getBound(),mode)) return;

  // push the node's state.
  StateSet* node_state = node.getStateSet();
  if (node_state) pushStateSet(node_state);

  Vec3 eye_local = getEyeLocal();
  Matrix* matrix = getCurrentMatrix();

  for(int i=0;i<node.getNumDrawables();++i)
  {
  const Vec3& pos = node.getPos(i);

  Drawable* drawable = node.getDrawable(i);
  // need to modify isCulled to handle the billboard offset.
  // if (isCulled(drawable->getBound())) continue;

  //       updateCalculatedNearFar(pos);

  Matrix* billboard_matrix = createOrReuseMatrix();
  node.calcTransform(eye_local,pos,*billboard_matrix);

  StateSet* stateset = drawable->getStateSet();
        
  bool isTransparent = stateset && stateset->getRenderingHint()==osg::StateSet::TRANSPARENT_BIN;
  if (isTransparent)
  {

  if (matrix)
  {
  billboard_matrix->mult(*billboard_matrix,*matrix);
  }

  Vec3 center;
  if (matrix)
  {
  center = pos*(*matrix);
  }
  else
  {
  center = pos;
  }
  Vec3 delta_center = center-_tvs->_eyePoint;

  if (g_debugging2)
  {
  notify(INFO) << "center ["<<center.x()<<","<<center.y()<<","<<center.z()<<"]"<<endl;
  notify(INFO) << "delta_center ["<<delta_center.x()<<","<<delta_center.y()<<","<<delta_center.z()<<"]"<<endl;
  notify(INFO) << "_lookVector ["<<_tvs->_lookVector.x()<<","<<_tvs->_lookVector.y()<<","<<_tvs->_lookVector.z()<<"]"<<endl;
  }

  float depth;
  switch(_tsm)
  {
  case(LOOK_VECTOR_DISTANCE):depth = _tvs->_lookVector*delta_center;break;
  case(OBJECT_EYE_POINT_DISTANCE):
  default: depth = delta_center.length2();break;
  }

  if (stateset) pushStateSet(stateset);
  addDrawableAndDepth(drawable,billboard_matrix,depth);
  if (stateset) popStateSet();

  }
  else
  {
  if (stateset) pushStateSet(stateset);
  addDrawable(drawable,billboard_matrix);
  if (stateset) popStateSet();
  }

  }

  // pop the node's state off the geostate stack.    
  if (node_state) popStateSet();
  */
}


void PickVisitor::apply(LightSource& node)
{
  /*
  // push the node's state.
  StateSet* node_state = node.getStateSet();
  if (node_state) pushStateSet(node_state);

  Matrix* matrix = getCurrentMatrix();
  Light* light = node.getLight();
  if (light)
  {
  addLight(light,matrix);
  }

  // pop the node's state off the geostate stack.    
  if (node_state) popStateSet();
  */
}


void PickVisitor::apply(Group& node)
{
  // return if object's bounding sphere is culled.
  CullViewState::CullingMode mode = _cullingModeStack.back();
  //	if(isCulled(node.getBound(),mode)) 
  //		return;

  // push the culling mode.
  _cullingModeStack.push_back(mode);

  // push the node's state.
  StateSet* node_state = node.getStateSet();
  if(node_state) 
    pushStateSet(_state.get(),node_state);

  if(!node.getName().empty())
    {
      pushPickData(node);
      traverse(node);
      popPickData();
    }
  else
    traverse(node);

  // pop the node's state off the render graph stack.    
  if(node_state) 
    popStateSet(_state.get());

  // pop the culling mode.
  _cullingModeStack.pop_back();
}


void PickVisitor::apply(MatrixTransform& node)
{
  // return if object's bounding sphere is culled.
  CullViewState::CullingMode mode = _cullingModeStack.back();
  //    if(isCulled(node.getBound(),mode))
  //		return;

  // push the culling mode.
  _cullingModeStack.push_back(mode);

  // push the node's state.
  StateSet* node_state = node.getStateSet();
  if (node_state) 
    pushStateSet(_state.get(),node_state);

  pushCullViewState(&node.getMatrix());

  if(!node.getName().empty())
    {
      pushPickData(node);
      traverse(node);
      popPickData();
    }
  else
    traverse(node);

  popCullViewState();

  // pop the node's state off the render graph stack.    
  if(node_state) 
    popStateSet(_state.get());

  // pop the culling mode.
  _cullingModeStack.pop_back();
}


void PickVisitor::apply(Switch& node)
{
  apply((Group&)node);
}


void PickVisitor::apply(LOD& node)
{
  /*
  // return if object's bounding sphere is culled.
  CullViewState::CullingMode mode = _cullingModeStack.back();
  //    if (isCulled(node.getBound(),mode)) 
  //		return;

  int eval = node.evaluate(getEyeLocal(),_LODBias);
  if (eval<0) 
  return;

  // push the culling mode.
  _cullingModeStack.push_back(mode);

  // push the node's state.
  StateSet* node_state = node.getStateSet();
  if(node_state) 
  pushStateSet(_state.get(),node_state);

  //notify(INFO) << "selecting child "<<eval<<endl;
  node.getChild(eval)->accept(*this);

  // pop the node's state off the render graph stack.    
  if(node_state) 
  popStateSet(_state.get());

  // pop the culling mode.
  _cullingModeStack.pop_back();
  */
}

void PickVisitor::apply(Projection& node)
{
  /*
  // return if object's bounding sphere is culled.
  CullingMode mode = _cullingModeStack.back();

  if (!node.getCullingActive()) mode = 0;
  else if (node.getNumChildrenWithCullingDisabled()==0 && 
  isCulled(node.getBound(),mode)) return;

  // push the culling mode.
  _cullingModeStack.push_back(mode);

  // push the node's state.
  StateSet* node_state = node.getStateSet();
  if (node_state) pushStateSet(node_state);


  // record previous near and far values.
  float previous_znear = _computed_znear;
  float previous_zfar = _computed_zfar;
    
  _computed_znear = FLT_MAX;
  _computed_zfar = -FLT_MAX;

  osg::Matrix &matrix = *createOrReuseMatrix(node.getMatrix());
  pushProjectionMatrix(matrix.get());
    
  handle_cull_callbacks_and_traverse(node);

  popProjectionMatrix();

  _computed_znear = previous_znear;
  _computed_zfar = previous_zfar;


  // pop the node's state off the render graph stack.    
  if (node_state) popStateSet();

  // pop the culling mode.
  _cullingModeStack.pop_back();
  */
}

void PickVisitor::apply(osg::ClearNode& node)
{
  /*
  // simply override the current earth sky.
  setEarthSky(&node);

  // push the node's state.
  StateSet* node_state = node.getStateSet();
  if (node_state) pushStateSet(node_state);

  handle_cull_callbacks_and_traverse(node);

  // pop the node's state off the render graph stack.    
  if (node_state) popStateSet();
  */
}



void PickVisitor::apply(Impostor& node)
{
  /*
    const BoundingSphere& bs = node.getBound();

    // return if object's bounding sphere is culled.
    CullViewState::CullingMode mode = _cullingModeStack.back();
    if (isCulled(bs,mode)) return;

    osg::Vec3 eyeLocal = getEyeLocal();

    int eval = node.evaluate(eyeLocal,_LODBias);
    if (eval<0){
    return;
    }

    // push the culling mode.
    _cullingModeStack.push_back(mode);

    // push the node's state.
    StateSet* node_state = node.getStateSet();
    if (node_state) pushStateSet(node_state);


    float distance2 = (eyeLocal-bs.center()).length2();
    if (!_impostorActive ||
    distance2*_LODBias*_LODBias<node.getImpostorThreshold2() ||
    distance2<bs.radius2()*2.0f)
    {
    // outwith the impostor distance threshold therefore simple
    // traverse the appropriate child of the LOD.
    node.getChild(eval)->accept(*this);
    }
    else if (!_viewport.valid())
    {
    // need to use impostor but no valid viewport is defined to simply
    // default to using the LOD child as above.
    node.getChild(eval)->accept(*this);
    }
    else
    {    
    // within the impostor distance threshold therefore attempt
    // to use impostor instead.
        
    Matrix* matrix = getCurrentMatrix();

    // search for the best fit ImpostorSprite;
    ImpostorSprite* impostorSprite = node.findBestImpostorSprite(eyeLocal);
        
    if (impostorSprite)
    {
    // impostor found, now check to see if it is good enough to use
    float error = impostorSprite->calcPixelError(*_camera,*_viewport,matrix);
            
    if (error>_impostorPixelErrorThreshold)
    {
    // chosen impostor sprite pixel error is too great to use
    // from this eye point, therefore invalidate it.
    impostorSprite=NULL;
    }
    }
        

    // need to think about sprite reuse and support for multiple context's.

    if (impostorSprite==NULL)
    {
    // no appropriate sprite has been found therefore need to create
    // one for use.
            
    // create the impostor sprite.
    impostorSprite = createImpostorSprite(node);

    }
        
    if (impostorSprite)
    {
            
    //           updateCalculatedNearFar(impostorSprite->getBound());

    StateSet* stateset = impostorSprite->getStateSet();
            
    if (stateset) pushStateSet(stateset);
            
    if (_depthSortImpostorSprites)
    {
    Vec3 center;
    if (matrix)
    {
    center = node.getCenter()*(*matrix);
    }
    else
    {
    center = node.getCenter();
    }
    Vec3 delta_center = center-_tvs->_eyePoint;

    if (g_debugging2)
    {
    notify(INFO) << "center ["<<center.x()<<","<<center.y()<<","<<center.z()<<"]"<<endl;
    notify(INFO) << "delta_center ["<<delta_center.x()<<","<<delta_center.y()<<","<<delta_center.z()<<"]"<<endl;
    notify(INFO) << "_lookVector ["<<_tvs->_lookVector.x()<<","<<_tvs->_lookVector.y()<<","<<_tvs->_lookVector.z()<<"]"<<endl;
    }

    float depth;
    switch(_tsm)
    {
    case(LOOK_VECTOR_DISTANCE):depth = _tvs->_lookVector*delta_center;break;
    case(OBJECT_EYE_POINT_DISTANCE):
    default: depth = delta_center.length2();break;
    }

    addDrawableAndDepth(impostorSprite,matrix,depth);
    }
    else
    {
    addDrawable(impostorSprite,matrix);
    }

    if (stateset) popStateSet();
            
    // update frame number to show that impostor is in action.
    impostorSprite->setLastFrameUsed(getTraversalNumber());
            
    }
    else
    {
    // no impostor has been selected or created so default to 
    // traversing the usual LOD selected child.
    node.getChild(eval)->accept(*this);
    }
                
    }

    // pop the node's state off the render graph stack.    
    if (node_state) popStateSet();

    // pop the culling mode.
    _cullingModeStack.pop_back();
  */
}

// apply
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// pick

bool PickVisitor::
pick(osg::Viewport&	viewport,
     osg::Camera&	camera,
     osg::Node&		startNode,
     osg::Vec2&		pickPoint,
     osg::Vec2&		pickSize)
{
  setView(camera,
          viewport,
          pickPoint,
          pickSize);
	
  ///////////////////////////////////////////////////////////////////////////
  // init selectionMode
  glSelectBuffer(512, _pickBuffer);
  glInitNames();

  glRenderMode(GL_SELECT);

  // clear hitList
  _hitList.clear();
  // clear _pickHashList
  _pickHashList.clear();

  _state->reset();
  //    _state->setFrameStamp(_frameStamp.get());

  // note, to support multi-pipe systems the deletion of OpenGL display list
  // and texture objects is deferred until the OpenGL context is the correct
  // context for when the object were originally created.  Here we know what
  // context we are in so can flush the appropriate caches.
  osg::Drawable::flushDeletedDisplayLists(_state->getContextID());
  osg::Texture::flushDeletedTextureObjects(_state->getContextID());

  ///////////////////////////////////////////////////////////////////////////
  // set view
  _viewport->apply(*_state);

  //	_state->setCamera(_camera.get());

  // set up projection
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glMultMatrixf((float*)_projectionMatrix.ptr());

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // set up camera modelview.
  glMultMatrixf((float*)_modelViewMatrix.ptr());
    
	
  // draw directly
  // traverse the visitor
  startNode.accept((NodeVisitor&)*this);
	
  ///////////////////////////////////////////////////////////////////////////
  // check hits
  int hits=0;
  hits = glRenderMode(GL_RENDER);

  if(hits == 0)
    return false;
 
  // pick obj suchen
  int		i, j;
  std::string			spacing;
  GLuint				names, *ptr;
  HitReportList		hitSortList;
  float				zDepth1;
  float				zDepth2;
  float				currentDepth;
  HitContainerList	hitContainerList;

  ptr = (GLuint *) _pickBuffer;
  for (i = 0; i < hits; i++) 
    {
      names = *ptr;
      ptr++;
		
      zDepth1=(float) *ptr/0x7fffffff;
      ptr++;
		
      zDepth2=(float) *ptr/0x7fffffff;
      ptr++;

      if(_hitReportingMode==ONLY_FARTHEST_HIT)
        currentDepth=zDepth2;
      else
        currentDepth=zDepth1;

      hitContainerList.clear();
      for (j = 0; j < (int)names; j++) 
        {	
          if(*ptr!=0)
            {
              if( ((_pickHashList[*ptr])._node) != NULL)
                {
                  hitContainerList.push_back(HitContainer((_pickHashList[*ptr])._node) );
                }
              else
                {
                  hitContainerList.push_back(HitContainer((_pickHashList[*ptr])._drawable) );
                }
            }
          else
            osg::notify(osg::NOTICE) << spacing << "<- picking Name : unknown , mayby wrong hashValue ?????" << endl;

          ptr++;
        }
		
      hitSortList.insert(std::pair<float,Hit>(currentDepth,Hit(hitContainerList,zDepth1,zDepth2)));
    }

  // generate hitlist
  switch(_hitReportingMode)
    {
    case ONLY_NEAREST_HIT:
      _hitList.push_back(hitSortList.begin()->second);
      break;
    case ALL_HITS:
      {
        for(HitReportList::iterator itr= hitSortList.begin();
            itr!= hitSortList.end();
            itr++)
          _hitList.push_back(itr->second);
      }
      break;
    case ONLY_FARTHEST_HIT:
      //		_hitList.push_back(hitSortList.rbegin()->second);
      break;
    };
		
	
  return  _hitList.size() > 0 ;
}

HitList& PickVisitor::
getHitList(void)
{
  return _hitList;
}

void  PickVisitor::
pushPickData(osg::Node& node)
{
  unsigned int  hashValue=getHashValue(node.getName(),(int)&node);
	
  _pickHashList[hashValue]=HitContainer(&node);

  _nameStack.push(hashValue);
  glPushName(hashValue);

}

void PickVisitor::
pushPickData(osg::Geode& geode,osg::Drawable& drawable)
{
  unsigned int  hashValue=getHashValue(geode.getName(),(int)&geode,(int)&drawable);
	
  _pickHashList[hashValue]=HitContainer(&drawable);

  _nameStack.push(hashValue);
  glPushName(hashValue);
}


void  PickVisitor::
popPickData(void)
{
  // nodeStack
  _nameStack.pop();

  glPopName();
}

long PickVisitor::
getHashValue(const std::string& name,int id)
{
  static locale loc;
  static const collate<char>& coll = use_facet<collate<char> >(loc);
  //	static const collate<char>& coll = use_facet(loc, (collate<char>*)NULL, true);

  char			strId[80];
  sprintf(strId, "_%d",id);

  std::string		hashStr;
  hashStr.resize(200);

  hashStr += name;
  hashStr += strId;
		
  //	return coll.hash(hashStr.begin(), hashStr.end());
  return coll.hash(hashStr.c_str(), hashStr.c_str() + strlen(hashStr.c_str()));
}

long PickVisitor::
getHashValue(const std::string& name,int id,int idSec)
{
  static locale loc;
  static const collate<char>& coll = use_facet<collate<char> >(loc);
  //	static const collate<char>& coll = use_facet(loc, (collate<char>*)NULL, true);

  char			strId[80];
  sprintf(strId, "_%d_%d",id,idSec);

  std::string		hashStr;
  hashStr.resize(200);

  hashStr = name;
  hashStr += strId;

  //	return coll.hash(hashStr.begin(), hashStr.end());
  return coll.hash(hashStr.c_str(), hashStr.c_str() + strlen(hashStr.c_str()));
}


// pick
///////////////////////////////////////////////////////////////////////////////

