cl /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Gm /EHsc /RTC1 /MDd /Fd"vc80.pdb" /W3 /nologo /c /Wp64 /ZI /TP /errorReport:prompt LiveMap.cpp NMEA.cpp SerialStream.cpp common.cpp Datum.cpp MapIndex.cpp PatternDrawable.cpp PhotoIndex.cpp
@if not "%ERRORLEVEL%" == "0" goto end
link /OUT:"LiveMap.exe" /INCREMENTAL /NOLOGO /MANIFEST /MANIFESTFILE:"LiveMap.exe.manifest" /NODEFAULTLIB /DEBUG /PDB:"LiveMap.pdb" /SUBSYSTEM:CONSOLE /MACHINE:X86 /ERRORREPORT:PROMPT oldnames.lib msvcrtd.lib msvcprtd.lib kernel32.lib freetype219_D.lib libjpegd.lib libpngd.lib libtiffd.lib libungifd.lib osgd.lib osgDBd.lib osgGAd.lib osgParticled.lib osgProducerd.lib osgSimd.lib osgTextd.lib osgUtild.lib producerd.lib zlibd.lib OpenGL32.lib winMM.lib OpenThreadsWin32d.lib %PORTAUDIO%\build\msvc\Debug_x86\portaudio_x86.lib %LIBEXIF%\debug\libexif.lib LiveMap.obj NMEA.obj SerialStream.obj common.obj Datum.obj MapIndex.obj PatternDrawable.obj PhotoIndex.obj
@if not "%ERRORLEVEL%" == "0" goto end
mt -manifest LiveMap.exe.manifest -outputresource:LiveMap.exe
@if not "%ERRORLEVEL%" == "0" goto end
dir LiveMap.exe
:end