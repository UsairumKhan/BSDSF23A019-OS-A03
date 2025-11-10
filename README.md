his version adds input and output redirection to myshell, allowing commands to read from or write to files instead of the terminal.

Features

Supports standard Unix redirection operators:

< Redirects standard input from a file

> Redirects standard output to a file (overwrite)

>> Redirects standard output to a file (append)

Works with any executable command.

Integrates seamlessly with previous features (history, readline, etc.).

Implementation

The shell scans each command line for redirection symbols before executing.

When a redirection operator is found:

< uses open(file, O_RDONLY)

> uses open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)

>> uses open(file, O_WRONLY | O_CREAT | O_APPEND, 0644)

dup2() is used to replace the standard file descriptors:

STDIN_FILENO for <

STDOUT_FILENO for > and >>

Redirection tokens are removed from the argument list before calling execvp()
