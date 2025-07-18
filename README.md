# UNIX-Shell-Command-Line-Interpreter-in-C
# 🐚 UNIX Shell – Command-Line Interpreter in C

A **personal project** developed in **C** to implement a mini UNIX-like shell from scratch. This custom shell supports built-in commands, external program execution, history tracking, I/O redirection, pipelines, and more — all through a command-line interface.

<img width="850" height="414" alt="Image" src="https://github.com/user-attachments/assets/db7260cc-5148-4d64-b0cb-7647df00c8c1" />
<img width="1542" height="799" alt="Image" src="https://github.com/user-attachments/assets/90ffde8b-495b-46a6-9edb-79c30d290db4" />
---

## 🔧 Features

- 🧾 **Interactive shell prompt** with command number and hostname  
- ⚙️ **External command execution** using `execvp()`  
- 📚 **Built-in commands**:  
  - `hist` – View last 10 commands  
  - `curPid` – Show current process ID  
  - `pPid` – Show parent process ID  
  - `cd` – Change working directory  
  - `quit` – Exit the shell  
- 🔀 **Command sequencing** with `;` (e.g., `ls; pwd; whoami`)  
- 🔗 **Pipelining** using `|` (e.g., `ls -l | grep .c | less`)  
- 📤 **I/O Redirection**:  
  - `>` to redirect output to a file  
  - `<` to read input from a file  
- 🕘 **Command history**: Maintains and displays the last 10 entered commands  
- 💻 **Single command mode**: Run a one-off command using `-c` (e.g., `./PROG1 -c ls -l`)  

---

## 🛠️ Tech Stack

- **Language:** C  
- **System Calls:** `fork()`, `execvp()`, `wait()`, `dup2()`, `pipe()`, `open()`  
- **Core Concepts:**  
  - Process creation and management  
  - Command parsing and tokenization  
  - File descriptor and redirection handling  
  - Inter-process communication using pipes  
  - History implementation with arrays

---

## 🎯 Objective

This project was created as a personal challenge to explore core Linux system programming concepts by replicating the behavior of a simplified command-line interpreter.

---
## ***SOURCE CODE PROVIDED***
