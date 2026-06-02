pushd "C:\Program Files\OpenSceneGraph\lib"
md ..\blib
for %%f in (*.lib) do coff2omf -r -v %%f ..\blib\%%f
popd