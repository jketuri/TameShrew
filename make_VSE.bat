cl /O2 /GL /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /EHsc /MD /Fd"vc80.pdb" /W3 /nologo /c /Wp64 /Zi /TP /errorReport:prompt LiveMap.cpp NMEA.cpp SerialStream.cpp common.cpp Datum.cpp MapIndex.cpp PatternDrawable.cpp PhotoIndex.cpp
@if not "%ERRORLEVEL%" == "0" goto end
link /OUT:"LiveMap.exe" /INCREMENTAL:NO /NOLOGO /MANIFEST /MANIFESTFILE:"LiveMap.exe.manifest" /DEBUG /PDB:"LiveMap.pdb" /SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF /LTCG /MACHINE:X86 /ERRORREPORT:PROMPT oldnames.lib msvcrt.lib msvcprt.lib kernel32.lib freetype219.lib libjpeg.lib libpng.lib libtiff.lib libungif.lib osg.lib osgDB.lib osgGA.lib osgParticle.lib osgProducer.lib osgSim.lib osgText.lib osgUtil.lib producer.lib zlib.lib OpenGL32.lib winmm.lib OpenThreadsWin32.lib %PORTAUDIO%\build\msvc\Release_x86\portaudio_x86.lib %LIBEXIF%\release\libexif.lib LiveMap.obj NMEA.obj SerialStream.obj common.obj Datum.obj MapIndex.obj PatternDrawable.obj PhotoIndex.obj
@if not "%ERRORLEVEL%" == "0" goto end
mt -manifest LiveMap.exe.manifest -outputresource:LiveMap.exe
@if not "%ERRORLEVEL%" == "0" goto end
dir LiveMap.exe
:end