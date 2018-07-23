#include <asm/asm.h>
#include <pmap.h>
#include <env.h>
#include <printf.h>
#include <kclock.h>
#include <trap.h>

extern char aoutcode[];
extern char boutcode[];

void mips_init()
{
	printf("init.c:\tmips_init() is called%08x\n", 1222);
	mips_detect_memory();
	
	mips_vm_init();
	page_init();
	env_init();
	//env_check();
    //ENV_CREATE(user_pingpong);
    //ENV_CREATE(user_fktest);
    //ENV_CREATE(user_ipc_p0);
    //ENV_CREATE(user_ipc_p1);
    //ENV_CREATE(user_fstest);
    //ENV_CREATE(user_testpipe);
    //ENV_CREATE(user_testpiperace);
    ENV_CREATE(user_icode);
    ENV_CREATE(fs_serv);
    trap_init();
	kclock_init();

	
	while(1);
	panic("init.c:\tend of mips_init() reached!");
}

void bcopy(const void *src, void *dst, size_t len)
{
	void *max;

	max = dst + len;
	// copy machine words while possible
	while (dst + 3 < max)
	{
		*(int *)dst = *(int *)src;
		dst+=4;
		src+=4;
	}
	// finish remaining 0-3 bytes
	while (dst < max)
	{
		*(char *)dst = *(char *)src;
		dst+=1;
		src+=1;
	}
}

void bzero(void *b, size_t len)
{
	void *max;

	max = b + len;

	//printf("init.c:\tzero from %x to %x\n",(int)b,(int)max);
	
	// zero machine words while possible

	while (b + 3 < max)
	{
		*(int *)b = 0;
		b+=4;
	}
	
	// finish remaining 0-3 bytes
	while (b < max)
	{
		*(char *)b++ = 0;
	}		
	
}
