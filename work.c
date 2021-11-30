#include "apue.h"
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define N 5

int nsecs;
static char *chops[N] = {"chops0", "chops1", "chops2", "chops3", "chops4"};


void initlock(const char *lockfile)
{
    int i;
    unlink(lockfile);
}

void lock(const char *lockfile)
{
    int fd;
    while ( (fd = open(lockfile, O_RDONLY | O_CREAT | O_EXCL, FILE_MODE)) < 0)
        sleep(1);
    close(fd);
}

void unlock(const char *lockfile)
{
    unlink(lockfile);
}

void takeFork(int i)
{
    if (i == N - 1) {
        lock(chops[0]);
        lock(chops[N - 1]);
    } else {
        lock(chops[i]);
        lock(chops[i + 1]);
    }
}

void putFork(int i)
{
    if (i == N - 1) {
        unlock(chops[0]);
        unlock(chops[N - 1]);
    } else {
        unlock(chops[i]);
        unlock(chops[i + 1]);
    }
}

void thinking(int i)
{
    printf("philosopher %d is thinking\n", i);
    sleep(nsecs);
}

void eating(int i)
{
    printf("philosopher %d is eating\n", i);
    sleep(nsecs);
}

void philosopher(int i)
{
    while(1) {
        thinking(i);
        takeFork(i);
        eating(i);
        putFork(i);
    }
}

int main(int argc, char* argv[])
{
    pid_t pid;

    if (argc == 1) {
        nsecs = 2;
    } else if (argc == 3 && !strcmp(argv[1], "-t")) {
        nsecs = atoi(argv[2]);
    } else {
        err_sys("Invalid argument!");
    }

    for (int i = 0; i < N; i ++ ) initlock(chops[i]);

    for (int i = 0; i < N; i ++ )
    {
        if ((pid = fork()) < 0){ 
            err_sys("The %d fork error!\n", i);
        } else if (pid == 0) {
            philosopher(i);
        }
    }

    wait(NULL);
}
