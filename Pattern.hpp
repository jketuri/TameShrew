/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2003 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

#ifndef OSG_PATTERN
#define OSG_PATTERN 1

#include <osg/Array>
#include <osg/GL>
#include <osg/Notify>
#include <osg/Object>
#include <osg/Plane>
#include <osg/Quat>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Vec3>

using namespace osg;

// forward declare visitors.
class PatternVisitor;
class ConstPatternVisitor;


/** META_StateAttribute macro define the standard clone, isSameKindAs,
  * className and getType methods.
  * Use when subclassing from Object to make it more convinient to define 
  * the standard pure virtual methods which are required for all Object 
  * subclasses.*/
#define META_Pattern(library,name) \
        virtual osg::Object* cloneType() const { return new name(); } \
        virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new name (*this,copyop); } \
        virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const name *>(obj)!=NULL; } \
        virtual const char* libraryName() const { return #library; } \
        virtual const char* className() const { return #name; } \
        virtual void accept(osg::ShapeVisitor& sv) { ((PatternVisitor &)sv).apply(*this); } \
        virtual void accept(osg::ConstShapeVisitor& csv) const { ((ConstPatternVisitor &)csv).apply(*this); }

/** Base class for all pattern types. 
  * Patterns are used to either for culling and collision detection or
  * to define the geometric pattern of procedurally generate Geometry.
*/

// forward declartions of Pattern types.
class Circle;
class Oblong;
class Cornet;
class Pipe;

class PatternVisitor : public ShapeVisitor
{
    public:
    
    	PatternVisitor() {}
    
    	virtual void apply(Circle&) {}
    	virtual void apply(Oblong&) {}
    	virtual void apply(Cornet&) {}
    	virtual void apply(Pipe&) {}
};

class ConstPatternVisitor : public ConstShapeVisitor
{
    public:
    
    	ConstPatternVisitor() {}

    	virtual void apply(const Circle&) {}
    	virtual void apply(const Oblong&) {}
    	virtual void apply(const Cornet&) {}
    	virtual void apply(const Pipe&) {}
};

class Circle : public Sphere
{
    public:
    	
        Circle():
	    _center(0.0f,0.0f,0.0f),
	    _radius(1.0f),
		_startAngle(0.0f),
		_endAngle(0.0f),
		_width(0.01f) {}
	
        Circle(const osg::Vec3& center,float radius):
	    _center(center),
	    _radius(radius),
		_startAngle(0.0f),
		_endAngle(0.0f),
		_width(0.01f) {}

        Circle(const osg::Vec3& center, float radius, float startAngle, float endAngle, float width):
	    _center(center),
	    _radius(radius),
		_startAngle(startAngle),
		_endAngle(endAngle),
		_width(width) {}

        Circle(const Circle& circle,const CopyOp& copyop=CopyOp::SHALLOW_COPY):
            Sphere(circle, copyop), _startAngle(circle._startAngle), _endAngle(circle._endAngle),
			_width(circle._width) {}

    	META_Pattern(osg, Circle);

	inline bool valid() const { return _radius>=0.0f; }
	
	inline void set(const Vec3& center, float radius)
	{
	    _center = center;
	    _radius = radius;
	}

	inline void setCenter(const Vec3& center) { _center = center; }
	inline const Vec3& getCenter() const { return _center; }
	
	inline void setRadius(float radius) { _radius = radius; }
	inline float getRadius() const { return _radius; }

	inline void setStartAngle(float startAngle) { _startAngle = startAngle; }
	inline float getStartAngle() const { return _startAngle; }

	inline void setEndAngle(float endAngle) { _endAngle = endAngle; }
	inline float getEndAngle() const { return _endAngle; }

	inline void setWidth(float width) { _width = width; }
	inline float getWidth() const { return _width; }

    protected:
    
    	virtual ~Circle() {}
    
    	Vec3    _center;
		float	_radius;
		float	_startAngle;
		float	_endAngle;
		float	_width;

};

class Oblong : public Box
{
    public:
    	
        Oblong():
	    _center(0.0f,0.0f,0.0f),
	    _halfLengths(0.5f,0.5f,0.5f) {}
	
        Oblong(const osg::Vec3& center,float width):
	    _center(center),
	    _halfLengths(width*0.5f,width*0.5f,width*0.5f) {}

        Oblong(const osg::Vec3& center,float lengthX,float lengthY, float lengthZ):
	    _center(center),
	    _halfLengths(lengthX*0.5f,lengthY*0.5f,lengthZ*0.5f) {}

        Oblong(const Oblong& oblong,const CopyOp& copyop=CopyOp::SHALLOW_COPY) : 
            Box(oblong,copyop) {}

    	META_Pattern(osg, Oblong);
	
	inline bool valid() const { return _halfLengths.x()>=0.0f; }
	
	inline void set(const Vec3& center,const Vec3& halfLengths)
	{
	    _center = center;
	    _halfLengths = halfLengths;
	}

	inline void setCenter(const Vec3& center) { _center = center; }
	inline const Vec3& getCenter() const { return _center; }
	
	inline void setHalfLengths(const Vec3& halfLengths) { _halfLengths = halfLengths; }
	inline const Vec3& getHalfLengths() const { return _halfLengths; }
	
    	inline void setRotation(const Quat& quat) { _rotation = quat; }
    	inline const Quat&  getRotation() const { return _rotation; }
    	inline Matrix getRotationMatrix() const { Matrix matrix; _rotation.get(matrix); return matrix; }
    	inline bool zeroRotation() const { return _rotation.zeroRotation(); } 

    protected:
    
    	virtual ~Oblong() {}
    
    	Vec3    _center;
    	Vec3    _halfLengths;
    	Quat	_rotation;	
	
};

class Cornet : public Cone
{
    public:
    	
        Cornet():
	    _center(0.0f,0.0f,0.0f),
	    _radius(1.0f),
	    _height(1.0f) {}
	
        Cornet(const osg::Vec3& center,float radius,float height):
	    _center(center),
	    _radius(radius),
	    _height(height) {}

        Cornet(const Cornet& cornet,const CopyOp& copyop=CopyOp::SHALLOW_COPY) : 
            Cone(cornet,copyop) {}

    	META_Pattern(osg, Cornet);
	
	inline bool valid() const { return _radius>=0.0f; }
	
	inline void set(const Vec3& center,float radius, float height)
	{
	    _center = center;
	    _radius = radius;
	    _height = height;
	}

	inline void setCenter(const Vec3& center) { _center = center; }
	inline const Vec3& getCenter() const { return _center; }
	
	inline void setRadius(float radius) { _radius = radius; }
	inline float getRadius() const { return _radius; }

	inline void setHeight(float height) { _height = height; }
	inline float getHeight() const { return _height; }

   	inline void setRotation(const Quat& quat) { _rotation = quat; }
   	inline const Quat& getRotation() const { return _rotation; }
   	inline Matrix getRotationMatrix() const { Matrix matrix; _rotation.get(matrix); return matrix; }
   	inline bool zeroRotation() const { return _rotation.zeroRotation(); } 
	
	inline float getBaseOffsetFactor() const { return 0.25f; } 
	inline float getBaseOffset() const { return -getBaseOffsetFactor()*getHeight(); } 

    protected:
    
    	virtual ~Cornet() {}
    
    	Vec3    _center;
		float 	_radius;
		float	_height;
    	Quat	_rotation;
};

class Pipe : public Cylinder
{
    public:
    	
        Pipe():
	    _center(0.0f,0.0f,0.0f),
	    _radius(1.0f),
	    _height(1.0f) {}
	
        Pipe(const osg::Vec3& center,float radius,float height):
	    _center(center),
	    _radius(radius),
	    _height(height) {}

        Pipe(const Pipe& cylinder,const CopyOp& copyop=CopyOp::SHALLOW_COPY): 
            Cylinder(cylinder, copyop) {}

    	META_Pattern(osg, Pipe);
	
	inline bool valid() const { return _radius>=0.0f; }
	
	inline void set(const Vec3& center,float radius, float height)
	{
	    _center = center;
	    _radius = radius;
	    _height = height;
	}

	inline void setCenter(const Vec3& center) { _center = center; }
	inline const Vec3& getCenter() const { return _center; }
	
	inline void setRadius(float radius) { _radius = radius; }
	inline float getRadius() const { return _radius; }

	inline void setHeight(float height) { _height = height; }
	inline float getHeight() const { return _height; }

   	inline void setRotation(const Quat& quat) { _rotation = quat; }
   	inline const Quat& getRotation() const { return _rotation; }
   	inline Matrix getRotationMatrix() const { Matrix matrix; _rotation.get(matrix); return matrix; }
   	bool zeroRotation() const { return _rotation.zeroRotation(); } 

    protected:
    
    	virtual ~Pipe() {}
    
    	Vec3    _center;
		float 	_radius;
		float	_height;
    	Quat	_rotation;	
};

#endif
