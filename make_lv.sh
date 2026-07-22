#!/bin/bash
check()
{
    VALUE=$?
	if test $VALUE != 0
	then
		exit $VALUE
	fi
    return 0;
}

if test "$DEBUG" = "1"
then
OPTIONS="-g -ggdb -D DEBUG -D DEBUG1"
else
OPTIONS="-O2"
fi

gcc $OPTIONS -W -Wall -fPIC -c Datum.cpp LiveView.cpp NMEA.cpp SerialStream.cpp SocketStream.cpp USBStream.cpp common.cpp
check
libtool --mode=link gcc $OPTIONS Datum.o LiveView.o NMEA.o SerialStream.o SocketStream.o USBStream.o common.o -lGL -lstdc++ -lexif -ljpeg -lm -losg -losgDB -losgGA -losgParticle -losgSim -losgText -losgUtil -losgViewer -lportaudio -ltiff -lz -lOpenThreads -o LiveView
check
ls -l LiveView
check
