
package FI.realitymodeler.gps;

import FI.realitymodeler.common.*;
import java.io.*;
import java.sql.*;
import java.text.*;
import java.util.*;
import javax.vecmath.*;

public class MapIndex implements FileFilter
{
	Connection connection = null;
	PreparedStatement insert = null, update = null, select = null;
	ReferenceEllipsoid international1924 = new ReferenceEllipsoid("International 1924", 6378388.0, 297.0);
	Datum WGS84 = new Datum("WGS 84", new ReferenceEllipsoid("WGS 84", 6378137.0, 298.25722), 0.0, 0.0, 0.0),
	european1979 = new Datum("European 1979", international1924, -86.0, -98.0, -119.0);
	Projection projection = new Projection("Transverse Mercator", international1924, Math.toRadians(27.0), Math.toRadians(0.0), 1.0, 3500000.0, 0.0);
	Vector maps = new Vector();

public MapIndex(Connection connection, File mapFileDirectory) throws SQLException, IOException, ParseException
{
	this.connection = connection;
	String path = "FI/realitymodeler/gps/resources/LiveMap_init";
	BufferedReader reader = new BufferedReader(new InputStreamReader(ClassLoader.getSystemResourceAsStream(path)));
	boolean executed = Support.initializeDatabase(connection, reader, path, null, new PrintWriter(System.out, true), LiveMap.DEBUG);
	try
	{
		insert = connection.prepareStatement("insert into maps values (?,?,?,?,?,?,?)");
		update = connection.prepareStatement("update maps set coordinate_x1 = ?, coordinate_y1 = ?, coordinate_x2 = ?, coordinate_y2 = ?, x_scale = ?, y_scale = ?\nwhere filepath = ?");
		select = connection.prepareStatement("select * from maps\nwhere coordinate_x2 >= ? and coordinate_x1 <= ? and coordinate_y2 >= ? and coordinate_y1 <= ?");
	}
	catch (SQLException ex)
	{
		try
		{
			close();
		}
		finally
		{
			throw ex;
		}
	}
	if (executed) generate(mapFileDirectory);
}

public void close() throws SQLException
{
	try
	{
		if (insert != null) insert.close();
	}
	finally
	{
		try
		{
			if (update != null) update.close();
		}
		finally
		{
			if (select != null) select.close();
		}
	}
}

public boolean accept(File file)
{
	return file.isDirectory() || file.getName().toUpperCase().endsWith(".TAB") &&
	new File(file.getParent(), file.getName().substring(0, file.getName().length() - 4) + ".TIF").exists();
}

public void generate(File mapFileDirectory) throws SQLException, IOException, ParseException
{
	if (LiveMap.DEBUG) System.out.println("Generating map index form directory " + mapFileDirectory);
	File files[] = mapFileDirectory.listFiles(this);
	for (int i = 0; i < files.length; i++)
	{
		if (LiveMap.DEBUG) System.out.println("Processing file " + files[i].getCanonicalPath());
		if (files[i].isDirectory())
		{
//			generate(files[i]);
			continue;
		}
		boolean foundCoordinate1 = false, foundCoordinate3 = false;
		int point_x1 = 0, point_y1 = 0, point_x2 = 0, point_y2 = 0;
		MapEntry entry = new MapEntry();
		entry.filepath = new File(files[i].getParent(), files[i].getName().substring(0, files[i].getName().length() - 4) + ".TIF").getCanonicalPath();
		BufferedReader reader = null;
		try
		{
			reader = new BufferedReader(new FileReader(files[i]));
			for (String line; (line = reader.readLine()) != null;)
			{
				boolean isCoordinate1;
				if ((isCoordinate1 = line.indexOf("Label \"Pt 1\"") != -1) || line.indexOf("Label \"Pt 3\"") != -1)
				{
					int leftParen = line.indexOf('('), rightParen = line.indexOf(')', leftParen + 1);
					if (leftParen == -1 || rightParen == -1)
					{
						if (LiveMap.DEBUG) System.out.println("Invalid .TAB-file line " + line);
						break;
					}
					StringTokenizer st = new StringTokenizer(line.substring(leftParen + 1, rightParen), ",");
					leftParen = line.indexOf('(', rightParen + 1);
					rightParen = line.indexOf(')', leftParen + 1);
					if (leftParen == -1 || rightParen == -1)
					{
						if (LiveMap.DEBUG) System.out.println("Invalid .TAB-file line " + line);
						break;
					}
					StringTokenizer st1 = new StringTokenizer(line.substring(leftParen + 1, rightParen), ",");
					if (isCoordinate1)
					{
						entry.coordinate_x1 = Double.parseDouble(st.nextToken().trim());
						entry.coordinate_y2 = Double.parseDouble(st.nextToken().trim());
						point_x1 = Integer.parseInt(st1.nextToken().trim());
						point_y1 = Integer.parseInt(st1.nextToken().trim());
						foundCoordinate1 = true;
					}
					else
					{
						entry.coordinate_x2 = Double.parseDouble(st.nextToken().trim());
						entry.coordinate_y1 = Double.parseDouble(st.nextToken().trim());
						point_x2 = Integer.parseInt(st1.nextToken().trim());
						point_y2 = Integer.parseInt(st1.nextToken().trim());
						foundCoordinate3 = true;
					}
				}
			}
		}
		catch (IOException ex)
		{
			if (LiveMap.DEBUG) ex.printStackTrace();
			continue;
		}
		finally
		{
			if (reader != null)
			try
			{
				reader.close();
			}
			catch (IOException ex) {}
		}
		if (!foundCoordinate1 || !foundCoordinate3)
		{
			if (LiveMap.DEBUG) System.out.println("Invalid .TAB-file " + files[i].getCanonicalPath());
			continue;
		}
		entry.x_scale = (double)(point_x2 - point_x1 + 1) / (entry.coordinate_x2 - entry.coordinate_x1);
		entry.y_scale = (double)(point_y2 - point_y1 + 1) / (entry.coordinate_y2 - entry.coordinate_y1);
		if (LiveMap.DEBUG) System.out.println("entry=" + entry);
		update.setDouble(1, entry.coordinate_x1);
		update.setDouble(2, entry.coordinate_y1);
		update.setDouble(3, entry.coordinate_x2);
		update.setDouble(4, entry.coordinate_y2);
		update.setDouble(5, entry.x_scale);
		update.setDouble(6, entry.y_scale);
		update.setString(7, entry.filepath);
		if (update.executeUpdate() == 1) continue;
		insert.setString(1, entry.filepath);
		insert.setDouble(2, entry.coordinate_x1);
		insert.setDouble(3, entry.coordinate_y1);
		insert.setDouble(4, entry.coordinate_x2);
		insert.setDouble(5, entry.coordinate_y2);
		insert.setDouble(6, entry.x_scale);
		insert.setDouble(7, entry.y_scale);
		insert.executeUpdate();
	}
}

/** This method searches all map images partially or completely containing in map shape. */
public Vector search(MapShape shape, double x_factor, double y_factor)
{
	if (LiveMap.DEBUG) System.out.println("search for " + shape);
	ResultSet resultSet = null;
	try
	{
		select.setDouble(1, (double)shape.point_x1 * x_factor);
		select.setDouble(2, (double)shape.point_x2 * x_factor);
		select.setDouble(3, (double)shape.point_y1 * y_factor);
		select.setDouble(4, (double)shape.point_y2 * y_factor);
		resultSet = select.executeQuery();
		Vector entries = new Vector();
		while (resultSet.next())
		{
			MapEntry entry = new MapEntry();
			entry.filepath = resultSet.getString(1);
			entry.coordinate_x1 = resultSet.getDouble(2);
			entry.coordinate_y1 = resultSet.getDouble(3);
			entry.coordinate_x2 = resultSet.getDouble(4);
			entry.coordinate_y2 = resultSet.getDouble(5);
			entry.x_scale = resultSet.getDouble(6);
			entry.y_scale = resultSet.getDouble(7);
			entries.addElement(entry);
		}
		if (LiveMap.DEBUG) System.out.println("found " + entries);
		return entries;
	}
	catch (SQLException ex)
	{
		if (LiveMap.DEBUG) ex.printStackTrace();
	}
	finally
	{
		if (resultSet != null)
		try
		{
			resultSet.close();
		}
		catch (SQLException ex) {}
	}
	return null;
}

/** Converts geodesical coordinates in vector 'from' given in decimal
degrees from WGS84 Datum to European 1979 Datum and these coordinates in
turn to Universal Transverse Mercator Coordinates in vector 'to'. Uses
finnish unified one zone grid with false easting of 3500000 in 27E.

@param from latitude in x and longitude in y given in decimal degrees
@param to easting in x and northing in y given in meters */
public void convert(Vector3d from, Vector3d to)
{
	Vector3d to0 = new Vector3d();
	WGS84.convert(new Vector3d(Math.toRadians(from.x), Math.toRadians(from.y), from.z), to0, european1979);
//	if (LiveMap.DEBUG) System.out.println("convert " + from + " to " + to0);
	projection.forward(to0, to);
//	if (LiveMap.DEBUG) System.out.println("forward " + to0 + " to " + to);
}

}