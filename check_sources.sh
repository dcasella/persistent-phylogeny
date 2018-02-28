#!/usr/bin/env bash

printf 'Running PPP...\n'

for file; do
	printf "File: $file\n"

	bin/ppp "$file" -m -x -v | grep --color -n -e 'No for safe source'
done

printf 'Done\n'

exit 0
