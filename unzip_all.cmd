pushd map
for /r %%f in (*.zip) do unzip "%%f"
popd
