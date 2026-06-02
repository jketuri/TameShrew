if "%CONFIG%" == "Debug" (
cl /c /MDd /Zi /nologo /W4 /WX- /Od /Ob0 /Oy- /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Gm- /EHsc /RTC1 /GS /fp:precise /Zc:wchar_t /Zc:forScope /GR /Fp"Debug\LiveMap.pch" /Gd /TP /analyze- /errorReport:queue LiveMap.cpp NMEA.cpp SerialStream.cpp SocketStream.cpp common.cpp Datum.cpp MapIndex.cpp PhotoIndex.cpp
@if not ERRORLEVEL 0 goto end
link /OUT:"LiveMap.exe" /INCREMENTAL /NOLOGO "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" "libjpegD.lib" "libtiff.lib" "osgd.lib" "osgDBd.lib" "osgShadowd.lib" "osgSimd.lib" "osgTerraind.lib" "osgFXd.lib" "osgManipulatord.lib" "osgTextd.lib" "osgGAd.lib" "osgUtild.lib" "osgViewerd.lib" "osgWidgetd.lib" "OpenThreadsd.lib" "ws2_32.lib" "winmm.lib" "wldap32.lib" "zlibstat.lib" "gdal_i.lib" "glu32.lib" "opengl32.lib" "SetupAPI.lib" "portaudio_x86.lib" "libexif.dll.a" /MANIFEST /ALLOWISOLATION /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /DEBUG /SUBSYSTEM:CONSOLE /STACK:"10000000" /PGD:"D:\ht\vs\LiveMap\Debug\LiveMap.pgd" /TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X86 /ERRORREPORT:QUEUE LiveMap.obj NMEA.obj SerialStream.obj SocketStream.obj common.obj Datum.obj MapIndex.obj PhotoIndex.obj
@if not "%ERRORLEVEL%" == "0" goto end
) else (
cl /c /nologo /W3 /WX- /O2 /Ob2 /Oy- /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_SCL_SECURE_NO_WARNINGS" /D "_CRT_SECURE_NO_DEPRECATE" /D "_MBCS" /Gm- /EHsc /MD /GS /fp:precise /Zc:wchar_t /Zc:forScope /GR /Fp"LiveMap.pch" /Gd /TP /analyze- /errorReport:queue LiveMap.cpp NMEA.cpp SerialStream.cpp common.cpp Datum.cpp MapIndex.cpp PhotoIndex.cpp
@if not "%ERRORLEVEL%" == "0" goto end
link /OUT:"LiveMap.exe" /INCREMENTAL:NO /NOLOGO "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "comdlg32.lib" "advapi32.lib" "osg.lib" "osgDB.lib" "osgShadow.lib" "osgSim.lib" "osgTerrain.lib" "osgFX.lib" "osgManipulator.lib" "osgText.lib" "osgGA.lib" "osgUtil.lib" "osgViewer.lib" "osgWidget.lib" "OpenThreads.lib" "ws2_32.lib" "winmm.lib" "wldap32.lib" "libjpeg.lib" "libtiff.lib" "zlibstat.lib" "gdal_i.lib" "glu32.lib" "opengl32.lib" "portaudio_x86.lib" "libexif.a" LiveMap.obj NMEA.obj SerialStream.obj common.obj Datum.obj MapIndex.obj PatternDrawable.obj PhotoIndex.obj /MANIFEST /ALLOWISOLATION /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /SUBSYSTEM:CONSOLE /STACK:"10000000" /TLBID:1 /DYNAMICBASE /NXCOMPAT /IMPLIB:"LiveMap.lib" /MACHINE:X86 /ERRORREPORT:QUEUE
@if not "%ERRORLEVEL%" == "0" goto end
)
mt -manifest LiveMap.exe.manifest -outputresource:LiveMap.exe
@if not "%ERRORLEVEL%" == "0" goto end
dir LiveMap.exe
call setup.cmd
:end