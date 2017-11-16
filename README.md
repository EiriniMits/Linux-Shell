# Linux-Shell

A simple Linux shell written in C

## Features
### Internal commands: 
```
- exit 
- clear 
- cd
```
### External commands:
```
- I/O redirection via dup2 system call (e.g. <cmd> <args> > <output>, <cmd> <args> < <input> > <output>).
- Pipelining via pipe and dup2 system calls (e.g. <cmd1> | <cmd2>). Multiple piping is allowed.
- Wildcard character * 
```
