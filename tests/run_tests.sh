#!/bin/bash

# Script to run all tests with coverage

set -e

echo "Building and running C++ tests with coverage..."

# Clean previous runs
make clean

# Build and run tests
make coverage

echo ""
echo "Test execution completed!"
echo ""
echo "Coverage Summary:"
echo "=================="

# Generate detailed coverage report
echo "Detailed coverage analysis:"
for gcov_file in *.gcov; do
    if [ -f "$gcov_file" ]; then
        filename=$(basename "$gcov_file" .gcov)
        
        # Count executable lines and missed lines
        lines_missed=$(grep -c '    #####:' "$gcov_file" 2>/dev/null || echo 0)
        lines_executed=$(grep -c '^    [0-9]' "$gcov_file" 2>/dev/null || echo 0)
        lines_total=$((lines_missed + lines_executed))
        
        if [ $lines_total -gt 0 ]; then
            coverage_pct=$(echo "scale=1; $lines_executed * 100 / $lines_total" | bc -l 2>/dev/null || echo "0")
            
            printf "%-20s: %6.1f%% coverage (%d/%d lines)\n" "$filename" "$coverage_pct" "$lines_executed" "$lines_total"
            
            # Check if coverage is below 90%
            if [ $(echo "$coverage_pct < 90" | bc -l) -eq 1 ]; then
                echo "  WARNING: Coverage below 90% target!"
            fi
        fi
    fi
done

echo ""
echo "Test artifacts generated:"
echo "========================"
echo "- *.gcov files: Line-by-line coverage data"
echo "- Test executables: test_*"
echo "- Coverage data: *.gcda, *.gcno files"

echo ""
echo "To view detailed coverage for a specific file:"
echo "  cat <filename>.gcov"