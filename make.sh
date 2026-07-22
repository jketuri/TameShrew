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

gcc $OPTIONS -W -Wall -fPIC -c LiveMap.cpp NMEA.cpp SerialStream.cpp common.cpp Datum.cpp MapIndex.cpp PhotoIndex.cpp SocketStream.cpp
check
#libtool --mode=link gcc $OPTIONS LiveMap.o NMEA.o SerialStream.o common.o Datum.o MapIndex.o PatternDrawable.o PhotoIndex.o $PORTAUDIO/lib/libportaudio.la -lstdc++ -lexif -lfreetype -ljpeg -lpng -ltiff -lungif -losg -losgDB -losgGA -losgParticle -losgProducer -losgSim -losgText -losgUtil -lProducer -lz -lGLU -lGL -lXi -lXmu -lX11 -lOpenThreads -o LiveMap
#libtool --mode=link gcc $OPTIONS LiveMap.o NMEA.o SerialStream.o common.o Datum.o MapIndex.o PhotoIndex.o -lstdc++ -lexif -lfreetype -lportaudio -ljpeg -lpng -ltiff -lungif -losg -losgDB -losgGA -losgParticle -losgSim -losgText -losgUtil -lz -lGLU -lGL -lXi -lXmu -lX11 -lOpenThreads -o LiveMap
libtool --mode=link gcc $OPTIONS Datum.o LiveMap.o MapIndex.o NMEA.o PhotoIndex.o SerialStream.o SocketStream.o common.o -lGL -lstdc++ -lexif -ljpeg -lm -losg -losgDB -losgGA -losgParticle -losgSim -losgText -losgUtil -losgViewer -lportaudio -ltiff -lz -lOpenThreads -o LiveMap
check
ls -l LiveMap
