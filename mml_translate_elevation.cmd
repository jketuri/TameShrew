if "%1" == "" (
	set MAP=%CD%\map
) else (
	set MAP=%1
)
if not exist elevation (
	mkdir elevation
)
for /r %MAP% %%f in (*.asc) do (
	gdal_translate -of GTiff -a_srs epsg:3067 -co compress=deflate "%%f" "elevation\%%~nf.tif"
)
