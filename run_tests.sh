echo "Compiling tests...\n"
make tests

echo "\nRun tests:\n"
run-parts tests/ &&

echo "\nCleaning tests...\n" &&
make tests/clean
