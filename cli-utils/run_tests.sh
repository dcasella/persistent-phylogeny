#!/usr/bin/env bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd $DIR/../

printf 'Compiling tests...\n'

make tests || exit 1

printf '\nRun tests:\n'

run-parts tests/ || exit 1

printf '\nCleaning tests...\n'

make tests/clean

cd $DIR
