#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int paused = 0;

void ctrl_z(int signum){
    if (!paused){
        paused = 1;
        printf("\nWaiting for CTRL+Z - continuation, or CTRL+C - terminate program\n");
    }
    else{
        paused = 0;
    }

}

void ctrl_c(int signum){
    printf("\nReceived CTRL+C\n");
    exit(1);
}

void current_hour(){

    while(1){

        if (!paused) {
            struct tm *now_tm;

            time_t now = time(NULL);
            now_tm = localtime(&now);
            int hour = now_tm->tm_hour;
            int minute = now_tm->tm_min;
            int sec = now_tm->tm_sec;

            printf("Current time: %0*d:%0*d:%0*d\n", 2, hour, 2, minute, 2, sec);
        }
        sleep(1);
    }

}

int main(void) {

    struct sigaction act;
    act.sa_handler = ctrl_z;
    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;

    sigaction (SIGTSTP, &act, NULL);
    signal(SIGINT, ctrl_c);

    current_hour();

    return 0;
}