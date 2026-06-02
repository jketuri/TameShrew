@rem CMake:
@rem DIRECTX_INCLUDE_DIR:
@rem C:/Program Files/Microsoft SDKs/Windows/v7.0A/Include

@rem Dependency DirectShow Video Library: Configuration Properties: General: Additional Include Directories:
@rem C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Include;C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Include\atl;C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Include\crt
@rem Dependency DirectShow Video Library: Configuration Properties: Linker: Input: Ignore Specific Default Libraries:
@rem atlthunk.lib

pushd %HOME%
svn co http://artoolkit.svn.sourceforge.net/svnroot/artoolkit/branches/hartmut-pre-2_8/artoolkit
touch artoolkit\include\atlcomcli.h
goto end
md artoolkit\DSVL
unzip dsvl-0.0.8c -d artoolkit\DSVL
copy artoolkit\DSVL\lib\*.* artoolkit\lib\Debug
:end
popd
copy artoolkit\DSVL_GraphManager.cpp %HOME%\artoolkit\lib\SRC\VideoWin32DirectShow\_ext\dsvl-0.0.8h\src
