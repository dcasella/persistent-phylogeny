#!/usr/bin/env bash

printf 'Compiling tests...\n'

make tests || exit 1

printf '\nRun tests:\n'

run-parts tests/ || exit 1

printf '\nCleaning tests...\n'

make tests/clean
