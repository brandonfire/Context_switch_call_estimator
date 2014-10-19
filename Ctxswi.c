#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <linux/futex.h>



int main(void) {
    int loops = 10000;
    long timeu;
    struct timeval start, end;
    const int id = shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT | 0666);
    const pid_t other = fork();
    int* f = shmat(id, NULL, 0);
    *f = 0xA;
    if (other == 0) {
        for (int i = 0; i < loops; i++) {
            sched_yield();
            while (syscall(SYS_futex, f, FUTEX_WAIT, 0xA, NULL, NULL, 42)) {
                sched_yield();
            }
            *f = 0xB;
            while (!syscall(SYS_futex, f, FUTEX_WAKE, 1, NULL, NULL, 42)) {
                sched_yield();
            }
        }
        return 0;
    }
    cpu_set_t my_set;    
    CPU_ZERO(&my_set);   
    CPU_SET(2, &my_set); 
    sched_setaffinity(0, sizeof(cpu_set_t), &my_set);//set to one CPU represent 2
    gettimeofday(&start,NULL);
    for (int i = 0; i < loops; i++) {
        *f = 0xA;
        while (!syscall(SYS_futex, f, FUTEX_WAKE, 1, NULL, NULL, 42)) {
            sched_yield();
        }
        sched_yield();
        while (syscall(SYS_futex, f, FUTEX_WAIT, 0xB, NULL, NULL, 42)) {
            sched_yield();
        }
    }
    gettimeofday(&end,NULL);
    timeu=(end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec);
    int n = loops << 2;
    printf("run %i process context switches in %ldus (%fus/ctxsw)\n",
           n, timeu, (timeu / (float) n));
    wait(f);
    return 0;
}
