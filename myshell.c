#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h> 
#include <signal.h>
#include <unistd.h>

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
    pid_t ppid;
    int* symbol = find_special_symbol(count, arglist);
    if (symbol[0] == 0){
        ppid = fork()
        if (ppid < 0){
            perror("An Error Has Occurred");
            exit(1);
        }
        if (fork == 0){
            if (execvp(arglist[0],arglist) == -1){
                perror("An Error Has Occurred");
                exit(1);
            }
        }
        wait(NULL);
        return 0;
    }

}

int finalize(void){
    return 0;
}