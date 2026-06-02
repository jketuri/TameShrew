set PORTAUDIO=C:\n\portaudio_v18_1
set OPENSCENEGRAPH=C:\n\OpenSceneGraph-0.9.5-200308062325
set VC7=C:\Program Files\Microsoft Visual Studio .NET\Vc7\
@rem set OPENSCENEGRAPH=C:\Program Files\OpenSceneGraph
set INCLUDE=%PORTAUDIO%\pa_common;%PORTAUDIO%\pa_win_ds;%OPENSCENEGRAPH%\include;%VC7%\include;%VC7%\PlatformSDK\Include;%VC7%\PlatformSDK\Include\prerelease
set LIB=%OPENSCENEGRAPH%\lib;%VC7%\lib;%VC7%\PlatformSDK\lib;%VC7%\lib;%VC7%\PlatformSDK\lib\prerelease
set PATH=C:\bin;%PORTAUDIO%;%OPENSCENEGRAPH%\bin;C:\Program Files\Microsoft Visual Studio .NET\Vc7\bin;C:\Program Files\Microsoft Visual Studio .NET\Common7\IDE;C:\unxutils\usr\local\wbin;C:\MinGW\bin;%PATH%