#!/bin/bash

# Script to set up git hooks for the project

echo "Setting up git hooks..."

# Create pre-commit hook
cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash

# Pre-commit hook to run tests before allowing commits

echo "Running pre-commit checks..."

# Check if wordlist.txt is being committed
if git diff --cached --name-only | grep -q "wordlist.txt"; then
    echo "ERROR: wordlist.txt files should not be committed!"
    echo "Please remove wordlist.txt from staging area:"
    echo "  git reset HEAD wordlist.txt"
    exit 1
fi

# Run tests in Docker
echo "Running unit tests in Docker..."
if make test-quick > /dev/null 2>&1; then
    echo "✓ Quick tests passed"
else
    echo "✗ Tests failed! Commit aborted."
    echo "Please fix failing tests before committing."
    echo "Run 'make test' to see detailed test results."
    exit 1
fi

echo "Pre-commit checks completed successfully."
exit 0
EOF

# Make it executable
chmod +x .git/hooks/pre-commit

echo "✓ Pre-commit hook installed successfully!"
echo ""
echo "The hook will now:"
echo "1. Prevent committing wordlist.txt files"
echo "2. Run unit tests before each commit"
echo "3. Abort commit if tests fail"
echo ""
echo "To bypass the hook (not recommended):"
echo "  git commit --no-verify"