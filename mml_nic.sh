mkdir -p ~/LiveMapData/elevation
pushd ~/LiveMapData/elevation
wget -P . -e robots=off -nd -np -r https://www.nic.funet.fi/index/geodata/mml/dem25m/tif/2000/
popd
