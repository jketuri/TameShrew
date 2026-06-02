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
if test "$1" = "install"
then
cp -v gwaldron-osgearth-dbaa285/build/lib/Release/*.so /Library/Application\ Support/OpenSceneGraph/PlugIns/
check
pushd gwaldron-osgearth-dbaa285/src
check
for dir in osg*
do
echo $dir
mkdir -p /Library/Frameworks/${dir}.framework/Versions/A/Libraries
check
mkdir -p /Library/Frameworks/${dir}.framework/Versions/A/Headers
check
if test -f ../build/lib/Release/lib${dir}.dylib
then
cp -v ../build/lib/Release/lib${dir}.dylib /Library/Frameworks/${dir}.framework/Versions/A/Libraries/
check
fi
pushd $dir
for file in $(find . -regex '\.[^\.]*' -type f)
do
echo $file
cp -v $file /Library/Frameworks/${dir}.framework/Versions/A/Headers/
check
done
popd
if test ! -h /Library/Frameworks/${dir}.framework/Versions/Current
then
ln -s /Library/Frameworks/${dir}.framework/Versions/A /Library/Frameworks/${dir}.framework/Versions/Current
check
fi
if test ! -h /Library/Frameworks/${dir}.framework/Headers
then
ln -s /Library/Frameworks/${dir}.framework/Versions/Current/Headers /Library/Frameworks/${dir}.framework/Headers
check
fi
done
else
pushd gwaldron-osgearth-dbaa285
mkdir -p build
cd build
cmake -D CMAKE_OSX_ARCHITECTURES:STRING=i386 -G Xcode ..
popd
fi
