pushd elevation
dir /b >\elevation.txt
gdalbuildvrt -input_file_list \elevation.txt -overwrite \elevation.vrt
popd