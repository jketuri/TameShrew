pushd map
del ..\mml_info.txt
for %%f in (*.tif) do (
	gdalinfo %%f >>..\mml_info.txt
)
popd
