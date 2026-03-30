// Run OS commands and print stdout (stderr is not captured).
// Syntax: exec "shell command", variableName;

exec "uname -s", os;
print os;

exec "whoami", user;
print user;

// Example: list current directory (macOS/Linux)
exec "ls -1 | head -5", listing;
print listing;
