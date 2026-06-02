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
if test "$1" = "co"
then
cvs -d :pserver:cvsguest@andesengineering.com:/cvs/Producer co Producer
check
else
cp -Rv Producer-1.2.0-1/Xcode/Producer/build/Deployment/Producer.framework /Library/Frameworks
check
pushd Producer-1.2.0-1/include/Producer
check
mkdir -p /Library/Frameworks/Producer.framework/Versions/A/Headers
check
for file in *
do
echo $file
name=`echo $file | sed -e "s%\(.*\)\.h%\1%"`
echo $name
cp -v $file /Library/Frameworks/Producer.framework/Versions/A/Headers/
check
ln -s /Library/Frameworks/Producer.framework/Versions/A/Headers/$file /Library/Frameworks/Producer.framework/Versions/A/Headers/$name
check
done
popd
ln -s /Library/Frameworks/Producer.framework/Versions/A /Library/Frameworks/Producer.framework/Versions/Current
check
ln -s /Library/Frameworks/Producer.framework/Versions/Current/Headers /Library/Frameworks/Producer.framework/Headers
check
fi
