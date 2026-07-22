#!/bin/bash
check()
{
	if test $? != 0
	then
		exit 1
	fi
}

if test "$DEBUG" = "1"
then
OPTIONS="-g -ggdb -D DEBUG -D DEBUG1"
else
OPTIONS="-O2"
fi

gcc $OPTIONS -W -Wall -fPIC -c Datum.cpp Garmin.cpp LiveMap.cpp MapIndex.cpp NMEA.cpp PhotoIndex.cpp SerialStream.cpp SocketStream.cpp USBStream.cpp common.cpp
check
libtool --mode=link gcc $OPTIONS Datum.o Garmin.o LiveMap.o MapIndex.o NMEA.o PhotoIndex.o SerialStream.o SocketStream.o USBStream.o common.o -lGL -lexif -ljpeg -lm -losg -losgDB -losgGA -losgParticle -losgSim -losgText -losgUtil -losgViewer -lportaudio -lstdc++ -ltiff -lusb-1.0 -lz -lOpenThreads -o LiveMap
check
ls -l LiveMap
