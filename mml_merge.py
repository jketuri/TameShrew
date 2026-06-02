import imp
import os
import sys
if len(sys.argv) > 1:
	path = sys.argv[1]
else:
	path = os.path.join(os.getcwd(), "map")
argv = ["", "-v", "-pct"] + [os.path.join(path, root, f) for root, dirs, files in os.walk(path) for f in files if f.endswith('.png') or f.endswith('.tif')]
gdal_merge = imp.load_source('gdal_merge', os.environ.get('GDAL') + '\\swig\\python\\scripts\\gdal_merge.py')
if os.path.exists("out.tif"):
	os.remove("out.tif")
gdal_merge.main(argv)
