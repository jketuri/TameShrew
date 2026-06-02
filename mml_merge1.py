import imp
import os
import sys
if len(sys.argv) > 1:
	path = sys.argv[1]
else:
	path = os.path.join(os.getcwd(), "map")
files = [os.path.join(path, root, f) for root, dirs, files in os.walk(path) for f in files if f.endswith('.png') or f.endswith('.tif')]
gdal_merge = imp.load_source('gdal_merge', os.environ.get('GDAL') + '\\swig\\python\\scripts\\gdal_merge.py')
print "files=" + str(files[0:2])
if os.path.exists("out.tif"):
	os.remove("out.tif")
gdal_merge.main(["", "-v", "-pct", "-separate", files[0], files[1]])
for index in range(len(files) - 2):
	if os.path.exists("out0.tif"):
		os.remove("out0.tif")
	os.rename("out.tif", "out0.tif")
	gdal_merge.main(["", "-v", "out0.tif", files[index + 2]])
