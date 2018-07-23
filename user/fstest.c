#include "lib.h"

static char *msg = "This is the NEW message of the day!\r\n\r\n";
static char *diff_msg = "This is a different massage of the day!\r\n\r\n";

void umain()
{
        int r;
        int fdnum;
        char buf[512];
        int n;
       /* 
        create("/sdy");
        if((r = open("/sdy", O_RDWR))<0)
                user_panic("create failed!!!\n");
        fdnum = r;
        writef("create and open successfully\n");
        if((r = write(fdnum, msg, strlen(msg)+1))<0)
            user_panic("write failed!!!\n");
        writef("write msg successfuuly!\n");
        close(fdnum);
        r = open("/sdy", O_RDONLY);
        if(r<0) user_panic("open failed!!\n");
        fdnum = r;
        writef("reopen successfully under the mode read only!\n");
        n = read(fdnum, buf, 511);
        if(n<0) user_panic("read falied");
        if(strcmp(buf, msg) !=0)
            user_panic("wrong data!!\n");
        writef("read successfully!\n");
        if((r = write(fdnum, msg, strlen(msg)+1))==0)
            user_panic("read only is not work!!\n");
        writef("write protect successfully!\n");
        remove("/sdy");
        writef("remove sucessfully!\n");
        if(open("/sdy", O_RDONLY)==0)
            user_panic("remove failed!!\n");
        */
        if ((r = open("/newmotd", O_RDWR)) < 0) {
                user_panic("open /newmotd: %d", r);
        }
        fdnum = r;
        writef("open is good\n");
         
        if ((n = read(fdnum, buf, 511)) < 0) {
                user_panic("read /newmotd: %d", r);
        }
        if (strcmp(buf, diff_msg) != 0) {
                user_panic("read returned wrong data");
        }
        writef("read is good\n");

        if ((r = ftruncate(fdnum, 0)) < 0) {
                user_panic("ftruncate: %d", r);
        }
        seek(fdnum, 0);

        if ((r = write(fdnum, msg, strlen(msg) + 1)) < 0) {
                user_panic("write /newmotd: %d", r);
        }

        if ((r = close(fdnum)) < 0) {
                user_panic("close /newmotd: %d", r);
        }

        //read again
        if ((r = open("/newmotd", O_RDONLY)) < 0) {
                user_panic("open /newmotd: %d", r);
        }
        fdnum = r;
        writef("open again: OK\n");

        if ((n = read(fdnum, buf, 511)) < 0) {
                user_panic("read /newmotd: %d", r);
        }
        if (strcmp(buf, msg) != 0) {
                user_panic("read returned wrong data");
        }
        writef("read again: OK\n");

        if ((r = close(fdnum)) < 0) {
                user_panic("close /newmotd: %d", r);
        }

        writef("file rewrite is good\n");
        
        while (1) {
                //writef("IDLE!");
        }
}

