// implement fork from user space

#include "lib.h"
#include <mmu.h>
#include <env.h>

/* ----------------- help functions ---------------- */

/* Overview:
 *  Copy `len` bytes from `src` to `dst`.
 *
 * Pre-Condition:
 *  `src` and `dst` can't be NULL. Also, the `src` area 
 *   shouldn't overlap the `dest`, otherwise the behavior of this 
 *   function is undefined.
 */
void user_bcopy(const void *src, void *dst, size_t len)
{
    void *max;

    //  writef("~~~~~~~~~~~~~~~~ src:%x dst:%x len:%x\n",(int)src,(int)dst,len);
    max = dst + len;

    // copy machine words while possible
    if (((int)src % 4 == 0) && ((int)dst % 4 == 0)) {
        while (dst + 3 < max) {
            *(int *)dst = *(int *)src;
            dst += 4;
            src += 4;
        }
    }

    // finish remaining 0-3 bytes
    while (dst < max) {
        *(char *)dst = *(char *)src;
        dst += 1;
        src += 1;
    }

    //for(;;);
}

/* Overview:
 *  Sets the first n bytes of the block of memory 
 * pointed by `v` to zero.
 * 
 * Pre-Condition:
 *  `v` must be valid.
 *
 * Post-Condition:
 *  the content of the space(from `v` to `v`+ n) 
 * will be set to zero.
 */
void user_bzero(void *v, u_int n)
{
    char *p;
    int m;

    p = v;
    m = n;

    while (--m >= 0) {
        *p++ = 0;
    }
}
/*--------------------------------------------------------------*/

/* Overview: 
 *  Custom page fault handler - if faulting page is copy-on-write,
 * map in our own private writable copy.
 * 
 * Pre-Condition:
 *  `va` is the address which leads to a TLBS exception.
 *
 * Post-Condition:
 *  Launch a user_panic if `va` is not a copy-on-write page.
 * Otherwise, this handler should map a private writable copy of 
 * the faulting page at correct address.
 */

static void
pgfault(u_int va)
{ 
    u_int *tmp = UXSTACKTOP-2*BY2PG;
    int ret = 0;
    va = ROUNDDOWN(va, BY2PG);
    //writef("fork.c:pgfault():\t va:%x\n",va);
    u_int perm = (*vpt)[VPN(va)]&0xfff;
    if(perm&PTE_COW==0) user_panic("not a COW page");
    //map the new page at a temporary place
    ret = syscall_mem_alloc(syscall_getenvid(), tmp,perm&(~PTE_COW));
    if(ret<0)   user_panic("syscall_mem_alloc error\n");
    //copy the content
    user_bcopy((void*)va, (void*)tmp, BY2PG);   
    //map the page on the appropriate place
    ret = syscall_mem_map(syscall_getenvid(), tmp, syscall_getenvid(), va, perm&(~PTE_COW));
    if(ret<0)   user_panic("syscall_mem_map error\n");
    //unmap the temporary place
    ret = syscall_mem_unmap(syscall_getenvid(), tmp);
    if(ret<0)   user_panic("syscall_mem_unmap error\n");
}

/*
static void
pgfault(u_int va)
{
        u_int *temp;
        //      writef("fork.c:pgfault():\t va:%x\n",va);
    va = ROUNDDOWN(va,BY2PG);
    
    temp = UXSTACKTOP-2*BY2PG;

    u_int perm = (*vpt)[VPN(va)]& 0xfff;
    
    //writef("fork.c:pgfault():\t va:%x\n",va);
    
    if(perm & PTE_COW){
        
        //map the new page at a temporary place
        if(syscall_mem_alloc(syscall_getenvid(),temp,perm & (~PTE_COW))<0)
        {
            user_panic("sys_mem_alloc error.\n");
        }
        
            //copy the content
        user_bcopy((void *)va,(void *)temp,BY2PG);
        
        //map the page on the appropriate place
        if(syscall_mem_map(syscall_getenvid(),temp,syscall_getenvid(),va,perm & (~PTE_COW))<0)
        {
            user_panic("sys_mem_map error.\n");
        }
        
        //unmap the temporary place
        if(syscall_mem_unmap(syscall_getenvid(),temp)<0)
        {
            user_panic("sys_mem_unmap error.\n");
        }
    }
    else
    {
        user_panic("va page is not PTE_COW.\n");
    }
}
*/
/* Overview:    
 *  Map our virtual page `pn` (address pn*BY2PG) into the target `envid`
 * at the same virtual address. 
 *
 * Post-Condition:
 *  if the page is writable or copy-on-write, the new mapping must be 
 * created copy on write and then our mapping must be marked 
 * copy on write as well. In another word, both of the new mapping and
 * our mapping should be copy-on-write if the page is writable or 
 * copy-on-write.
 * 
 * Hint:
 *  PTE_LIBRARY indicates that the page is shared between processes.
 * A page with PTE_LIBRARY may have PTE_R at the same time. You
 * should process it correctly.
 */ 

static void 
duppage(u_int envid, u_int pn)
{    
    u_int addr;
    u_int perm;
    addr = pn*BY2PG;
    perm = (*vpt)[pn]&0xfff;
    if((perm&PTE_V)&&(perm&PTE_R)){
        if(perm&PTE_LIBRARY){
            syscall_mem_map(0, addr, envid, addr, perm);
            syscall_mem_map(0, addr, 0, addr, perm);
        }
        else{
            syscall_mem_map(0, addr, envid, addr, perm|PTE_COW);
            syscall_mem_map(0, addr, 0, addr, perm|PTE_COW);
        }
    } 
    else if(perm&PTE_V){
        syscall_mem_map(0, addr, envid, addr, perm);
    }    
    //  user_panic("duppage not implemented");
}   /*

static void
duppage(u_int envid, u_int pn)
{
        u_int addr;
        u_int perm;
    
    addr = pn * BY2PG;
    perm = (*vpt)[pn] & 0xfff;

    if(((perm & PTE_R) != 0) && (perm & PTE_V))
    {
        if(perm & PTE_LIBRARY)
        {
            perm = PTE_V | PTE_R | PTE_LIBRARY | perm;
        }
        else
        {
            perm = PTE_V | PTE_R | PTE_COW | perm;
        }
        if(syscall_mem_map(syscall_getenvid(), addr, envid, addr, perm) < 0)
        {
            writef("%x\n", addr);
            user_panic("sys_mem_map for son failed.\n");
        }
        if(syscall_mem_map(syscall_getenvid(), addr, syscall_getenvid(), addr, perm) < 0)
        { 
            user_panic("sys_mem_map for father failed.\n");
        }
    }
    else
    {
        if(syscall_mem_map(syscall_getenvid(), addr, envid, addr, perm) < 0){
            user_panic("sys_mem_map for son failed.1\n");
        
        }
    }
    
        //      user_panic("duppage not implemented");
} 
*/

/* O verview :  
 *  User-level fork. Create a child and then copy our address space
 * and page fault handler setup to the child.
 *
 * Hint: use vpd, vpt, and duppage.
 * Hint: remember to fix "env" in the child process!
 * Note: `set_pgfault_handler`(user/pgfault.c) is different from 
 *       `syscall_set_pgfault_handler`. 
 */  
 extern void __asm_pgfault_handler(void);
///*
int
fork(void)
{    
    // Your code here.
    u_int newenvid;
    extern struct Env *envs;
    extern struct Env *env;
    int ret = 0;
    u_int i;
    set_pgfault_handler(pgfault);
    newenvid = syscall_env_alloc();
    if(newenvid==0){ 
        env = &envs[ENVX(syscall_getenvid())];
        return 0;
    } 
    for(i=0;i<USTACKTOP;i+=BY2PG)
        if(((*vpd)[VPN(i)/1024]&PTE_V) && ((*vpt)[VPN(i)])!=0){
            duppage(newenvid, VPN(i));
        }
    ret =syscall_mem_alloc(newenvid, UXSTACKTOP-BY2PG, PTE_V|PTE_R);
    if(ret<0)   user_panic("mem_alloc\n");
    //The parent installs pgfault using set_pgfault_handler
    ret = syscall_set_pgfault_handler(newenvid, __asm_pgfault_handler, UXSTACKTOP);
    if(ret<0)   user_panic("pgfault\n");
    //alloc a new alloc
    ret = syscall_set_env_status(newenvid, ENV_RUNNABLE);
    if(ret<0)   user_panic("env_status\n");
    return newenvid;
}    
//*/ 
/*
int
fork(void)
{
        // Your code here.
        u_int newenvid;
        extern struct Env *envs;
        extern struct Env *env;
        u_int i;
    set_pgfault_handler(pgfault);
    if((newenvid = syscall_env_alloc()) == 0)
    {
        env = &envs[ENVX(syscall_getenvid())];
        return 0;
    }
  
    for(i = 0;i < USTACKTOP;i += BY2PG)
    {
        if(((*vpd)[VPN(i)/1024]) != 0 && ((*vpt)[VPN(i)]) != 0)
        {
            duppage(newenvid, VPN(i));
        }
    }
    if(syscall_mem_alloc(newenvid, UXSTACKTOP-BY2PG, PTE_V | PTE_R) < 0){
        user_panic("failed alloc UXSTACK.\n");
        return 0;
    }
    if(syscall_set_pgfault_handler(newenvid, __asm_pgfault_handler, UXSTACKTOP) < 0){
        user_panic("page fault handler setup failed.\n");
        return 0;
    }
    syscall_set_env_status(newenvid, ENV_RUNNABLE);
    //writef("ENV_ID: %d\n", newenvid);
    return newenvid;
} 
*/

// Challenge!
int
sfork(void)
{     
    user_panic("sfork not implemented");
    return -E_INVAL;
}

