
package FI.realitymodeler.gps;

import javax.vecmath.*;

public class Datum
{
	String name;
	ReferenceEllipsoid ellipsoid;
	double deltaX, deltaY, deltaZ;

public Datum(String name, ReferenceEllipsoid ellipsoid, double deltaX, double deltaY, double deltaZ)
{
	this.name = name;
	this.ellipsoid = ellipsoid;
	this.deltaX = deltaX;
	this.deltaY = deltaY;
	this.deltaZ = deltaZ;
}

/** Converts coordinates from one datum to other datum. */
public Vector3d convert(Vector3d from, Vector3d to, Datum toDatum)
{
	double dx = deltaX - toDatum.deltaX, dy = deltaY - toDatum.deltaY, dz = deltaZ - toDatum.deltaZ,
	deltaEquatorialRadius = toDatum.ellipsoid.equatorialRadius - ellipsoid.equatorialRadius,
	deltaFlattening = toDatum.ellipsoid.flattening - ellipsoid.flattening,
	sinX = Math.sin(from.x), cosX = Math.cos(from.x), sinY = Math.sin(from.y), cosY = Math.cos(from.y),
	sinY2 = sinY * sinY, sinYcosY = sinY * cosY,
	radiusOfCurvatureInPrimeVertical = ellipsoid.equatorialRadius /
	Math.sqrt(1.0 - ellipsoid.eccentricitySquared * sinY2),
	radiusOfCurvatureInPrimeMeridian = ellipsoid.equatorialRadius *
	(1.0 - ellipsoid.eccentricitySquared) /
	Math.pow(1.0 - ellipsoid.eccentricitySquared * sinY2, 1.5);
	to.x = from.x + (-dx * sinX + dy * cosX) / ((radiusOfCurvatureInPrimeVertical + from.z) *  cosY);
	to.y = from.y + (-dx * sinY * cosX - dy * sinY * sinX + dz * cosY +
	deltaEquatorialRadius * (radiusOfCurvatureInPrimeVertical * ellipsoid.eccentricitySquared *
	sinYcosY / ellipsoid.equatorialRadius) + deltaFlattening *
	(radiusOfCurvatureInPrimeMeridian / ellipsoid.polarRadiusDividedByEquatorialRadius +
	radiusOfCurvatureInPrimeVertical * ellipsoid.polarRadiusDividedByEquatorialRadius) * sinYcosY) /
	(radiusOfCurvatureInPrimeMeridian + from.z);
	to.z = from.z + dx * cosY * cosX + dy * cosY * sinX + dz * sinY - deltaEquatorialRadius *
	(ellipsoid.equatorialRadius / radiusOfCurvatureInPrimeVertical) +
	deltaFlattening * ellipsoid.polarRadiusDividedByEquatorialRadius * radiusOfCurvatureInPrimeVertical * sinY2;
	return to;
}

/** Converts coordinates from one datum to Earth Centered Fixed Cartesian Coordinates. */
public Vector3d convert(Vector3d from, Vector3d ecef)
{
	double sinX = Math.sin(from.x), cosX = Math.cos(from.x), sinY = Math.sin(from.y), cosY = Math.cos(from.y), sinY2 = sinY * sinY,
	radiusOfCurvatureInPrimeVertical = ellipsoid.equatorialRadius /
	Math.sqrt(1.0 - ellipsoid.eccentricitySquared * sinY2),
	sinEs = Math.sin(1.0 - ellipsoid.eccentricitySquared);
	ecef.x = (radiusOfCurvatureInPrimeVertical + from.z) * cosY * cosX;
	ecef.y = (radiusOfCurvatureInPrimeVertical + from.z) * cosY * sinX;
	ecef.z = (ellipsoid.equatorialRadius / Math.sqrt(1.0 - ellipsoid.eccentricitySquared * sinEs * sinEs) + from.z) * sinY;
	return ecef;
}

public static Vector3d convertFromNgcsToWGS84(Vector3d ngcs, Vector3d wgs84)
{
	if (ngcs.y <= 7200000)
	{
		wgs84.y = -125.056 + 0.9999955294 * ngcs.y + 0.0000068348 * ngcs.x;
		wgs84.x = -107.850 + 0.9999955294 * ngcs.x - 0.0000068348 * ngcs.y;
	}
	else
	{
		wgs84.y = -132.881 + 1.0000006841 * ngcs.y - 0.0000015179 * ngcs.x;
		wgs84.x = -185.993 + 1.0000006841 * ngcs.x + 0.0000015179 * ngcs.y;
	}
	return wgs84;
}

public static Vector3d convertFromWGS84toNgcs(Vector3d wgs84, Vector3d ngcs)
{
//System.out.println("y="+(7200000-125.056)/1.0000044706);
	if (ngcs.y <= 7200000)
	{
		ngcs.x =  125.056 + 1.0000044706 * wgs84.x - 0.0000068348 * wgs84.y;
		ngcs.y =  107.850 + 1.0000044706 * wgs84.y + 0.0000068348 * wgs84.x;
	}
	else
	{
		ngcs.x =  132.881 + 0.9999993159 * wgs84.x + 0.0000015179 * wgs84.y;
		ngcs.y =  185.993 + 0.9999993159 * wgs84.y - 0.0000015179 * wgs84.x;
	}
	return ngcs;
}

}