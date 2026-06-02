	setlocal enabledelayedexpansion
if "%1" == "" (
	set MAP=map\
) else (
	set MAP=%1
)
set a=
for /r %MAP% %%f in (*.tif) do (
    set a=!a! %%f
)
python %GDAL%\swig\python\scripts\gdal_merge.py -pct%a%
endlocal