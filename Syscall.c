//STUDENT: Chengbin Hu
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>


int main() {
    const int loops = 10000;//run 10000 loops
    long timen;
    struct timeval start, end;// start and end time
    gettimeofday(&start, NULL);// record start time
    for (int i = 0; i < loops; i++) {//10000 loops to read 0 bytes
        read(0,0,0);
	}
    gettimeofday(&end,NULL);//record end time
    timen=(end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec);
    printf("run %i system calls in %ldus (%fus/syscall)\n",
           loops, timen, (timen / (float) loops));//get result
    return 0;
}
