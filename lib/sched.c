#include <env.h>
#include <pmap.h>
#include <printf.h>

#define SCREAM(x) printf("(x)");

/* Overview:   
 *  Check whether current environment has used up its time. If yes, then 
 *  check whether the list env_sched_list[pos] is empty. If yes, change the
 *  pointer to point the other list. Next, if current list is not empty, 
 *  pop the first one and allocate running time to it.
 * Hints:
 *  The variable which is for counting should be defined as 'static'.
 */ 
/*
void sched_yield(void){
	static int pos = 0;
	static int times = 0;
	static struct Env *e;
    struct Env *tag;
     if(!curenv){
        //LIST_INIT(&env_sched_list[0]);
        //LIST_INIT(&env_sched_list[1]);
        int i;
        for(i=0;i<NENV;i++)
            if(envs[i].env_status==ENV_RUNNABLE)
                LIST_INSERT_HEAD(&env_sched_list[0],&envs[i],env_sched_link);
        e = LIST_FIRST(&env_sched_list[0]);
        LIST_REMOVE(e, env_sched_link);
    }
    else if(curenv&&curenv->env_pri==times){
        times = 1;
        if(LIST_EMPTY(&env_sched_list[(pos+1)%2]))
            LIST_INSERT_HEAD(&env_sched_list[(pos+1)%2], curenv,env_sched_link);
        else{
            LIST_FOREACH(tag, &env_sched_list[pos], env_sched_link)
                if(!LIST_NEXT(tag,env_sched_link))
                    LIST_INSERT_AFTER(tag, curenv, env_sched_link);
            LIST_INSERT_AFTER(LIST_FIRST(&env_sched_list[(pos+1)%2]),curenv,env_sched_link);
        }
        if(LIST_EMPTY(&env_sched_list[pos]))
            pos = (pos+1)%2;
        if(!(LIST_EMPTY(&env_sched_list[pos]))){
            e = LIST_FIRST(&env_sched_list[pos]);
            LIST_REMOVE(e,env_sched_link);
        }
    }
    else{
        times++;
        e = curenv;
    }
    env_run(e);
    return;
}*/

/*
void sched_yield(void){
    static int pos = 0;
    static int times = 0;
    static struct Env *e;
    static int posid = 0;
    struct Env *tag;
    e = curenv;
    if(--times<=0||(e&&e->env_status!=ENV_RUNNABLE)){
        if(curenv&&curenv->env_status==ENV_RUNNABLE){
            if(!LIST_EMPTY(&env_sched_list[1-pos])){
                LIST_FOREACH(tag, &env_sched_list[1-pos], env_sched_link)
                    if(LIST_NEXT(tag,env_sched_link)==NULL){
                        LIST_INSERT_AFTER(tag, e, env_sched_link);
                        break;
                    }
            }
            else
                LIST_INSERT_HEAD(&env_sched_list[1-pos], e, env_sched_link);
        }
        if(LIST_EMPTY(&env_sched_list[pos])){
            pos = 1 - pos;
        }
        if(!LIST_EMPTY(&env_sched_list[pos])){
            e = LIST_FIRST(&env_sched_list[pos]);
            LIST_REMOVE(e, env_sched_link);
            times = e->env_pri;
        }
    }
    printf("changeing!!!%x\n", e->env_id);
    env_run(e);
    return;
}
*/

/*
void sched_yield(void){
    static int pos = 0;
    static int times = 0;
    static struct Env *e;
    if(!curenv){
        times = 1;
        e = &envs[0];
    }
    else if(curenv&&curenv->env_pri==times){
        times = 1;
        pos = (pos+1)%2;
        e = &envs[pos];
    }
    else{
        times++;
        e = curenv;
    }
    env_run(e);
    return;
}
*/


void sched_yield(void){
    static int pos = 0;
    static int times = 0;
    static struct Env* e;
    if(--times<=0||curenv==NULL||curenv->env_status!=ENV_RUNNABLE){
       if(LIST_EMPTY(&env_sched_list[pos])){
            pos = 1 -pos;
    
       }
       e = LIST_FIRST(&env_sched_list[pos]);
       if(e==NULL){
            while(1);
       }
       LIST_REMOVE(e, env_sched_link);
       LIST_INSERT_HEAD(&env_sched_list[1-pos], e, env_sched_link);
       times = e->env_pri;
    }
    env_run(e);
    return;

}

