del \LiveMap.zip
rd /s /q %HOME%\LiveMap
@if not "%ERRORLEVEL%" == "0" goto end
mkdir %HOME%\LiveMap
@if not "%ERRORLEVEL%" == "0" goto end
if not exist "%HOME%\LiveMapData" mkdir "%HOME%\LiveMapData"
@if not "%ERRORLEVEL%" == "0" goto end
if not exist "%HOME%\LiveMapData\database" mkdir "%HOME%\LiveMapData\database"
@if not "%ERRORLEVEL%" == "0" goto end
if not exist "%HOME%\LiveMapData\GPT" mkdir "%HOME%\LiveMapData\GPT"
@if not "%ERRORLEVEL%" == "0" goto end
if not exist "%HOME%\LiveMapData\GPTNotes" mkdir "%HOME%\LiveMapData\GPTNotes"
@if not "%ERRORLEVEL%" == "0" goto end
if not exist "%HOME%\LiveMapData\map" mkdir "%HOME%\LiveMapData\map"
@if not "%ERRORLEVEL%" == "0" goto end
if not exist "%HOME%\LiveMapData\NMEA" mkdir "%HOME%\LiveMapData\NMEA"
@if not "%ERRORLEVEL%" == "0" goto end
if not exist "%HOME%\LiveMapData\photo" mkdir "%HOME%\LiveMapData\photo"
@if not "%ERRORLEVEL%" == "0" goto end
if not exist "%HOME%\LiveMapData\photoShots" mkdir "%HOME%\LiveMapData\photoShots"
@if not "%ERRORLEVEL%" == "0" goto end
copy LiveMap.exe %HOME%\LiveMap
@if not "%ERRORLEVEL%" == "0" goto end
copy LiveMapMS.properties %HOME%\LiveMap\LiveMap.properties
@if not "%ERRORLEVEL%" == "0" goto end
copy Lentoestetietokanta.csv %HOME%\LiveMapData
@if not "%ERRORLEVEL%" == "0" goto end
copy mpower.csv %HOME%\LiveMapData
@if not "%ERRORLEVEL%" == "0" goto end
pushd %OSG%\bin
@if not "%ERRORLEVEL%" == "0" goto end
copy *.dll %HOME%\LiveMap
@if not "%ERRORLEVEL%" == "0" goto end
@rem del %HOME%\LiveMap\*d.dll
@if not "%ERRORLEVEL%" == "0" goto end
popd
pushd %HOME%\3rdParty\x86\bin
@if not "%ERRORLEVEL%" == "0" goto end
copy *.dll %HOME%\LiveMap
@if not "%ERRORLEVEL%" == "0" goto end
popd
pushd %LIBEXIF%\libexif\.libs
@if not "%ERRORLEVEL%" == "0" goto end
copy *.dll %HOME%\LiveMap
@if not "%ERRORLEVEL%" == "0" goto end
popd
@rem cd %OSG%\OpenThreads\bin\win32
@rem @if not "%ERRORLEVEL%" == "0" goto end
@rem copy *.dll %HOME%\LiveMap
@rem @if not "%ERRORLEVEL%" == "0" goto end
@rem cd %OSG%\Producer\bin\win32
@rem @if not "%ERRORLEVEL%" == "0" goto end
@rem copy *.dll %HOME%\LiveMap
@rem @if not "%ERRORLEVEL%" == "0" goto end
cd %PORTAUDIO%\bin\Win32\Release
@if not "%ERRORLEVEL%" == "0" goto end
copy *.dll %HOME%\LiveMap
@if not "%ERRORLEVEL%" == "0" goto end
cd %HOME%
@if not "%ERRORLEVEL%" == "0" goto end
zip -r \LiveMap.zip LiveMap LiveMapData\GPT
@if not "%ERRORLEVEL%" == "0" goto end
zip -u \LiveMap.zip LiveMapData\* LiveMapData\GPTNotes LiveMapData\map LiveMapData\NMEA LiveMapData\photo LiveMapData\photoShots
@if not "%ERRORLEVEL%" == "0" goto end
dir \LiveMap.zip
:end

