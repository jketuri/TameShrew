
#include "Datum.hpp"

Coordinates Coordinates::origo;

ostream& operator << (ostream &out, const Coordinates &coordinates)
{
    return out << "Coordinates{x=" << coordinates.x << ",y=" << coordinates.y << ",z=" << coordinates.z << "}";
}

ostream& operator << (ostream &out, const ReferenceEllipsoid &referenceEllipsoid)
{
    out << "ReferenceEllipsoid{name=" << referenceEllipsoid.name << ",equatorialRadius=" << referenceEllipsoid.equatorialRadius
        << ",polarRadius=" << referenceEllipsoid.polarRadius << ",flattening=" << referenceEllipsoid.flattening << ",reciprocalFlattening=" << referenceEllipsoid.reciprocalFlattening
        << ",eccentricitySquared=" << referenceEllipsoid.eccentricitySquared << ",polarRadiusDividedByEquatorialRadius=" << referenceEllipsoid.polarRadiusDividedByEquatorialRadius
        << ",polarEccentricitySquard=" << referenceEllipsoid.polarEccentricitySquared << "}";
    return out;
}

ostream& operator << (ostream &out, const Datum &datum)
{
    return out << "Datum{name=" << datum.name << ",referenceEllipsoid=" << *datum.referenceEllipsoid <<
        "deltaX=" << datum.deltaX << ",deltaY=" << datum.deltaY << ",deltaZ=" << datum.deltaZ;
}

ostream& operator << (ostream &out, const Projection &projection)
{
    return out << "Projection{name=" << projection.name << ",referenceEllipsoid=" << *projection.referenceEllipsoid <<
        "centralLongitude=" << projection.centralLongitude << ",centralLatitude=" << projection.centralLatitude <<
        "scale=" << projection.scale << ",falseEasting=" << projection.falseEasting << ",falseNorthing=" << projection.falseNorthing;
}

Coordinates::Coordinates() : x(0.0), y(0.0), z(0.0)
{
}

Coordinates::Coordinates(double x, double y, double z) : x(x), y(y), z(z)
{
}

Coordinates::Coordinates(const Coordinates &coordinates) : x(coordinates.x), y(coordinates.y), z(coordinates.z)
{
}

bool Coordinates::operator==(Coordinates &coordinates)
{
    return x == coordinates.x &&
        y == coordinates.y &&
        z == coordinates.z;
}

bool Coordinates::operator!=(Coordinates &coordinates)
{
    return x != coordinates.x ||
        y != coordinates.y ||
        z != coordinates.z;
}

Coordinates Coordinates::operator+(Coordinates &coordinates)
{
    Coordinates sum;
    sum.x = x + coordinates.x;
    sum.y = y + coordinates.y;
    sum.z = z + coordinates.z;
    return sum;
}

Coordinates Coordinates::operator-(Coordinates &coordinates)
{
    Coordinates difference;
    difference.x = x - coordinates.x;
    difference.y = y - coordinates.y;
    difference.z = z - coordinates.z;
    return difference;
}

double Coordinates::dot(Coordinates &coordinates)
{
    return x * coordinates.x + y * coordinates.y;
}

double Coordinates::dot3(Coordinates &coordinates)
{
    return x * coordinates.x + y * coordinates.y + z * coordinates.z;
}

double Coordinates::distance(Coordinates &coordinates)
{
    Coordinates delta = *this - coordinates;
    return sqrt(delta.dot3(delta));
}

double Coordinates::clearance(Coordinates &coordinates)
{
    Coordinates delta = *this - coordinates;
    return sqrt(delta.dot(delta));
}

double Coordinates::clearance(Coordinates &coordinates1, Coordinates &coordinates2, Coordinates *nearestCoordinates)
{
    Coordinates v = coordinates2 - coordinates1,
        w = *this - coordinates1;
    double c1 = w.dot(v);
    if (c1 <= 0.0)
        {
            if (nearestCoordinates) *nearestCoordinates = coordinates1;
            return clearance(coordinates1);
        }
    double c2 = v.dot(v);
    if (c2 <= c1)
        {
            if (nearestCoordinates) *nearestCoordinates = coordinates2;
            return clearance(coordinates2);
        }
    double b = c1 / c2;
    v.x *= b;
    v.y *= b;
    w = coordinates1 + v;
    if (nearestCoordinates) *nearestCoordinates = w;
    return clearance(w);
}

double Coordinates::clearance(Coordinates &coordinates1, Coordinates &coordinates2)
{
    return clearance(coordinates1, coordinates2, NULL);
}

double Coordinates::angle(Coordinates &coordinates)
{
    Coordinates delta = *this - coordinates;
    return atan2(delta.y, delta.x);
}

void ReferenceEllipsoid::init(double equatorialRadius, double reciprocalFlattening)
{
    this->equatorialRadius = equatorialRadius;
    this->reciprocalFlattening = reciprocalFlattening;
    flattening = 1.0 / reciprocalFlattening;
    polarRadiusDividedByEquatorialRadius = 1.0 - flattening;
    polarRadius = polarRadiusDividedByEquatorialRadius * equatorialRadius;
    eccentricitySquared = 2.0 * flattening - flattening * flattening;
    polarEccentricitySquared = eccentricitySquared / (1.0 - eccentricitySquared);
    eccentricitySquared2 = eccentricitySquared * eccentricitySquared;
    eccentricitySquared3 = eccentricitySquared2 * eccentricitySquared;
}

ReferenceEllipsoid::ReferenceEllipsoid(string name, double equatorialRadius, double reciprocalFlattening) : name(name)
{
    init(equatorialRadius, reciprocalFlattening);
}

ReferenceEllipsoid::ReferenceEllipsoid(string name, double equatorialRadius, double polarRadius, bool radiuses) : name(name)
{
    init(equatorialRadius, 1.0 / ((equatorialRadius - polarRadius) / equatorialRadius));
    (void)&radiuses;
}

Projection::Projection(string name, ReferenceEllipsoid *referenceEllipsoid, double centralLongitude, double centralLatitude, double scale, double falseEasting, double falseNorthing) : name(name), referenceEllipsoid(referenceEllipsoid), centralLongitude(centralLongitude), centralLatitude(centralLatitude), scale(scale), falseEasting(falseEasting), falseNorthing(falseNorthing)
{
}

double Projection::meridinal(double latitude)
{
    double mint = latitude * (1.0 - referenceEllipsoid->eccentricitySquared / 4.0 - 3.0 * referenceEllipsoid->eccentricitySquared2 / 64.0 - 5 * referenceEllipsoid->eccentricitySquared3 / 256.0);
    mint -= sin(2.0 * latitude) * (3.0 * referenceEllipsoid->eccentricitySquared / 8.0 + 3.0 * referenceEllipsoid->eccentricitySquared2 / 32.0 + 45.0 * referenceEllipsoid->eccentricitySquared3 / 1024.0);
    mint += sin(4.0 * latitude) * referenceEllipsoid->eccentricitySquared2 * (15.0 / 256.0 + 45.0 * referenceEllipsoid->eccentricitySquared / 1024.0);
    return referenceEllipsoid->equatorialRadius * (mint - sin(6.0 * latitude) * 35.0 * referenceEllipsoid->eccentricitySquared3 / 3072.0);
}

/** Performs forward-projection from ellipsoid to transverse Mercator */
double Projection::forward(Coordinates &from, Coordinates &to)
{
    double tanY = tan(from.y), cosY = cos(from.y), sinY = sin(from.y), cosY2 = cosY * cosY, sinY2 = sinY * sinY,
        t = tanY * tanY, t2 = t * t, c = referenceEllipsoid->polarEccentricitySquared * cosY2, w = from.x - centralLongitude, a = w * cosY,
        m = meridinal(from.y), m0 = meridinal(centralLatitude),
        nu = referenceEllipsoid->equatorialRadius / sqrt(1.0 - referenceEllipsoid->eccentricitySquared * sinY2),
        rho = referenceEllipsoid->equatorialRadius *
        (1.0 - referenceEllipsoid->eccentricitySquared) /
        pow(1.0 - referenceEllipsoid->eccentricitySquared * sinY2, 1.5),
        psi = nu / rho, psi2 = psi * psi;
    to.x = (5.0 - 18.0 * t + t2 + 72.0 * c - 58.0 * referenceEllipsoid->polarEccentricitySquared) * a * a / 120.0 + (1 - t + c) / 6.0;
    to.x = (1 + to.x * a * a) * a * scale * nu + falseEasting;
    to.y = a * a * (61.0 - 58.0 * t + t2 + 600.0 * c - 330.0 * referenceEllipsoid->polarEccentricitySquared) / 720.0 + (5.0 - t + 9.0 * c + 4.0 * c * c) / 24.0;
    to.y = (0.5 + a * a * to.y) * a * a * nu * tanY + m - m0;
    to.y = falseNorthing + scale * to.y;
    return -w * sinY - (pow(w, 3.0) / 3.0) * sinY * cosY2 * (2.0 * psi2 - psi) - (pow(w, 5.0) / 15.0) * sinY * pow(cosY, 4.0) * (pow(psi, 4.0) * (11.0 - 24.0 * t) - pow(psi, 3.0) * (11.0 - 36.0 * t) + 2.0 * psi2 * (1.0 - 7.0 * t) + psi * t) - (pow(w, 7.0) / 315.0) * sinY * pow(cosY, 6.0) * (17.0 - 26.0 * t + 2.0 * pow(tanY, 4.0));
}

Datum::Datum(string name, ReferenceEllipsoid *referenceEllipsoid, double deltaX, double deltaY, double deltaZ) : name(name), referenceEllipsoid(referenceEllipsoid), deltaX(deltaX), deltaY(deltaY), deltaZ(deltaZ)
{
}

Coordinates& Datum::convert(const Coordinates &from, Coordinates &to, Datum &toDatum)
{
    double dx = deltaX - toDatum.deltaX, dy = deltaY - toDatum.deltaY, dz = deltaZ - toDatum.deltaZ,
        deltaEquatorialRadius = toDatum.referenceEllipsoid->equatorialRadius - referenceEllipsoid->equatorialRadius,
        deltaFlattening = toDatum.referenceEllipsoid->flattening - referenceEllipsoid->flattening,
        sinX = sin(from.x), cosX = cos(from.x), sinY = sin(from.y), cosY = cos(from.y),
        sinY2 = sinY * sinY, sinYcosY = sinY * cosY,
        radiusOfCurvatureInPrimeVertical = referenceEllipsoid->equatorialRadius /
        sqrt(1.0 - referenceEllipsoid->eccentricitySquared * sinY2),
        radiusOfCurvatureInPrimeMeridian = referenceEllipsoid->equatorialRadius *
        (1.0 - referenceEllipsoid->eccentricitySquared) /
        pow(1.0 - referenceEllipsoid->eccentricitySquared * sinY2, 1.5);
    to.x = from.x + (-dx * sinX + dy * cosX) / ((radiusOfCurvatureInPrimeVertical + from.z) *  cosY);
    to.y = from.y + (-dx * sinY * cosX - dy * sinY * sinX + dz * cosY +
                     deltaEquatorialRadius * (radiusOfCurvatureInPrimeVertical * referenceEllipsoid->eccentricitySquared *
                                              sinYcosY / referenceEllipsoid->equatorialRadius) + deltaFlattening *
                     (radiusOfCurvatureInPrimeMeridian / referenceEllipsoid->polarRadiusDividedByEquatorialRadius +
                      radiusOfCurvatureInPrimeVertical * referenceEllipsoid->polarRadiusDividedByEquatorialRadius) * sinYcosY) /
        (radiusOfCurvatureInPrimeMeridian + from.z);
    to.z = from.z + dx * cosY * cosX + dy * cosY * sinX + dz * sinY - deltaEquatorialRadius *
        (referenceEllipsoid->equatorialRadius / radiusOfCurvatureInPrimeVertical) +
        deltaFlattening * referenceEllipsoid->polarRadiusDividedByEquatorialRadius * radiusOfCurvatureInPrimeVertical * sinY2;
    return to;
}

/** Converts coordinates from one datum to Earth Centered Fixed Cartesian Coordinates. */
Coordinates& Datum::convert(const Coordinates &from, Coordinates &ecef)
{
    double sinX = sin(from.x), cosX = cos(from.x),
        sinY = sin(from.y), cosY = cos(from.y), sinY2 = sinY * sinY,
        radiusOfCurvatureInPrimeVertical = referenceEllipsoid->equatorialRadius /
        sqrt(1.0 - referenceEllipsoid->eccentricitySquared * sinY2),
        sinEs = sin(1.0 - referenceEllipsoid->eccentricitySquared);
    ecef.x = (radiusOfCurvatureInPrimeVertical + from.z) * cosY * cosX;
    ecef.y = (radiusOfCurvatureInPrimeVertical + from.z) * cosY * sinX;
    ecef.z = (referenceEllipsoid->equatorialRadius / sqrt(1.0 - referenceEllipsoid->eccentricitySquared * sinEs * sinEs) + from.z) * sinY;
    return ecef;
}
