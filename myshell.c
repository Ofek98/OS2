#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h> 
#include <signal.h>
#include <unistd.h>


void print_error_and_exit(){
    fprintf(stderr,"%s\n", stderror(errno));
    exit(1);
}

pid_t fork_and_check_for_error(){
    int ppid = fork();
    if (ppid < 0){
        print_error_and_exit();
    } 
    return ppid;
}

int* find_special_symbol(int count, char** arglist)
/*scans the argument line for one of the following symbols: "%","|", ">". 
will return a 2 elements array, first element will indicate the symbol and second eill indicate it's location at the command.
first element value: 0 - no special discription, 1 - "&", 2 - "|", 3 - "<"
*/
{
    int* result = (int*) malloc(2 * sizeof(int));
    for (int i = 0; i < count+1; i++){
        if (strcmp(arglist[i],"&")==0){
            result[0] = 1; 
            result[1] = i; 
            return result;
        }
        if (strcmp(arglist[i],"|")==0){
            result[0] = 2; 
            result[1] = i; 
            return result;
        }
        if (strcmp(arglist[i],"<")==0){
            result[0] = 3; 
            result[1] = i; 
            return result;
        }
    }
    result[0] = 0; 
    result[1] = 0; 
    return result;
}

int prepare(void){
    return 0;
}

int process_arglist(int count, char** arglist){
    pid_t ppid_1, ppid_2;
    int pipefd[2];
    int* symbol = find_special_symbol(count, arglist);

    if (symbol[0] == 0){
        ppid_1 = fork_and_check_for_error();  
        if (ppid_1 == 0){
            if (execvp(arglist[0],arglist) == -1){
                print_error_and_exit();
            }
        }
        else{
            if(wait(NULL) < 0){
                print_error_and_exit();
            }
            free(symbol);
            return 0;
        }
    }

    else if (symbol[0] == 1){
        ppid_1 = fork_and_check_for_error();
        if (ppid_1 == 0){
            arglist[count] = NULL;
            if (execvp(arglist[0],arglist) == -1){
                print_error_and_exit();
            }
        }
        else{
            if(wait(NULL) < 0){
                print_error_and_exit();
            }
            free(symbol);
            return 0;
        }
    }

    else{
        arglist[symbol[1]] == NULL;

        if (symbol[0] == 2){
            if (pipe(pipefd) < 0){
                print_error_and_exit();
            }

            ppid_1 = fork_and_check_for_error(); 

            if (ppid_1 == 0){
                if(dup2(pipefd[1], STDOUT_FILENO) < 0){
                    print_error_and_exit();
                }
                if (close(pipefd[1]) < 0){
                    print_error_and_exit();
                }
                if (execvp(arglist[0],arglist) == -1){
                    print_error_and_exit();
                }
            }

            else {
                ppid_2 = fork_and_check_for_error(); 
                if (ppid_2 == 0){
                    if(dup2(pipefd[0], STDIN_FILENO) < 0){
                        print_error_and_exit();
                    }
                    if (close(pipefd[0]) < 0){
                        print_error_and_exit();
                    }
                    if (execvp(arglist[symbol[1]],arglist+symbol[1]+1) == -1){
                        print_error_and_exit();
                    }
                }
                else{
                    if(wait(NULL) < 0){
                         print_error_and_exit();
                    }
                    free(symbol);
                    return 0;
                }
            }

        }
    }

}

int finalize(void){
    return 0;
}