check()
{
	if test $? != 0
	then
		exit 1
	fi
}

rm -fv $HOME/LiveMap.zip
check
rm -frv $HOME/LiveMap
check
mkdir -v $HOME/LiveMap
check
mkdir -pv $HOME/LiveMapData
check
mkdir -pv $HOME/LiveMapData/database
check
mkdir -pv $HOME/LiveMapData/GPT
check
mkdir -pv $HOME/LiveMapData/GPTNotes
check
mkdir -pv $HOME/LiveMapData/map
check
mkdir -pv $HOME/LiveMapData/NMEA
check
mkdir -pv $HOME/LiveMapData/photo
check
mkdir -pv $HOME/LiveMapData/photoShots
check
if test -d .libs
then
cp -LRv .libs/* $HOME/LiveMap/
check
fi
cp -v LiveMapUX.properties $HOME/LiveMap/LiveMap.properties
check
cp -v Lentoestetietokanta.csv $HOME/LiveMapData/
check
cp -v mpower.csv $HOME/LiveMapData/
check
if test "`uname`" = "Linux"
then
pushd $OPENSCENEGRAPH/lib/Linux32
check
cp -LRv * $HOME/LiveMap/
check
cd $OSGHOME/OpenThreads/lib/Linux32
check
cp -LRv * $HOME/LiveMap/
check
cd $OSGHOME/Producer/lib/Linux32
check
cp -LRv * $HOME/LiveMap/
check
cd $PORTAUDIO/lib
check
cp -LRv .libs/* $HOME/LiveMap/
check
mkdir -p $HOME/LiveMap/fonts/truetype/freefont
check
cp -Lv /usr/share/fonts/truetype/freefont/FreeSans.ttf $HOME/LiveMap/fonts/truetype/freefont/
check
popd
fi
cd $HOME
check
zip -rv $HOME/LiveMap LiveMap LiveMapData -x "LiveMapData/GPTNotes/*" -x "LiveMapData/map/*" -x "LiveMapData/NMEA/*" -x "LiveMapData/photo/*" -x "LiveMapData/photoShots/*"
check
ls -l $HOME/LiveMap.zip
