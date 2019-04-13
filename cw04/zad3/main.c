#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int L;
int type;
int parentSent;
int parentReceived;
int childReceived;
int to_break;

pid_t child_id;


void sigusr1_handler(int signum){
    childReceived++;
    kill(getppid(), SIGUSR1);
}

void sigusr2_handler(int signum){
    printf("signals received by child: %d\n", childReceived);
    exit(0);
}


void sigint_handler_parent(int signum){
    to_break = 1;
    kill(child_id, SIGUSR2);
}

void sigint_handler_child(int signum){
    kill(getppid(), SIGINT);
}

void sigusr1_handler_parent(int signum){
    parentReceived++;
}

void sigrtmax_handler(int signum){
    childReceived++;
    kill(getppid(), SIGRTMAX);
}

void sigrtmin_handler(int signum){
    printf("signals received by child: %d\n", childReceived);
    exit(0);
}

void sigrtmax_handler_parent(int signum){
    parentReceived++;
}

void sigint_rt_handler_parent(int signum){
    to_break = 1;
    kill(child_id, SIGRTMIN);
}

void sigint_rt_handler_child(int signum){
    kill(getppid(), SIGINT);
}

int main(int argc, char* argv[]) {

    if (argc < 3){
        printf("Too few arguments\n");
        return 1;
    }

    L = atoi(argv[1]);
    type = atoi(argv[2]);
    if (type > 3){
        printf("Type must be 1, 2 or 3\n");
        return 1;
    }
    parentSent = 0;
    parentReceived = 0;
    childReceived = 0;
    to_break = 0;

    pid_t child = fork();

    if (child == 0){
        //child
        if (type == 1 || type == 2){
            signal(SIGUSR1,sigusr1_handler);
            signal(SIGUSR2, sigusr2_handler);
            signal(SIGINT, sigint_handler_child);
            while(1);
        }
        else if (type == 3){
            signal(SIGRTMAX, sigrtmax_handler);
            signal(SIGRTMIN, sigrtmin_handler);
            signal(SIGINT, sigint_rt_handler_child);
            while(1);
        }

    }
    else{
        //parent
        printf("\n");
        child_id = child;

        if (type == 1){
            signal(SIGUSR1, sigusr1_handler_parent);
            signal(SIGINT, sigint_handler_parent);
            while(L && !to_break){
                kill(child, SIGUSR1);
                parentSent++;
                L--;
            }
            kill(child,SIGUSR2);
            waitpid(child, NULL, 0);

            printf("signals sent from parent: %d\n", parentSent);
            printf("signals received by parent: %d\n", parentReceived);
        }
        else if (type == 2){
            signal(SIGUSR1, sigusr1_handler_parent);
            signal(SIGINT, sigint_handler_parent);

            for (int i = 1; i <= L && !to_break; i++){
                kill(child, SIGUSR1);
                parentSent++;
                while(i != parentReceived && !to_break);
            }

            kill(child, SIGUSR2);
            waitpid(child, NULL, 0);

            printf("signals sent from parent: %d\n", parentSent);
            printf("signals received by parent: %d\n", parentReceived);
        }
        else if (type == 3){
            signal(SIGRTMAX, sigrtmax_handler_parent);
            signal(SIGINT, sigint_rt_handler_parent);
            while(L && !to_break){
                kill(child, SIGRTMAX);
                parentSent++;
                L--;
            }
            kill(child,SIGRTMIN);
            waitpid(child, NULL, 0);

            printf("signals sent from parent: %d\n", parentSent);
            printf("signals received by parent: %d\n", parentReceived);
        }

    }
    return 0;
}