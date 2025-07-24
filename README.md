# Debugger_PoC

Proof of concept for an upcoming debugger project. Demonstrated is the fundamental implementation technique using the ptrace system call.

## What is it and how does it work?

This PoC is a lighweight debugger which enables instruction-level tracing of a program's execution via the ptrace syscall. It is done by tracking the RIP register / instruction pointer - which acts as the bedrock to understanding how debuggers work at a lower level. 

### Steps

1. Create child process for the target process
2. Use ptrace's `PTRACE_TRACEME` in the child to allow debugging
3. Execute the target program WITHIN the child process.
4. In the parent process, which is out debugger:
    - At each execution step step capture the register state
    - Display RIP / instruction pointer location
    - Use `PTRACE_SINGLESTEP`