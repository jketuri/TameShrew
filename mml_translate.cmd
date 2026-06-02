if "%1" == "" (
	set MAP=%CD%\map
) else (
	set MAP=%1
)
if not exist map_out (
	mkdir map_out
)
for /r %MAP% %%f in (*.tif) do (
	gdal_translate -of GTiff -co "TILED=YES" "%%f" "map_out\%%~nxf"
)
