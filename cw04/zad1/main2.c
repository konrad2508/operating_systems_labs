#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

int paused;
pid_t child;
int do_fork;

void ctrl_z(int signum){
    if (child == -1){
        do_fork = 1;
    }
    else if (child > 0){
        printf("\nWaiting for CTRL+Z - continuation, or CTRL+C - terminate program\n");
        kill(child, SIGINT);
        child = -1;
    }

}

void ctrl_c(int signum){
    printf("\nReceived CTRL+C\n");
    if (child != -1) kill(child, SIGINT);
    exit(0);
}

int main(void) {

    paused = 0;
    child = -1;
    do_fork = 1;

    struct sigaction act;
    act.sa_handler = ctrl_z;
    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;

    sigaction (SIGTSTP, &act, NULL);
    signal(SIGINT, ctrl_c);

    while(1) {
        if (do_fork){
            do_fork = 0;
            child = fork();
        }
        if (child == 0) {
            printf("\n");
            execl("./date.sh", "./date.sh", NULL);
        } else if (child > 0) {
            wait(NULL);
        }
    }

    return 0;
}