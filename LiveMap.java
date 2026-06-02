
package FI.realitymodeler.gps;

import FI.realitymodeler.common.*;
import FI.realitymodeler.image.*;
import FI.realitymodeler.sql.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.sql.*;
import java.text.*;
import java.util.*;
import javax.comm.*;
import javax.media.j3d.*;
import javax.vecmath.*;

class LiveMapShutdownHook extends Thread
{
	LiveMap liveMap;

LiveMapShutdownHook(LiveMap liveMap)
{
	this.liveMap = liveMap;
}

public void run()
{
	try
	{
		liveMap.connection.close();
	}
	catch (SQLException ex) {}
}

}

public class LiveMap extends Frame implements Runnable
{
	static
	{
		DatabaseDriver.prepare();
	}

	static final boolean DEBUG = false;

	BranchGroup branch;
	Connection connection = null;
	HashMap mapImages = new HashMap();
	MapShapeUpdate mapShapeUpdate = null;
	RoutesUpdate routesUpdate = null;
	Shape3D trackShape = null;
	InputStream in;
	MapIndex mapIndex;
	QuadArray plane;
	SerialPort serialPort;
	TransformGroup arrowHeadTransform, viewPlatformTransform;
	Transform3D arrowHeadTransform3d = new Transform3D(), vesselTransform3d = new Transform3D(), viewPlatformTransform3d = new Transform3D();
	Vector3d vesselPosition = new Vector3d(), lastVesselPosition = new Vector3d(), deltaVesselPosition = new Vector3d(),
	vesselVelocity = new Vector3d(), vesselVector = new Vector3d(), vpVector = new Vector3d(), gllVector = new Vector3d();
	HashMap mapShapes = new HashMap();
	Vector waypoints = new Vector();
	HashMap waypointLocations = new HashMap();
	NMEA nmea = new NMEA(183);
	{
/*
// Waypoint Arrival Alarm
		nmea.nmeaSentences.put("AAM", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				char arrivalCircleEnteredStatus = parseChar((String)dataFields.elementAt(0)),
				perpendicularPassedAtWaypointStatus = parseChar((String)dataFields.elementAt(1));
				double arrivalCircleRadius = parseDouble((String)dataFields.elementAt(2));
				String waypointID = (String)dataFields.elementAt(4);
			}
		});
// GPS Almanac Data
		nmea.nmeaSentences.put("ALM", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				int totalNumberOfMessages = parseInt((String)dataFields.elementAt(0)),
				messageNumber = parseInt((String)dataFields.elementAt(1)),
				satellitePRNnumber = parseInt((String)dataFields.elementAt(2)),
				gpsWeekNumber = parseInt((String)dataFields.elementAt(3));
				String SVhealt = (String)dataFields.elementAt(4),
				eccentricity = (String)dataFields.elementAt(5),
				almanacRefenceTime = (String)dataFields.elementAt(6),
				inclinationAngle = (String)dataFields.elementAt(7),
				rateOfRightAscension = (String)dataFields.elementAt(8),
				rootOfSemiMajorAxis = (String)dataFields.elementAt(9),
				argumentOfPerigee = (String)dataFields.elementAt(10),
				longitudeOfAcension = (String)dataFields.elementAt(11),
				meanAnomaly = (String)dataFields.elementAt(12),
				clockParameter1 = (String)dataFields.elementAt(13),
				clockParameter2 = (String)dataFields.elementAt(14);
			}
		});
// Autopilot Sentence "B"
		nmea.nmeaSentences.put("APB", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				char status1 = parseChar((String)dataFields.elementAt(0)),
				status2 = parseChar((String)dataFields.elementAt(1));
				double crossTrackError = parseDouble((String)dataFields.elementAt(2));
				char directionToSteer = parseChar((String)dataFields.elementAt(3)),
				arrivalCircleEnteredStatus = parseChar((String)dataFields.elementAt(5)),
				perpendicularPassedAtWaypoint = parseChar((String)dataFields.elementAt(6));
				double bearingOriginToDestination = parseDouble((String)dataFields.elementAt(7));
				char bearingOriginToDestinationMagneticOrTrue = parseChar((String)dataFields.elementAt(8));
				String destinationWaypointID = (String)dataFields.elementAt(9);
				double presentPositionToDestination = parseDouble((String)dataFields.elementAt(10));
				char presentPositionToDestinationMagneticOrTrue = parseChar((String)dataFields.elementAt(11));
				double headingToSteerToDestinationWaypoint = parseDouble((String)dataFields.elementAt(12));
				char headintToSteerToDestinationWaypointMagneticOrTrue = parseChar((String)dataFields.elementAt(13));
			}
		});
// Bearing & Distance to Waypoint - Dead Reckoning
		nmea.nmeaSentences.put("BEC", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields) throws ParseException
			{
				String utcOfObservation = (String)dataFields.elementAt(0);
				double latitude = parseLatitude((String)dataFields.elementAt(1), (String)dataFields.elementAt(2)),
				longitude = parseLongitude((String)dataFields.elementAt(3), (String)dataFields.elementAt(4)),
				bearingMagnetic = parseDouble((String)dataFields.elementAt(5)),
				distance = parseDouble((String)dataFields.elementAt(7));
				String waypointID = (String)dataFields.elementAt(9);
			}
		});
// Bearing - Origin to Destination
		nmea.nmeaSentences.put("BOD", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				double bearingTrue = parseDouble((String)dataFields.elementAt(0)),
				bearingMagnetic = parseDouble((String)dataFields.elementAt(2));
				String destinationWaypointID = (String)dataFields.elementAt(4),
				originWaypointID = (String)dataFields.elementAt(5);
			}
		});
// Bearing & Distance to Waypoint - Great Circle
		nmea.nmeaSentences.put("BWC", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields) throws ParseException
			{
				String utcOfObservation = (String)dataFields.elementAt(0);
				double latitude = parseLatitude((String)dataFields.elementAt(1), (String)dataFields.elementAt(2)),
				longitude = parseLongitude((String)dataFields.elementAt(3), (String)dataFields.elementAt(4)),
				bearingTrue = parseDouble((String)dataFields.elementAt(5)),
				bearingMagnetic = parseDouble((String)dataFields.elementAt(7)),
				distance = parseDouble((String)dataFields.elementAt(9));
				String waypointID = (String)dataFields.elementAt(11);
			}
		});
// Bearing & Distance to Waypoint - Rhumb Line
		nmea.nmeaSentences.put("BWR", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields) throws ParseException
			{
				String utcOfObservation = (String)dataFields.elementAt(0);
				double latitude = parseLatitude((String)dataFields.elementAt(1), (String)dataFields.elementAt(2)),
				longitude = parseLongitude((String)dataFields.elementAt(3), (String)dataFields.elementAt(4)),
				bearingTrue = parseDouble((String)dataFields.elementAt(5)),
				bearingMagnetic = parseDouble((String)dataFields.elementAt(7)),
				distance = parseDouble((String)dataFields.elementAt(9));
				String waypointID = (String)dataFields.elementAt(11);
			}
		});
// Global Positioning System Fix Data
		nmea.nmeaSentences.put("GGA", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields) throws ParseException
			{
				String timeOfFix = (String)dataFields.elementAt(0);
				double latitude = parseLatitude((String)dataFields.elementAt(1), (String)dataFields.elementAt(2)),
					longitude = parseLongitude((String)dataFields.elementAt(3), (String)dataFields.elementAt(4));
				int fixQualityIndicator = parseInt((String)dataFields.elementAt(5)),
					numberOfSatellitesBeingTracked = parseInt((String)dataFields.elementAt(6));
				double horizontalDilutionOfPosition = parseDouble((String)dataFields.elementAt(7)),
					antennaAltitudeAboveMeanSeaLevel = parseDouble((String)dataFields.elementAt(8)),
					geoidalSeparation = parseDouble((String)dataFields.elementAt(10)),
					ageOfDifferentialPositionData = parseDouble((String)dataFields.elementAt(12));
				if (dataFields.size() < 14) return;
				String differentialReferenceStatiodID = (String)dataFields.elementAt(13);
			}
		});
*/
// Geographic Position, Latitude and Longitude
		nmea.nmeaSentences.put("GLL", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields) throws ParseException
			{
System.out.println("dataFields="+dataFields);
				String timeOfFix = dataFields.size() > 4 ? (String)dataFields.elementAt(4) : "";
				double longitude = parseLongitude((String)dataFields.elementAt(2), (String)dataFields.elementAt(3)),
					latitude = parseLatitude((String)dataFields.elementAt(0), (String)dataFields.elementAt(1));
System.out.println("longitude="+longitude+",latitude="+latitude);
				gllVector.set(longitude, latitude, 0.0);
				char status = dataFields.size() > 5 ? parseChar((String)dataFields.elementAt(5)) : 0;
				relocateVessel(gllVector);
				if (followVessel) setViewPlatformWithVessel(vesselPosition);
			}
		});
/*
// GPS DOP and Active Satellites
		nmea.nmeaSentences.put("GSA", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				char mode = parseChar((String)dataFields.elementAt(0));
				int currentMode = parseInt((String)dataFields.elementAt(1));
				double positionDilutionOfPrecision = parseDouble((String)dataFields.elementAt(14)),
				horizontalDilutionOfPrecision = parseDouble((String)dataFields.elementAt(15)),
				verticalDilutionOfPrecision = parseDouble((String)dataFields.elementAt(16));
			}
		});
// GPS Satellites in View
		nmea.nmeaSentences.put("GSV", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				int numberOfSentences = parseInt((String)dataFields.elementAt(0)),
				sentenceNumber = parseInt((String)dataFields.elementAt(1)),
				numberOfSatellitesInView = parseInt((String)dataFields.elementAt(2)),
				satellitePRNnumber = parseInt((String)dataFields.elementAt(3)),
				elevation = parseInt((String)dataFields.elementAt(4)),
				azimuth = parseInt((String)dataFields.elementAt(5)),
				signalStrength = parseInt((String)dataFields.elementAt(6));
			}
		});
// Recommended minimum navigation information
		nmea.nmeaSentences.put("RMB", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields) throws ParseException
			{
				char dataStatus = parseChar((String)dataFields.elementAt(0));
				double crossTrackError = parseDouble(((String)dataFields.elementAt(1)));
				char steerDirection = parseChar((String)dataFields.elementAt(2));
				String originWaypointID = (String)dataFields.elementAt(3),
				destionationWaypointID = (String)dataFields.elementAt(4);
				double destinationWaypointLatitude = parseLatitude((String)dataFields.elementAt(5), (String)dataFields.elementAt(6)),
				destinationWaypointLongitude = parseLongitude((String)dataFields.elementAt(7), (String)dataFields.elementAt(8)),
				rangeToDestination = parseDouble((String)dataFields.elementAt(9)),
				trueBearingToDestination = parseDouble((String)dataFields.elementAt(10)),
				velocityTowardsDestination = parseDouble((String)dataFields.elementAt(11));
				char arrivalAlarm = parseChar((String)dataFields.elementAt(12));
			}
		});
// Recommended Minimum Specific GPS/Transit Data
		nmea.nmeaSentences.put("RMC", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields) throws ParseException
			{
				String timeOfFix = (String)dataFields.elementAt(0);
				char status = parseChar((String)dataFields.elementAt(1));
				double latitude = parseLatitude((String)dataFields.elementAt(2), (String)dataFields.elementAt(3)),
				longitude = parseLongitude((String)dataFields.elementAt(4), (String)dataFields.elementAt(5)),
				groundSpeed = parseDouble((String)dataFields.elementAt(6)),
				trackMadeGoodTrue = parseDouble((String)dataFields.elementAt(7));
				String dateOfFix = (String)dataFields.elementAt(8);
				double magneticVariation = parseDouble((String)dataFields.elementAt(9));
				if (parseChar((String)dataFields.elementAt(10)) == 'W') magneticVariation = -magneticVariation;
			}
		});
*/
// Waypoints in active route
		nmea.nmeaSentences.put("RTE", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				int numberOfSentences = parseInt((String)dataFields.elementAt(0)),
				sentenceNumber = parseInt((String)dataFields.elementAt(1));
				char mode = parseChar((String)dataFields.elementAt(2));
				String routeId = (String)dataFields.elementAt(3);
				waypointsPending = false;
				if (sentenceNumber <= 1) waypoints.removeAllElements();
				int l = dataFields.size();
				for (int i = 4; i < l; i++) waypoints.addElement(dataFields.elementAt(i));
//System.out.println("rte sn="+sentenceNumber+",nos="+numberOfSentences+",mode="+mode);
				if (sentenceNumber == numberOfSentences)
				{
					if (mode == 'W')
					{
						waypoints.insertElementAt("", 0);
						waypointLocations.put("", gllVector);
					}
					if (routesUpdate == null || !routesUpdate.waypoints.equals(waypoints))
					if (waypointLocations.size() >= waypoints.size()) updateRoutes();
					else waypointsPending = true;
				}
			}
		});
/*
// Track Made Good and Ground Speed
		nmea.nmeaSentences.put("VTG", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				double trackMadeGoodTrue = parseDouble((String)dataFields.elementAt(0)),
				trackMadeGoodMagnetic = parseDouble((String)dataFields.elementAt(1)),
				groundSpeedInKnots = parseDouble((String)dataFields.elementAt(2)),
				groundSpeedInKMH = parseDouble((String)dataFields.elementAt(4));
			}
		});
*/
// Waypoint location
		nmea.nmeaSentences.put("WPL", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields) throws ParseException
			{
				double latitude = parseLatitude((String)dataFields.elementAt(0), (String)dataFields.elementAt(1)),
				longitude = parseLongitude((String)dataFields.elementAt(2), (String)dataFields.elementAt(3));
				String waypointID = (String)dataFields.elementAt(4);
				waypointLocations.put(waypointID, new Vector3d(latitude, longitude, 0.0));
				if (waypointsPending && waypointLocations.size() >= waypoints.size())
				{
					waypointsPending = false;
					updateRoutes();
				}
//System.out.println("wpl id="+waypointID+",latitude="+latitude+",longitude="+longitude);
			}
		});
/*
// Time & Date
		nmea.nmeaSentences.put("ZDA", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				String utc = (String)dataFields.elementAt(0);
				int day = parseInt((String)dataFields.elementAt(1)),
				month = parseInt((String)dataFields.elementAt(2)),
				year = parseInt((String)dataFields.elementAt(3)),
				localZoneDescriptionHours = parseInt((String)dataFields.elementAt(4)),
				localZoneDescriptionMinutes = parseInt((String)dataFields.elementAt(5));
			}
		});
// Garmin
		nmea.nmeaSentences.put("PGRME", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				double estimatedHorizontalPositionError = parseDouble((String)dataFields.elementAt(0)),
				estimatedVerticalPositionError = parseDouble((String)dataFields.elementAt(2)),
				overallSphericalEquivalentPositionError = parseDouble((String)dataFields.elementAt(4));
			}
		});
// Currently active horizontal datum
		nmea.nmeaSentences.put("PGRMM", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				String name = (String)dataFields.elementAt(0);
			}
		});
		nmea.nmeaSentences.put("PGRMZ", new NMEASentence()
		{
			public void sentenceData(String id, Vector dataFields)
			{
				double altitude = parseDouble((String)dataFields.elementAt(0));
				char positionFixDimensions = parseChar((String)dataFields.elementAt(2));
			}
		});
*/
	}
	boolean followVessel = false, tracksBranchInserted = false, waypointsPending = false;
	double map_shape_width, map_shape_height, x_factor = 20.0, y_factor = 20.0;
	int rectangle_side = 512, half_rectangle_side = rectangle_side / 2;
	long current_offset_point_x = -1L, current_offset_point_y = -1L, current_point_x, current_point_y;
	long vesselTime;

class Interaction extends Behavior
{

	private	static	final	int	UP_ARROW	= (1<<0);
	private	static	final	int	DOWN_ARROW	= (1<<1);
	private	static	final	int	LEFT_ARROW	= (1<<2);
	private	static	final	int	RIGHT_ARROW	= (1<<3);
	private	static	final	int	PLUS_SIGN	= (1<<4);
	private	static	final	int	MINUS_SIGN	= (1<<5);
	private	static	final	int	PAGE_UP		= (1<<6);
	private	static	final	int	PAGE_DOWN	= (1<<7);
	private	static	final	int	HOME_DIR	= (1<<8);
	private	static	final	int	HOME_NOMINAL= (1<<9);

	private	static	final	int	SHIFT		= (1<<10);
	private	static	final	int	ALT			= (1<<11);
	private	static	final	int	CONTROL		= (1<<12);

	private	static	final	int	KEY_UP		= (1<<13);
	private	static	final	int	KEY_DOWN	= (1<<14);

//	double mmx = 128.0,
	double mmx = 0.01,
	vpScale,
	fwdVMax = -mmx,
	bwdVMax = mmx,
	leftVMax = -mmx,
	rightVMax = mmx,
	upVMax = mmx,
	downVMax = -mmx,
	rightRotAngle = Math.PI * 2.0 / 8.0,
	leftRotAngle = -rightRotAngle,
	upRotAngle = rightRotAngle,
	downRotAngle = -upRotAngle;
	Vector3d navVec = new Vector3d(0.0, 0.0, 0.0),
	fwdAcc = new Vector3d(0.0, mmx, 0.0),
	bwdAcc = new Vector3d(0.0, -mmx, 0.0),
	leftAcc = new Vector3d(-mmx, 0.0, 0.0),
	rightAcc = new Vector3d(mmx, 0.0, 0.0),
	upAcc = new Vector3d(0.0, 0.0, mmx),
	downAcc = new Vector3d(0.0, 0.0, -mmx),
	fwdDrag = new Vector3d(0.0, -mmx, 0.0),
	bwdDrag = new Vector3d(0.0, mmx, 0.0),
	leftDrag = new Vector3d(mmx, 0.0, 0.0),
	rightDrag = new Vector3d(-mmx, 0.0, 0.0),
	upDrag = new Vector3d(0.0, 0.0, -mmx),
	downDrag = new Vector3d(0.0, 0.0, mmx),
	a = new Vector3d(),
	dv = new Vector3d(),
	vpPos = new Vector3d(),
	nda = new Vector3d(),
	temp = new Vector3d();
	Point3d dp = new Point3d();
	Quat4d udQuat = new Quat4d(),
	lrQuat = new Quat4d(),
	vpQuat = new Quat4d();
	Matrix4d vpMatrix = new Matrix4d(),
	mat = new Matrix4d();
	Transform3D nominal = new Transform3D();
	WakeupOr condition = new WakeupOr(new WakeupCriterion[]
	{new WakeupOnAWTEvent(KeyEvent.KEY_PRESSED),
	new WakeupOnAWTEvent(KeyEvent.KEY_RELEASED)});
	int key_state = 0, modifier_key_state = 0;
	long time = System.currentTimeMillis();

public void initialize()
{
	viewPlatformTransform.getTransform(nominal);
	wakeupOn(condition);
}

private long getDeltaTime()
{
	long newTime = System.currentTimeMillis();
	long deltaTime = newTime - time;
	time = newTime;
	if (deltaTime > 2000L) return 0L;
	return deltaTime;
}

/* Generate a quaterion as a rotation of radians av about 0/x 1/y 2/z axis */
private void genRotQuat(double av, int axis, Quat4d q)
{
	double b;

	q.x = q.y = q.z = 0.0;
	q.w = Math.cos(av / 2.0);

	b = 1.0 - q.w * q.w;

	if (b > 0.0) b = Math.sqrt(b);
	else return;

	if (av < 0.0) b = -b;
	if (axis == 0) q.x = b;
	else if (axis == 1) q.y = b;
	else q.z = b;
}

private final void accKeyAdd(Vector3d a, Vector3d da, Vector3d drag, double scaleVel)
{
	/* Scaling of acceleration due to modification keys */
	nda.scale(scaleVel, da);
	/* Addition of sufficent acceleration to counteract drag */
	nda.sub(drag);

	/* Summing into overall acceleration */
	a.add(nda);

}


	/**
	 * Computes a new transform for the next frame based on
	 * the current transform, accumulated keyboard inputs, and
	 * elapsed time.  This new transform is written into the target
	 * transform group.
	 * This method should be called once per frame.
	 */
public void integrateTransformChanges()
{
	double scaleVel, scaleRot, scaleScale, pre, udAng, lrAng, r;

	// Get the current View Platform transform into a transform3D object.
	viewPlatformTransform.getTransform(viewPlatformTransform3d);
	// Extract the position, quaterion, and scale from the transform3D.
	vpScale = viewPlatformTransform3d.get(vpQuat, vpPos);

	double deltaTime = (double)getDeltaTime();
	deltaTime *= 0.001;

	/* Calculate scale due to modification keys */
	if ((modifier_key_state & SHIFT) != 0 && (modifier_key_state & CONTROL) == 0)
	{
		scaleVel = 5.0;
		scaleRot = 2.0;
		scaleScale = 4.0;
	}
	else if ((modifier_key_state & SHIFT) == 0 && (modifier_key_state & CONTROL) != 0)
	{
		scaleVel = 7.0;
		scaleRot = 2.0;
		scaleScale = 4.0;
	}
	else if ((modifier_key_state & SHIFT) != 0 && (modifier_key_state & CONTROL) != 0)
	{
		scaleVel = 10.0;
		scaleRot = 2.0;
		scaleScale = 4.0;
	}
	else
	{
		scaleRot = scaleVel = 1.0;
		scaleScale = 4.0;
	}

	/*
	 *  Processing of rectilinear motion keys.
	 */

	a.x = a.y = a.z = 0.0;  /* acceleration initially 0 */

	/* Acceleration due to keys being down */
	if ((key_state & UP_ARROW) != 0)
	{
		if ((key_state & DOWN_ARROW) == 0) accKeyAdd(a, fwdAcc, fwdDrag, scaleVel);
	}
	else if ((key_state & DOWN_ARROW) != 0) accKeyAdd(a, bwdAcc, bwdDrag, scaleVel);

	if ((key_state & LEFT_ARROW) != 0)
	{
		if ((key_state & RIGHT_ARROW) == 0) accKeyAdd(a, leftAcc, leftDrag, scaleVel);
	}
	else if ((key_state & RIGHT_ARROW) != 0) accKeyAdd(a, rightAcc, rightDrag, scaleVel);

	if ((key_state & PAGE_UP) != 0)
	{
		if ((key_state & PAGE_DOWN) == 0) accKeyAdd(a, upAcc, upDrag, scaleVel);
	}
	else if ((key_state & PAGE_DOWN) != 0) accKeyAdd(a, downAcc, downDrag, scaleVel);

	/*
	 *  Drag due to new or existing motion
	 */
	pre = navVec.z + a.z * deltaTime;
	if (pre < 0.0)
		if (pre + downDrag.z * deltaTime < 0.0) a.add(downDrag);
		else a.z -= pre / deltaTime;
	else if (pre > 0.0)
		if (pre + upDrag.z * deltaTime > 0.0) a.add(upDrag);
		else a.z -= pre / deltaTime;

	pre = navVec.x + a.x * deltaTime;
	if (pre < 0.0)
		if (pre + leftDrag.x * deltaTime < 0.0) a.add(leftDrag);
		else a.x -= pre / deltaTime;
	else if (pre > 0.0)
		if (pre + rightDrag.x * deltaTime > 0.0) a.add(rightDrag);
		else a.x -= pre / deltaTime;

	pre = navVec.y + a.y * deltaTime;
	if (pre < 0.0)
		if (pre + bwdDrag.y * deltaTime < 0.0) a.add(bwdDrag);
		else a.y -= pre / deltaTime;
	else if (pre > 0.0)
		if (pre + fwdDrag.y * deltaTime > 0.0) a.add(fwdDrag);
		else a.y -= pre / deltaTime;

	/* Integration of acceleration to velocity */
	dv.scale(deltaTime, a);
	navVec.add(dv);

	/* Speed limits */
	if (navVec.z < scaleVel * fwdVMax) navVec.z = scaleVel * fwdVMax;
	if (navVec.z > scaleVel * bwdVMax) navVec.z = scaleVel * bwdVMax;
	if (navVec.x < scaleVel * leftVMax) navVec.x = scaleVel * leftVMax;
	if (navVec.x > scaleVel * rightVMax) navVec.x = scaleVel * rightVMax;
	if (navVec.y > scaleVel * upVMax) navVec.y = scaleVel * upVMax;
	if (navVec.y < scaleVel * downVMax) navVec.y = scaleVel * downVMax;

	/* Integration of velocity to distance */
	dp.scale(deltaTime, navVec);

	/* Scale our motion to the current avatar scale */
	// 1.0 eventually needs to be a more complex value (see hs).
	//	  r = workplace_coexistence_to_vworld_ori.scale/
	//	one_to_one_coexistence_to_vworld_ori.scale;
	r = vpScale / 1.0;
	dp.scale(r, dp);

	/*
	 *  Processing of rotation motion keys.
	 */
	udAng = lrAng = 0.0;

	/* Rotation due to keys being down */
	if ((modifier_key_state & ALT) != 0)
	{
		if ((key_state & LEFT_ARROW) != 0)
		{
			if ((key_state & RIGHT_ARROW) == 0) lrAng = leftRotAngle;
		}
		else if ((key_state & RIGHT_ARROW) != 0) lrAng = rightRotAngle;

		if ((key_state & UP_ARROW) != 0)
		{
			if ((key_state & DOWN_ARROW) == 0) udAng = upRotAngle;
		}
		else if ((key_state & DOWN_ARROW) != 0) udAng = downRotAngle;
	}

	lrAng *= scaleRot;
	udAng *= scaleRot;

	/* Scaling of angle change to delta time */
	lrAng *= deltaTime;
	udAng *= deltaTime;

	/* Addition to existing orientation */
	// vr_quat_inverse(&workplace_coexistence_to_vworld_ori.quat, &vpQuat);
	// vpQuat gotten at top of method.
	vpQuat.inverse();

	if (lrAng != 0.0)
	{
		genRotQuat(lrAng, 1, lrQuat);
		vpQuat.mul(lrQuat, vpQuat);
	}

	if (udAng != 0.0)
	{
		genRotQuat(udAng, 0, udQuat);
		vpQuat.mul(udQuat, vpQuat);
	}

	/* Rotation of distance vector */
	vpQuat.inverse();
	vpQuat.normalize();
	mat.set(vpQuat);
	mat.transform(dp);

	/* Processing of scale */
	if ((key_state & PLUS_SIGN) != 0)
	{
		vpScale *= (1.0 + (scaleScale * deltaTime));
		if (vpScale > 10e+14) vpScale = 1.0;
	}
	else if ((key_state & MINUS_SIGN) != 0)
	{
		vpScale /= (1.0 + (scaleScale * deltaTime));
		if (vpScale < 10e-14) vpScale = 1.0;
	}

	// add dp into current vp position.
	vpPos.add(dp);

	if ((key_state & HOME_NOMINAL) != 0)
	{
		resetVelocity();
		// Extract the position, quaterion, and scale from the nominal
		// transform
		vpScale = nominal.get(vpQuat, vpPos);
	}

	/* Final update of view platform */
	// Put the transform back into the transform group.
	viewPlatformTransform3d.set(vpQuat, vpPos, vpScale);
	viewPlatformTransform.setTransform(viewPlatformTransform3d);

	viewPlatformTransform3d.get(vpVector);
	updateMapShapes(countPoint(vpVector));
}

/**
 * Resets the keyboard navigation velocity to 0.
 */
private void resetVelocity()
{
	navVec.x = navVec.y = navVec.z = 0.0;
}

public void processStimulus(Enumeration criteria)
{
	while (criteria.hasMoreElements())
	{
		WakeupCriterion criterion = (WakeupCriterion)criteria.nextElement();
		if (criterion instanceof WakeupOnAWTEvent)
		{
			AWTEvent events[] = ((WakeupOnAWTEvent)criterion).getAWTEvent();
			for (int i = 0; i < events.length; i++)
			if (events[i] instanceof KeyEvent)
			{
				KeyEvent keyEvent = (KeyEvent)events[i];
				if (keyEvent.getID() == KeyEvent.KEY_RELEASED)
				{
					int keyCode = keyEvent.getKeyCode(), keyChar = keyEvent.getKeyChar();
					if (keyChar == '+') key_state &= ~PLUS_SIGN;
					else
					switch (keyCode) {
					case KeyEvent.VK_UP:
						key_state &= ~UP_ARROW;
						break;
					case KeyEvent.VK_DOWN:
						key_state &= ~DOWN_ARROW;
						break;
					case KeyEvent.VK_LEFT:
						key_state &= ~LEFT_ARROW;
						break;
					case KeyEvent.VK_RIGHT:
						key_state &= ~RIGHT_ARROW;
						break;
					case KeyEvent.VK_PAGE_UP:
						key_state &= ~PAGE_UP;
						break;
					case KeyEvent.VK_PAGE_DOWN:
						key_state &= ~PAGE_DOWN;
						break;
					case KeyEvent.VK_EQUALS:
						key_state &= ~HOME_NOMINAL;
						break;
					default:
						switch(keyChar) {
						case '-':
							key_state &= ~MINUS_SIGN;
							break;
						}
					}
				}
				else if (keyEvent.getID() == KeyEvent.KEY_PRESSED)
				{
					int keyCode = keyEvent.getKeyCode(), keyChar = keyEvent.getKeyChar();
					if (keyChar == '+') key_state |=  PLUS_SIGN;
					switch (keyCode) {
					case KeyEvent.VK_UP:
						key_state |= UP_ARROW;
						break;
					case KeyEvent.VK_DOWN:
						key_state |= DOWN_ARROW;
						break;
					case KeyEvent.VK_LEFT:
						key_state |= LEFT_ARROW;
						break;
					case KeyEvent.VK_RIGHT:
						key_state |= RIGHT_ARROW;
						break;
					case KeyEvent.VK_PAGE_UP:
						key_state |=  PAGE_UP;
						break;
					case KeyEvent.VK_PAGE_DOWN:
						key_state |=  PAGE_DOWN;
						break;
					case KeyEvent.VK_EQUALS:
						key_state |= HOME_NOMINAL;
						break;
					case KeyEvent.VK_F1:
						followVessel = true;
						setViewPlatformWithVessel(countPoint(vesselVector));
						break;
					case KeyEvent.VK_F2:
						followVessel = false;
						break;
					default:
						switch(keyChar) {
						case '-':
							key_state |=  MINUS_SIGN;
							break;
						}
					}
				}

				/* Check modifier keys */
				if (keyEvent.isShiftDown()) modifier_key_state |=  SHIFT;
				else modifier_key_state &= ~SHIFT;

				if (keyEvent.isControlDown()) modifier_key_state |= CONTROL;
				else modifier_key_state &= ~CONTROL;
	
				if (keyEvent.isAltDown()) modifier_key_state |=  ALT;
				else modifier_key_state &= ~ALT;
			}
		}
	}
	integrateTransformChanges();
	wakeupOn(condition);
}

}

class MapShapeUpdate extends Thread implements Comparator
{
	OwnLock lock = new OwnLock(true);
	boolean canceled = false;

public int compare(Object o1, Object o2)
{
	return ((MapShape)o1).precedence - ((MapShape)o2).precedence;
}

public boolean equals(Object o)
{
	return o instanceof LiveMap;
}

public void run()
{
	while (!isInterrupted())
	try
	{
		lock.lock();
		canceled = false;
		long offset_point_x = current_offset_point_x, offset_point_y = current_offset_point_y,
		point_x = current_point_x, point_y = current_point_y;
/*
		for (int index = mapShapes.size() - 1; index >= 0; index--)
		{
			MapShape shape = (MapShape)mapShapes.elementAt(index);
			if (Math.abs(shape.point_x1 - offset_point_x) > rectangle_side ||
			Math.abs(shape.point_y1 - offset_point_y) > rectangle_side)
			synchronized (mapShapes)
			{
				if (DEBUG) System.out.println("removing map shape in " + shape.point_x1 + "," + shape.point_y1);
				mapShapes.removeElementAt(index);
				branch.removeChild(index);
			}
		}
*/
		if (mapShapes.isEmpty())
		{
			if (DEBUG) System.out.println("centering map shapes");
			current_offset_point_x = offset_point_x = point_x - half_rectangle_side;
			current_offset_point_y = offset_point_y = point_y - half_rectangle_side;
		}
		Vector mapShapeVector = new Vector();
		long limit_point_x = offset_point_x + rectangle_side, limit_point_y = offset_point_y + rectangle_side;
		for (long point_y1 = offset_point_y - rectangle_side; point_y1 <= limit_point_y; point_y1 += rectangle_side)
		for (long point_x1 = offset_point_x - rectangle_side; point_x1 <= limit_point_x; point_x1 += rectangle_side)
		{
			MapShape shape = (MapShape)mapShapes.get(point_x1 + "," + point_y1);
			if (shape != null) continue;
			shape = new MapShape();
			shape.point_x1 = point_x1;
			shape.point_y1 = point_y1;
			shape.point_x2 = point_x1 + rectangle_side - 1;
			shape.point_y2 = point_y1 + rectangle_side - 1;
			mapShapeVector.addElement(shape);
		}
System.out.println("mapShapeVector.size()="+mapShapeVector.size());
		if (canceled) continue;
		while (!mapShapeVector.isEmpty() && !canceled)
		{
			long time = System.currentTimeMillis();
			Iterator iter = mapShapeVector.iterator();
			while (iter.hasNext())
			synchronized (mapShapes)
			{
				MapShape shape = (MapShape)iter.next();
				shape.precedence = (int)(Math.abs(shape.point_x1 + half_rectangle_side - point_x) +
				Math.abs(shape.point_y1 + half_rectangle_side - point_y));
			}
			Collections.sort(mapShapeVector, this);
			MapShape shape = (MapShape)mapShapeVector.firstElement();
			mapShapeVector.removeElementAt(0);
			Vector entries = mapIndex.search(shape, x_factor, y_factor);
			if (entries.isEmpty()) continue;
			if (canceled) break;
			TiffImage images[] = new TiffImage[entries.size()];
			Rectangle rectangles[] = new Rectangle[images.length];
			for (int i = 0; i < images.length; i++)
			{
				MapEntry entry = (MapEntry)entries.elementAt(i);
				long entry_point_x1 = (long)(entry.coordinate_x1 / x_factor + .5),
				entry_point_y1 = (long)(entry.coordinate_y1 / x_factor + .5),
				entry_point_x2 = (long)(entry.coordinate_x2 / y_factor + .5),
				entry_point_y2 = (long)(entry.coordinate_y2 / y_factor + .5);
				TiffImage image = (TiffImage)mapImages.get(entry.filepath);
				if (image == null) mapImages.put(entry.filepath, image = new TiffImage(entry.filepath, 1, 1));
				long x1 = Math.max(shape.point_x1, entry_point_x1), y1 = Math.max(shape.point_y1, entry_point_y1),
				x2 = Math.min(shape.point_x2, entry_point_x2), y2 = Math.min(shape.point_y2, entry_point_y2);
				rectangles[i] = new Rectangle((int)(x1 - shape.point_x1), (int)(shape.point_y2 - y2),
				(int)(x2 - x1 + 1L), (int)(y2 - y1 + 1L));
				images[i] = (TiffImage)image.getSubimage((int)(x1 - entry_point_x1), (int)(entry_point_y2 - y2),
				rectangles[i].width, rectangles[i].height);
				if (DEBUG) System.out.println("rectangle " + i + "=" + rectangles[i]);
			}
			GroupImage image = new GroupImage(rectangle_side, rectangle_side, images, rectangles);
			ImageComponent2D image2d = new ImageComponent2D(ImageComponent2D.FORMAT_RGB, rectangle_side, rectangle_side);
			image2d.set(image);
			Texture2D texture = new Texture2D(Texture2D.BASE_LEVEL, Texture2D.RGB, image2d.getWidth(), image2d.getHeight());
			texture.setImage(0, image2d);
			texture.setEnable(true);
			double x = (double)shape.point_x1 / (double)half_rectangle_side,
			y = (double)shape.point_y1 / (double)half_rectangle_side;
			TransformGroup mapTransform = new TransformGroup();
			Transform3D mapTransform3d = new Transform3D();
			mapTransform3d.setTranslation(new Vector3d(x, y, 0.0));
			mapTransform.setTransform(mapTransform3d);
			BranchGroup mapBranch = new BranchGroup();
			mapBranch.setCapability(BranchGroup.ALLOW_DETACH);
			Appearance appearance = new Appearance();
			appearance.setTexture(texture);
			mapTransform.addChild(new Shape3D(plane, appearance));
			mapBranch.addChild(mapTransform);
			mapBranch.compile();
			if (DEBUG) System.out.println("inserting map shape in " + shape.point_x1 + "," + shape.point_y1);
			synchronized (mapShapes)
			{
				mapShapes.put(shape.point_x1 + "," + shape.point_y1, shape);
				branch.addChild(mapBranch);
				long deltaTime = System.currentTimeMillis() - time;
				point_x = current_point_x + (long)((double)deltaTime * vesselVelocity.x + .5);
				point_y = current_point_y + (long)((double)deltaTime * vesselVelocity.y + .5);
			}
		}
	}
	catch (Exception ex)
	{
		if (ex instanceof InterruptedException) break;
		ex.printStackTrace();
	}
}

}

class RoutesUpdate extends Thread
{
	RoutesUpdate currentRoutesUpdate;
	Vector waypoints;

RoutesUpdate(RoutesUpdate currentRoutesUpdate, Vector waypoints)
{
	this.currentRoutesUpdate = currentRoutesUpdate;
	this.waypoints = waypoints;
}

Vector3d getVector(Vector3d waypointLocation)
{
	Vector3d vector = new Vector3d();
	mapIndex.convert(waypointLocation, vector);
	vector.x /= x_factor * (double)half_rectangle_side;
	vector.y /= y_factor * (double)half_rectangle_side;
	return vector;
}

public void run()
{
	try
	{
		if (DEBUG) System.out.println("Updating route shapes, waypoints="+ waypoints + ",waypointLocations=" + waypointLocations);
		if (currentRoutesUpdate != null && currentRoutesUpdate.isAlive()) currentRoutesUpdate.join();
		if (tracksBranchInserted) branch.removeChild(branch.numChildren() - 1);
		BranchGroup tracksBranch = new BranchGroup();
		tracksBranch.setCapability(BranchGroup.ALLOW_DETACH);
		Transform3D transform3d = new Transform3D();
		Vector3d fromWaypointVector = null, scaleVector = new Vector3d(1.0, 1.0, 1.0);
		int l = waypoints.size();
		for (int i = 0; i < l; i++)
		{
			String waypointId = (String)waypoints.elementAt(i);
			Vector3d waypointLocation = (Vector3d)waypointLocations.get(waypointId);
			if (waypointLocation == null) continue;
			Vector3d toWaypointVector = getVector(waypointLocation);
			if (fromWaypointVector != null)
			{
				TransformGroup trackTransform = new TransformGroup();
				Transform3D trackTransform3d = new Transform3D();
				trackTransform3d.setTranslation(fromWaypointVector);
				Vector3d deltaTrackVector = new Vector3d();
				deltaTrackVector.sub(toWaypointVector, fromWaypointVector);
				double h = Math.sqrt(deltaTrackVector.x * deltaTrackVector.x + deltaTrackVector.y * deltaTrackVector.y),
				angle = h > 0.0 ? Math.acos(deltaTrackVector.x / h) : 0.0;
				if (deltaTrackVector.y < 0.0) angle = -angle;
				scaleVector.x = h;
				transform3d.setScale(scaleVector);
				trackTransform3d.mul(transform3d);
				transform3d.rotZ(angle);
				trackTransform3d.mul(transform3d);
				trackTransform.setTransform(trackTransform3d);
				trackTransform.addChild(trackShape);
				tracksBranch.addChild(trackTransform);
				if (DEBUG) System.out.println("Added track shape to " + toWaypointVector);
			}
			fromWaypointVector = toWaypointVector;
		}
		tracksBranch.compile();
		synchronized (mapShapes)
		{
			branch.insertChild(tracksBranch, branch.numChildren());
			tracksBranchInserted = true;
		}
	}
	catch (Exception ex)
	{
		ex.printStackTrace();
	}
}

}

public static final double parseLatitude(String latitude, String hemisphere) throws ParseException
{
	if (latitude == null) throw new ParseException("Latitude is null", 0);
	double decimal = Double.parseDouble(latitude.substring(0, 2)) + Double.parseDouble(latitude.substring(2)) / 60.0;
	return parseChar(hemisphere) == 'S' ? -decimal : decimal;
}

public static final double parseLongitude(String longitude, String hemisphere) throws ParseException
{
	if (longitude == null) throw new ParseException("Longitude is null", 0);
	double decimal = Double.parseDouble(longitude.substring(0, 3)) + Double.parseDouble(longitude.substring(3)) / 60.0;
	return parseChar(hemisphere) == 'W' ? -decimal : decimal;
}

public static final double parseDouble(String string)
{
	return string != null ? Double.parseDouble(string) : 0.0;
}

public static final int parseInt(String string)
{
	return string != null ? Integer.parseInt(string) : 0;
}

public static final char parseChar(String string)
{
	return string != null ? Character.toUpperCase(string.charAt(0)) : 0;
}

public void run()
{
	while (!Thread.currentThread().isInterrupted())
	try
	{
		nmea.readInputStream(in);
	}
	catch (Exception ex)
	{
		if (ex instanceof InterruptedException) break;
		ex.printStackTrace();
	}
}

public String getBeanInfo()
{
	return getClass().getName() + "/2000-5-2";
}

void relocateVessel(Vector3d coordinates)
{
	mapIndex.convert(coordinates, vesselPosition);
	vesselPosition.x /= x_factor;
	vesselPosition.y /= y_factor;
	deltaVesselPosition.sub(vesselPosition, lastVesselPosition);
	double h = Math.sqrt(deltaVesselPosition.x * deltaVesselPosition.x + deltaVesselPosition.y * deltaVesselPosition.y),
	angle = h > 0.0 ? Math.acos(deltaVesselPosition.x / h) : 0.0;
	if (deltaVesselPosition.y < 0.0) angle = -angle;
	lastVesselPosition.set(vesselPosition);
	long ctm = System.currentTimeMillis(), deltaTime = ctm - vesselTime;
	synchronized (mapShapes)
	{
		if (deltaTime > 0L)
		{
			vesselVelocity.x = deltaVesselPosition.x / (double)deltaTime;
			vesselVelocity.y = deltaVesselPosition.y / (double)deltaTime;
		}
		else
		{
			vesselVelocity.x = deltaVesselPosition.x;
			vesselVelocity.y = deltaVesselPosition.y;
		}
	}
	vesselTime = ctm;
	vesselVector.x = vesselPosition.x / (double)half_rectangle_side;
	vesselVector.y = vesselPosition.y / (double)half_rectangle_side;
	vesselVector.z = 0.0;
	vesselTransform3d.rotZ(angle);
	arrowHeadTransform3d.setIdentity();
	arrowHeadTransform3d.setTranslation(vesselVector);
	arrowHeadTransform3d.mul(vesselTransform3d);
	arrowHeadTransform.setTransform(arrowHeadTransform3d);
}

Vector3d countPoint(Vector3d vector)
{
	Vector3d point = new Vector3d();
	point.x = vector.x * (double)half_rectangle_side;
	point.y = vector.y * (double)half_rectangle_side;
	return point;
}

void setViewPlatformWithVessel(Vector3d point)
{
	vpVector.x = vesselVector.x;
	vpVector.y = vesselVector.y;
	viewPlatformTransform3d.setTranslation(vpVector);
	viewPlatformTransform.setTransform(viewPlatformTransform3d);
	updateMapShapes(point);
}

/** When location of view port has changed, 3X3 grid of map shapes is
updated, so that view port is located in the center grid.
@param point coordinates given in image coordinates. */
synchronized final void updateMapShapes(Vector3d point)
{
	synchronized (mapShapes)
	{
		current_point_x = (long)(point.x + .5);
		current_point_y = (long)(point.y + .5);
	}
	long offset_point_x1 = (current_point_x - current_offset_point_x) % rectangle_side,
	offset_point_y1 = (current_point_y - current_offset_point_y) % rectangle_side;
	offset_point_x1 = offset_point_x1 < 0 ? current_point_x - offset_point_x1 - rectangle_side : current_point_x - offset_point_x1;
	offset_point_y1 = offset_point_y1 < 0 ? current_point_y - offset_point_y1 - rectangle_side : current_point_y - offset_point_y1;
	if (current_offset_point_x == offset_point_x1 && current_offset_point_y == offset_point_y1) return;
	mapShapeUpdate.canceled = true;
	current_offset_point_x = offset_point_x1;
	current_offset_point_y = offset_point_y1;
	if (DEBUG) System.out.println("updateMapShapes at " + current_offset_point_x + "," + current_offset_point_y);
	mapShapeUpdate.lock.unlock();
}

synchronized final void updateRoutes()
{
	routesUpdate = new RoutesUpdate(routesUpdate, waypoints);
	routesUpdate.start();
}

public LiveMap(Properties properties) throws IOException, InterruptedException, PortInUseException, NoSuchPortException, UnsupportedCommOperationException, TooManyListenersException, SQLException, ParseException, ClassNotFoundException, InstantiationException, IllegalAccessException
{
	super();
	setTitle(getBeanInfo());
	Properties sqlProperties = new Properties();
	String dataDirectory = null, mapFileDirectory = null, sqlDrivers = null, sqlUrl = null;
	double defaultHeight = 2.0, defaultXCoordinate = 0.0, defaultYCoordinate = 0.0;
	int baudRate = -1, dataBits = -1, stopBits = -1, parity = -1, flowControl = -1;
	String propertyBaseName = getClass().getName() + "/", portName = null, nmeaFilename = null;
	Enumeration names = properties.propertyNames();
	while (names.hasMoreElements())
	{
		String propertyName = (String)names.nextElement();
		if (!propertyName.startsWith(propertyBaseName)) continue;
		String value = properties.getProperty(propertyName), name = propertyName.substring(propertyBaseName.length());
		if (name.equals("baudRate")) baudRate = Integer.parseInt(value);
		else if (name.equals("dataBits"))
		switch (Integer.parseInt(value)) {
		case 5:
			dataBits = SerialPort.DATABITS_5;
			break;
		case 6:
			dataBits = SerialPort.DATABITS_6;
			break;
		case 7:
			dataBits = SerialPort.DATABITS_7;
			break;
		case 8:
			dataBits = SerialPort.DATABITS_8;
			break;
		default:
			throw new IOException("Invalid number of data bits, must be 5-8");
		}
		else if (name.equals("stopBits"))
		switch (Integer.parseInt(value)) {
		case 1:
			stopBits = SerialPort.STOPBITS_1;
			break;
		case 2:
			stopBits = SerialPort.STOPBITS_2;
			break;
		case 3:
			stopBits = SerialPort.STOPBITS_1_5;
			break;
		default:
			throw new IOException("Invalid number of stop bits, must be 1-3");
		}
		else if (name.equals("parity"))
		switch (Character.toLowerCase(value.charAt(0))) {
		case 'n':
			parity = SerialPort.PARITY_NONE;
			break;
		case 'o':
			parity = SerialPort.PARITY_ODD;
			break;
		case 'e':
			parity = SerialPort.PARITY_EVEN;
			break;
		case 'm':
			parity = SerialPort.PARITY_MARK;
			break;
		case 's':
			parity = SerialPort.PARITY_SPACE;
			break;
		default:
			throw new IOException("Invalid parity, must be n, o, e, m or s");
		}
		else if (name.equals("flowControl"))
		switch (Character.toLowerCase(value.charAt(0))) {
		case 'n':
			flowControl = SerialPort.FLOWCONTROL_NONE;
			break;
		case 'r':
			flowControl = SerialPort.FLOWCONTROL_RTSCTS_IN | SerialPort.FLOWCONTROL_RTSCTS_OUT;
			break;
		case 'x':
			flowControl = SerialPort.FLOWCONTROL_XONXOFF_IN | SerialPort.FLOWCONTROL_XONXOFF_OUT;
			break;
		default:
			throw new IOException("Invalid flow control, must be n, r or x");
		}
		else if (name.equals("dataDirectory")) dataDirectory = value;
		else if (name.equals("defaultHeight")) defaultHeight = Double.parseDouble(value);
		else if (name.equals("defaultXCoordinate")) defaultXCoordinate = Double.parseDouble(value);
		else if (name.equals("defaultYCoordinate")) defaultYCoordinate = Double.parseDouble(value);
		else if (name.equals("followVessel")) followVessel = Support.isTrue(value, propertyBaseName + "followVessel");
		else if (name.equals("mapFileDirectory")) mapFileDirectory = value;
		else if (name.equals("nmeaFilename")) nmeaFilename = value;
		else if (name.equals("portName")) portName = value;
		else if (name.equals("sqlDrivers")) sqlDrivers = value;
		else if (name.equals("sqlUrl")) sqlUrl = value;
		else if (name.startsWith("sqlProperties")) sqlProperties.setProperty(name.substring(14), value);
		else throw new IOException("Unknown property name " + propertyName);
	}

	StringTokenizer sqlDriverToks = new StringTokenizer(sqlDrivers, ",");
	while (sqlDriverToks.hasMoreTokens()) Class.forName(sqlDriverToks.nextToken().trim()).newInstance();
	Enumeration drivers = DriverManager.getDrivers();
	while (drivers.hasMoreElements()) System.out.println(drivers.nextElement());

	if (portName != null)
	{
		CommPortIdentifier cpi = CommPortIdentifier.getPortIdentifier(portName);
		serialPort = (SerialPort)cpi.open(getClass().getName(), 5000);
		serialPort.setSerialPortParams(baudRate != -1 ? baudRate : 4800,
		dataBits != -1 ? dataBits : SerialPort.DATABITS_8,
		stopBits != -1 ? stopBits : SerialPort.STOPBITS_1,
		parity != -1 ? parity : SerialPort.PARITY_NONE);
		serialPort.setFlowControlMode(flowControl != -1 ? flowControl : SerialPort.FLOWCONTROL_NONE);
		in = serialPort.getInputStream();
	}
	else if (nmeaFilename != null) in = new BufferedInputStream(new FileInputStream(nmeaFilename));
	if (mapFileDirectory != null)
	{
		if (sqlUrl == null) sqlUrl = "jdbc:FI.realitymodeler:" + new File(dataDirectory, "LiveMapDatabase").getAbsolutePath();
		if (DEBUG) System.out.println("Opening connection to " + sqlUrl);
		connection = DriverManager.getConnection(sqlUrl, sqlProperties);
		mapIndex = new MapIndex(connection, new File(mapFileDirectory));
	}

	Toolkit toolkit = Toolkit.getDefaultToolkit();
	setSize(toolkit.getScreenSize());

	map_shape_width = ((double)rectangle_side / (double)rectangle_side) * 2.0;
	map_shape_height = 2.0;

	if (DEBUG) System.out.println("map_shape_width=" + map_shape_width + ",map_shape_height=" + map_shape_height);

	plane = new QuadArray(4, GeometryArray.COORDINATES | GeometryArray.TEXTURE_COORDINATE_2);
	plane.setCoordinate(0, new Point3d(0.0, map_shape_height, 0.0));
	plane.setCoordinate(1, new Point3d(0.0, 0.0, 0.0));
	plane.setCoordinate(2, new Point3d(map_shape_width, 0.0, 0.0));
	plane.setCoordinate(3, new Point3d(map_shape_width, map_shape_height, 0.0));
	plane.setTextureCoordinate(0, 0, new TexCoord2f(0.0f, 1.0f));
	plane.setTextureCoordinate(0, 1, new TexCoord2f(0.0f, 0.0f));
	plane.setTextureCoordinate(0, 2, new TexCoord2f(1.0f, 0.0f));
	plane.setTextureCoordinate(0, 3, new TexCoord2f(1.0f, 1.0f));

	TriangleArray arrowHead = new TriangleArray(12, TriangleArray.COORDINATES | TriangleArray.COLOR_3);

	arrowHead.setCoordinate(0, new Point3d(0.0, 0.0, 0.3));
	arrowHead.setCoordinate(1, new Point3d(-0.1, 0.0, 0.35));
	arrowHead.setCoordinate(2, new Point3d(-0.1, -0.05, 0.3));
	Color3f color1 = new Color3f(0.8f, 0.8f, 0.8f);
	for (int i = 0; i < 3; i++) arrowHead.setColor(i, color1);

	arrowHead.setCoordinate(3, new Point3d(0.0, 0.0, 0.3));
	arrowHead.setCoordinate(4, new Point3d(-0.1, 0.05, 0.3));
	arrowHead.setCoordinate(5, new Point3d(-0.1, 0.0, 0.35));
	Color3f color2 = new Color3f(0.6f, 0.6f, 0.6f);
	for (int i = 3; i < 6; i++) arrowHead.setColor(i, color2);

	arrowHead.setCoordinate(6, new Point3d(0.0, 0.0, 0.3));
	arrowHead.setCoordinate(7, new Point3d(-0.1, 0.05, 0.3));
	arrowHead.setCoordinate(8, new Point3d(-0.1, -0.05, 0.3));
	Color3f color3 = new Color3f(0.4f, 0.4f, 0.4f);
	for (int i = 6; i < 9; i++) arrowHead.setColor(i, color3);

	arrowHead.setCoordinate(9, new Point3d(-0.1, -0.05, 0.3));
	arrowHead.setCoordinate(10, new Point3d(-0.1, 0.0, 0.35));
	arrowHead.setCoordinate(11, new Point3d(-0.1, 0.05, 0.3));
	Color3f color4 = new Color3f(0.2f, 0.2f, 0.2f);
	for (int i = 9; i < 12; i++) arrowHead.setColor(i, color4);

	IndexedQuadArray track = new IndexedQuadArray(8, TriangleArray.COORDINATES | TriangleArray.COLOR_3, 24);

	track.setCoordinate(0, new Point3d(0.0, 0.5, 0.0));
	track.setCoordinate(1, new Point3d(0.0, -0.5, 0.0));
	track.setCoordinate(2, new Point3d(1.0, -0.5, 0.0));
	track.setCoordinate(3, new Point3d(1.0, 0.5, 0.0));

	track.setCoordinate(4, new Point3d(0.0, 0.25, 0.1));
	track.setCoordinate(5, new Point3d(0.0, -0.25, 0.1));
	track.setCoordinate(6, new Point3d(1.0, -0.25, 0.1));
	track.setCoordinate(7, new Point3d(1.0, 0.25, 0.1));

	track.setColor(0, color1);
	track.setColor(1, color2);
	track.setColor(2, color3);
	track.setColor(3, color4);

	track.setColor(4, color1);
	track.setColor(5, color2);
	track.setColor(6, color3);
	track.setColor(7, color4);

	track.setCoordinateIndex(0, 0);
	track.setCoordinateIndex(1, 1);
	track.setCoordinateIndex(2, 2);
	track.setCoordinateIndex(3, 3);
	for (int i = 0; i < 4; i++) track.setColorIndex(i, 3);

	track.setCoordinateIndex(4, 4);
	track.setCoordinateIndex(5, 5);
	track.setCoordinateIndex(6, 6);
	track.setCoordinateIndex(7, 7);
	for (int i = 4; i < 8; i++) track.setColorIndex(i, 0);

	track.setCoordinateIndex(8, 0);
	track.setCoordinateIndex(9, 3);
	track.setCoordinateIndex(10, 4);
	track.setCoordinateIndex(11, 7);
	for (int i = 8; i < 12; i++) track.setColorIndex(i, 1);

	track.setCoordinateIndex(12, 1);
	track.setCoordinateIndex(13, 2);
	track.setCoordinateIndex(14, 5);
	track.setCoordinateIndex(15, 6);
	for (int i = 12; i < 16; i++) track.setColorIndex(i, 1);

	track.setCoordinateIndex(16, 0);
	track.setCoordinateIndex(17, 1);
	track.setCoordinateIndex(18, 4);
	track.setCoordinateIndex(19, 5);
	for (int i = 16; i < 20; i++) track.setColorIndex(i, 2);

	track.setCoordinateIndex(20, 2);
	track.setCoordinateIndex(21, 3);
	track.setCoordinateIndex(22, 6);
	track.setCoordinateIndex(23, 7);
	for (int i = 20; i < 24; i++) track.setColorIndex(i, 3);

	trackShape = new Shape3D(track, null);

	setLayout(new BorderLayout());
	Canvas3D canvas3d = new Canvas3D(GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice().getBestConfiguration(new GraphicsConfigTemplate3D()));
	add("Center", canvas3d);

	arrowHeadTransform = new TransformGroup();
	arrowHeadTransform.getTransform(arrowHeadTransform3d);
	arrowHeadTransform.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
	arrowHeadTransform.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);

	VirtualUniverse virtualUniverse = new VirtualUniverse();
	javax.media.j3d.Locale locale = new javax.media.j3d.Locale(virtualUniverse);
	View view = new View();
	ViewPlatform viewPlatform = new ViewPlatform();
	PhysicalBody physicalBody = new PhysicalBody();
	PhysicalEnvironment physicalEnvironment = new PhysicalEnvironment();
	view.attachViewPlatform(viewPlatform);
	view.setPhysicalBody(physicalBody);
	view.setPhysicalEnvironment(physicalEnvironment);
	view.addCanvas3D(canvas3d);
	viewPlatformTransform = new TransformGroup();
	viewPlatformTransform.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
	viewPlatformTransform.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
	viewPlatformTransform.addChild(viewPlatform);
	viewPlatformTransform.getTransform(viewPlatformTransform3d);

	relocateVessel(new Vector3d(defaultXCoordinate, defaultYCoordinate, 0.0));
	vpVector.x = vesselVector.x;
	vpVector.y = vesselVector.y;
	vpVector.z = defaultHeight;
	viewPlatformTransform3d.setTranslation(vpVector);
	viewPlatformTransform.setTransform(viewPlatformTransform3d);

	Interaction interaction = new Interaction();
	interaction.setSchedulingBounds(new BoundingSphere(new Point3d(), 1000.0));
	viewPlatformTransform.addChild(interaction);

	branch = new BranchGroup();
	branch.setCapability(Group.ALLOW_CHILDREN_EXTEND);
	branch.setCapability(Group.ALLOW_CHILDREN_READ);
	branch.setCapability(Group.ALLOW_CHILDREN_WRITE);

	arrowHeadTransform.addChild(new Shape3D(arrowHead, null));
	branch.addChild(arrowHeadTransform);
	branch.compile();

	BranchGroup viewBranch = new BranchGroup();
	viewBranch.addChild(viewPlatformTransform);
	viewBranch.compile();

	locale.addBranchGraph(branch);
	locale.addBranchGraph(viewBranch);

	show();

	mapShapeUpdate = new MapShapeUpdate();
	mapShapeUpdate.start();

	if (in != null) new Thread(this).start();
	else updateMapShapes(vesselPosition);
}

public static void main(String argv[]) throws Exception
{
	Properties properties = new Properties();
	String javaHome = System.getProperty("java.home", ".");
	properties.load(new FileInputStream(new File(javaHome, "lib/FI_realitymodeler_gps_LiveMap.properties")));
	Runtime.getRuntime().addShutdownHook(new LiveMapShutdownHook(new LiveMap(properties)));
}

}