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
    pid_t child; // Grab the process ID.

    if (argc < 2){
        fprintf(stderr, 'Usage: %s program [args...]\n');
        exit(1);
    }

    child = fork(); // Create new process, a duplicate of the current process.

    if(child == 0){
        // Child process.
        int status;
        ptrace(PTRACE_TRACEME, 0, NULL, NULL); 
        execvp(argv[1], &argv[2]);
        perror("execvp");
        exit(1);
    }else if(child > 0) 
    {   // Parent process (debugger).
        int status;
        struct user_regs_struct registers;

        waitpid(child, &status, 0);
        printf("Child process started. PID:  %d\n", child); // Not seen?? 

        // Continue until child exists.
        while(WIFSTOPPED(status)){
            // Get registers.
            ptrace(PTRACE_GETREGS, child, NULL, &registers); 

            // Print the instruction pointer.
            printf("RIP: 0x%llx\n", registers.rip);

        }

        

    }
}