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
OPTIONS="-D DEBUG -D DEBUG1 -arch i386 -mmacosx-version-min=10.4 -g -ggdb -m32"
else
OPTIONS="-O2 -arch i386 -mmacosx-version-min=10.4 -m32"
fi

gcc $OPTIONS -W -Wall -fPIC -c LiveMap.cpp NMEA.cpp SerialStream.cpp SocketStream.cpp USBStream.cpp common.cpp Datum.cpp MapIndex.cpp PhotoIndex.cpp
check
gcc $OPTIONS -framework OpenGL -framework OpenThreads -framework osg -framework osgDB -framework osgGA -framework osgSim -framework osgText -framework osgUtil -framework osgViewer LiveMap.o NMEA.o SerialStream.o SocketStream.o USBStream.o common.o Datum.o MapIndex.o PhotoIndex.o -lportaudio -lstdc++ -lexif -ljpeg -ltiff -lusb-1.0 -o LiveMap
check
ls -l LiveMap
check
cp -v LiveMap $HOME/LiveMap/
check
