#include "lib.h"

char ugetc();
void uwritef(char *fmt, ...);


char ugetc(){ 
    char a;
    while(1){ 
        syscall_read_dev(&a, 0x10000000, 1);
        if(a!=0){
            syscall_write_dev(&a, 0x10000000, 1);
            return a;
        } 
        else
            syscall_yield();
    }
}
static void mout(void *arg, const char *s, int l){
    int i;
    if ((l == 1) && (s[0] == '\0')) {
		return;
	}

	for (i = 0; i < l; i++) {
		syscall_write_dev(&s[i], 0x10000000, 1);

		if (s[i] == '\n') {
			syscall_write_dev(&s[i], 0x10000000, 1);
		}
	}

}


void uwritef(char *fmt, ...){ 
    va_list ap;
    va_start(ap, fmt);
    user_lp_print(mout, 0, fmt, ap);
    va_end(ap);
    return;
}

