#include <stdlib.h>
#include <unistd.h>
int fildes[2];
/* buf size is 100 */ 
char buf[100];
int status;
int main(){
    status = pipe(fildes);
    if (status == -1 ) {
        /* an error occurred */ 
        printf("error\n");
    }
    switch (fork()) {
        case -1: 
            /* Handle error */
            break;
        case 0: 
            close(fildes[0]);
            write(fildes[1], "Hello world\n", 12); 
            close(fildes[1]); 
            exit(EXIT_SUCCESS);
        default: 
            close(fildes[1]);
            read(fildes[0], buf, 100); 
            printf("parent-process read:%s",buf); 
            close(fildes[0]); 
            exit(EXIT_SUCCESS);
    }
}
