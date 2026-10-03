#include "systemcalls.h"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    if (cmd == NULL) {
        printf("Command is NULL\n");
    } else {
        int wstatus = system(cmd);
        if (wstatus == -1) {
            printf("Call to system failed - %s\n", strerror(errno));
        } else if (wstatus == 127) {
            printf("Child of system call could not execute a shell\n");
        } else {
            if (WIFEXITED(wstatus)) {
                int exitStatus = WEXITSTATUS(wstatus);
                printf("Child of system call exited with status %d\n", exitStatus);
                if (exitStatus == 0) {
                    return true;
                }
            } else if (WIFSIGNALED(wstatus)) {
                printf("Child of system call terminated by signal %d\n", WTERMSIG(wstatus));
                if (WCOREDUMP(wstatus)) {
                    printf("Child of system call produced a core dump\n");
                }
            } else if (WIFCONTINUED(wstatus)) {
                printf("Child of system call was continued...\n");
            } else {
                printf("Unexpected return value from call to system\n");
            }
        }
    }

    return false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    bool result = false;
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    //command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    pid_t pid = fork();
    int ret = -1;

    if (pid == -1) {
        printf("Error forking - %s\n", strerror(errno));
    } else if (pid == 0) {
        // Child process
        pid_t pid = getpid();
        printf("[%d] Child process spawned\n", pid);
        for(int i=0; i<count; i++)
        {
            printf("[%d] arg[%d] %s\n", pid, i, command[i]);
        }
        ret = execv(command[0], &command[0]);
        if (ret == -1) {
            printf("[%d] Error calling execv - %s\n", pid, strerror(errno));
        }
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int wstatus = 0;
        ret = waitpid(pid, &wstatus, 0);
        if (ret == pid) {
            // Wait success, return value same as child pid
            printf("Wait on child process succeeded\n");
            if (WIFEXITED(wstatus)) {
                int exitStatus = WEXITSTATUS(wstatus);
                printf("Child exited w/ status %d\n", exitStatus);
                if (exitStatus == 0) {
                    result = true;
                }
            } else if (WIFSIGNALED(wstatus)) {
                printf("Child terminated by signal %d\n", WTERMSIG(wstatus));
                if (WCOREDUMP(wstatus)) {
                    printf("Child produced a core dump\n");
                }
            } else if (WIFCONTINUED(wstatus)) {
                printf("Child was continued...\n");
            }

        } else if (ret == -1) {
            printf("Error waiting for child process - %s\n", strerror(errno));
        } else {
            printf("Unexpected return code from call to wait\n");
        }
    }

    va_end(args);

    return result;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    bool result = false;
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    //command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd == -1) {
        printf("Error calling open - %s\n", strerror(errno));
    } else {
        pid_t pid = fork();
        if (pid == -1) {
            printf("Error forking - %s\n", strerror(errno));
        } else if (pid == 0) {
            // Child process
            pid_t pid = getpid();
            printf("[%d] Child process spawned\n", pid);
            for(int i=0; i<count; i++)
            {
                printf("[%d] arg[%d] %s\n", pid, i, command[i]);
            }
            if (dup2(fd, 1) < 0) { 
                perror("dup2");
                close(fd);
                exit(EXIT_FAILURE);
            }
            close(fd);
            execv(command[0], &command[0]); 
            perror("execvp"); 
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            close(fd);
            int wstatus = 0;
            int ret = waitpid(pid, &wstatus, 0);
            if (ret == pid) {
                // Wait success, return value same as child pid
                printf("Wait on child process succeeded\n");
                if (WIFEXITED(wstatus)) {
                    int exitStatus = WEXITSTATUS(wstatus);
                    printf("Child exited w/ status %d\n", exitStatus);
                    if (exitStatus == 0) {
                        result = true;
                    }
                } else if (WIFSIGNALED(wstatus)) {
                    printf("Child terminated by signal %d\n", WTERMSIG(wstatus));
                    if (WCOREDUMP(wstatus)) {
                        printf("Child produced a core dump\n");
                    }
                } else if (WIFCONTINUED(wstatus)) {
                    printf("Child was continued...\n");
                }

            } else if (ret == -1) {
                printf("Error waiting for child process - %s\n", strerror(errno));
            } else {
                printf("Unexpected return code from call to wait\n");
            }
        }
    }

    va_end(args);

    return result;
}
