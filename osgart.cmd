@rem CMake:
@rem ARTOOLKIT2_INCLUDE_DIR: D:/lm/artoolkit/include
@rem ARTOOLKIT2_LIBAR: D:/lm/artoolkit/lib/Release/AR.lib
@rem ARTOOLKIT2_LIBARGSUB_LITE: D:/lm/artoolkit/lib/Release/ARgsub_lite.lib
@rem ARTOOLKIT2_LIBARMULTI: D:/lm/artoolkit/lib/Release/ARMulti.lib
@rem ARTOOLKIT2_LIBARVIDEO: D:/lm/artoolkit/lib/Release/ARvideo.lib
@rem OPENTHREADS_INCLUDE_DIR: D:/lm/OpenSceneGraph-3.0.1/include/OpenThreads
@rem OPENTHREADS_LIBRARY: D:/lm/OpenSceneGraph-3.0.1/lib/OpenThreads.lib
@rem OPENTHREADS_LIBRARY_DEBUG: D:/lm/OpenSceneGraph-3.0.1/lib/OpenThreadsd.lib
@rem OSGDB_LIBRARY: D:/lm/OpenSceneGraph-3.0.1/lib/osgDB.lib
@rem OSGFX_LIBRARY: D:/lm/OpenSceneGraph-3.0.1/lib/osgFX.lib
@rem OSGGA_LIBRARY: D:/lm/OpenSceneGraph-3.0.1/lib/osgGA.lib
@rem OSGTERRAIN_LIBRARY: D:/lm/OpenSceneGraph-3.0.1/lib/osgTerrain.lib
@rem OSGTEXT_LIBRARY: D:/lm/OpenSceneGraph-3.0.1/lib/osgText.lib
@rem OSGUTIL_LIBRARY: D:/lm/OpenSceneGraph-3.0.1/lib/osgUtil.lib
@rem OSGVIEWER_LIBRARY: D:/lm/OpenSceneGraph-3.0.1/lib/osgViewer.lib
@rem OSG_INCLUDE: D:/lm/OpenSceneGraph-3.0.1/include
@rem OSG_LIBRARY: D:/lm/OpenSceneGraph-3.0.1/lib/osg.lib

pushd %HOME%
mkdir osgart
cd osgart
unzip ..\Osgart_2.0_rc3.zip
@rem copy osgart\ImageStreamCallback %OSG_ART%\include\osgART
popd
