#!/usr/bin/env bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$1" = 'clear' ] || [ "$1" = 'clean' ]; then
	printf 'Clearing...\n'

	rm -rf $DIR/../tests/output

	printf 'Done\n'

	exit 0
fi

mkdir -p $DIR/../tests/output

printf 'Running PPP...\n'

for file in $DIR/../tests/input/??_??_*.txt; do
	printf "File: $file\n"

	$DIR/../bin/ppp "$file" -m -x -v > $DIR/../tests/output/$(basename $file .txt).log
done

cd $DIR/../tests/output

printf 'Zipping...\n'

tar -cf ok_10_N.tar.gz ok_10_*_M.log &&
tar -cf ok_20_N.tar.gz ok_20_*_M.log &&
tar -cf ok_40_N.tar.gz ok_40_*_M.log &&
tar -cf ok_80_N.tar.gz ok_80_*_M.log &&
tar -cf no_10_N.tar.gz no_10_*_M.log &&
tar -cf no_20_N.tar.gz no_20_*_M.log &&
tar -cf no_40_N.tar.gz no_40_*_M.log &&
tar -cf no_80_N.tar.gz no_80_*_M.log

cd $DIR

printf 'Done\n'

exit 0
