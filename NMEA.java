
package FI.realitymodeler.gps;

import FI.realitymodeler.common.*;
import java.io.*;
import java.util.*;

public class NMEA
{
// old NMEA complex data status byte flags
	public static final byte MANUAL_CYCLE_CLOCK = 1,
	LOW_SNR = 1 << 1,
	CYCLE_JUMP = 1 << 2,
	BLINK = 1 << 3,
	ARRIVAL_ALARM = 1 << 4,
	DISCONTINUITY_OF_TDS = 1 << 5;

	static final boolean DEBUG = false;

	public int version;
	public HashMap nmeaSentences = new HashMap();

public NMEA(int version)
{
	this.version = version;
}

public void simpleDataInvalid(int simpleData) throws IOException
{
}

public void simpleCrossTrackError(int crossTrackError) throws IOException
{
}

public void complexDataInvalid(String complexData) throws IOException
{
}

public void wrongChecksum(String complexData) throws IOException
{
}

public void complexData(char crossTrackErrorUnits, double crossTrackErrorValue, char crossTrackErrorPosition,
char trueOrMagneticBearing, int bearingToNextWayPoint, String presentLatitude, String presentLongitude, byte status) throws IOException
{
}

public void sentenceDataInvalid(String sentence) throws IOException
{
}

public void sentenceData(String id, Vector dataFields) throws IOException
{
	if (DEBUG) System.out.println("Unrecognized sentence " + id + " " + dataFields);
}

public int readOldByte(InputStream in) throws IOException
{
	for (int c = in.read(); c != -1;)
	if ((c & 0x80) == 0)
	{
// bit 7 is 0, simple data format
		if ((c & 0x40) == 0)
		{
// bit 6 is 0, invalid data
			simpleDataInvalid(c);
			continue;
		}
// bits 0 - 5 give the cross-track error
		simpleCrossTrackError(c & 0x3f);
		continue;
	}
	else return c;
	return -1;
}

public void readOldInputStream(InputStream in) throws IOException
{
	for (int c; (c = readOldByte(in)) != -1;)
	{
		if (c != '$') continue;
		StringBuffer buffer = new StringBuffer();
		buffer.append((char)c);
		while ((c = readOldByte(in)) != -1 && c != 0x83) buffer.append((char)c);
		String complexData = buffer.toString();
		if (complexData.length() < 36 || complexData.charAt(1) != 'M' && complexData.charAt(2) != 'P')
		{
			complexDataInvalid(complexData);
			if (c == -1) break;
			continue;
		}
		char crossTrackErrorUnits = complexData.charAt(3);
		if (crossTrackErrorUnits != 'K' &&	// kilometres
		crossTrackErrorUnits != 'N' &&		// nautical miles
		crossTrackErrorUnits != 'U')		// microseconds
		{
			complexDataInvalid(complexData);
			if (c == -1) break;
			continue;
		}
		double crossTrackErrorValue = Double.parseDouble(complexData.substring(4, 4));
		char crossTrackErrorPosition = complexData.charAt(8);
		if (crossTrackErrorPosition != 'L' || crossTrackErrorPosition != 'R')
		{
			complexDataInvalid(complexData);
			if (c == -1) break;
			continue;
		}
		char trueOrMagneticBearing = complexData.charAt(9);
		if (trueOrMagneticBearing != 'T' || trueOrMagneticBearing != 'M')
		{
			complexDataInvalid(complexData);
			if (c == -1) break;
			continue;
		}
		int bearingToNextWayPoint = Integer.parseInt(complexData.substring(10, 3));
		String presentLatitude = complexData.substring(13, 10),
		presentLongitude = complexData.substring(23, 11);
		byte status = (byte)complexData.charAt(34);
		complexData(crossTrackErrorUnits, crossTrackErrorValue, crossTrackErrorPosition,
		trueOrMagneticBearing, bearingToNextWayPoint, presentLatitude, presentLongitude, status);
		if (c == -1) break;
	}
}

public void readNewInputStream(InputStream in) throws IOException
{
	for (String sentence; (sentence = Support.readLine(in)) != null;)
	{
		if (DEBUG) System.out.println(sentence);
		StringTokenizer st = new StringTokenizer(sentence, ",*$", true);
		if (!st.hasMoreTokens() || !st.nextToken().equals("$") || !st.hasMoreTokens())
		{
			sentenceDataInvalid(sentence);
			continue;
		}
		String id = st.nextToken();
		if (id.length() < 5 || ",*$".indexOf(id.charAt(0)) != -1)
		{
			sentenceDataInvalid(sentence);
			continue;
		}
		Vector dataFields = new Vector();
		String token = null;
		for (String lastToken = null; st.hasMoreTokens(); lastToken = token)
		{
			token = st.nextToken();
			if (token.equals(","))
			{
				if (lastToken == null || !lastToken.equals(",")) continue;
				dataFields.addElement(null);
				continue;
			}
			if (token.equals("*"))
			{
				if (!st.hasMoreTokens()) break;
				int checkSum = Integer.parseInt(st.nextToken(), 16), countedCheckSum = 0;
				for (int i = sentence.lastIndexOf('*') - 1; i > 0; i--) countedCheckSum ^= sentence.charAt(i);
				if (checkSum != countedCheckSum)
				{
					wrongChecksum(sentence);
					id = null;
					break;
				}
				continue;
			}
			if (token.equals("$"))
			{
				sentenceDataInvalid(sentence);
				id = null;
				break;
			}
			dataFields.addElement(token);
		}
		if (id == null) continue;
		if (DEBUG) System.out.println("id="+id+",dataFields="+dataFields);
		NMEASentence nmeaSentence = (NMEASentence)nmeaSentences.get(id);
		if (nmeaSentence == null && !id.startsWith("P")) nmeaSentence = (NMEASentence)nmeaSentences.get(id.substring(2));
		try
		{
			if (nmeaSentence != null) nmeaSentence.sentenceData(id, dataFields);
			else sentenceData(id, dataFields);
		}
		catch (Exception ex)
		{
			ex.printStackTrace();
		}
	}
}

public void readInputStream(InputStream in) throws IOException
{
	if (version <= 182) readOldInputStream(in);
	else readNewInputStream(in);
}

}
