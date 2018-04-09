#!/usr/bin/env bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

printf 'Running PPP...\n'

for file
do

	if ! diff -q <($DIR/../bin/ppp "$file") <($DIR/../bin/ppp-old "$file") \
		> /dev/null
	then
		printf "\e[31mFile: $file\e[m\n"
	else
		printf "File: $file\n"
	fi
done

printf 'Done\n'

exit 0
