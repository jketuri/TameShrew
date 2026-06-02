if "%1" == "d" (
set CONFIG=Debug
set OSG_SUFFIX=d
set OSGART_SUFFIX=_debug
) else (
set CONFIG=Release
set OSG_SUFFIX=
set OSGART_SUFFIX=
)
set DRIVE=C:
set HOME=%DRIVE%\lm
set GDAL=%HOME%\gdal-1.10.1
set GDAL_DATA=%GDAL%\data
set PYTHONPATH=%GDAL%\bin\gdal\python;%GDAL%\bin\gdal\python\osgeo;%GDAL%\bin\ms\python
set PROJ_LIB=%GDAL%\bin\proj\SHARE
set PROJ=%HOME%\proj-4.8.0\src
set OSG=%HOME%\OpenSceneGraph-3.6.5-VC2022-64-Release
set OSG_PLUGINS=%OSG%\bin\osgPlugins-3.6.5
set OSG_EARTH=%HOME%\gwaldron-osgearth-6bc5b12
set OSG_ART=%HOME%\osgart\osgART_2.0_RC3
set ARTOOLKIT=%HOME%\artoolkit
set LIBUSB=%HOME%\libusb
set LIBEXIF=%HOME%\libexif-0.6.24
set PORTAUDIO=%HOME%\portaudio
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64
@rem call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd"
set INCLUDE=%ARTOOLKIT%\include;%OSG%\include;%OSG%\include\OpenThreads;%OSG_EARTH%\src;%OSG_ART%\include;%HOME%\3rdParty\include;%PORTAUDIO%\include;%LIBUSB%\include;%LIBEXIF%;%INCLUDE%
set LIB=%OSG%\lib;%OSG_EARTH%\lib\%CONFIG%;%OSG_ART%\lib\%CONFIG%;%HOME%\3rdParty\lib;%PORTAUDIO%\bin\Win32\Release;%LIBUSB%\MS32\dll;%LIBEXIF%\libexif\.libs;%LIB%
set PATH=%OSG%\bin;%OSG_PLUGINS%;%OSG_EARTH%\bin\%CONFIG%;%OSG_EARTH%\lib\%CONFIG%;%OSG_ART%\lib\%CONFIG%;%ARTOOLKIT%\bin;%ARTOOLKIT%\lib\%CONFIG%;%PROJ%;%HOME%\3rdParty\x86\bin;%LIBEXIF%\libexif\.libs;%DRIVE%\bin;%DRIVE%\msys\1.0\bin;%DRIVE%\emacs-23.1\bin;%DRIVE%\Program Files\7-Zip;%PORTAUDIO%\bin\Win32\Release;%LIBUSB%\MS32\dll;C:\Program Files\Subversion\bin;C:\Python26;%GDAL%;%GDAL%\apps;%PATH%
set OSG_FILE_PATH=%OSG%\OpenSceneGraph-Data
@rem set OSGNOTIFYLEVEL=DEBUG
