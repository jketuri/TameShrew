#!/bin/sh

check()
{
    RV="$?"
    if test $RV != "0"
	then
        echo "RV=$RV"
	    exit 1
    fi
}

mkdir -p $HOME/back/ht
cp -rv * $HOME/back/ht/
check
rm -f $HOME/ht.zip
check
zip -rv $HOME/ht * -x "LiveMap" -x "*.exe" -x "*.exp" -x "*.idb" -x "*.ils" -x "*.lib" -x "*.map" -x "*.obj" -x "*.pdb" -x "*.sln" -x "*.o" -x "*~"
check
cp -v $HOME/ht.zip $HOME/ht_s
check
bzip2 $HOME/ht_s
check
mv -v $HOME/ht_s.bz2 $HOME/ht_s
check
ls -l $HOME/ht.zip $HOME/ht_s
check
if test "$1" = ""
then
exit
fi
mount /mnt/floppy
check
cp -v $HOME/ht.zip /mnt/floppy
check
ls -l /mnt/floppy
check
umount /mnt/floppy
check
ls -l /mnt/floppy
check
