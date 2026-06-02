
#ifndef _js_Datum_hpp_
#define _js_Datum_hpp_

#include <math.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

class Coordinates
{

public:

    Coordinates();
    Coordinates(double x, double y, double z);
    Coordinates(const Coordinates &coordinates);
    bool operator==(Coordinates &coordinates);
    bool operator!=(Coordinates &coordinates);
    Coordinates operator+(Coordinates &coordinates);
    Coordinates operator-(Coordinates &coordinates);
    double dot(Coordinates &coordinates);
    double dot3(Coordinates &coordinates);
    double distance(Coordinates &coordinates);
    double clearance(Coordinates &coordinates);
    double clearance(Coordinates &coordinates1, Coordinates &coordinates2, Coordinates *nearestCoordinates);
    double clearance(Coordinates &coordinates1, Coordinates &coordinates2);
    double angle(Coordinates &coordinates);

    static Coordinates origo;
    double x, y, z;

};

class ReferenceEllipsoid
{

public:

    ReferenceEllipsoid(string name, double equatorialRadius, double reciprocalFlattening);
    ReferenceEllipsoid(string name, double equatorialRadius, double polarRadius, bool radiuses);
    string toString();

    string name;
    double equatorialRadius, polarRadius;
    double flattening, reciprocalFlattening;
    double polarRadiusDividedByEquatorialRadius, polarEccentricitySquared;
    double eccentricitySquared, eccentricitySquared2, eccentricitySquared3;

private:

    void init(double equatorialRadius, double reciprocalFlattening);

};

class Projection
{

public:

    Projection(string name, ReferenceEllipsoid *referenceEllipsoid, double centralLongitude, double centralLatitude, double scale, double falseEasting, double falseNorthing);

    double forward(Coordinates &from, Coordinates &to);

    string name;
    ReferenceEllipsoid *referenceEllipsoid;
    double centralLongitude, centralLatitude;
    double scale;
    double falseEasting, falseNorthing;

private:

    double meridinal(double latitude);

};

class Datum
{

public:

    Datum(string name, ReferenceEllipsoid *referenceEllipsoid, double deltaX, double deltaY, double deltaZ);

    Coordinates& convert(const Coordinates &from, Coordinates &to, Datum &toDatum);
    Coordinates& convert(const Coordinates &from, Coordinates &ecef);

    string name;
    ReferenceEllipsoid *referenceEllipsoid;
    double deltaX, deltaY, deltaZ;

};

ostream& operator << (ostream &out, const Coordinates &coordinates);
ostream& operator << (ostream &out, const ReferenceEllipsoid &referenceEllipsoid);
ostream& operator << (ostream &out, const Datum &datum);
ostream& operator << (ostream &out, const Projection &projection);

#endif
