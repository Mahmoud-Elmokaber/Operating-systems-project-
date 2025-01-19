This project is a simple shell implemented in C, capable of executing basic shell functionalities such as executing commands, handling redirection (`>`, `<`, `>>`), and piping (`|`). It also supports sequential command execution using `;`.

## Features

- **Command Execution:**
  - Runs basic commands like `ls`, `cat`, `pwd`, etc.
  - Handles built-in commands such as `cd` and `exit`.
  
- **Redirection Support:**
  - Redirect standard output to files using `>` and `>>`.
  - Redirect standard error using `2>`.

- **Piping:**
  - Facilitates inter-process communication using pipes (`|`) between commands.
  - Example: `ls | grep txt`.

- **Sequential Execution:**
  - Supports multiple commands separated by `;`.
  - Example: `ls; pwd`.

- **User-friendly Prompt:**
  - Displays the current user, host, and directory.
