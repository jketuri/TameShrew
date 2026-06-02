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
if test "$1" = "svn"
then
svn co http://www.openscenegraph.org/svn/VirtualPlanetBuilder/tags/VirtualPlanetBuilder-0.9.10/ VirtualPlanetBuilder
check
else
pushd VirtualPlanetBuilder
mkdir -p build
cd build
cmake -D CMAKE_OSX_ARCHITECTURES:STRING=i386 -G Xcode ..
popd
fi
