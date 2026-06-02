
package FI.realitymodeler.gps;

public class MapEntry
{
	String filepath;
	double coordinate_x1 = 0.0, coordinate_y1 = 0.0;
	double coordinate_x2 = 0.0, coordinate_y2 = 0.0;
	double x_scale = 0.0, y_scale = 0.0;

public String toString()
{
	return "{MapEntry: filepath=" + filepath +
	",coordinate_x1=" + coordinate_x1 + ",coordinate_y1=" + coordinate_y1 +
	",coordinate_x2=" + coordinate_x2 + ",coordinate_y2=" + coordinate_y2 +
	",x_scale=" + x_scale + ",y_scale=" + y_scale + "}";
}

}