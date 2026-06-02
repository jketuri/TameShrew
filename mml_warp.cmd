if "%1" == "" (
	set MAP=%CD%\map
) else (
	set MAP=%1
)
if not exist map_out (
	mkdir map_out
)
for /r %MAP% %%f in (*.tif) do (
	gdalwarp -s_srs EPSG:3067 -t_srs WGS84 "%%f" "map_out\%%~nxf"
)
