if not exist "%HOME%\back\ht" mkdir "%HOME%\back\ht"
xcopy /s /y *.* "%HOME%\back\ht\"
del /q \ht.zip
zip -r \ht * -x *.exe -x *.idb -x *.ilk -x *.ils -x *.ipch -x *.lib -x -x *.log *.obj -x *.pdb -x *.sdf -x *.tlog
pushd \
bzip2 ht.zip
del ht_s
ren ht.zip.bz2 ht_s
popd
@if "%1" == "" goto end
pause
dir a:
chkdsk a:
xcopy \ht.zip a: /v /y
unzip -t a:\ht.zip
dir a:
:end
