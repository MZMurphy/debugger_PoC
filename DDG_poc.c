#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h> // For fork and execvp syscalls. 
#include <sys/wait.h> // For ptrace debugging & process attachment. 
#include <sys/user.h> // For CPU register access via user_regs_structs.
#include <errno.h>


// This is a Proof of Concept debugger, and depicts the registers only. 

int main(int argc, char *argv[])
{
    pid_t child_process; // Grab the process ID.

    if (argc < 2){
        fprintf(stderr, 'Usage: %s program [args...]\n');
        exit(1);
    }

    child_process = fork(); // Create new process, a duplicate of the current process.

    if(child_process == 0){
        // Child process.
        int status;

        ptrace(PTRACE_TRACEME, 0, NULL, NULL); 
        execvp(argv[1], &argv[2]);
        perror("execvp");
        exit(1);

    }else if(child_process > 0) 
    {   
        // Parent process (debugger).
        int status;
        struct user_regs_struct registers;

        waitpid(child_process, &status, 0);
        printf("Child process started. PID:  %d\n", child_process); // Not seen?? 

        // Continue until child exists.
        while(WIFSTOPPED(status)){
            // Get registers.
            ptrace(PTRACE_GETREGS, child_process, NULL, &registers); 

            // Print the instruction pointer.
            printf("RIP: 0x%llx\n", registers.rip);

            // Below is the core of the single-step function.

            ptrace(PTRACE_SINGLESTEP, child_process, NULL, NULL); // Tells kernel to exeute one machine instruction & the stop. Does so via SIGRAP.
            waitpid(child_process, &status, 0); // Pauses the parent process, which is the DDG, until child process changes state. Updates status variable with why process stopped. 
            // Above the PIDof the child process is returned. 
        }

        printf("Child exited with status %d\n", WEXITSTATUS(status));

    }else{
        perror("fork");
        exit(1);
    }
    return 0;
}