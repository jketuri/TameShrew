setlocal enabledelayedexpansion
if "%1" == "" (
	set MAP=%CD%\map
) else (
	set MAP=%1
)
if not exist map_out (
	mkdir map_out
)
set /p="-s_srs EPSG:3067 -t_srs WGS84 "<nul>gdalwarp.txt 
for /r %MAP% %%f in (*.tif) do (
	set /p="%%f "<nul >>gdalwarp.txt
)
gdalwarp --optfile gdalwarp.txt "map_out\fin.tif"
endlocal
:end