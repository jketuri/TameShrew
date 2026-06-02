pushd $HOME/lm/MPOWER/gpt
rm $HOME/mpower.csv
for file in `find . -name '*.mdb' | sed -e 's/\.\/GPT\(.*\)\.mdb/\1/' | sort -n`
do echo GPT${file}.mdb
mdb-export GPT${file}.mdb TrackNotes >>$HOME/mpower.csv
done
ls -l $HOME/mpower.csv
popd
