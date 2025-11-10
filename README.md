## Version 4 - Readline Integration (v4)

This version replaces the manual input routine with the GNU Readline library to provide:
- Line editing (arrow keys, delete/backspace, etc.)
- Command recall using Up/Down arrows
- Tab completion for filenames (Readline's default completion)
- Readline-managed history (plus an internal history kept for the `history` builtin and `!n`)

Notes:
- Readline `add_history()` is called automatically when the user types a non-empty line.
- The shell still keeps an internal history array so the `history` command and `!n` recall (assignment requirement) continue to work.

Build:
```bash
sudo apt install libreadline-dev   # if readline not installed
make clean
make
./bin/myshell
