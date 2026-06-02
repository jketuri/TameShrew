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
./configure CFLAGS="-arch i386" CXXFLAGS="-arch i386"
make
sudo make install
