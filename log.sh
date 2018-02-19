#!/usr/bin/env bash

if [ "$1" = 'clear' ]; then
	printf 'Clearing...\n'

	rm -rf tests/output

	printf 'Done\n'

	exit 0
fi

mkdir -p tests/output

printf 'Running PPP...\n'

for file; do
	printf "File: $file\n"

	bin/ppp "$file" -m -x -v > tests/output/$(basename $file .txt).log
done

cd tests/output

printf 'Zipping...\n'

tar -cf ok_10_N.tar.gz ok_10_*_M.log &&
tar -cf ok_20_N.tar.gz ok_20_*_M.log &&
tar -cf ok_40_N.tar.gz ok_40_*_M.log &&
tar -cf ok_80_N.tar.gz ok_80_*_M.log &&
tar -cf no_10_N.tar.gz no_10_*_M.log &&
tar -cf no_20_N.tar.gz no_20_*_M.log &&
tar -cf no_40_N.tar.gz no_40_*_M.log &&
tar -cf no_80_N.tar.gz no_80_*_M.log

cd $PWD

printf 'Done\n'

exit 0
