
package FI.realitymodeler.gps;

import javax.vecmath.*;

public class Projection
{
	String name;
	ReferenceEllipsoid ellipsoid;
	double centralLongitude, centralLatitude;
	double scale;
	double falseEasting, falseNorthing;

public Projection(String name, ReferenceEllipsoid ellipsoid, double centralLongitude, double centralLatitude, double scale, double falseEasting, double falseNorthing)
{
	this.name = name;
	this.ellipsoid = ellipsoid;
	this.centralLongitude = centralLongitude;
	this.centralLatitude = centralLatitude;
	this.scale = scale;
	this.falseEasting = falseEasting;
	this.falseNorthing = falseNorthing;
}

final double meridinal(double latitude)
{
	double mint = latitude * (1.0 - ellipsoid.eccentricitySquared / 4.0 - 3.0 * ellipsoid.eccentricitySquared2 / 64.0 - 5 * ellipsoid.eccentricitySquared3 / 256.0);
	mint -= Math.sin(2.0 * latitude) * (3.0 * ellipsoid.eccentricitySquared / 8.0 + 3.0 * ellipsoid.eccentricitySquared2 / 32.0 + 45.0 * ellipsoid.eccentricitySquared3 / 1024.0);
	mint += Math.sin(4.0 * latitude) * ellipsoid.eccentricitySquared2 * (15.0 / 256.0 + 45.0 * ellipsoid.eccentricitySquared / 1024.0);
	return ellipsoid.equatorialRadius * (mint - Math.sin(6.0 * latitude) * 35.0 * ellipsoid.eccentricitySquared3 / 3072.0);
}

/** Performs forward-projection from ellipsoid to transverse Mercator */
public Vector3d forward(Vector3d from, Vector3d to)
{
	double tanY = Math.tan(from.y), cosY = Math.cos(from.y), sinY = Math.sin(from.y),
	t = tanY * tanY, t2 = t * t, c = ellipsoid.polarEccentricitySquared * cosY * cosY, a = (from.x - centralLongitude) * cosY,
	m = meridinal(from.y), m0 = meridinal(centralLatitude),
	nu = ellipsoid.equatorialRadius / Math.sqrt(1.0 - ellipsoid.eccentricitySquared * sinY * sinY);
	to.x = (5.0 - 18.0 * t + t2 + 72.0 * c - 58.0 * ellipsoid.polarEccentricitySquared) * a * a / 120.0 + (1 - t + c) / 6.0;
	to.x = (1 + to.x * a * a) * a * scale * nu + falseEasting;
	to.y = a * a * (61.0 - 58.0 * t + t2 + 600.0 * c - 330.0 * ellipsoid.polarEccentricitySquared) / 720.0 + (5.0 - t + 9.0 * c + 4.0 * c * c) / 24.0;
	to.y = (0.5 + a * a * to.y) * a * a * nu * tanY + m - m0;
	to.y = falseNorthing + scale * to.y;
	return to;
}

}