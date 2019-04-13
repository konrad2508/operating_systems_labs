#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <bits/types/siginfo_t.h>
#include <sys/wait.h>

int N;
int M;
int k;
int* childid;
int childcnt;
int* sent;
int q;

void sigusr1_handler(int signo, siginfo_t *info, void *context){
    printf("Request received from %d\n", info->si_pid);
    for (int i = 0; i < N; i++){
        if (childid[i] == info->si_pid) {
            sent[i] = 1;
            break;
        }
    }
    k++;
    if (k == M){
        printf("Request threshold reached\n");
        for(int i = 0; i < N && childid[i] != 0; i++){
            if (sent[i] == 1) {
                printf("Granting request to %d (met threshold)\n", childid[i]);
                kill(childid[i], SIGCONT);
            }
        }
    }
    else if (k > M){
        printf("Granting request to %d\n", info->si_pid);
        kill(info->si_pid, SIGCONT);
    }
}

void sigint_handler(int signo, siginfo_t *info, void *context){
    for (int i = 0; i < N && childid[i] != 0; i++){
        kill(childid[i], SIGINT);
    }
    exit(0);
}

void sigrt_handler(int signo, siginfo_t *info, void *context){
    printf("Received signal %d from %d\n", signo, info->si_pid);
    q++;
    childcnt--;
}

int main(int argc, char* argv[]) {


    if (argc < 3){
        printf("Too few arguments\n");
        return 1;
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);
    if (N < M){
        printf("N must be less than M\n");
        return 1;
    }
    k = 0;
    childcnt = 0;

    childid = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++){
        childid[i] = 0;
    }
    sent = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++){
        sent[i] = 0;
    }

    struct sigaction act;
    act.sa_flags = SA_SIGINFO | SA_NOCLDWAIT;
    sigemptyset(&act.sa_mask);

    act.sa_handler = (void*)sigusr1_handler;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_handler = (void*)sigint_handler;
    sigaction(SIGINT, &act, NULL);

    for (int i = SIGRTMIN; i <= SIGRTMAX; i++){
        act.sa_handler = (void*)sigrt_handler;
        sigaction(i, &act, NULL);
    }

    for (int i = 0; i < N; i++){
        pid_t child = fork();

        if (child == 0){

            srand((unsigned)time(NULL) + getpid());

            int sleeptime = (unsigned int)rand()%10 + 1;
            sleep(sleeptime);
            kill(getppid(), SIGUSR1);
            raise(SIGSTOP);
            int sigid = SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN));
            kill(getppid(), sigid);
            printf("Killing %d, slept for %d", getpid(), sleeptime);
            exit(0);
        }
        else if (child > 0){
            childcnt++;
            printf("Forked new child %d\n", child);
            childid[i] = child;
        }

    }

    while (childcnt > 0);

    free(childid);
    free(sent);

    return 0;
}