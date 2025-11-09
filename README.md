## Version 2 - Built-in Commands
**Tag:** v2  
This version adds internal (built-in) shell commands that execute directly inside the shell process.

### Implemented Commands
| Command | Description |
|----------|-------------|
| `exit` | Exit the shell. |
| `cd <directory>` | Change the current working directory. |
| `help` | Display information about available built-ins. |
| `jobs` | Placeholder for future job control implementation. |

### Key Learning
- Difference between **built-in** and **external** commands.
- Using `chdir()` to change the working directory.
- Handling control flow within the main shell loop.

### Example

