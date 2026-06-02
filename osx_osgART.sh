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
cp -v osgART_2.0_RC3/build/lib/* /Library/Application\ Support/OpenSceneGraph/PlugIns/
check
mkdir -p /Library/Frameworks/osgART.framework/Versions/A/Headers
check
cp -rv osgART_2.0_RC3/include/osgART/* /Library/Frameworks/osgART.framework/Versions/A/Headers/
check
ln -s /Library/Frameworks/osgART.framework/Versions/A /Library/Frameworks/osgART.framework/Versions/Current
check
ln -s /Library/Frameworks/osgART.framework/Versions/Current/Headers /Library/Frameworks/osgART.framework/Headers
check
else
pushd osgART_2.0_RC3
check
sed -e "s%check_osg_version(\${OSG_INCLUDE_DIR}/osg/Version)%check_osg_version\(\${OSG_INCLUDE_DIR}/Headers/Version\)%" <CMakeLists.txt >CMakeLists.txt.out
check
mv -v CMakeLists.txt.out CMakeLists.txt
check
mkdir -p build
check
cd build
check
cmake -D CMAKE_OSX_ARCHITECTURES:STRING=i386 -D CMAKE_CXX_FLAGS:STRING="-mmacosx-version-min=10.4" -D CMAKE_SHARED_LINKER_FLAGS:STRING="-mmacosx-version-min=10.4" ..
check
make VERBOSE=1
check
popd
fi
