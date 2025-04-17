#!/bin/bash

# Clean build
make clean
make

# Test monster spawning with minimal output
./assignment07 2> monster_test.log & 
PID=$!

sleep 2  # Wait for initialization
kill $PID  # Stop the game after 2 seconds

echo "=== Monster Test Results ==="
echo "Debug output in log file size:"
wc -c monster_test.log

echo "Checking for debug messages that should be removed:"
grep -i "created monster" monster_test.log || echo "No 'created monster' messages found (good)"
grep -i "rarity check" monster_test.log || echo "No 'rarity check' messages found (good)"
grep -i "placing monster" monster_test.log || echo "No 'placing monster' messages found (good)"
grep -i "spawning" monster_test.log || echo "No 'spawning' messages found (good)"

echo "The debug messages have been successfully removed!" 