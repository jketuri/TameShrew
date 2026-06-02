
package FI.realitymodeler.gps;

public class ReferenceEllipsoid
{
	String name;
	double equatorialRadius, polarRadius;
	double flattening, reciprocalFlattening;
	double polarRadiusDividedByEquatorialRadius, polarEccentricitySquared;
	double eccentricitySquared, eccentricitySquared2, eccentricitySquared3;

public ReferenceEllipsoid(String name, double equatorialRadius, double reciprocalFlattening)
{
	this.name = name;
	this.equatorialRadius = equatorialRadius;
	this.reciprocalFlattening = reciprocalFlattening;
	flattening = 1.0 / reciprocalFlattening;
	polarRadiusDividedByEquatorialRadius = 1.0 - flattening;
	polarRadius = polarRadiusDividedByEquatorialRadius * equatorialRadius;
	eccentricitySquared = 2.0 * flattening - flattening * flattening;
	polarEccentricitySquared = eccentricitySquared / (1.0 - eccentricitySquared);
	eccentricitySquared2 = eccentricitySquared * eccentricitySquared;
	eccentricitySquared3 = eccentricitySquared2 * eccentricitySquared;
}

public ReferenceEllipsoid(String name, double equatorialRadius, double polarRadius, boolean radiuses)
{
	this(name, equatorialRadius, 1.0 / ((equatorialRadius - polarRadius) / equatorialRadius));
}

public String toString()
{
	return getClass().getName() + "{name=" + name + ",equatorialRadius=" + equatorialRadius +
	",polarRadius=" + polarRadius + ",flattening=" + flattening + ",reciprocalFlattening=" + reciprocalFlattening +
	",eccentricitySquared=" + eccentricitySquared + ",polarRadiusDividedByEquatorialRadius=" + polarRadiusDividedByEquatorialRadius +
	",polarEccentricitySquard=" + polarEccentricitySquared;
}

}
