#!/usr/bin/env bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

printf 'Running PPP...\n'

for file; do
	printf "File: $file\n"

	$DIR/../bin/ppp "$file" -m -x -v | grep --color -n -e 'No for safe source'
done

printf 'Done\n'

exit 0
