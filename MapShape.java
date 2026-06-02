
package FI.realitymodeler.gps;

import java.util.*;
import javax.media.j3d.*;

public class MapShape implements Comparator
{
	long point_x1 = 0L, point_y1 = 0L;
	long point_x2 = 0L, point_y2 = 0L;
	int precedence = 0;
	int index = 0;

public int compare(Object o1, Object o2)
{
	return ((MapShape)o1).index - ((MapShape)o2).index;
}

public boolean equals(Object o)
{
	return o instanceof MapShape;
}

public String toString()
{
	return "{MapShape: point_x1=" + point_x1 + ",point_y1=" + point_y1 +
	",point_x2=" + point_x2 + ",point_y2=" + point_y2 + "}";
}

}