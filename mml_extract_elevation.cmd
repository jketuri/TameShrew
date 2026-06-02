if "%1" == "" (
	set MAP=%CD%\korkeusmalli_2m
) else (
	set MAP=%1
)
if not exist map (
	mkdir map
)
for /r %MAP% %%f in (*.zip) do (
	unzip "%%f" -d "map"
)
