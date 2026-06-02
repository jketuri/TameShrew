#This Script Installs the files from the Xcode build into the correct location.

DEVELOPMENT="Release 32bit Carbon"

#echo Copy Examples
#cp -r ../OpenSceneGraph/build/"$DEVELOPMENT"/*.app Binary/Examples/
echo Install Applications
sudo cp -v build/"$DEVELOPMENT"/osgviewer.app/Contents/MacOS/osgviewer /usr/local/bin/
sudo cp -v build/"$DEVELOPMENT"/osgarchive.app/Contents/MacOS/osgarchive /usr/local/bin/
sudo cp -v build/"$DEVELOPMENT"/osgversion.app/Contents/MacOS/osgversion /usr/local/bin/
sudo cp -v build/"$DEVELOPMENT"/osgconv.app/Contents/MacOS/osgconv /usr/local/bin/
sudo cp -v build/"$DEVELOPMENT"/osgmovie.app/Contents/MacOS/osgmovie /usr/local/bin/

echo Install Frameworks
sudo cp -v -r build/"$DEVELOPMENT"/*.framework /Library/Frameworks/

echo Install Plugins
sudo cp -v build/"$DEVELOPMENT"/*.so /Library/Application\ Support/OpenSceneGraph/PlugIns/
#sudo cp -v build/"$DEVELOPMENT"/osgtext /Library/Application\ Support/OpenSceneGraph/PlugIns/
#sudo cp -v build/"$DEVELOPMENT"/osgpick /Library/Application\ Support/OpenSceneGraph/PlugIns/

echo Install Complete
